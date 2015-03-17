#ifndef CONTACT_POINT
#define CONTACT_POINT

#include "definitions.h"
// #include "helpers.h"
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>

enum class ErrorCode {
    NO_ERROR,
    NO_CUT_WITH_REFERENCE_RADIUS
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
            forbiddenAreaEndPoint = glm::rotate(forbiddenAreaEndPoint, rotation);
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
        point = vec2(0, 0); //is possible, but results in a brake of the basic requirement of a gear tooth system (Verzahnungsgesetz) – keine winkeltreue Übersetzung!
        normal = vec2(0, 0); //is possible, but results in a brake of the basic requirement of a gear tooth system (Verzahnungsgesetz) – keine winkeltreue Übersetzung!
        originPoint = cp.originPoint;
        originNormal = cp.originNormal;
        contactPosition = vec2(0, 0);
        normalInContact = vec2(0, 0);
        forbiddenAreaLength = cp.forbiddenAreaLength;
        forbiddenAreaEndPoint = vec2(0, 0);
        usedLargerValue = true; //is not possible, when no cut with reference radius is available
        examinedIndex = -1;
        isCovered = cp.isCovered;
        isRotated = cp.isRotated;
        error = ErrorCode::NO_CUT_WITH_REFERENCE_RADIUS;
    }
    NoneContactPoint(const NoneContactPoint &other) : ContactPoint(other),
        points(other.points),
        normals(other.normals),
        contactPositions(other.contactPositions),
        normalsInContact(other.normalsInContact),
        examinedIndex(other.examinedIndex)
    {}
    vector<vec2> points; // points, which when chosen for the gear outline, do brake the basic requirement of a gear tooth system!
    vector<vec2> normals; // normals to points above
    vector<vec2> contactPositions; // would be the contact positions of points
    vector<vec2> normalsInContact; // would be the normal in the contact – would NOT go through pitch point!
    int examinedIndex; // is possibility to specify a specific position of the NoneContactPoint => points[examinedIndex], normals[examinedIndex], contactPositions[examinedIndex], normalsInContact[examinedIndex]

    void rotate(real rotation) {
        for(uint i = 0; i < points.size(); ++i) {
            points[i] = glm::rotate(points[i], rotation);
            normals[i] = glm::rotate(normals[i], rotation);
        }
        isRotated = true;
    }

};

struct WrongContactPoint : public ContactPoint {
    WrongContactPoint(const NoneContactPoint &ncp) : ContactPoint(ncp) {
        point = ncp.points[ncp.examinedIndex];
        normal = ncp.normals[ncp.examinedIndex];
        contactPosition = ncp.contactPositions[ncp.examinedIndex];
        normalInContact = ncp.normalsInContact[ncp.examinedIndex];
        forbiddenAreaEndPoint = point + normal * ncp.forbiddenAreaLength;
    }
};

#endif //CONTACT_POINT
