#ifndef DEFINITIONS
#define DEFINITIONS

#include "stable.h"
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat2x2.hpp>

typedef double          real;
typedef unsigned int    uint;
typedef glm::dvec2      vec2;
typedef glm::dvec3      vec3;
typedef glm::dmat2x2    m2x2;


//As typedefs must not include a template
//C++11 introduced a new way achieving it:
template <typename T>
using vector = typename std::vector<T>;

template <typename T>
using list = typename std::list<T>;

enum Type {
    QGraphicsItemType,
    GraphicsItemType,
    //All GraphicsItems go here...
    GraphicsPointType,
    GraphicsScheduleItemType,
    //...and all also GraphicsScheduleItems go here!
    GraphicsSplineType,
    GraphicsSplineGearType,
    GraphicsGearPairType,
    GraphicsInvoluteGearType,
    NumberOfTypes
};

#endif // DEFINITIONS
