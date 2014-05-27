#ifndef HELPERS
#define HELPERS

#include "stable.h"
#include "definitions.h"
#include "graphics_objects/Point.h"
#include "basic_objects/Spline.h"

std::ostream& operator<<(std::ostream &os, const Point &point);

std::ostream& operator<<(std::ostream &os, const QPointF &point);

std::ostream& operator<<(std::ostream &os, const QRectF &rect);

std::ostream& operator <<(std::ostream &os, const vec2 vec);

std::ostream& operator <<(std::ostream &os, const QVector<vec2> vector);

std::ostream& operator <<(std::ostream &os, const QVector<real> vector);

std::ostream& operator <<(std::ostream &os, const Spline &spline);

QColor darkenColor(const QColor &color, int degree = 60);

#endif // HELPERS
