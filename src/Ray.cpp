#include "Ray.h"
#include <Eigen/LU>
#include <Eigen/Dense>

Ray::Ray() {
}

Ray::Ray(vec2 origin, vec2 direction) : m_origin(origin), m_direction(direction) {
    if(m_direction == vec2(0,0));
        throw;
    m_direction.normalize();
}

const vec2& Ray::origin() const {
    return m_origin;
}

const vec2& Ray::direction() const {
    return m_direction;
}

void Ray::translateInDirection(real delta) {
    m_origin += (delta * m_direction);
}

//TODO: return a negative value instead of the max???
real Ray::distanceToPoint(vec2 &point) const {
    //Search the ray ray(t) = origin + t * direction for the shortest distance to point point
    //Shortest distance exists, where (point - ray(t1)) is vertical to direction
    // => ((origin + t1 * direction) - point) * direction = 0  as direction is normalized:
    // <=> t = (point - origin) * direction
    real t1 = (point - m_origin).dot(m_direction);
    if(t1 >= 0.0f)
        return ((m_origin + t1 * m_direction) - point).norm();
    else
        return  std::numeric_limits<real>::max();
}

bool Ray::intersect(vec2 startLine, vec2 endLine, vec2 &intersectionPoint) const {
    //Use quasi barycentric coordinates for the calculation.
    //If x and y were a basis, t would be the coordinates for z
    //As the direction vector only has an origin and no length, it is best to use
    //it as one of the base vectors, even if this would probably not be, what one
    //would do at a first glance. But otherwise we could only give a ratio of x and y
    //to express the direction vector. With this choice we get an intersection at the line
    //if it lies in positive direction of m_direction (=> t.y >= 0) and somewhere on the
    //line (=> 0 <= t.x <= 1)
    vec2 x = startLine - endLine;
    vec2 y = m_direction;
    vec2 z = startLine - m_origin;
    m2x2 matrix;
    matrix << x, y;
    if(matrix.determinant() != 0) {
        m2x2 invMatrix = matrix.inverse();
        vec2 t = invMatrix * z;
        if (0 <= t(0) && t(0) <= 1 && 0 <= t(1)) {
            intersectionPoint = m_origin + t(1) * y;
            return true;
        }
    }
    return false;
}
