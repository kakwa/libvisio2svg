/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */

#include <unordered_map>
#include <iostream>
#include <string>
#include <stdio.h>
#include <sstream>
#include <librevenge-stream/librevenge-stream.h>
#include <librevenge-generators/librevenge-generators.h>
#include <librevenge/librevenge.h>
#include <libvisio/libvisio.h>
#include <string>
#include <stdlib.h>
#include <argp.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "visio2svg/TitleGenerator.h"
#include "visio2svg/Visio2Svg.h"
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <emf2svg.h>

namespace visio2svg {

Visio2Svg::Visio2Svg() {
}

Visio2Svg::~Visio2Svg() {
}

int Visio2Svg::vss2svg(std::string &in,
                       std::unordered_map<std::string, std::string> &out) {
    librevenge::RVNGStringStream input((const unsigned char *)in.c_str(),
                                       in.size());

    if (!libvisio::VisioDocument::isSupported(&input)) {
        std::cerr << "ERROR: Unsupported file format (unsupported version) or "
                     "file is encrypted!" << std::endl;
        return 1;
    }

    librevenge::RVNGStringVector output_names;
    visio2svg::TitleGenerator generator_names(output_names);

    if (!libvisio::VisioDocument::parseStencils(&input, &generator_names)) {
        std::cerr << "ERROR: SVG Generation failed!" << std::endl;
        return 1;
    }
    if (output_names.empty()) {
        std::cerr << "ERROR: No SVG document generated!" << std::endl;
        return 1;
    }

    librevenge::RVNGStringVector output;
    librevenge::RVNGSVGDrawingGenerator generator(output, NULL);
    if (!libvisio::VisioDocument::parseStencils(&input, &generator)) {
        std::cerr << "ERROR: SVG Generation failed!" << std::endl;
        return 1;
    }
    if (output.empty()) {
        std::cerr << "ERROR: No SVG document generated!" << std::endl;
        return 1;
    }

    for (unsigned k = 0; k < output.size(); ++k) {
        char *post_treated;
        postTreatement(&output[k], &output_names[k], &post_treated);
        std::pair<std::string, std::string> item(output_names[k].cstr(),
                                                 std::string(post_treated));
        // output[k].cstr());
        out.insert(item);
    }
    return 0;
}
int Visio2Svg::vsd2svg(std::string &in,
                       std::unordered_map<std::string, std::string> &out) {
    return 0;
}

#define WHITESPACE 64
#define EQUALS 65
#define INVALID 66

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

static void convert_iterator(xmlNode *a_node) {
    xmlNode *cur_node = NULL;
    xmlNode *next_node;

    for (cur_node = a_node; cur_node;) {
        next_node = cur_node->next;

        if ((!xmlStrcmp(cur_node->name, (const xmlChar *)"image"))) {
            xmlAttr *attribute = cur_node->properties;
            double x = 0;
            double y = 0;
            double width = 0;
            double height = 0;
            xmlChar *imgb64 = NULL;
            while (attribute) {
                xmlChar *value =
                    xmlNodeListGetString(cur_node->doc, attribute->children, 1);
                if ((!xmlStrcmp(attribute->name, (const xmlChar *)"x"))) {
                    x = atof((const char *)value);
                    xmlFree(value);
                }
                if ((!xmlStrcmp(attribute->name, (const xmlChar *)"y"))) {
                    y = atof((const char *)value);
                    xmlFree(value);
                }
                if ((!xmlStrcmp(attribute->name, (const xmlChar *)"width"))) {
                    width = atof((const char *)value);
                    xmlFree(value);
                }
                if ((!xmlStrcmp(attribute->name, (const xmlChar *)"height"))) {
                    height = atof((const char *)value);
                    xmlFree(value);
                }
                if ((!xmlStrcmp(attribute->name, (const xmlChar *)"href"))) {
                    imgb64 = value;
                }
                attribute = attribute->next;
            }
            if ((!xmlStrncmp(imgb64, (const xmlChar *)"data:image/emf;base64,",
                             22))) {
                xmlAttr *attribute = cur_node->properties;
                xmlNode *node = xmlNewNode(NULL, (const xmlChar *)"toto");
                xmlCopyPropList(node, attribute);
                while (attribute) {
                    printf("%s\n", attribute->name);
                    xmlCopyProp(node, attribute);
                    if (xmlStrcmp(attribute->name, (const xmlChar *)"href")) {
                        xmlNewProp(node, attribute->name,
                                   xmlNodeListGetString(
                                       cur_node->doc, attribute->children, 1));
                    }
                    attribute = attribute->next;
                }

                // configure generator options
                generatorOptions *options =
                    (generatorOptions *)calloc(1, sizeof(generatorOptions));
                options->verbose = false;
                options->emfplus = true;
                //// options->nameSpace = (char *)"svg";
                options->nameSpace = NULL;
                options->svgDelimiter = true;
                options->imgWidth = width;
                options->imgHeight = height;

                //// recover content
                char *svg_out = NULL;
                size_t len = strlen((char const *)imgb64);
                size_t emf_size = len; //(len * 3 / 4 + 4);
                unsigned char *emf_content =
                    (unsigned char *)calloc(emf_size, 1);
                int ret = 0;
                ret = base64decode((char *)(imgb64 + 22), (len - 22),
                                   emf_content, &emf_size);
                // printf("%s\n", (imgb64 + 21));

                if (ret)
                    std::cerr << "ERROR: Base64 decode failed" << std::endl;

                //// convert
                ret = emf2svg((char *)emf_content, emf_size, &svg_out, options);
                if (!ret)
                    std::cerr << "ERROR: Failed to convert emf" << std::endl;
                xmlDocPtr doc;
                xmlNode *root_element = NULL;
                doc = xmlReadMemory(svg_out, strlen(svg_out), NULL, NULL, 0);
                root_element = xmlDocGetRootElement(doc);
                xmlAddChildList(node, root_element->children);
                xmlAddChildList(cur_node->parent, node);
                //// remove image node
                xmlUnlinkNode(cur_node);
                //// insert new nodes

                //// freeing some memory
                // xmlFreeNode(cur_node);
                // free(emf_content);
                // free(options);
                // free(svg_out);
            } else {
                convert_iterator(cur_node->children);
            }
            // free(imgb64);
        } else {
            convert_iterator(cur_node->children);
        }
        cur_node = next_node;
    }
}

void Visio2Svg::postTreatement(const librevenge::RVNGString *in,
                               const librevenge::RVNGString *name, char **out) {
    std::stringstream buffout;
    xmlDocPtr doc;
    xmlNode *root_element = NULL;
    doc = xmlReadMemory(in->cstr(), in->size(), name->cstr(), NULL, 0);
    root_element = xmlDocGetRootElement(doc);
    convert_iterator(root_element);
    xmlBufferPtr nodeBuffer = xmlBufferCreate();
    xmlNodeDump(nodeBuffer, doc, root_element, 0, 1);
    *out = (char *)nodeBuffer->content;
    // std::cout << *out << std::endl;
    // xmlFreeDoc(doc);
}
}

/* vim:set shiftwidth=4 softtabstop=4 noexpandtab: */
