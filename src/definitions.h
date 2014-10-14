#ifndef DEFINITIONS
#define DEFINITIONS

#include "stable.h"
#include <Eigen/Core>

typedef double          real;
typedef unsigned int    uint;
typedef Eigen::Vector2d vec2;
typedef Eigen::Vector3d vec3;
typedef Eigen::Matrix2d m2x2;


//As typedefs must not include a template
//C++11 introduced a new way achieving it:
template <typename T>
using vector = typename std::vector<T>;

enum Type {
    QGraphicsItemType,
    GraphicsItemType,
    GraphicsSplineType,
    GraphicsSimpleGearType,
    GraphicsInvoluteGearType,
    NumberOfTypes
};

#endif // DEFINITIONS
