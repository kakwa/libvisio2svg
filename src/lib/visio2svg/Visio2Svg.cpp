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
        unsigned char *post_treated;
        postTreatement(&output[k], &output_names[k], &post_treated);
        std::pair<std::string, std::string> item(output_names[k].cstr(),
                                                 output[k].cstr());
        out.insert(item);
    }
    return 0;
}
int Visio2Svg::vsd2svg(std::string &in,
                       std::unordered_map<std::string, std::string> &out) {
    return 0;
}

static void convert_iterator(xmlDocPtr doc, xmlNode * a_node, int level, std::stringstream *out) {
    xmlNode *cur_node = NULL;

    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if ((!xmlStrcmp(cur_node->name, (const xmlChar *)"image"))){
            xmlUnlinkNode(cur_node);
            xmlFreeNode(cur_node);
            //printf("img node type: Element, name: %s\n", cur_node->name);
        }else{
            //printf("node type: Element, name: %s\n", cur_node->name);
        }

        convert_iterator(doc, cur_node->children, (level + 1), out);
    }
}


void Visio2Svg::postTreatement(const librevenge::RVNGString *in,
                               const librevenge::RVNGString *name, unsigned char **out) {
    std::stringstream buffout;
    xmlDocPtr doc;
    xmlNode *root_element = NULL;
    doc = xmlReadMemory(in->cstr(), in->size(), name->cstr(), NULL, 0);
    root_element = xmlDocGetRootElement(doc);
    convert_iterator(doc, root_element, 0, &buffout);
    xmlBufferPtr nodeBuffer = xmlBufferCreate();
    xmlNodeDump(nodeBuffer, doc, root_element, 0, 1);
    *out = nodeBuffer->content;
    std::cout << *out << std::endl;
    xmlFreeDoc(doc);
}
}

/* vim:set shiftwidth=4 softtabstop=4 noexpandtab: */
