#ifndef CONTACT_POINT
#define CONTACT_POINT

#include "definitions.h"
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>

enum class ErrorCode {
    NO_ERROR,
    NO_CUT_WITH_REFERENCE_RADIUS,
    NO_THICKNESS
};

struct ContactPoint {
    vec2 point; //Point on driven gear
    vec2 normal; //Must be normalized! normal on driven gear
    vec2 originPoint; //Point on driving gear
    vec2 originNormal; //Must be normalized! Normal on driving gear
    vec2 contactPosition; //Position where contact took place
    vec2 normalInContact; //Must be normalized! Normal of driving gear in contact point
    real forbiddenAreaLength; //Thickness of the driving gear
    vec2 forbiddenAreaEndPoint; //Opposite point of driving gear transferred to driven gear
    bool isCovered; //is set to true, when this point can't become a contact point
    ErrorCode error;

    void rotate(real rotation) {
        if(error != ErrorCode::NO_CUT_WITH_REFERENCE_RADIUS) {
            point = glm::rotate(point, rotation);
            normal = glm::rotate(normal, rotation);
            if(error != ErrorCode::NO_THICKNESS) {
                forbiddenAreaEndPoint = glm::rotate(forbiddenAreaEndPoint, rotation);
            }
        }
    }
};


#endif //CONTACT_POINT
