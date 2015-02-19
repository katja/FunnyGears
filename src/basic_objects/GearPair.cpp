#include "GearPair.h"
#include "basic_objects/SplineGear.h"
#include "basic_objects/Ray.h"

GearPair::GearPair(SplineGear *drivingGear) :
    m_drivingGear(drivingGear),
    m_maxDriftAngle(DefaultMaxDrift),
    m_samplingRate(DefaultSamplingRate),
    m_liveCalculation(false)
{
    //Set the default values, assume that a mostly similar mating gear
    m_drivenGear = new SplineGear(Spline(m_drivingGear->degree()));
    m_drivenGear->setNumberOfTeeth(m_drivingGear->numberOfTeeth());
    m_drivenGearPitchRadius = m_drivingGear->referenceRadius();

    m_drivingGearPitchRadius = m_drivingGear->referenceRadius();

    m_module = 2.0 * m_drivingGear->referenceRadius() / m_drivingGear->numberOfTeeth();

    m_drivenGearIndependentReferenceRadius = m_module * m_drivenGear->numberOfTeeth() / 2.0;

    m_distanceOfCenters = m_drivingGearPitchRadius + m_drivenGearPitchRadius;

    m_completeToothProfile = m_drivingGear->completeToothProfile();
    m_stepSize = (m_completeToothProfile->upperDomainLimit() - m_completeToothProfile->lowerDomainLimit())
                    / (m_samplingRate - 1);

    constructListOfPossiblePairingPoints();
    chooseCorrectPoints();
}

GearPair::~GearPair() {
}

//TODO: remove this!!!!
std::list<ContactPoint>* GearPair::foundPoints() {
    return &m_allContactPoints;
}

//TODO: remove this!!!!
Spline* GearPair::completeToothProfile() {
    return m_completeToothProfile;
}

//TODO: remove this!!!!
std::list< PositionList* >* GearPair::pointsInSortedLists() {
    return m_allContactPoints.positionLists();
}

void GearPair::setNumberOfTeethOfDrivenGear(uint numberOfTeeth) {
    m_drivenGear->setNumberOfTeeth(numberOfTeeth);
}

uint GearPair::numberOfTeethOfDrivenGear() const {
    return m_drivenGear->numberOfTeeth();
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

real GearPair::getDistanceOfCenters() const {
    return m_distanceOfCenters;
}

const real GearPair::DefaultMaxDrift = 3.0;
const uint GearPair::DefaultSamplingRate = 80;

void GearPair::constructListOfPossiblePairingPoints() {
    real startValue = m_completeToothProfile->lowerDomainLimit();
    vec2 nextPoint = m_completeToothProfile->evaluate(startValue);
    vec2 nextNormal = normalAt(startValue);

    real stepValue;

    for(uint step = 1; step < m_samplingRate; ++step) {
        stepValue = startValue + m_stepSize * (real)step;
        if(stepValue > m_completeToothProfile->upperDomainLimit()) {
            stepValue = m_completeToothProfile->upperDomainLimit();
        }

        vec2 point = nextPoint;
        vec2 normal = nextNormal;

        nextPoint = m_completeToothProfile->evaluate(stepValue);
        nextNormal = normalAt(stepValue);

        ContactPoint contactPoint = contactPointOf(point, normal);
        m_allContactPoints.push_back(contactPoint);

        real angleBetweenNormals = angleBetweenN(normal, nextNormal);
        real direction = (normal.x * nextNormal.y > normal.y * nextNormal.x) ? 1.0 : -1.0;
        if(angleBetweenNormals > m_maxDriftAngle) {
            //TODO: Here we do not use an evaluation of the spline, instead only a connection from one to the next point and normal is created. Is this good????
            uint partitions = static_cast<uint>(angleBetweenNormals / m_maxDriftAngle);
            vec2 pointDiff = nextPoint - point;
            for(uint partition = 1; partition <= partitions; ++partition) {
                vec2 partitionPoint = point + pointDiff * static_cast<real>(partition / partitions);
                vec2 partitionNormal = glm::rotate(normal, m_maxDriftAngle * direction * partition);
                ContactPoint contactPoint = contactPointOf(partitionPoint, partitionNormal);
                m_allContactPoints.push_back(contactPoint);
            }
        }
    }
}


void GearPair::chooseCorrectPoints() {
    m_allContactPoints.sort(
        m_drivenGear->numberOfTeeth(),
        !(m_drivingGear->toothDescribedInClockDirection())
    );
}

ContactPoint GearPair::contactPointOf(const vec2 &point, const vec2 &normal) const {
    ContactPoint cp;
    cp.originPoint = point;
    cp.originNormal = normal;
    cp.error = ErrorCode::NO_ERROR;

    real valueUnderRoot = square(dot(normal, point))
                            - dot(point, point)
                            + square(m_drivingGearPitchRadius);
    if(valueUnderRoot < 0) {
        cp.error = ErrorCode::NO_CUT_WITH_REFERENCE_RADIUS;
        return cp;
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
    cp.contactPosition = pointOfContact;
    cp.point = glm::rotate((pointOfContact - vec2(m_distanceOfCenters, 0.0)), angleB);
    cp.normalInContact = glm::rotate(normal, angleA);
    cp.normal = glm::rotate(vec2(-cp.normalInContact.x, -cp.normalInContact.y), angleB);

    insertThicknessInContactPoint(cp);

    return cp;

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
        // std::cout << "For CP " << contactPoint.originPoint << " " << intersectionPoints.size() << " intersection points were found" << std::endl;
        // std::cout << "Found Points: ";
        real epsilon = 1.0; //prevent finding the point itself as intersection point
        real thickness = 3.0 * m_drivingGear->maximumDistanceToCenter(); //gear can only have a thickness of 2.0f * maximumDistanceToCenter()

        for(vector<vec2>::iterator it = intersectionPoints.begin(), end = intersectionPoints.end(); it != end; ++it) {
            real distance = glm::length(*it - contactPoint.originPoint);
            std::cout << *it << " => " << distance << ",\t";
            if(distance > epsilon && distance < thickness) {
                thickness = distance;
            }
        }
        contactPoint.forbiddenAreaLength = thickness;
        contactPoint.forbiddenAreaEndPoint = contactPoint.point + contactPoint.normal * thickness;
        // std::cout << "\nand chosen one: " << contactPoint.forbiddenAreaLength << std::endl;
        // std::cout << std::endl;
    }
}

vec2 GearPair::normalAt(real t) const {
    vec2 normal = m_completeToothProfile->normal(t);
    if(m_drivingGear->toothDescribedInClockDirection()) {
        normal = normal * -1.0;
    }
    return normal;
}
