#ifndef HELPERS
#define HELPERS

std::ostream& operator<<(std::ostream& os, const Point& point) {
    return os << "(" << point.pos().x() << ", " << point.pos().y() << ")";
}

std::ostream& operator<<(std::ostream& os, const QPointF& point) {
    return os << "(" << point.x() << ", " << point.y() << ")";
}

std::ostream& operator<<(std::ostream& os, const QRectF& rect) {
    return os << "" << rect.topLeft() << ", " << rect.bottomRight() << "";
}

#endif // HELPERS
