#ifndef HELPERS
#define HELPERS

#include "stable.h"
#include "definitions.h"
#include "graphics_objects/Point.h"
#include "basic_objects/Spline.h"

////////////////////////////////////
////////// STREAMING ///////////////

std::ostream& operator <<(std::ostream &os, const Point &point);

std::ostream& operator <<(std::ostream &os, const QPointF &point);

std::ostream& operator <<(std::ostream &os, const QRectF &rect);

std::ostream& operator <<(std::ostream &os, const vec2 vec);

std::ostream& operator <<(std::ostream &os, const QVector<vec2> v);

std::ostream& operator <<(std::ostream &os, const vector<vec2> v);

std::ostream& operator <<(std::ostream &os, const QVector<real> v);

std::ostream& operator <<(std::ostream &os, const vector<real> v);

std::ostream& operator <<(std::ostream &os, const Spline &spline);

////////////////////////////////////
////////// OTHER ///////////////////

QColor darkenColor(const QColor &color, int degree = 60);

real angleBetween(const vec2 &a, const vec2 &b);

#endif // HELPERS
