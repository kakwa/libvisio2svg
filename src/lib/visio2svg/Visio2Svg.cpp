/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */

#include <unordered_map>
#include <iostream>
#include <string>
#include <sstream>
#include <stdio.h>
#include <string.h>
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
void Visio2Svg::postTreatement() {
}
}

/* vim:set shiftwidth=4 softtabstop=4 noexpandtab: */
