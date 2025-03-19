Libvisio2svg
============

[![Build Status](https://travis-ci.org/kakwa/libvisio2svg.svg?branch=master)](https://travis-ci.org/kakwa/libvisio2svg)


Library/Utilities to convert Microsoft (MS) Visio Documents and Stencils (VSS and VSD) to SVG.

Motivation
==========

There are tons of publicly available MS Visio stencils, for example 
[the Cisco ones](http://www.cisco.com/c/en/us/products/visio-stencil-listing.html) 
or the stencils from [VisioCafe](http://www.visiocafe.com/).
This library and utilities were created to be able to convert these stencils to SVG
and reuse them outside of Microsoft Visio, in programs like 
[yEd](https://www.yworks.com/products/yed), [Inkscape](https://inkscape.org), 
[Dia](http://dia-installer.de/), [Calligra Flow](https://www.calligra.org/flow/)...

This library is mainly the glue between [librevenge](http://sourceforge.net/p/libwpd/wiki/librevenge/)/[libvisio](https://github.com/LibreOffice/libvisio), [libemf2svg](https://github.com/kakwa/libemf2svg) and [libwmf](http://wvware.sourceforge.net/libwmf.html).

About libemf2svg
----------------

[libemf2svg](https://github.com/kakwa/libemf2svg) is another library of mine.
It was developed to handle MS EMF (Enhanced Metafile) blobs which constitute most of Visio VSS shapes.

Librevenge/Libvisio would otherwise only dump the EMF blob as base64 in an \<image\> SVG tag, which most viewer/editor/browser
would be unable to display.

License
=======

Libvisio2svg is licensed under GPLv2.

Dependencies
============

* [librevenge](http://sourceforge.net/p/libwpd/wiki/librevenge/)
* [libvisio](https://github.com/LibreOffice/libvisio)
* [libemf2svg](https://github.com/kakwa/libemf2svg)
* [libxml2](http://www.xmlsoft.org/)
* [libwmf](http://wvware.sourceforge.net/libwmf.html)
* [fonts from the ghostscript project](https://www.ghostscript.com/) (libwmf requirement, usually the distribution package is named `gsfonts`)

Building
========

Commands to build this project:

```bash
# CMAKE_INSTALL_PREFIX is optional, default is /usr/local/
$ cmake . -DCMAKE_INSTALL_PREFIX=/usr/

# compilation
$ make

# installation
$ make install
```

Regex
-----

The vss2svg and vsd2svg utilities rely on regular expressions to construct safe file names from stencils/sheets names.
It will replace anything that is not matching **[A-Za-z0-9-]** by an underscore.

This functionality relies on regexp from libstdc++ (c++11 standard), but older libraries doesn't support regex.

Yet you can compile without this file name sanitization:

```
$ cmake . -DUNSAFE_FILENAME=ON
$ make
```

However, be cautious with the stencil/sheet names, otherwise some files might be written outside the output directory.

Usage
=====

Command Line
------------

Convert VSS:

```bash
# conversion
$ vss2svg-conv -i ./2960CX.vss -o ./out/ -s 4.5

$ ls out/
'Cisco R42610 Front.svg'      'WS-C2960CX-8PC-L Rear.svg'   'WS-C2960CX-8TC-L Rear.svg'
'WS-C2960CX-8PC-L Front.svg'  'WS-C2960CX-8TC-L Front.svg'

# help
$ vss2svg-conv --help
```

Convert VSD:

```bash
# conversion
$ vsd2svg-conv -i ./my.VSD -o ./out/ -s 7

$ ls out/
Page-1.svg  Page-2.svg  Page-3.svg  Page-4.svg

# output to screen
$ vsd2svg-conv -i ./my.VSD

# help
$ vsd2svg-conv --help
```

Library
=======

Convert Visio Documents
-----------------------

example:
```cpp
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <visio2svg/Visio2Svg.h>

int main(int argc, char **argv) {
    // Put visio file in an std::string
    // (no error checking, should be done in actual code)
    std::ifstream stin(argv[1]);

    std::string visio_in((std::istreambuf_iterator<char>(stin)),
                         std::istreambuf_iterator<char>());

    // Initialize converter and output map
    visio2svg::Visio2Svg converter;
    std::unordered_map<std::string, std::string> out;

    // return code
    int ret = 0;

    // Pick one of the following

    // Convert vsd documents
    ret = converter.vsd2svg(visio_in, out);

    // Convert vss (Stencils) documents
    ret = converter.vss2svg(visio_in, out);

    // or with rescaling
    ret = converter.vsd2svg(visio_in, out, 4.5);
    ret = converter.vss2svg(visio_in, out, 4.5);

    if (ret)
        std::cerr << "Conversion errors occured"
                  << "\n";

    // Do something with the output
    for (const auto &rule_pair : out) {
        std::cout << "Sheet Title: " << rule_pair.first << std::endl;
        std::cout << rule_pair.second << std::endl;
    }
}
```

Recover symbol titles
---------------------

This library also comes with a RVNG generator to recover symbol titles:

```cpp
#include <iostream>
#include <librevenge-generators/librevenge-generators.h>
#include <librevenge-stream/librevenge-stream.h>
#include <librevenge/librevenge.h>
#include <libvisio/libvisio.h>
#include <visio2svg/TitleGenerator.h>

int main(int argc, char **argv) {
    // put the Visio document in an RVNGFileStream 
    // (no error checking, should be done in actual code)
    librevenge::RVNGFileStream input(argv[1]);

    /* String stream version (need to be set)
    librevenge::RVNGStringStream input;
    */

    // Recover Titles of each sheets
    librevenge::RVNGStringVector output_names;
    visio2svg::TitleGenerator generator_names(output_names);

    bool ret = libvisio::VisioDocument::parseStencils(&input, &generator_names);
    /* or this for vsd
    ret = libvisio::VisioDocument::parse(&input, &generator_names);
    */

    if (!ret || output_names.empty()) {
        std::cerr << "ERROR: Failed to recover sheets titles failed!"
                  << std::endl;
        return 1;
    }

    // do something with the names
    for (unsigned k = 0; k < output_names.size(); ++k) {
        std::cout << "Title of stencil " << std::to_string(k) << std::endl;
        if (output_names[k].empty()) {
            std::cout << "no title set in stencil number " << std::to_string(k)
                      << std::endl;
        } else {
            std::cout << output_names[k].cstr() << std::endl;
        }
    }
}
```

Changelog
=========

0.5.5
-----

* add LIB_INSTALL_DIR variable in cmake

0.5.4
-----

* fix freetype discovery in cmake

0.5.3
-----

* fix segfault if shape doesn't contain title

0.5.2
-----

* fix error in setting title field with titles containing "&"
* add XML_PARSE_HUGE in xml parsing to allow big image blobs.
* better precision for image dimensions (keep 10 decimals)

0.5.1
-----

* add support for OSX

0.5.0
-----

* adding support to convert wmf blob (using libwmf)
* requires new libraries as dependencies: libwmf and freetype)
* requires libemf2svg version >= 1.0.0
* cleaner and more explicit dependencies resolving in cmake

0.4.1
-----

* adding safeguard regarding unsafe output file paths

0.3.0
-----

* better error handling

0.2.0
-----

* add rescaling option + title

0.1.0
-----

* first release
