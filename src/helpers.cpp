#include "helpers.h"
#include "definitions.h"
#include "graphics_objects/Point.h"

////////////////////////////////////
////////// STREAMING ///////////////

std::ostream& operator<<(std::ostream &os, const Point &point) {
    return os << "(" << point.pos().x() << ", " << point.pos().y() << ")";
}

std::ostream& operator<<(std::ostream &os, const QPointF &point) {
    return os << "(" << point.x() << ", " << point.y() << ")";
}

std::ostream& operator<<(std::ostream &os, const QRectF &rect) {
    return os << "" << rect.topLeft() << ", " << rect.bottomRight() << "";
}

std::ostream& operator <<(std::ostream &os, const vec2 vec) {
    return os << "[" << vec.x() << "| " << vec.y() << "]";
}

std::ostream& operator <<(std::ostream &os, const QVector<vec2> vector) {
    os << "[";
    int i = 0;
    for(; i < vector.size() - 1; ++i) {
        os << vector.at(i) << "|\t";
    }
    os << vector.at(i) << "]";
    return os;
}

std::ostream& operator <<(std::ostream &os, const vector<vec2> v) {
    os << "[";
    uint i;
    for(i = 0; i < v.size() - 1; ++i) {
        os << v[i] << "|\t";
    }
    os << v[i] << "]";
    return os;
}

std::ostream& operator <<(std::ostream &os, const QVector<real> vector) {
    for(int i = 0; i < vector.size(); ++i) {
        os << vector.at(i);
        if(i != vector.size() - 1)
            os << ", ";
    }
    return os;
}

std::ostream& operator <<(std::ostream &os, const vector<real> v) {
    for(uint i = 0; i < v.size(); ++i) {
        os << v[i];
        if(i != v.size() - 1)
            os << ", ";
    }
    return os;
}

std::ostream& operator <<(std::ostream &os, const Spline &spline) {
    os << "Control Points: ";
    const QVector<vec2> controlPoints = spline.controlPoints();
    foreach(vec2 point, controlPoints) {
        os << point << "    ";
    }
    os << "\n";
    os << "Knots: ";
    const QVector<real> knots = spline.knots();
    for(int i = 0; i < knots.size(); ++i) {
        os << knots.at(i);
        if(i != knots.size() - 1)
            os << ", ";
    }
    os << "\n";
    os << "TornToEdges? ";
    if(spline.isTornToEdges())
        os << "yes";
    else
        os << "no";
    os << "\n";
    os << "Degree: " << spline.degree();
    return os;
}

////////////////////////////////////
////////// OTHER ///////////////

QColor darkenColor(const QColor &color, int degree) {
    QColor darkenedColor;
    darkenedColor.setRed(color.red() < degree ? 0 : color.red() - degree);
    darkenedColor.setGreen(color.green() < degree ? 0 : color.green() - degree);
    darkenedColor.setBlue(color.blue() < degree ? 0 : color.blue() - degree);
    return darkenedColor;
}

real angleBetween(const vec2 &a, const vec2 &b) {
    //normally one should use abs(a.dot(b)), but this uses the std C abs(), which is only defined on integers and therefore causes wrong results.
    real angle = acos((a.dot(b)) / (a.norm() * b.norm()));
    return angle;
}
