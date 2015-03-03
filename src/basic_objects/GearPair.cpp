#include "GearPair.h"
#include "basic_objects/SplineGear.h"
#include "basic_objects/Ray.h"

const real GearPair::DefaultMaxDrift = 0.26; // ~ 15° //0.05; // ~ 3°
const uint GearPair::DefaultSamplingRate = 60;//80;

GearPair::GearPair(const SplineGear &drivingGear) :
    m_drivingGear(new SplineGear(drivingGear)),
    //Set the default values, assume that a mostly similar mating gear
    m_drivenGear(new SplineGear(Spline())),
    m_completeToothProfile(m_drivingGear->completeToothProfile()),
    m_maxDriftAngle(DefaultMaxDrift),
    m_samplingRate(DefaultSamplingRate)
{

    m_drivenGear->setNumberOfTeeth(m_drivingGear->numberOfTeeth());
    calculateAgainWithAllAttributes();
}

GearPair::~GearPair() {
}

void GearPair::calculateAgainWithAllAttributes() {
    m_drivingGearPitchRadius = m_drivingGear->referenceRadius();
    m_module = 2.0 * m_drivingGear->referenceRadius() / m_drivingGear->numberOfTeeth();
    m_drivenGearPitchRadius = m_module * m_drivenGear->numberOfTeeth() / 2.0;
    m_distanceOfCenters = m_drivingGearPitchRadius + m_drivenGearPitchRadius;
    calculateAgainWithNewToothProfile();
}

void GearPair::calculateAgainWithNewToothProfile() {
    if(m_completeToothProfile != nullptr)
        delete m_completeToothProfile;
    m_completeToothProfile = m_drivingGear->completeToothProfile();
    calculateAgainWithUnchangedAttributes();
}

void GearPair::calculateAgainWithUnchangedAttributes() {
    m_allContactPoints.clear(); //deletes all ContactPoint* of the list and when again sort(...) is called, the other saved lists are deleted, too

    m_stepSize = (m_completeToothProfile->upperDomainLimit() - m_completeToothProfile->lowerDomainLimit())
                    / (m_samplingRate - 1);

    constructListOfPossiblePairingPoints();
    chooseCorrectPoints();
}

const ContactPointSortingList& GearPair::foundPoints() {
    return m_allContactPoints;
}

SplineGear* GearPair::drivingGear() const {
    return m_drivingGear;
}

SplineGear* GearPair::drivenGear() const {
    return m_drivenGear;
}

void GearPair::setNumberOfTeethOfDrivenGear(uint numberOfTeeth) {
    m_drivenGear->setNumberOfTeeth(numberOfTeeth);
}

uint GearPair::numberOfTeethOfDrivenGear() const {
    return m_drivenGear->numberOfTeeth();
}

real GearPair::transmissionRatio() const {
    return -(static_cast<real>(m_drivenGear->numberOfTeeth()) / static_cast<real>(m_drivingGear->numberOfTeeth()));
}

real GearPair::drivingGearPitchRadius() const {
    return m_drivingGearPitchRadius;
}

real GearPair::drivenGearPitchRadius() const {
    return m_drivenGearPitchRadius;
}

real GearPair::getDistanceOfCenters() const {
    return m_distanceOfCenters;
}

void GearPair::setMaxDriftAngleInDegree(real degree) {
    m_maxDriftAngle = toRad(degree);
}

void GearPair::setMaxDriftAngle(real rad) {
    m_maxDriftAngle = rad;
}

real GearPair::maxDriftAngleInDegree() const {
    return toDegree(m_maxDriftAngle);
}
real GearPair::maxDriftAngle() const {
    return m_maxDriftAngle;
}

void GearPair::setSamplingRate(uint samplingRate) {
    m_samplingRate = samplingRate;
}

uint GearPair::samplingRate() const {
    return m_samplingRate;
}

