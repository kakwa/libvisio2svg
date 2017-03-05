/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */

#include "visio2svg/Visio2Svg.h"
#include "visio2svg/TitleGenerator.h"
#include <emf2svg.h>
#include <iostream>
#include <librevenge-generators/librevenge-generators.h>
#include <librevenge-stream/librevenge-stream.h>
#include <librevenge/librevenge.h>
#include <libvisio/libvisio.h>
#include <libwmf/api.h>
#include <libwmf/ipa.h>
#include <libwmf/svg.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <math.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string>
#include <sys/types.h>
#include <unordered_map>

#ifdef DARWIN
#include "memstream.c"
#endif

#define VISIOVSS 1
#define VISIOVSD 2

#define WHITESPACE 64
#define EQUALS 65
#define INVALID 66
#define WMF2SVG_MAXPECT (1 << 0)

namespace visio2svg {

static void or_wmf_svg_device_begin(wmfAPI *API) {
    wmf_svg_t *ddata = WMF_SVG_GetData(API);

    wmfStream *out = ddata->out;

    if (out == 0)
        return;

    if ((out->reset(out->context)) &&
        ((API->flags & WMF_OPT_IGNORE_NONFATAL) == 0)) {
        API->err = wmf_E_DeviceError;
        return;
    }

    if ((ddata->bbox.BR.x <= ddata->bbox.TL.x) ||
        (ddata->bbox.BR.y <= ddata->bbox.TL.y)) {
        API->err = wmf_E_Glitch;
        return;
    }

    if ((ddata->width == 0) || (ddata->height == 0)) {
        ddata->width = (unsigned int)ceil(ddata->bbox.BR.x - ddata->bbox.TL.x);
        ddata->height = (unsigned int)ceil(ddata->bbox.BR.y - ddata->bbox.TL.y);
    }

    wmf_stream_printf(API, out, (char *)"<g>\n");

    if (ddata->Description) {
        wmf_stream_printf(API, out, (char *)"<desc>%s</desc>\n",
                          ddata->Description);
    }
}

/* This is called from the end of each play for page termination
 */
static void or_wmf_svg_device_end(wmfAPI *API) {
    wmf_svg_t *ddata = WMF_SVG_GetData(API);

    wmfStream *out = ddata->out;
    wmf_stream_printf(API, out, (char *)"</g>\n");

    WMF_DEBUG(API, "~~~~~~~~wmf_[svg_]device_end");

    if (out == 0)
        return;
}

Visio2Svg::Visio2Svg() {
}

Visio2Svg::~Visio2Svg() {
}

typedef struct _ImageContext ImageContext;

struct _ImageContext {
    int number;
    char *prefix;
};

int Visio2Svg::vss2svg(std::string &in,
                       std::unordered_map<std::string, std::string> &out,
                       double scaling) {
    return visio2svg(in, out, scaling, VISIOVSS);
}

int Visio2Svg::vss2svg(std::string &in,
                       std::unordered_map<std::string, std::string> &out) {
    return visio2svg(in, out, 1.0, VISIOVSS);
}

int Visio2Svg::vsd2svg(std::string &in,
                       std::unordered_map<std::string, std::string> &out) {
    return visio2svg(in, out, 1.0, VISIOVSD);
}

int Visio2Svg::vsd2svg(std::string &in,
                       std::unordered_map<std::string, std::string> &out,
                       double scaling) {
    return visio2svg(in, out, scaling, VISIOVSD);
}

int explicit_wmf_error(char const *str, wmf_error_t err) {
    int status = 0;

    switch (err) {
    case wmf_E_None:
#ifdef DEBUG
        fprintf(stderr, "%s returned with wmf_E_None.\n", str);
#endif
        status = 0;
        break;

    case wmf_E_InsMem:
#ifdef DEBUG
        fprintf(stderr, "%s returned with wmf_E_InsMem.\n", str);
#endif
        status = 1;
        break;

    case wmf_E_BadFile:
#ifdef DEBUG
        fprintf(stderr, "%s returned with wmf_E_BadFile.\n", str);
#endif
        status = 1;
        break;

    case wmf_E_BadFormat:
#ifdef DEBUG
        fprintf(stderr, "%s returned with wmf_E_BadFormat.\n", str);
#endif
        status = 1;
        break;

    case wmf_E_EOF:
#ifdef DEBUG
        fprintf(stderr, "%s returned with wmf_E_EOF.\n", str);
#endif
        status = 1;
        break;

    case wmf_E_DeviceError:
#ifdef DEBUG
        fprintf(stderr, "%s returned with wmf_E_DeviceError.\n", str);
#endif
        status = 1;
        break;

    case wmf_E_Glitch:
#ifdef DEBUG
        fprintf(stderr, "%s returned with wmf_E_Glitch.\n", str);
#endif
        status = 1;

    case wmf_E_Assert:
#ifdef DEBUG
        fprintf(stderr, "%s returned with wmf_E_Assert.\n", str);
#endif
        status = 1;
        break;

    default:
#ifdef DEBUG
        fprintf(stderr, "%s returned unexpected value.\n", str);
#endif
        status = 1;
        break;
    }

    return (status);
}

int wmf2svg_draw(char *content, size_t size, float wmf_width, float wmf_height,
                 char **out, size_t *out_length) {
    int status = 0;

    unsigned long flags;

    FILE *out_f;
    out_f = open_memstream(out, out_length);

    ImageContext IC;

    wmf_error_t err;

    wmf_svg_t *ddata = 0;

    wmfAPI *API = 0;
    wmfD_Rect bbox;

    wmfAPI_Options api_options;

    flags = 0;

    flags |= WMF_OPT_FUNCTION;
    flags |= WMF_OPT_IGNORE_NONFATAL;

    api_options.function = wmf_svg_function;

    err = wmf_api_create(&API, flags, &api_options);
    status = explicit_wmf_error("wmf_api", err);

    wmfFunctionReference *FR = (wmfFunctionReference *)API->function_reference;

    FR->device_begin = or_wmf_svg_device_begin;
    FR->device_end = or_wmf_svg_device_end;

    if (status) {
        if (API)
            wmf_api_destroy(API);
        return (status);
    }

    err = wmf_mem_open(API, (unsigned char *)content, (long)size);
    status = explicit_wmf_error("open", err);

    if (status) {
        wmf_api_destroy(API);
        return (status);
    }

    err = wmf_scan(API, 0, &bbox);
    status = explicit_wmf_error("scan", err);

    if (status) {
        wmf_api_destroy(API);
        return (status);
    }

    ddata = WMF_SVG_GetData(API);

    float width;
    float height;
    wmf_size(API, &width, &height);

    if ((width <= 0) || (height <= 0)) {
#ifdef DEBUG
        fprintf(stderr, "Bad image size - but this error shouldn't occur...\n");
#endif
        status = 1;
        wmf_api_destroy(API);
        return (status);
    }

    // ddata->type = wmf_gd_jpeg;

    // ddata->flags |= WMF_SVG_OUTPUT_FILE;
    ddata->out = wmf_stream_create(API, out_f);

    ddata->bbox = bbox;
    ddata->width = wmf_width;
    ddata->height = wmf_height;
    ddata->flags |= WMF_SVG_INLINE_IMAGES;

    wmfD_Rect d_r;
    if (status == 0) {
        err = wmf_play(API, 0, &d_r);
        status = explicit_wmf_error("play", err);
    }

    fclose(out_f);
    wmf_api_destroy(API);

#ifdef DEBUG
    printf("%d, %d\n", err, status);
#endif
    return (status);
}

int Visio2Svg::visio2svg(std::string &in,
                         std::unordered_map<std::string, std::string> &out,
                         double scaling, int mode) {
    librevenge::RVNGStringStream input((const unsigned char *)in.c_str(),
                                       in.size());

    // check document type
    if (!libvisio::VisioDocument::isSupported(&input)) {
#ifdef DEBUG
        std::cerr << "ERROR: Unsupported file format (unsupported version) or "
                     "file is encrypted!"
                  << std::endl;
#endif
        return 1;
    }

    int ret = 0;

    // Recover Titles of each sheets
    librevenge::RVNGStringVector output_names;
    visio2svg::TitleGenerator generator_names(output_names);
    if (mode == VISIOVSS) {
        ret = libvisio::VisioDocument::parseStencils(&input, &generator_names);
    } else {
        ret = libvisio::VisioDocument::parse(&input, &generator_names);
    }

    if (!ret || output_names.empty()) {
#ifdef DEBUG
        std::cerr << "ERROR: Failed to recover sheets titles failed!"
                  << std::endl;
#endif
        return 1;
    }

    // Convert vss/vsd to SVG
    librevenge::RVNGStringVector output;
    librevenge::RVNGSVGDrawingGenerator generator(output, NULL);
    if (mode == VISIOVSS) {
        ret = libvisio::VisioDocument::parseStencils(&input, &generator);
    } else {
        ret = libvisio::VisioDocument::parse(&input, &generator);
    }
    if (!ret || output.empty()) {
#ifdef DEBUG
        std::cerr << "ERROR: SVG Generation failed!" << std::endl;
#endif
        return 1;
    }
    ret = 0;

    // Post Treatment loop and construction of the output hash table
    for (unsigned k = 0; k < output.size(); ++k) {
        char *post_treated;
        // Convert <image> tag containing emf blobs
        // and resize image
        ret |= postTreatement(&output[k], &output_names[k], &post_treated,
                              scaling);
        std::pair<std::string, std::string> item(output_names[k].cstr(),
                                                 std::string(post_treated));
        // output[k].cstr());
        free(post_treated);
        out.insert(item);
    }
    return ret;
}

// base64 decoder
// just copy paste from
// https://en.wikibooks.org/wiki/Algorithm_Implementation/Miscellaneous/Base64
static const unsigned char d[] = {
    66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 64, 66, 66, 66, 66, 66, 66, 66, 66,
    66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
    66, 66, 66, 66, 66, 62, 66, 66, 66, 63, 52, 53, 54, 55, 56, 57, 58, 59, 60,
    61, 66, 66, 66, 65, 66, 66, 66, 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10,
    11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 66, 66, 66, 66,
    66, 66, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42,
    43, 44, 45, 46, 47, 48, 49, 50, 51, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
    66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
    66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
    66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
    66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
    66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
    66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
    66, 66, 66, 66, 66, 66, 66, 66, 66};
int base64decode(char *in, size_t inLen, unsigned char *out, size_t *outLen) {
    char *end = in + inLen;
    char iter = 0;
    size_t buf = 0, len = 0;

    while (in < end) {
        unsigned char c = d[*in++];

        switch (c) {
        case WHITESPACE:
            continue; /* skip whitespace */
        case INVALID:
            return 1; /* invalid input, return error */
        case EQUALS:  /* pad character, end of data */
            in = end;
            continue;
        default:
            buf = buf << 6 | c;
            iter++; // increment the number of iteration
            /* If the buffer is full, split it into bytes */
            if (iter == 4) {
                if ((len += 3) > *outLen)
                    return 1; /* buffer overflow */
                *(out++) = (buf >> 16) & 255;
                *(out++) = (buf >> 8) & 255;
                *(out++) = buf & 255;
                buf = 0;
                iter = 0;
            }
        }
    }

    if (iter == 3) {
        if ((len += 2) > *outLen)
            return 1; /* buffer overflow */
        *(out++) = (buf >> 10) & 255;
        *(out++) = (buf >> 2) & 255;
    } else if (iter == 2) {
        if (++len > *outLen)
            return 1; /* buffer overflow */
        *(out++) = (buf >> 4) & 255;
    }

    *outLen = len; /* modify to reflect the actual output size */
    return 0;
}

// Recursive convert.
// Parse each node of the svg generated by librevenge/libvisio.
// If it encounters an <image> node, it checks if it's an emf blob
// and replace it with the result of emf2svg put inside a <g> node
// with proper translate() to get proper position.
int convert_iterator(xmlNode *a_node, xmlDocPtr root_doc) {
    xmlNode *cur_node = NULL;
    xmlNode *next_node;
    int ret = 0;

    for (cur_node = a_node; cur_node;) {
        next_node = cur_node->next;

        // image node specific treatment
        if ((!xmlStrcmp(cur_node->name, (const xmlChar *)"image"))) {
            xmlAttr *attribute = cur_node->properties;
            double x = 0;
            double y = 0;
            double width = 0;
            double height = 0;
            xmlChar *imgb64 = NULL;
            // recover some attributes needed for the conversion and the base64
            // encoded image.
            while (attribute) {
                xmlChar *value =
                    xmlNodeListGetString(cur_node->doc, attribute->children, 1);
                if ((!xmlStrcmp(attribute->name, (const xmlChar *)"x"))) {
                    x = atof((const char *)value);
                }
                if ((!xmlStrcmp(attribute->name, (const xmlChar *)"y"))) {
                    y = atof((const char *)value);
                }
                if ((!xmlStrcmp(attribute->name, (const xmlChar *)"width"))) {
                    width = atof((const char *)value);
                }
                if ((!xmlStrcmp(attribute->name, (const xmlChar *)"height"))) {
                    height = atof((const char *)value);
                }
                if ((!xmlStrcmp(attribute->name, (const xmlChar *)"href"))) {
                    imgb64 = value;
                    attribute = attribute->next;
                    continue;
                }
                attribute = attribute->next;
                xmlFree(value);
            }

            // detect the image type
            // right now, we handle emf and wmf
            // if image_type stays equal to UNKNOWN_IMGTYPE, it's not a type we
            // handle
            IMG_TYPE image_type = UNKNOWN_IMGTYPE;
            if (imgb64 != NULL) {
                if ((!xmlStrncmp(imgb64,
                                 (const xmlChar *)"data:image/emf;base64,",
                                 22))) {
                    image_type = EMF_IMGTYPE;
                } else if ((!xmlStrncmp(
                               imgb64,
                               (const xmlChar *)"data:image/wmf;base64,",
                               22))) {
                    image_type = WMF_IMGTYPE;
                }
            }

            // if the image is something we handle, convert it to SVG
            // else, just free the imgb64 and keep the node
            if (image_type != UNKNOWN_IMGTYPE) {
                xmlAttr *attribute = cur_node->properties;
                // create an svg group node (emf conversion will be put in it)
                xmlNode *node = xmlNewNode(NULL, (const xmlChar *)"g");

                // prepare translate to position emf conversion
                // (inside transform attribute).
                size_t tlen =
                    (size_t)snprintf(NULL, 0, " translate(%f,%f)  ", x, y);
                char *translate = (char *)malloc(tlen);
                tlen = snprintf(translate, tlen, " translate(%f,%f)  ", x, y);
                bool translate_set = false;

                // copy all attributes of the image node in the group node
                // except a few
                while (attribute) {
                    if (xmlStrcmp(attribute->name, (const xmlChar *)"href") &&
                        xmlStrcmp(attribute->name, (const xmlChar *)"x") &&
                        xmlStrcmp(attribute->name, (const xmlChar *)"y") &&
                        xmlStrcmp(attribute->name, (const xmlChar *)"width") &&
                        xmlStrcmp(attribute->name, (const xmlChar *)"height")) {
                        xmlChar *value = xmlNodeListGetString(
                            cur_node->doc, attribute->children, 1);
                        // special treatement for transform, must append the
                        // translate
                        // previously prepared
                        if ((!xmlStrcmp(attribute->name,
                                        (const xmlChar *)"transform"))) {
                            translate_set = true;
                            value = xmlStrncat(
                                value, (const xmlChar *)translate, tlen);
                        }
                        xmlNewProp(node, attribute->name, value);
                    }
                    attribute = attribute->next;
                }
                // if there was no "transform" attribute, add it with our
                // translate
                if (!(translate_set)) {
                    xmlNewProp(node, (const xmlChar *)"transform",
                               (const xmlChar *)translate);
                }

                // recover content (base64 encoded)
                char *svg_out = NULL;
                size_t len_out = 0;
                size_t len = strlen((char const *)imgb64);
                size_t size = len; //(len * 3 / 4 + 4);
                unsigned char *content = (unsigned char *)calloc(size, 1);
                int b64e = base64decode((char *)(imgb64 + 22), (len - 22),
                                        content, &size);
                ret |= b64e;
                xmlFree(imgb64);
                imgb64 = NULL;
                int e2se;
                xmlNodePtr s_sta;
                xmlNodePtr s_end;
                if (b64e) {
#ifdef DEBUG
                    std::cerr << "ERROR: Base64 decode failed" << std::endl;
#endif
                    ret = 1;
                }

                switch (image_type) {
                case EMF_IMGTYPE: {
                    // configure generator options
                    generatorOptions *options =
                        (generatorOptions *)calloc(1, sizeof(generatorOptions));
                    options->verbose = false;
                    options->emfplus = true;
                    // options->nameSpace = (char *)"svg";
                    options->nameSpace = NULL;
                    options->svgDelimiter = false;
                    options->imgWidth = width;
                    options->imgHeight = height;

                    // convert emf
                    e2se = emf2svg((char *)content, size, &svg_out, &len_out,
                                   options);
                    if (!e2se) {
#ifdef DEBUG
                        std::cerr << "ERROR: Failed to convert emf blob"
                                  << std::endl;
#endif
                        ret = 1;
                    }
                    free(options);
                    s_sta = xmlNewDocComment(
                        root_doc, (const unsigned char *)"emf-blob start");
                    s_end = xmlNewDocComment(
                        root_doc, (const unsigned char *)"emf-blob end");
                    break;
                }
                case WMF_IMGTYPE: {
                    e2se = wmf2svg_draw((char *)content, size, width, height,
                                        &svg_out, &len_out);
                    s_sta = xmlNewDocComment(
                        root_doc, (const unsigned char *)"wmf-blob start");
                    s_end = xmlNewDocComment(
                        root_doc, (const unsigned char *)"wmf-blob end");
                    if (e2se) {
#ifdef DEBUG
                        std::cerr << "ERROR: Failed to convert wmf blob"
                                  << std::endl;
#endif
                        ret = 1;
                    }
                    break;
                }
                default: {
#ifdef DEBUG
                    std::cerr << "ERROR: Unknown image type" << std::endl;
#endif
                    ret = 1;
                    break;
                }
                }

                xmlDocPtr doc;
                xmlNode *root_element = NULL;

                // parse svg generated by emf2svg or wmf2svg with libxml2
                doc = xmlReadMemory(svg_out, len_out, NULL, NULL,
                                    XML_PARSE_RECOVER | XML_PARSE_NOBLANKS |
                                        XML_PARSE_NONET | XML_PARSE_NOERROR |
                                        XML_PARSE_HUGE);
                root_element = xmlDocGetRootElement(doc);

                xmlAddChild(node, s_sta);

                // insert new nodes
                xmlNode *blob_svg = xmlCopyNodeList(root_element);
                xmlAddChildList(node, blob_svg);
                xmlAddChildList(cur_node->parent, node);
                xmlAddChild(node, s_end);

                // remove image node
                xmlUnlinkNode(cur_node);

                // freeing some memory
                xmlFreeNode(cur_node);
                free(content);
                free(svg_out);
                free(translate);
                xmlFreeDoc(doc);
            } else {
                ret |= convert_iterator(cur_node->children, root_doc);
            }
            free(imgb64);
        } else {
            ret |= convert_iterator(cur_node->children, root_doc);
        }
        cur_node = next_node;
    }
    return ret;
}

int Visio2Svg::scale_title(xmlNode **root, xmlDocPtr *doc, double scaling,
                           const xmlChar *title, int title_len) {
    int ret = 0;
    // create a group with transform="scale(<scaling>)" attribute
    xmlNode *node = xmlNewNode(NULL, (const xmlChar *)"g");
    size_t tlen = (size_t)snprintf(NULL, 0, " scale(%f)  ", scaling);
    char *translate = (char *)malloc(tlen);
    tlen = snprintf(translate, tlen, " scale(%f)  ", scaling);
    xmlNewProp(node, (const xmlChar *)"transform", (const xmlChar *)translate);

    xmlNode *new_root = xmlNewNode(NULL, (const xmlChar *)"svg");
    xmlAttr *attribute = (*root)->properties;
    double width = 0;
    double height = 0;
    while (attribute) {
        xmlChar *value = xmlNodeListGetString(*doc, attribute->children, 1);
        if ((!xmlStrcmp(attribute->name, (const xmlChar *)"width")) ||
            (!xmlStrcmp(attribute->name, (const xmlChar *)"height"))) {
            double geom = atof((const char *)value);
            char *cgeom = (char *)malloc(7);
            snprintf(cgeom, 6, "%4.f", geom * scaling);
            xmlNewProp(new_root, attribute->name, (const xmlChar *)cgeom);
            free(cgeom);
        } else {
            xmlNewProp(new_root, attribute->name, value);
        }
        attribute = attribute->next;
        xmlFree(value);
    }

    xmlNsPtr ns = (*root)->nsDef;
    while (ns) {
        xmlNewNs(new_root, ns->href, ns->prefix);
        ns = ns->next;
    }

    xmlDocPtr new_doc = xmlCopyDoc(*doc, 0);
    xmlNodePtr title_cdata = xmlNewCDataBlock(new_doc, title, title_len);
    xmlNodePtr title_node =
        xmlNewChild(new_root, NULL, (const xmlChar *)"title", NULL);
    xmlAddChild(title_node, title_cdata);
    xmlDocSetRootElement(new_doc, new_root);
    xmlNode *children = xmlCopyNodeList((*root)->children);
    xmlAddChildList(new_root, node);
    xmlAddChildList(node, children);
    xmlFreeDoc(*doc);
    *doc = new_doc;
    *root = new_root;
    free(translate);
    return ret;
}

int Visio2Svg::postTreatement(const librevenge::RVNGString *in,
                              const librevenge::RVNGString *name, char **out,
                              double scaling) {
    xmlDocPtr doc;
    xmlNode *root_element = NULL;
    int ret = 0;
// parse svg/xml generated by librevenge/libvisio with libxml2
#ifdef DEBUG
    std::cerr << "Converting: " << name->cstr() << std::endl;
//    std::cerr << "Converting: " << in->cstr() << std::endl;
#endif
    doc = xmlReadMemory(in->cstr(), in->size(), name->cstr(), NULL,
                        XML_PARSE_RECOVER | XML_PARSE_NOBLANKS |
                            XML_PARSE_NONET | XML_PARSE_HUGE);
    root_element = xmlDocGetRootElement(doc);
    xmlNodePtr comment = xmlNewDocComment(
        doc, (const unsigned char *)"converted by libvisio2svg");
    xmlAddChild(root_element, comment);
    // convert blobs (wmf, emf, ...)
    ret |= convert_iterator(root_element, doc);
    scale_title(&root_element, &doc, scaling, (const xmlChar *)name->cstr(),
                name->size());

    xmlBufferPtr nodeBuffer = xmlBufferCreate();
    xmlNodeDump(nodeBuffer, doc, root_element, 0, 1);
    // Dump the generated svg to out
    *out = (char *)nodeBuffer->content;
    // free some memory
    xmlFreeDoc(doc);
    nodeBuffer->content = NULL;
    xmlBufferFree(nodeBuffer);
    xmlCleanupParser();
    return ret;
}
}

/* vim:set shiftwidth=4 softtabstop=4 noexpandtab: */
