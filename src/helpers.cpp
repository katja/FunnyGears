#include "helpers.h"
#include "definitions.h"
#include "glm/glm.hpp"

////////////////////////////////////
////////// STREAMING ///////////////

std::ostream& operator<<(std::ostream &os, const QPointF &point) {
    return os << "(" << point.x() << ", " << point.y() << ")";
}

std::ostream& operator<<(std::ostream &os, const QRectF &rect) {
    return os << "" << rect.topLeft() << ", " << rect.bottomRight() << "";
}

std::ostream& operator <<(std::ostream &os, const vec2 vec) {
    return os << "[" << vec.x << "| " << vec.y << "]";
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
    os << "Degree: " << spline.degree();
    os << "\nControl Points: (" << spline.numberOfControlPoints() << " x)\n";
    const vector<vec2> controlPoints = spline.controlPoints();
    foreach(vec2 point, controlPoints) {
        os << point << "    ";
    }
    os << "\n";
    os << "Knots: (" << spline.numberOfKnots() << " x)\n";
    const vector<real> knots = spline.knots();
    for(uint i = 0; i < knots.size(); ++i) {
        os << knots.at(i);
        if(i != knots.size() - 1)
            os << ", ";
    }
    os << "\n";
    os << "Evaluable from " << spline.lowerDomainLimit() << " until " << spline.upperDomainLimit() << std::endl;
    os << "TornToEdges? ";
    if(spline.isTornToEdges())
        os << "yes";
    else
        os << "no";
    os << "\n";
    return os;
}

////////////////////////////////////
////////// OTHER ///////////////////

QColor darkenColor(const QColor &color, int degree) {
    QColor darkenedColor;
    darkenedColor.setRed(color.red() < degree ? 0 : color.red() - degree);
    darkenedColor.setGreen(color.green() < degree ? 0 : color.green() - degree);
    darkenedColor.setBlue(color.blue() < degree ? 0 : color.blue() - degree);
    return darkenedColor;
}

real angleBetween(const vec2 &a, const vec2 &b) {
    return acos(glm::dot(glm::normalize(a), glm::normalize(b)));
}

real absolute(const real &number) {
    if(number < 0.0)
        return -number;
    else
        return number;
}

real toRad(const real &degree) {
    return degree * M_PI / 180.0;
}

real toDegree(const real &rad) {
    return rad * 180.0 / M_PI;
}

QVector<QPointF> convertToQVectorWithQPointFs(const vector<vec2> &v) {
    QVector<QPointF> qV(v.size());
    for(uint i = 0; i < v.size(); ++i) {
        qV[i] = QPointF(v.at(i).x, v.at(i).y);
    }
    return qV;
};
