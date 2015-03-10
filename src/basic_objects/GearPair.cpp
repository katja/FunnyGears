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
    m_samplingRate(DefaultSamplingRate),
    m_gearPairInformation(new GearPairInformation(this))
{

    m_drivenGear->setNumberOfTeeth(m_drivingGear->numberOfTeeth());
    calculateAgainWithAllAttributes();
}

GearPair::~GearPair() {
    delete m_gearPairInformation;
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
    m_gearPairInformation->setBackAttributes();
    m_contactPointManager.clear(); //deletes all ContactPoint* of the list

    m_stepSize = (m_completeToothProfile->upperDomainLimit() - m_completeToothProfile->lowerDomainLimit())
                    / (m_samplingRate - 1);

    constructListOfPossiblePairingPoints();
    chooseCorrectPoints();
}

GearPairInformation* GearPair::gearPairInformation() {
    return m_gearPairInformation;
}

const ContactPointManager& GearPair::contactPointManager() {
    return m_contactPointManager;
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

real GearPair::module() const {
    return m_module;
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

    createAndInsertContactPoint(startPoint, nextNormal, startValue);

    for(uint step = 1; step < m_samplingRate; ++step) {
        vec2 normal = nextNormal;
        real evalValue = nextStepValue;

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
        insertRefinedContactPoints(evalValue, nextStepValue, partition);
    }
    m_contactPointManager.finishInsertion();
}

//It is really important, that the points with smaller evaluationValues are inserted first!!!
void GearPair::insertRefinedContactPoints(real evalValue, real nextStepValue, uint partition) {
    vec2 normal = normalAt(evalValue);
    vec2 nextNormal = normalAt(nextStepValue);

    if(partition > 0 && angleBetweenN(normal, nextNormal) > m_maxDriftAngle) {
        real refineAt = 0.5 * (evalValue + nextStepValue);
        insertRefinedContactPoints(evalValue, refineAt, partition - 1);
        insertRefinedContactPoints(refineAt, nextStepValue, partition - 1);

    } else if (partition > 0) {
        evalValue = nextStepValue;
        normal = nextNormal;
        vec2 point = m_completeToothProfile->evaluate(evalValue);
        createAndInsertContactPoint(point, normal, evalValue);

    } else { //partition == 0
        vec2 point = m_completeToothProfile->evaluate(evalValue);
        vec2 nextPoint = m_completeToothProfile->evaluate(nextStepValue);
        createAndInsertContactPoint(0.5 * (point + nextPoint),
                                    0.5 * (normal + nextNormal),
                                    0.5 * (evalValue + nextStepValue));
    }
}

void GearPair::chooseCorrectPoints() {
    m_contactPointManager.processPointsToGear(m_drivenGear->numberOfTeeth(), !(m_drivingGear->toothDescribedInClockDirection()));
    m_contactPointManager.translateForBottomClearance(3.0, 15); //bottomClearance, angleInDegree //Kopfspiel
    vector<vec2> gearPoints = m_contactPointManager.gearPoints();
    m_drivenGear->setDegree(1);
    m_drivenGear->setControlPointsForTooth(gearPoints);
}

void GearPair::createAndInsertContactPoint(const vec2 &point, const vec2 &normal, real evalValue) {

    //find cut of normal in originPoint with reference radius
    real valueUnderRoot = square(dot(normal, point))
                            - dot(point, point)
                            + square(m_drivingGearPitchRadius);
    if(valueUnderRoot < 0) {
        // No cut with reference radius, so this point should not have a contact with the mating gear
        // Nevertheless it may be very important on special forms to follow these points!
        m_contactPointManager.insert(noneContactPointOf(point, normal, evalValue));
        return;
    }
    if(glm::length(point) >= m_drivingGearPitchRadius) {
        // point lies outside pitch circle
        if(dot(normalize(point), normal) >= 0.0) { // normal points away from circle, so take larger value for t
            real t = -dot(normal, point) + sqrt(valueUnderRoot);
            m_contactPointManager.insert(contactPointOf(point, normal, evalValue, t, true));
        } else { // normal points to center => two cutting points with pitch circle in normal direction => take nearer one
            real t = -dot(normal, point) - sqrt(valueUnderRoot);
            m_contactPointManager.insert(contactPointOf(point, normal, evalValue, t, false));
        }
    } else {
        // point lies inside pitch circle
        // Examine both possibilities for cut with pitch circle, as long as the distance
        // to the cutting point is not larger than the pitch radius. As the value t
        // corresponds to the distance to the cutting, observe t
        real tBehind = -dot(normal, point) - sqrt(valueUnderRoot);
        real tAhead = -dot(normal, point) + sqrt(valueUnderRoot);
        std::cout << "in the circle and tBehind = " << tBehind << ", tAhead = " << tAhead << " whereas radius = " << m_drivingGearPitchRadius << std::endl;
        if((-tBehind) < m_drivingGearPitchRadius && tAhead < m_drivingGearPitchRadius) { // as tBehind is in contrary normal direction, it is negative
            std::cout << "              goes in BOTH" << std::endl;
            m_contactPointManager.insert(contactPointOf(point, normal, evalValue, tBehind, false),
                                         contactPointOf(point, normal, evalValue, tAhead, true));
        } else if(tAhead < m_drivingGearPitchRadius) {
            std::cout << "              goes in ahead" << std::endl;
            m_contactPointManager.insert(contactPointOf(point, normal, evalValue, tAhead, true));
        } else {
            std::cout << "              goes in behind" << std::endl;
            m_contactPointManager.insert(contactPointOf(point, normal, evalValue, tBehind, false));
        }
    }
}

ContactPoint* GearPair::contactPointOf(const vec2 &point, const vec2 &normal, real evalValue, real t, bool usedLargerValue) const {
    ContactPoint *cp = new ContactPoint();
    cp->evaluationValue = evalValue;
    cp->originPoint = point;
    cp->originNormal = normal;
    cp->usedLargerValue = usedLargerValue;
    cp->error = ErrorCode::NO_ERROR;

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

    if(!insertThicknessInContactPoint(*cp)) {
        delete cp;
        cp = nullptr;
        return nullptr;
    }
    return cp;
}


NoneContactPoint* GearPair::noneContactPointOf(const vec2 &point, const vec2 &normal, real evalValue) const {
    NoneContactPoint *ncp = new NoneContactPoint();
    ncp->evaluationValue = evalValue;
    ncp->originPoint = point;
    ncp->originNormal = normal;

    if(!insertThicknessInContactPoint(*ncp)) {
        delete ncp;
        ncp = nullptr;
        return nullptr;
    }

    real angleToY = angleBetweenN(normalize(ncp->originPoint), vec2(0, -1));
    real direction = (ncp->originPoint.x > 0) ? -1.0 : 1.0;

    // Inspect the ContactPoint cp when the gear turns
    // The exciting points are somewhere near the pitch point.
    // Therefore examine the angle covered by about 3 pitches, or a maximum of a half
    // of the gear. In each case those values around the pitch point will be taken.
    // For all of these: x > 0.

    real startAngle = M_PI / 6.0; // startAngle is measured starting from y-axis
    real endAngle = M_PI - startAngle;

    while(startAngle < endAngle) {

        real angleA = startAngle + (direction * angleToY);
        real angleB = angleA * m_drivingGearPitchRadius / m_drivenGearPitchRadius;

        vec2 pointOfContact = glm::rotate(ncp->originPoint, angleA);
        vec2 normalInContact = glm::rotate(ncp->originNormal, angleA);

        ncp->points.push_back(glm::rotate((pointOfContact - vec2(m_distanceOfCenters, 0.0)), angleB));
        ncp->normals.push_back(glm::rotate(vec2(-normalInContact.x, -normalInContact.y), angleB));
        ncp->contactPositions.push_back(pointOfContact);
        ncp->normalsInContact.push_back(normalInContact);

        startAngle += m_maxDriftAngle;
    }
    return ncp;
}

//Given ContactPoint already needs its point, normal, originPoint and originNormal!
bool GearPair::insertThicknessInContactPoint(ContactPoint& contactPoint) const {
    //send a ray in opposite direction of the gear normal to get the gear width in this direction
    Ray ray(vec2(contactPoint.originPoint), vec2(-contactPoint.originNormal));
    vector<vec2> intersectionPoints;
    m_drivingGear->getIntersectionPointsWithRay(ray, intersectionPoints);
    if(intersectionPoints.empty()) {
        //As Gear is closed Curve without crossings, every normal should find a cutting!
        std::cerr << "CURIOS THINGS HAPPENED! A ray of the gear hadn't found a cutting!" << std::endl;
        //If no cutting is found, curve has not a form of a gear!
        m_gearPairInformation->notValidGearShapeFound();
        return false;
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
        return true;
    }
}

vec2 GearPair::normalAt(real t) const {
    vec2 normal = m_completeToothProfile->normal(t);
    if(m_drivingGear->toothDescribedInClockDirection()) {
        normal = normal * -1.0;
    }
    return normal;
}



