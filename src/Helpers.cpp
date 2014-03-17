#include "Helpers.h"
#include "definitions.h"
#include "Point.h"

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
