// #include "basic_objects/SimpleGear.h"

// SimpleGear::SimpleGear(const ToothProfile &toothProfile) : m_toothProfile(toothProfile.clone()) {
//     // Assume for simplicity a radius which lies in the middle of max and min points around the center
//     m_radius = (m_toothProfile->maximumDistanceToOrigin() + m_toothProfile->minimumDistanceToOrigin()) / 2;
//     // If the tooth profile would suit 9.x times in a circle, only use 9 teeth to avoid overlapping start and end points
//     m_numberOfTeeth = (uint) floor((M_PI * 2.0) / getAngularPitch());
// }

// SimpleGear::SimpleGear(const SimpleGear& other) : m_toothProfile(other.m_toothProfile->clone()), m_radius(other.m_radius), m_numberOfTeeth(other.m_numberOfTeeth) {
// }

// SimpleGear::~SimpleGear() {
//     delete m_toothProfile;
// }

// real SimpleGear::getRadius() const {
//     return m_radius;
// }

// void SimpleGear::setRadius(real radius) {
//     if(m_radius < minRadius() || m_radius > maxRadius())
//         return;
//     m_radius = radius;
// }

// real SimpleGear::minRadius() const {
//     return m_toothProfile->minimumDistanceToOrigin();
// }

// real SimpleGear::maxRadius() const {
//     return m_toothProfile->maximumDistanceToOrigin();
// }

// real SimpleGear::getRootRadius() const {
//     //TODO: Maybe offer different possibilities for calculation of a root radius
//     real min = m_toothProfile->minimumDistanceToOrigin();
//     return min;
// }

// real SimpleGear::getTipRadius() const {
//     //TODO: Maybe offer different possibilities for calculation of a tip radius
//     real max = m_toothProfile->maximumDistanceToOrigin();
//     return max;
// }

// uint SimpleGear::getNumberOfTeeth() const {
//     return m_numberOfTeeth;
// }

// void SimpleGear::setNumberOfTeeth(uint numberOfTeeth) {
//     m_numberOfTeeth = numberOfTeeth;
// }

// real SimpleGear::getAngularPitch() const {
//     return 2 * M_PI / m_numberOfTeeth;
//     // vec2 start = m_toothProfile->start();
//     // vec2 stop  = m_toothProfile->stop();
//     // // if angularPitch > PI (more than half of circle), concstruction will fail!
//     // return angleBetween(first, last);
//     // //include cmath needed!:
//     // //return angleBetween(first, last);
// }

// void SimpleGear::getSampledProfileOfATooth(vector<vec2> &toothProfile) const {
//     m_toothProfile->sampledCurve(toothProfile);
//     // hpuint nPointsPerTooth = toothProfile.size();
//     // BSplineCurve<hpvec2> curve;
//     // m_toothProfile->getCurve(curve);
//     // real low,high;
//     // curve.getParameterRange(low, high);
//     // real delta = (high - low) / (real)(nPointsPerTooth - 1); // -1 so whole thooth is sampled
//     // for(hpuint i = 0; i < nPointsPerTooth; ++i) {
//     //     hpvec2 point = curve.getValueAt(low + (real) (i * delta));
//     //     toothProfile[i] = point;
//     // }
// }
