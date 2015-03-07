#ifndef CONTACT_POINT
#define CONTACT_POINT

#include "definitions.h"
#include "helpers.h"
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>

enum class ErrorCode {
    NO_ERROR,
    NO_CUT_WITH_REFERENCE_RADIUS,
    NO_THICKNESS
};

struct ContactPoint {
    ContactPoint() : isCovered(false), isRotated(false), error(ErrorCode::NO_ERROR) {}
    ContactPoint(const ContactPoint &other) :
        evaluationValue(other.evaluationValue),
        evaluationStep(other.evaluationStep),
        point(other.point),
        normal(other.normal),
        originPoint(other.originPoint),
        originNormal(other.originNormal),
        contactPosition(other.contactPosition),
        normalInContact(other.normalInContact),
        forbiddenAreaLength(other.forbiddenAreaLength),
        forbiddenAreaEndPoint(other.forbiddenAreaEndPoint),
        usedLargerValue(other.usedLargerValue),
        isCovered(other.isCovered),
        isRotated(other.isRotated),
        error(other.error)
    {}

    real evaluationValue; //Spline curve of driving gear was evaluated at this value to get this ContactPoint
    uint evaluationStep; //Evaluation number of ContactPoint
    vec2 point; //Point on driven gear
    vec2 normal; //Must be normalized! normal on driven gear
    vec2 originPoint; //Point on driving gear
    vec2 originNormal; //Must be normalized! Normal on driving gear
    vec2 contactPosition; //Position where contact took place
    vec2 normalInContact; //Must be normalized! Normal of driving gear in contact point
    real forbiddenAreaLength; //Thickness of the driving gear
    vec2 forbiddenAreaEndPoint; //Opposite point of driving gear transferred to driven gear
    bool usedLargerValue; // In most cases the normal of the driving gear has two cuts with reference radius => originPoint + t * originNormal => if maximum possible value is taken, this attribute is set to true
    bool isCovered; //is set to true, when this point can't become a contact point
    bool isRotated;
    ErrorCode error;

    void rotate(real rotation) {
        if(error != ErrorCode::NO_CUT_WITH_REFERENCE_RADIUS) {
            point = glm::rotate(point, rotation);
            normal = glm::rotate(normal, rotation);
            if(error != ErrorCode::NO_THICKNESS) {
                forbiddenAreaEndPoint = glm::rotate(forbiddenAreaEndPoint, rotation);
            }
            isRotated = true;
        }
    }
};

struct NoneContactPoint : public ContactPoint {
    NoneContactPoint() {
        error = ErrorCode::NO_CUT_WITH_REFERENCE_RADIUS;
    }
    NoneContactPoint(const ContactPoint &cp) {
        evaluationValue = cp.evaluationValue;
        evaluationStep = cp.evaluationStep;
        point = vec2(0, 0); //is not possible, when no cut with reference radius is available
        normal = vec2(0, 0); //is not possible, when no cut with reference radius is available
        originPoint = cp.originPoint;
        originNormal = cp.originNormal;
        contactPosition = vec2(0, 0);
        normalInContact = vec2(0, 0);
        forbiddenAreaLength = cp.forbiddenAreaLength;
        forbiddenAreaEndPoint = vec2(0, 0);
        usedLargerValue = true; //is not possible, when no cut with reference radius is available
        isCovered = cp.isCovered;
        isRotated = cp.isRotated;
        error = ErrorCode::NO_CUT_WITH_REFERENCE_RADIUS;
    }
    NoneContactPoint(const NoneContactPoint &other) : ContactPoint(other),
        points(other.points),
        normals(other.normals)
    {}
    vector<vec2> points;
    vector<vec2> normals;

    void rotate(real rotation) {
        for(uint i = 0; i < points.size(); ++i) {
            points[i] = glm::rotate(points[i], rotation);
            normals[i] = glm::rotate(normals[i], rotation);
        }
        isRotated = true;
    }

};


#endif //CONTACT_POINT
