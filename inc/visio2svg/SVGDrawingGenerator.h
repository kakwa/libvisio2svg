/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */

#include <librevenge/librevenge-api.h>

#include <librevenge/RVNGDrawingInterface.h>
#include <librevenge/RVNGStringVector.h>
#include <librevenge/RVNGSVGDrawingGenerator.h>

namespace visio2svg {

class REVENGE_API SVGDrawingGenerator
    : public librevenge::RVNGSVGDrawingGenerator {
public:
	void drawGraphicObject(
			    const librevenge::RVNGPropertyList &propList);
};
}

/* vim:set shiftwidth=4 softtabstop=4 noexpandtab: */
