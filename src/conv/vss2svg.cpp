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

#include <fstream>
#include <stdio.h>
#include <string.h>
#include <string>
#ifdef SAFE_FILENAME
#include <regex>
#endif
#include "visio2svg/Visio2Svg.h"
#include <argp.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unordered_map>

using namespace std;

const char *argp_program_version = V2S_VERSION;

const char *argp_program_bug_address = "<carpentier.pf@gmail.com>";

static char doc[] = "vss2svg -- Visio stencil to SVG converter";

static struct argp_option options[] = {
    {"input", 'i', "FILE", 0, "Input Visio .vss file"},
    {"output", 'o', "DIR", 0, "Output directory"},
    {"scaling", 's', "FLOAT", 0, "Scaling factor"},
    {"version", 'V', 0, 0, "Print vss2svg version"},
    {0}};

/* A description of the arguments we accept. */
static char args_doc[] = "[options] -i <in vss> -o <out dir>";

struct arguments {
    char *args[2]; /* arg1 & arg2 */
    bool version;
    char *output;
    char *scaling;
    char *input;
};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    /* Get the input argument from argp_parse, which we
       know is a pointer to our arguments structure. */
    struct arguments *arguments = (struct arguments *)state->input;

    switch (key) {
    case 'o':
        arguments->output = arg;
        break;
    case 'i':
        arguments->input = arg;
        break;
    case 's':
        arguments->scaling = arg;
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
    arguments.scaling = NULL;
    argp_parse(&argp, argc, argv, 0, 0, &arguments);

    double scaling;

    if (arguments.version) {
        std::cout << "vss2svg version: " << V2S_VERSION << "\n";
        return 0;
    }

    if (arguments.scaling == NULL) {
        scaling = 1.0;
    } else {
        scaling = atof(arguments.scaling);
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

    std::ifstream stin(arguments.input);
    if (!stin.is_open()) {
        std::cerr << "[ERROR] "
                  << "Impossible to open input file '" << arguments.input
                  << "'\n";
        return 1;
    }
    int ret = 0;
    std::string in((std::istreambuf_iterator<char>(stin)),
                   std::istreambuf_iterator<char>());

    visio2svg::Visio2Svg converter;
    std::unordered_map<std::string, std::string> out;

    ret = converter.vss2svg(in, out, scaling);

    std::string outputdir(arguments.output);
    #ifdef _WIN32
    mkdir(arguments.output);
#else
    mkdir(arguments.output, S_IRWXU);
#endif

    for (const auto &rule_pair : out) {
        ofstream myfile;
#ifdef SAFE_FILENAME
        std::regex e("[^A-Za-z0-9-]");
        std::basic_string<char> newfilename =
            outputdir + "/" + std::regex_replace(rule_pair.first, e, "_") +
            ".svg";
#else
        std::basic_string<char> newfilename =
            outputdir + rule_pair.first + ".svg";
#endif
        myfile.open(newfilename);
        myfile << rule_pair.second << std::endl;
        myfile.close();
        if (!myfile) {
            std::cerr << "[ERROR] "
                      << "Failed to write file '" << rule_pair.first << "'\n";
            ret = 1;
        }
    }

    return ret;
}
/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
