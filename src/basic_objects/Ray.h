#ifndef RAY
#define RAY

#include "definitions.h"

class Ray {

public:
    Ray();

    /**
     * @brief Ray constructor setting origin and direction of the ray and normalizes the direction.
     * @param origin origin of the ray.
     * @param direction direction of the ray. The direction must have a positive length or the constructor throws an exception.
     */
    Ray(vec2 origin, vec2 direction);

    /**
     * @brief gets origin of the ray.
     */
    const vec2& origin() const;

    /**
     * @brief gets direction of the ray.
     */
    const vec2& direction() const;

    /**
     * @brief translateInDirection moves the origin along the direction vector.
     * @param delta the length by which the origin of the ray is translated.
     */
    void translateInDirection(real delta);

    /**
     * @brief Calculates shortest distance between point and ray.
     */
    real distanceToPoint(vec2 &point) const;

    /**
     * @brief examines Ray for intersection with a line in 2D. Therefore Ray is reduced to 2D by an orthogonal projection on xy plane
     * @param startLine the start point of the line to be examined for intersection with ray
     * @param endLine the end point of the line to be examined for intersection with ray
     * @param intersectionPoint if an intersection is found, found intersection point is written in intersectionPoint. If none is found, intersectionPoint isn't touched.
     * @return true if ray intersects the line, false otherwise
     */
    bool intersect(vec2 startLine, vec2 endLine, vec2& intersectionPoint, real epsilon = 0.0001) const;

private:
    vec2 m_origin;
    vec2 m_direction; // Normalized vector pointing in the direction of the ray.

};


#endif // RAY