void GearPair::constructListOfPossiblePairingPoints() {
    real startValue = m_completeToothProfile->lowerDomainLimit();
    vec2 startPoint = m_completeToothProfile->evaluate(startValue);
    vec2 nextNormal = normalAt(startValue);
    real nextStepValue = startValue;

    ContactPoint *contactPoint = contactPointOf(startPoint, nextNormal, startValue);
    m_allContactPoints.push_back(contactPoint);

    for(uint step = 1; step < m_samplingRate; ++step) {
        vec2 normal = nextNormal;
        real stepValue = nextStepValue;

        nextStepValue = startValue + m_stepSize * (real)step;
        if(nextStepValue > m_completeToothProfile->upperDomainLimit()) {
            nextStepValue = m_completeToothProfile->upperDomainLimit();
        }
        nextNormal = normalAt(nextStepValue);

        real angleBetweenNormals = angleBetweenN(normal, nextNormal);
        uint partition = 1;
        if(angleBetweenNormals > m_maxDriftAngle)
            partition = 3 * (static_cast<uint>(angleBetweenNormals / m_maxDriftAngle)); // at least 3

        // for small enough angle between normals => partition = 1 => will walk only once
        // through insertRefinedContactPoints and insert a ContactPoint at nextStepValue
        insertRefinedContactPoints(stepValue, nextStepValue, partition);
    }
}

void GearPair::insertRefinedContactPoints(real stepValue, real nextStepValue, uint partition) {
    vec2 normal = normalAt(stepValue);
    vec2 nextNormal = normalAt(nextStepValue);

    if(partition > 0 && angleBetweenN(normal, nextNormal) > m_maxDriftAngle) {
        real refineAt = 0.5 * (stepValue + nextStepValue);
        insertRefinedContactPoints(stepValue, refineAt, partition - 1);
        insertRefinedContactPoints(refineAt, nextStepValue, partition - 1);

    } else if (partition > 0) {
        stepValue = nextStepValue;
        normal = nextNormal;
        vec2 point = m_completeToothProfile->evaluate(stepValue);
        ContactPoint *contactPoint = contactPointOf(point, normal, stepValue);
        m_allContactPoints.push_back(contactPoint);

    } else { //partition == 0
        vec2 point = m_completeToothProfile->evaluate(stepValue);
        vec2 nextPoint = m_completeToothProfile->evaluate(nextStepValue);
        ContactPoint *contactPoint = contactPointOf(0.5 * (point + nextPoint),
                                                    0.5 * (normal + nextNormal),
                                                    0.5 * (stepValue + nextStepValue));
        m_allContactPoints.push_back(contactPoint);
    }
}

void GearPair::chooseCorrectPoints() {
    m_allContactPoints.createCoveringLists(
        m_drivenGear->numberOfTeeth(),
        !(m_drivingGear->toothDescribedInClockDirection())
    );
    vector<vec2> gearPoints = m_allContactPoints.gearPoints();
    m_drivenGear->setDegree(1);
    m_drivenGear->setControlPointsForTooth(gearPoints);
}

ContactPoint* GearPair::contactPointOf(const vec2 &point, const vec2 &normal, real stepValue) {
    ContactPoint *cp = new ContactPoint();
    cp->evaluationValue = stepValue;
    cp->originPoint = point;
    cp->originNormal = normal;
    cp->error = ErrorCode::NO_ERROR;

    //find cut of normal in originPoint with reference radius
    real valueUnderRoot = square(dot(normal, point))
                            - dot(point, point)
                            + square(m_drivingGearPitchRadius);
    if(valueUnderRoot < 0) {
        //No cut with reference radius, so this point should not have a contact with the mating gear
        //Nevertheless it may be very important on special forms to follow these points!
        return convertToNoneContactPoint(cp);
    }
    real t;
    if(dot(normalize(point), normal) >= 0.0) {
        t = -dot(normal, point) + sqrt(valueUnderRoot); // normal points away of the pitch circle so take the larger value for t
    } else {
        t = -dot(normal, point) - sqrt(valueUnderRoot); // normal points to center of pitch circle so take the smaller value for t
    }
    vec2 directionToCutOnPitchRadius = normalize(point + t * normal);

    // as arcsin is only defined on [-pi/2, pi/2] we have to get the correct values for the left side of the circle, if the point is there
    real angleA;
    if(directionToCutOnPitchRadius.x >= 0) {
        // cut with pitch circle is on same side as pitch point
        angleA = -asin(directionToCutOnPitchRadius.y);

    } else {
        // cut is on opposite side of pitch point
        // we have to divide the circle somewhere, so use the point opposite of pitch point
        if(directionToCutOnPitchRadius.y > 0)
            angleA = -(M_PI - asin(directionToCutOnPitchRadius.y));
        else
            angleA = +(M_PI + asin(directionToCutOnPitchRadius.y)); //TODO: old code differs here! But I think, this is right solution?
    }

    real angleB = angleA * m_drivingGearPitchRadius / m_drivenGearPitchRadius;

    vec2 pointOfContact = glm::rotate(point, angleA);
    cp->contactPosition = pointOfContact;
    cp->point = glm::rotate((pointOfContact - vec2(m_distanceOfCenters, 0.0)), angleB);
    cp->normalInContact = glm::rotate(normal, angleA);
    cp->normal = glm::rotate(vec2(-cp->normalInContact.x, -cp->normalInContact.y), angleB);

    insertThicknessInContactPoint(*cp);

    return cp;
}

