/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */

#include <iostream>
#include <istream>
#include <librevenge/librevenge.h>
#include <libwmf/api.h>
#include <libwmf/ipa.h>
#include <libwmf/svg.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <string>
#include <unordered_map>

namespace visio2svg {

enum IMG_TYPE {
    UNKNOWN_IMGTYPE,
    EMF_IMGTYPE,
    WMF_IMGTYPE,
};

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
    int postTreatement(const librevenge::RVNGString *in,
                       const librevenge::RVNGString *name, char **out,
                       double scaling);
    int scale_title(xmlNode **root, xmlDocPtr *doc, double scaling,
                    const xmlChar *title, int title_len);
    int visio2svg(std::string &in,
                  std::unordered_map<std::string, std::string> &out,
                  double scaling, int mode);
};
}

/* vim:set shiftwidth=4 softtabstop=4 noexpandtab: */
