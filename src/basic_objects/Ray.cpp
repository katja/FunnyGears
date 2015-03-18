#include "basic_objects/Ray.h"
#include <glm/glm.hpp>

Ray::Ray() {
}

Ray::Ray(vec2 origin, vec2 direction) : m_origin(origin), m_direction(direction) {
    if(m_direction == vec2(0,0))
        throw;
    m_direction = glm::normalize(m_direction);
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

real Ray::distanceToPoint(vec2 &point) const {
    //Search the ray ray(t) = origin + t * direction for the shortest distance to point point
    //Shortest distance exists, where (point - ray(t)) is vertical to direction
    // => ((origin + t * direction) - point) * direction = 0
    // as direction is normalized:
    // <=> t = (point - origin) * direction
    real t = glm::dot((point - m_origin), m_direction);
    if(t >= 0.0)
        return glm::length((m_origin + t * m_direction) - point);
    else
        return t;
}

bool Ray::intersect(vec2 startLine, vec2 endLine, vec2 &intersectionPoint, real epsilon) const {
    //Use quasi barycentric coordinates for the calculation.
    //If x and y were a basis, t would be the coordinates for z
    //As the direction vector only has an origin and no length, it is best to use
    //it as one of the base vectors, even if this would probably not be, what one
    //would do at a first glance. But otherwise we could only give a ratio of x and y
    //to express the direction vector. With this choice we get an intersection at the line
    //if it lies in positive direction of m_direction (=> t.y >= 0) and somewhere on the
    //line (=> 0 <= t.x <= 1)

    //But first, move the origin value a little bit in the direction of the ray direction
    vec2 origin = m_origin + epsilon * m_direction;
    vec2 x = startLine - endLine;
    vec2 y = m_direction;
    vec2 z = startLine - origin;
    m2x2 matrix = m2x2(x, y);
    if(glm::determinant(matrix) != 0) {
        m2x2 invMatrix = glm::inverse(matrix);
        vec2 t = invMatrix * z;
        if(0 <= t.x && t.x <= 1 && 0 <= t.y) {
            intersectionPoint = origin + t.y * y;
            return true;
        }
    }
    return false;
}
