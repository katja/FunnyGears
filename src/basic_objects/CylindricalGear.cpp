#include "basic_objects/CylindricalGear.h"
#include <Eigen/Geometry>

void CylindricalGear::gearProfile(vector<vec2> &gearProfile) const {
    uint pointsPerTooth = gearProfile.size() / getNumberOfTeeth();

    //Return without doing anything, if the given gearProfile vector has a wrong size
    if(pointsPerTooth * getNumberOfTeeth() != gearProfile.size())
        return;

    vector<vec2> toothProfileVector(pointsPerTooth);
    toothProfile(toothProfileVector);
    uint numberOfTeeth = getNumberOfTeeth();

    //Test if the points of the toothProfileVector are given in clockwise or counterclockwise direction
    vec2 first = toothProfileVector[0];
    vec2 last = toothProfileVector[pointsPerTooth];
    int rotationDirection = 1;
    if((first[0] * last[1] - first[1] * last[0]) < 0)
        rotationDirection = -1;

    real rotationPerToothInRad = rotationDirection * 2 * M_PI / numberOfTeeth;

    real rotationInRad = 0.0f;
    for(uint i = 0; i < numberOfTeeth; ++i) {
        for(uint j = 0; j < pointsPerTooth; ++j) {
            gearProfile[i * pointsPerTooth + j] = Eigen::Rotation2D<real>(rotationInRad) * toothProfileVector[j];
        }
        rotationInRad += rotationPerToothInRad;
    }
}
