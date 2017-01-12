/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */

#include <librevenge/librevenge-api.h>

#include <librevenge-generators/librevenge-generators.h>
#include <librevenge-stream/librevenge-stream.h>
#include <librevenge/RVNGBinaryData.h>
#include <librevenge/RVNGDrawingInterface.h>
#include <librevenge/RVNGStringVector.h>
#include <librevenge/librevenge.h>

namespace visio2svg {

class REVENGE_API TitleGenerator : public librevenge::RVNGDrawingInterface {
  public:
    TitleGenerator(librevenge::RVNGStringVector &titles);
    ~TitleGenerator();
    void startPage(const librevenge::RVNGPropertyList &propList);
    void startDocument(const librevenge::RVNGPropertyList &propList);
    void endDocument();
    void setDocumentMetaData(const librevenge::RVNGPropertyList &propList);
    void defineEmbeddedFont(const librevenge::RVNGPropertyList &propList);
    void endPage();
    void startMasterPage(const librevenge::RVNGPropertyList &propList);
    void endMasterPage();
    void startLayer(const librevenge::RVNGPropertyList &propList);
    void endLayer();
    void startEmbeddedGraphics(const librevenge::RVNGPropertyList &propList);
    void endEmbeddedGraphics();

    void openGroup(const librevenge::RVNGPropertyList &propList);
    void closeGroup();

    void setStyle(const librevenge::RVNGPropertyList &propList);

    void drawRectangle(const librevenge::RVNGPropertyList &propList);
    void drawEllipse(const librevenge::RVNGPropertyList &propList);
    void drawPolyline(const librevenge::RVNGPropertyList &propList);
    void drawPolygon(const librevenge::RVNGPropertyList &propList);
    void drawPath(const librevenge::RVNGPropertyList &propList);
    void drawGraphicObject(const librevenge::RVNGPropertyList &propList);
    void drawConnector(const librevenge::RVNGPropertyList &propList);
    void startTextObject(const librevenge::RVNGPropertyList &propList);
    void endTextObject();

    void startTableObject(const librevenge::RVNGPropertyList &propList);
    void openTableRow(const librevenge::RVNGPropertyList &propList);
    void closeTableRow();
    void openTableCell(const librevenge::RVNGPropertyList &propList);
    void closeTableCell();
    void insertCoveredTableCell(const librevenge::RVNGPropertyList &propList);
    void endTableObject();

    void openOrderedListLevel(const librevenge::RVNGPropertyList &propList);
    void closeOrderedListLevel();

    void openUnorderedListLevel(const librevenge::RVNGPropertyList &propList);
    void closeUnorderedListLevel();
    void openListElement(const librevenge::RVNGPropertyList &propList);
    void closeListElement();

    void defineParagraphStyle(const librevenge::RVNGPropertyList &propList);
    void openParagraph(const librevenge::RVNGPropertyList &propList);
    void closeParagraph();

    void defineCharacterStyle(const librevenge::RVNGPropertyList &propList);
    void openSpan(const librevenge::RVNGPropertyList &propList);
    void closeSpan();

    void openLink(const librevenge::RVNGPropertyList &propList);
    void closeLink();

    void insertTab();
    void insertSpace();
    void insertText(const librevenge::RVNGString &text);
    void insertLineBreak();
    void insertField(const librevenge::RVNGPropertyList &propList);

  private:
    librevenge::RVNGStringVector &titles;
};
}

/* vim:set shiftwidth=4 softtabstop=4 noexpandtab: */