NoneContactPoint* GearPair::convertToNoneContactPoint(ContactPoint *cp) const {
    NoneContactPoint *ncp = new NoneContactPoint(*cp);
    delete cp;

    real angleToY = angleBetweenN(normalize(ncp->originPoint), vec2(0, -1));
    real direction = (ncp->originPoint.x > 0) ? -1.0 : 1.0;

    // Inspect the ContactPoint cp when the gear turns
    // The exciting points are somewhere near the pitch point.
    // Therefore examine the angle covered by about 3 pitches, or a maximum of a half
    // of the gear. In each case those values around the pitch point will be taken
    // For all of these x > 0.

    real startAngle = M_PI / 6.0; // startAngle is measured starting from y-axis
    real endAngle = M_PI - startAngle;
    // if(m_drivenGear->numberOfTeeth() > 6) { // enough teeth, so 'only' examine 3 pitches
    //     real angle = 3.0 * M_PI / m_drivenGear->numberOfTeeth(); // 3.0 <== 1.5 * 2.0 * M_PI
    //     startAngle = M_PI / 2.0 - angle;
    //     endAngle = M_PI / 2.0 + angle;
    // } else {
    //     startAngle = 0.0;
    //     endAngle = M_PI;
    // }

    while(startAngle < endAngle) {

        real angleA = startAngle + (direction * angleToY);
        real angleB = angleA * m_drivingGearPitchRadius / m_drivenGearPitchRadius;

        vec2 pointOfContact = glm::rotate(ncp->originPoint, angleA);
        vec2 normalInContact = glm::rotate(ncp->originNormal, angleA);

        ncp->points.push_back(glm::rotate((pointOfContact - vec2(m_distanceOfCenters, 0.0)), angleB));
        ncp->normals.push_back(glm::rotate(vec2(-normalInContact.x, -normalInContact.y), angleB));

        insertThicknessInContactPoint(*ncp);

        startAngle += m_maxDriftAngle;
    }
    return ncp;
}

//Given ContactPoint already needs its point, normal, originPoint and originNormal!
void GearPair::insertThicknessInContactPoint(ContactPoint& contactPoint) const {
    //send a ray in opposite direction of the gear normal to get the gear width in this direction
    Ray ray(vec2(contactPoint.originPoint), vec2(-contactPoint.originNormal));
    vector<vec2> intersectionPoints;
    m_drivingGear->getIntersectionPointsWithRay(ray, intersectionPoints);
    if(intersectionPoints.empty()) {
        std::cerr << "CURIOS THINGS HAPPENED! A ray of the gear hadn't found a cutting!" << std::endl;
        contactPoint.error = ErrorCode::NO_THICKNESS;
    } else {
        real epsilon = 1.0; //prevent finding the point itself as intersection point
        real thickness = 3.0 * m_drivingGear->maximumDistanceToCenter(); //gear can only have a thickness of 2.0f * maximumDistanceToCenter()

        for(vector<vec2>::iterator it = intersectionPoints.begin(), end = intersectionPoints.end(); it != end; ++it) {
            real distance = glm::length(*it - contactPoint.originPoint);
            if(distance > epsilon && distance < thickness) {
                thickness = distance;
            }
        }
        contactPoint.forbiddenAreaLength = thickness;
        if(contactPoint.error != ErrorCode::NO_CUT_WITH_REFERENCE_RADIUS)
            contactPoint.forbiddenAreaEndPoint = contactPoint.point + contactPoint.normal * thickness;
    }
}

vec2 GearPair::normalAt(real t) const {
    vec2 normal = m_completeToothProfile->normal(t);
    if(m_drivingGear->toothDescribedInClockDirection()) {
        normal = normal * -1.0;
    }
    return normal;
}



