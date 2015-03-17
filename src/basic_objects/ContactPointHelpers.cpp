#include "basic_objects/ContactPointHelpers.h"

TurningDirection turningDirectionOf(real turningVelocity) {
    if(turningVelocity < 0)
        return TurningDirection::CounterClockwise;
    else
        return TurningDirection::Clockwise;
}
