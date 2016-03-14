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
#include "visio2svg/TextGenerator.h"

namespace visio2svg {

void TextGenerator::startPage(const librevenge::RVNGPropertyList &propList) {
}
void TextGenerator::startDocument(
    const librevenge::RVNGPropertyList &propList){};
void TextGenerator::endDocument(){};
void TextGenerator::setDocumentMetaData(
    const librevenge::RVNGPropertyList &propList){};
void TextGenerator::defineEmbeddedFont(
    const librevenge::RVNGPropertyList &propList){};
void TextGenerator::endPage(){};
void TextGenerator::startMasterPage(
    const librevenge::RVNGPropertyList &propList){};
void TextGenerator::endMasterPage(){};
void TextGenerator::startLayer(const librevenge::RVNGPropertyList &propList){};
void TextGenerator::endLayer(){};
void TextGenerator::startEmbeddedGraphics(
    const librevenge::RVNGPropertyList &propList){};
void TextGenerator::endEmbeddedGraphics(){};

void TextGenerator::openGroup(const librevenge::RVNGPropertyList &propList){};
void TextGenerator::closeGroup(){};

void TextGenerator::setStyle(const librevenge::RVNGPropertyList &propList){};

void TextGenerator::drawRectangle(
    const librevenge::RVNGPropertyList &propList){};
void TextGenerator::drawEllipse(const librevenge::RVNGPropertyList &propList){};
void TextGenerator::drawPolyline(
    const librevenge::RVNGPropertyList &propList){};
void TextGenerator::drawPolygon(const librevenge::RVNGPropertyList &propList){};
void TextGenerator::drawPath(const librevenge::RVNGPropertyList &propList){};
void TextGenerator::drawGraphicObject(
    const librevenge::RVNGPropertyList &propList){};
void TextGenerator::drawConnector(
    const librevenge::RVNGPropertyList &propList){};
void TextGenerator::startTextObject(
    const librevenge::RVNGPropertyList &propList){};
void TextGenerator::endTextObject(){};

void TextGenerator::startTableObject(
    const librevenge::RVNGPropertyList &propList){};
void TextGenerator::openTableRow(
    const librevenge::RVNGPropertyList &propList){};
void TextGenerator::closeTableRow(){};
void TextGenerator::openTableCell(
    const librevenge::RVNGPropertyList &propList){};
void TextGenerator::closeTableCell(){};
void TextGenerator::insertCoveredTableCell(
    const librevenge::RVNGPropertyList &propList){};
void TextGenerator::endTableObject(){};

void TextGenerator::openOrderedListLevel(
    const librevenge::RVNGPropertyList &propList){};
void TextGenerator::closeOrderedListLevel(){};

void TextGenerator::openUnorderedListLevel(
    const librevenge::RVNGPropertyList &propList){};
void TextGenerator::closeUnorderedListLevel(){};
void TextGenerator::openListElement(
    const librevenge::RVNGPropertyList &propList){};
void TextGenerator::closeListElement(){};

void TextGenerator::defineParagraphStyle(
    const librevenge::RVNGPropertyList &propList){};
void TextGenerator::openParagraph(
    const librevenge::RVNGPropertyList &propList){};
void TextGenerator::closeParagraph(){};

void TextGenerator::defineCharacterStyle(
    const librevenge::RVNGPropertyList &propList){};
void TextGenerator::openSpan(const librevenge::RVNGPropertyList &propList){};
void TextGenerator::closeSpan(){};

void TextGenerator::openLink(const librevenge::RVNGPropertyList &propList){};
void TextGenerator::closeLink(){};

void TextGenerator::insertTab(){};
void TextGenerator::insertSpace(){};
void TextGenerator::insertText(const librevenge::RVNGString &text){};
void TextGenerator::insertLineBreak(){};
void TextGenerator::insertField(const librevenge::RVNGPropertyList &propList){};

TextGenerator::TextGenerator(std::list<librevenge::RVNGStringVector> &text)
    : text(text) {
}

TextGenerator::~TextGenerator() {
}
}

/* vim:set shiftwidth=4 softtabstop=4 noexpandtab: */
