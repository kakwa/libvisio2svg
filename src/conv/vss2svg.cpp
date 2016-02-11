/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

/* vss2svg
 * work based on vss2xhtml from libvisio
 */

// <<<<<<<<<<<<<<<<<<< START ORIGINAL HEADER >>>>>>>>>>>>>>>>>>>>>>>>>>>

/*
 * This file is part of the libvisio project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

// <<<<<<<<<<<<<<<<<<< END ORIGINAL HEADER >>>>>>>>>>>>>>>>>>>>>>>>>>>

#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <librevenge-stream/librevenge-stream.h>
#include <librevenge-generators/librevenge-generators.h>
#include <librevenge/librevenge.h>
#include <libvisio/libvisio.h>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <argp.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "visio2svg/TitleGenerator.h"

using namespace std;

const char *argp_program_version = V2S_VERSION;

const char *argp_program_bug_address = "<carpentier.pf@gmail.com>";

static char doc[] = "vss2svg -- Visio stencil to SVG converter";

static struct argp_option options[] = {
    {"verbose", 'v', 0, 0, "Produce verbose output"},
    {"input", 'i', "FILE", 0, "Input Visio .vss file"},
    {"output", 'o', "FILE/DIR", 0, "Output file (yED) or directory (svg)"},
    {"version", 'V', 0, 0, "Print vss2svg version"},
    {0}};

/* A description of the arguments we accept. */
static char args_doc[] = "[options] -i <in vss> -o <out dir>";

struct arguments {
    char *args[2]; /* arg1 & arg2 */
    bool version, verbose;
    char *output;
    char *input;
};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    /* Get the input argument from argp_parse, which we
       know is a pointer to our arguments structure. */
    struct arguments *arguments = (struct arguments *)state->input;

    switch (key) {
    case 'v':
        arguments->verbose = 1;
        break;
    case 'o':
        arguments->output = arg;
        break;
    case 'i':
        arguments->input = arg;
        break;
    case 'V':
        arguments->version = 1;
        break;
    case ARGP_KEY_ARG:
        if (state->arg_num >= 6)
            /* Too many arguments. */
            argp_usage(state);

        arguments->args[state->arg_num] = arg;

        break;

    case ARGP_KEY_END:
        break;
    default:
        return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

/* Our argp parser. */
static struct argp argp = {options, parse_opt, args_doc, doc};

int main(int argc, char *argv[]) {
    struct arguments arguments;
    arguments.version = 0;
    arguments.output = NULL;
    arguments.input = NULL;
    argp_parse(&argp, argc, argv, 0, 0, &arguments);

    librevenge::RVNGFileStream input(arguments.input);

    if (arguments.version) {
        std::cout << "vss2svg version: " << V2S_VERSION << "\n";
        return 0;
    }

    if (arguments.input == NULL) {
        std::cerr << "[ERROR] "
                  << "Missing --input=FILE argument\n";
        return 1;
    }

    if (arguments.output == NULL) {
        std::cerr << "[ERROR] "
                  << "Missing --output=DIR argument\n";
        return 1;
    }

    std::ifstream in(arguments.input);
    if (!in.is_open()) {
        std::cerr << "[ERROR] "
                  << "Impossible to open input file '" << arguments.input
                  << "'\n";
        return 1;
    }

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

    mkdir(arguments.output, S_IRWXU);

    std::string outputdir(arguments.output);
    mkdir(arguments.output, S_IRWXU);

    for (unsigned k = 0; k < output.size(); ++k) {
        ofstream myfile;
        std::basic_string<char> newfilename =
            outputdir + "/" + output_names[k].cstr() + ".svg";
        myfile.open(newfilename);
        myfile << output[k].cstr() << std::endl;
        myfile.close();
    }

    return 0;
}
/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
