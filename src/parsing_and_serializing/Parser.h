#ifndef PARSER
#define PARSER

#include "stable.h"
#include "graphics_objects/GraphicsGearPair.h"
#include "graphics_objects/GraphicsSpline.h"
#include "graphics_objects/GraphicsSplineGear.h"
#include "basic_objects/GearPair.h"
#include "basic_objects/Spline.h"
#include "basic_objects/SplineGear.h"
#include "main_gui_objects/Model.h"


class Parser {

public:
    Parser() {}
    virtual ~Parser() {}

    bool applyModelWith(const QString &savedFGProjectText, Model *model) const;

private:
    bool parseFunnyGearsProject(
        Model *model,
        const QString::const_iterator &startScope,
        const QString::const_iterator &endScope) const;

    GraphicsScheduleItem* parseGraphicsScheduleItem(
        const QString::const_iterator &startScope,
        const QString::const_iterator &endScope) const;

    GraphicsSpline* parseGraphicsSpline(
        const QString::const_iterator &startScope,
        const QString::const_iterator &endScope) const;

    GraphicsSplineGear* parseGraphicsSplineGear(
        const QString::const_iterator &startScope,
        const QString::const_iterator &endScope) const;

    GraphicsGearPair* parseGraphicsGearPair(
        const QString::const_iterator &startScope,
        const QString::const_iterator &endScope) const;

    Spline* parseSpline(
        const QString::const_iterator &startScope,
        const QString::const_iterator &endScope) const;

    SplineGear* parseSplineGear(
        const QString::const_iterator &startScope,
        const QString::const_iterator &endScope) const;

    GearPair* parseGearPair(
        const QString::const_iterator &startScope,
        const QString::const_iterator &endScope) const;

    vector<vec2> parseControlPoints(
        const QString::const_iterator &startScope,
        const QString::const_iterator &endScope) const;

    bool parseBool(
        const QString::const_iterator &startScope,
        const QString::const_iterator &endScope) const;

    bool parseBool(const QString &text) const;

    uint parseUInt(const QString &text) const;

    real parseReal(const QString &text) const;

    QColor parseColor(
        const QString::const_iterator &startScope,
        const QString::const_iterator &endScope) const;

    QString stepToNextSpace(
        QString::const_iterator &fromIterator,
        const QString::const_iterator &endScope) const;

    /** Searches for the next occurence of a symbol – like "<visibility>" or "</visibility>" – and returns
     *  the symbol name. So it would f. ex. return "visibility". If the end of savedFGProjectText
     *  is reached while scanning for the next symbol, an empty QString is returned.
     */
    QString findNextSymbol(
        QString::const_iterator &startScope,
        QString::const_iterator &endScope,
        QString::const_iterator &nextScope) const;

    int nextOccurenceOf(
        const QString &text,
        const QString::const_iterator &startIterator,
        const QString::const_iterator &endIterator) const;

    bool startsWith(
        const QString &text,
        const QString::const_iterator &it) const;

    bool nothingLeftBetween(
        const QString::const_iterator &a,
        const QString::const_iterator &b) const;

    QString innerText(const QString::const_iterator &a, const QString::const_iterator &b) const;

};

#endif // PARSER
