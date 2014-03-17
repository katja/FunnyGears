#ifndef HELPERS
#define HELPERS

#include "stable.h"
#include "definitions.h"
#include "Point.h"

std::ostream& operator<<(std::ostream &os, const Point &point);

std::ostream& operator<<(std::ostream &os, const QPointF &point);

std::ostream& operator<<(std::ostream &os, const QRectF &rect);

std::ostream& operator <<(std::ostream &os, const vec2 vec);

std::ostream& operator <<(std::ostream &os, const QVector<vec2> vector);

#endif // HELPERS
