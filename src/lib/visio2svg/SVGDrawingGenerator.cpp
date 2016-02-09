/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */

#include <map>
#include <sstream>
#include <string>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>

#include <librevenge-generators/librevenge-generators.h>
#include <librevenge/RVNGBinaryData.h>
#include "visio2svg/SVGDrawingGenerator.h"
#include <emf2svg.h>

namespace visio2svg {

void SVGDrawingGenerator::drawGraphicObject(
    const librevenge::RVNGPropertyList &propList) {
    if (!propList["librevenge:mime-type"] ||
        propList["librevenge:mime-type"]->getStr().len() <= 0)
        return;
    if (!propList["office:binary-data"])
        return;
    // emf binary blob handling
    if (propList["librevenge:mime-type"]->getStr() == "image/emf") {
        librevenge::RVNGBinaryData raw_data(
            propList["office:binary-data"]->getStr());
        // options->imgWidth = (int)propList["svg:width"]->getDouble();
        // options->imgHeight = (int)propList["svg:height"]->getDouble();
        char *svg_out = NULL;
        char *emf_content = (char *)raw_data.getDataBuffer();
        size_t emf_size = (size_t)raw_data.getBase64Data().size();
        generatorOptions *options =
            (generatorOptions *)calloc(1, sizeof(generatorOptions));
        options->verbose = false;
        options->emfplus = true;
        // options->nameSpace = (char *)"svg";
        //options->nameSpace = (char *)m_pImpl->m_nmSpace.c_str();
        options->svgDelimiter = false;
        options->imgWidth = propList["svg:width"]->getDouble() * 631;
        options->imgHeight = propList["svg:height"]->getDouble() * 631;

        // extract emf blob in separate file (DEBUGGING)
        // std::ofstream ofs (std::to_string(rand()), std::ios::out |
        // std::ios::binary);
        // ofs.write(emf_content, emf_size);
        // ofs.close();

        int ret = emf2svg(emf_content, emf_size, &svg_out, options);

        // m_pImpl->m_outputSink << "<!-- start emf conversion -->\n";
        //m_pImpl->m_outputSink << "<" << m_pImpl->getNamespaceAndDelim() << "g ";
        double x(propList["svg:x"]->getDouble());
        double y(propList["svg:y"]->getDouble());
        double width(propList["svg:width"]->getDouble());
        double height(propList["svg:height"]->getDouble());
        bool flipX(propList["draw:mirror-horizontal"] &&
                   propList["draw:mirror-horizontal"]->getInt());
        bool flipY(propList["draw:mirror-vertical"] &&
                   propList["draw:mirror-vertical"]->getInt());

        // m_pImpl->m_outputSink << "x=\"" << doubleToString(631*x) << "\" y=\""
        // << doubleToString(631*y) << "\" ";
        // m_pImpl->m_outputSink << "width=\"" << doubleToString(631*width) <<
        // "\" height=\"" << doubleToString(631*height) << "\" ";
        //m_pImpl->m_outputSink << "transform=\"";
        //m_pImpl->m_outputSink << " translate(" << doubleToString(631 * x)
        //                      << ", " << doubleToString(631 * y) << ") ";
        //m_pImpl->m_outputSink << "\" ";
        //m_pImpl->m_outputSink << " >\n";
        //m_pImpl->m_outputSink << svg_out;
        //m_pImpl->m_outputSink << "</" << m_pImpl->getNamespaceAndDelim()
        //                      << "g>\n";
        // m_pImpl->m_outputSink << "<!-- end emf conversion -->\n";
        free(svg_out);
        free(options);
        return;
    } else {
	  librevenge::RVNGSVGDrawingGenerator::drawGraphicObject(propList);
    }
    }
}

/* vim:set shiftwidth=4 softtabstop=4 noexpandtab: */
