/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */

#include <librevenge/librevenge.h>
#include <unordered_map>
#include <istream>
#include <iostream>
#include <string>
#include <libxml/parser.h>
#include <libxml/tree.h>

namespace visio2svg {

class Visio2Svg {
  public:
    Visio2Svg();
    ~Visio2Svg();
    int vss2svg(std::string &in,
                std::unordered_map<std::string, std::string> &out);
    int vsd2svg(std::string &in,
                std::unordered_map<std::string, std::string> &out);
    int vss2svg(std::string &in,
                std::unordered_map<std::string, std::string> &out,
                double scaling);
    int vsd2svg(std::string &in,
                std::unordered_map<std::string, std::string> &out,
                double scaling);

  private:
    void postTreatement(const librevenge::RVNGString *in,
                        const librevenge::RVNGString *name, char **out,
                        double scaling);
    void scale_title(xmlNode **root, xmlDocPtr *doc, double scaling, const xmlChar *title);
    int visio2svg(std::string &in,
                  std::unordered_map<std::string, std::string> &out,
                  double scaling, int mode);
};
}

/* vim:set shiftwidth=4 softtabstop=4 noexpandtab: */
