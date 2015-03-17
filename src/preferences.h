#ifndef PREFERENCES
#define PREFERENCES

#include "stable.h"

namespace Preferences {
    extern QColor SelectionColor;
    extern QColor HoverColor;
    extern QColor AttentionColor;
    extern QColor BackgroundGridColor;
    extern QColor CoordinateAxisColor;

    extern qreal SimpleLineWidth;
    extern qreal HighlightedLineWidth;

    extern qreal PointRadius;
    extern qreal SmallPointRadius;

    extern qreal TangentLength;

    extern qreal AngularPitchStrokesLength;

    extern qreal ZoomDefaultX;
    extern qreal ZoomDefaultY;
}

#endif // PREFERENCES
