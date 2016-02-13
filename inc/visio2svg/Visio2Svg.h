/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */

#include <librevenge/librevenge.h>
#include <unordered_map>
#include <istream>
#include <iostream>
#include <string>

namespace visio2svg {

class Visio2Svg {
  public:
    Visio2Svg();
    ~Visio2Svg();
    int vss2svg(std::string &in,
                std::unordered_map<std::string, std::string> &out);
    int vsd2svg(std::string &in,
                std::unordered_map<std::string, std::string> &out);

  private:
    void postTreatement(const librevenge::RVNGString *in,
                        const librevenge::RVNGString *name, char **out);
    int visio2svg(std::string &in,
                  std::unordered_map<std::string, std::string> &out,
                  double scaling, int mode);
};
}

/* vim:set shiftwidth=4 softtabstop=4 noexpandtab: */
