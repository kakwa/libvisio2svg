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
#include "visio2svg/TitleGenerator.h"

namespace visio2svg {

void TitleGenerator::startPage(const librevenge::RVNGPropertyList &propList) {
    titles.append(propList["draw:name"]->getStr().cstr());
}
void TitleGenerator::startDocument(
    const librevenge::RVNGPropertyList &propList){};
void TitleGenerator::endDocument(){};
void TitleGenerator::setDocumentMetaData(
    const librevenge::RVNGPropertyList &propList){};
void TitleGenerator::defineEmbeddedFont(
    const librevenge::RVNGPropertyList &propList){};
void TitleGenerator::endPage(){};
void TitleGenerator::startMasterPage(
    const librevenge::RVNGPropertyList &propList){};
void TitleGenerator::endMasterPage(){};
void TitleGenerator::startLayer(const librevenge::RVNGPropertyList &propList){};
void TitleGenerator::endLayer(){};
void TitleGenerator::startEmbeddedGraphics(
    const librevenge::RVNGPropertyList &propList){};
void TitleGenerator::endEmbeddedGraphics(){};

void TitleGenerator::openGroup(const librevenge::RVNGPropertyList &propList){};
void TitleGenerator::closeGroup(){};

void TitleGenerator::setStyle(const librevenge::RVNGPropertyList &propList){};

void TitleGenerator::drawRectangle(
    const librevenge::RVNGPropertyList &propList){};
void TitleGenerator::drawEllipse(
    const librevenge::RVNGPropertyList &propList){};
void TitleGenerator::drawPolyline(
    const librevenge::RVNGPropertyList &propList){};
void TitleGenerator::drawPolygon(
    const librevenge::RVNGPropertyList &propList){};
void TitleGenerator::drawPath(const librevenge::RVNGPropertyList &propList){};
void TitleGenerator::drawGraphicObject(
    const librevenge::RVNGPropertyList &propList){};
void TitleGenerator::drawConnector(
    const librevenge::RVNGPropertyList &propList){};
void TitleGenerator::startTextObject(
    const librevenge::RVNGPropertyList &propList){};
void TitleGenerator::endTextObject(){};

void TitleGenerator::startTableObject(
    const librevenge::RVNGPropertyList &propList){};
void TitleGenerator::openTableRow(
    const librevenge::RVNGPropertyList &propList){};
void TitleGenerator::closeTableRow(){};
void TitleGenerator::openTableCell(
    const librevenge::RVNGPropertyList &propList){};
void TitleGenerator::closeTableCell(){};
void TitleGenerator::insertCoveredTableCell(
    const librevenge::RVNGPropertyList &propList){};
void TitleGenerator::endTableObject(){};

void TitleGenerator::openOrderedListLevel(
    const librevenge::RVNGPropertyList &propList){};
void TitleGenerator::closeOrderedListLevel(){};

void TitleGenerator::openUnorderedListLevel(
    const librevenge::RVNGPropertyList &propList){};
void TitleGenerator::closeUnorderedListLevel(){};
void TitleGenerator::openListElement(
    const librevenge::RVNGPropertyList &propList){};
void TitleGenerator::closeListElement(){};

void TitleGenerator::defineParagraphStyle(
    const librevenge::RVNGPropertyList &propList){};
void TitleGenerator::openParagraph(
    const librevenge::RVNGPropertyList &propList){};
void TitleGenerator::closeParagraph(){};

void TitleGenerator::defineCharacterStyle(
    const librevenge::RVNGPropertyList &propList){};
void TitleGenerator::openSpan(const librevenge::RVNGPropertyList &propList){};
void TitleGenerator::closeSpan(){};

void TitleGenerator::openLink(const librevenge::RVNGPropertyList &propList){};
void TitleGenerator::closeLink(){};

void TitleGenerator::insertTab(){};
void TitleGenerator::insertSpace(){};
void TitleGenerator::insertText(const librevenge::RVNGString &text){};
void TitleGenerator::insertLineBreak(){};
void TitleGenerator::insertField(
    const librevenge::RVNGPropertyList &propList){};

TitleGenerator::TitleGenerator(librevenge::RVNGStringVector &titles)
    : titles(titles) {
}

TitleGenerator::~TitleGenerator() {
}
}

/* vim:set shiftwidth=4 softtabstop=4 noexpandtab: */
