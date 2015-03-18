#include "basic_objects/GearPairInformation.h"
#include "basic_objects/GearPair.h"
#include "basic_objects/SplineGear.h"

GearPairInformation::GearPairInformation(GearPair *gearPair) : m_gearPair(gearPair) {
    m_gearPair->informAboutChange(this);
    m_CPmanager = &(m_gearPair->contactPointManager());
    m_informationCalculated = false;
    m_BCinformationCalculated = false;
    m_updateNextTime = true;
}

GearPairInformation::~GearPairInformation() {
}

void GearPairInformation::update() {
    if(!m_updateNextTime)
        return;

    for(CalculationState state : {CalculationState::Simple, CalculationState::BottomClearance}) {
        if(m_CPmanager->gearPointsWereCreated(state)) {

            vector<ContactPoint*> contactPoints = m_CPmanager->gearContactPoints(state);
            vector<WrongContactPoint*> wrongContactPoints = m_CPmanager->gearWrongContactPoints(state);

            //Remark: better would probably be variables packed in a GearState and without
            //        an if-else here, but as this resulted in a much unreadable code, I
            //        switched back to this not very nice if statement.

            if(state == CalculationState::Simple) {
                countDirection(contactPoints, m_numberOfCPsInContact);
                countDirection(wrongContactPoints, m_numberOfWCPsInContact);
                updateFirstRequirement(m_firstRequirement, m_numberOfWCPsInContact);
                updateSecondRequirement(state, m_percentagePathOfContactCoversPitch);
                m_informationCalculated = true;

            } else { // state == CalculationState::BottomClearance
                countDirection(contactPoints, m_BCnumberOfCPsInContact);
                countDirection(wrongContactPoints, m_BCnumberOfWCPsInContact);
                updateFirstRequirement(m_BCfirstRequirement, m_BCnumberOfWCPsInContact);
                updateSecondRequirement(state, m_BCpercentagePathOfContactCoversPitch);
                m_BCinformationCalculated = true;
            }
        } else {
            if(state == CalculationState::Simple)
                m_informationCalculated = false;
            else
                m_BCinformationCalculated = false;
        }
    }
    m_updateNextTime = false;
    changed();
}

// void GearPairInformation::validGearShapeInfoText() const {
// Dieses Programm betrachtet nur außenverzahnte Stirnräder. Wird eine Kurve als Zahn gewählt,
// die Schnittpunkte enthält oder eine zu hohe Zähnezahl gewählt, so dass das Treibrad Schnittpunkte
// aufweist, kann keine korrekte Verzahnung berechnet werden.
// Verbesserung: Schnittpunkte entfernen!
// }

bool GearPairInformation::gearShapeOfDrivingGearIsValid() const {
    return m_CPmanager->validGearShapeFound();
}

bool GearPairInformation::gearOutlineConstructionOfDrivenGearFailed() const {
    return m_CPmanager->gearOutlineConstructionFailed();
}

bool GearPairInformation::basicRequirementsOfGearToothSystemAreFulfilled(TurningDirection directionOfDrivingGear) const {
    return firstBasicRequirementOfGearToothSystemIsFulfilled(directionOfDrivingGear)
        && secondBasicRequirementOfGearToothSystemIsFulfilled(directionOfDrivingGear);
}

bool GearPairInformation::firstBasicRequirementOfGearToothSystemIsFulfilled(TurningDirection directionOfDrivingGear) const {
    if(m_gearPair->isBottomClearanceUsed())
        return m_BCfirstRequirement.direction(directionOfDrivingGear);
    else
        return m_firstRequirement.direction(directionOfDrivingGear);
}

bool GearPairInformation::secondBasicRequirementOfGearToothSystemIsFulfilled(TurningDirection directionOfDrivingGear) const {
    if(m_gearPair->isBottomClearanceUsed())
        return m_BCpercentagePathOfContactCoversPitch.direction(directionOfDrivingGear) >= 1.0;
    else
        return m_percentagePathOfContactCoversPitch.direction(directionOfDrivingGear) >= 1.0;
}

real GearPairInformation::ratioOfCPsToWCPs(TurningDirection dir) const {
    if(m_gearPair->isBottomClearanceUsed())
        return (real)m_BCnumberOfCPsInContact.direction(dir) /
            (real)(m_BCnumberOfCPsInContact.direction(dir) + m_BCnumberOfWCPsInContact.direction(dir));
    else
        return (real)m_numberOfCPsInContact.direction(dir) /
            (real)(m_numberOfCPsInContact.direction(dir) + m_numberOfWCPsInContact.direction(dir));
}

real GearPairInformation::contactRatio(TurningDirection dir) const { //German: Profilüberdeckung (epsilon_alpha)
    if(m_gearPair->isBottomClearanceUsed())
        return m_BCpercentagePathOfContactCoversPitch.direction(dir);
    else
        return m_percentagePathOfContactCoversPitch.direction(dir);
}

void GearPairInformation::objectChanged(ChangingObject *object) {
    if(object == m_gearPair) {
        updateNecessary();
    }
}

void GearPairInformation::updateNecessary() {
    m_updateNextTime = true;
    m_informationCalculated = false;
    m_BCinformationCalculated = false;
    changed();
}

void GearPairInformation::informAboutChange(ChangingObjectListener *listener) {
    m_listeners.push_back(listener);
}

void GearPairInformation::noMoreInformAboutChange(ChangingObjectListener *listener) {
    m_listeners.remove(listener);
}

void GearPairInformation::changed() {
    for(ChangingObjectListener *listener : m_listeners) {
        listener->objectChanged(this);
    }
}

void GearPairInformation::countDirection(vector<ContactPoint*> &contactPoints, Directions<uint> &numberOfCPsInContact) const {
    numberOfCPsInContact = Directions<uint>(0, 0);
    for(ContactPoint *cp : contactPoints) {
        real direction = cross(normalize(cp->contactPosition), cp->normalInContact);
        if(direction > 0.0) //normal points in clock direction
            ++(numberOfCPsInContact.clockwise);
        else
            ++(numberOfCPsInContact.counterClockwise);
    }
}

void GearPairInformation::countDirection(vector<WrongContactPoint*> &wrongContactPoints, Directions<uint> &numberOfCPsInContact) const {
    numberOfCPsInContact = Directions<uint>(0, 0);
    for(WrongContactPoint *wcp : wrongContactPoints) {
        real direction = cross(normalize(wcp->contactPosition), wcp->normalInContact);
        if(direction > 0.0) //normal points in clock direction
            ++(numberOfCPsInContact.clockwise);
        else
            ++(numberOfCPsInContact.counterClockwise);
    }
}

void GearPairInformation::updateFirstRequirement(Directions<bool> &firstRequirement, Directions<uint> numberOfWCPsInContact) const {
    firstRequirement = Directions<bool>(true, true);
    if(numberOfWCPsInContact.clockwise != 0)
        firstRequirement.clockwise = false;
    if(numberOfWCPsInContact.counterClockwise != 0)
        firstRequirement.counterClockwise = false;
}

void GearPairInformation::updateSecondRequirement(CalculationState state, Directions<real> &percentagePathOfContactCoversPitch) const {
    for(TurningDirection dir : {TurningDirection::Clockwise, TurningDirection::CounterClockwise}) {

        vec2 startSeries = m_CPmanager->gearConsecutivelyContactPointsStart(dir, state);
        vec2 endSeries = m_CPmanager->gearConsecutivelyContactPointsStop(dir, state);

        bool startInDrivingGear, stopInDrivingGear;
        real alphaStart, alphaStop;

        if(startSeries.x < m_gearPair->drivingGearPitchRadius()) {
            startInDrivingGear = true;
            alphaStart = asin(normalize(startSeries).y);
        } else {
            startInDrivingGear = false;
            vec2 toContactPosition = normalize(startSeries - vec2(m_gearPair->getDistanceOfCenters(), 0));
            alphaStart = asin(toContactPosition.y);
        }

        if(endSeries.x < m_gearPair->drivingGearPitchRadius()) {
            stopInDrivingGear = true;
            alphaStop = asin(normalize(endSeries).y);
        } else {
            stopInDrivingGear = false;
            vec2 toContactPosition = normalize(endSeries - vec2(m_gearPair->getDistanceOfCenters(), 0));
            alphaStop = asin(toContactPosition.y);
        }

        if(startInDrivingGear == stopInDrivingGear) {
            real coverage = alphaStart - alphaStop;
            real angularPitch = startInDrivingGear ?
                    (m_gearPair->drivingGear()->angularPitch()) : (m_gearPair->drivenGear()->angularPitch());
            percentagePathOfContactCoversPitch.setValue(absolute(coverage / angularPitch), dir);

        } else {
            real totalCoverage = 0.0;
            if(startInDrivingGear) {
                totalCoverage += absolute(alphaStart / m_gearPair->drivingGear()->angularPitch());
                totalCoverage += absolute(alphaStop / m_gearPair->drivenGear()->angularPitch());
            } else {
                totalCoverage += absolute(alphaStart / m_gearPair->drivenGear()->angularPitch());
                totalCoverage += absolute(alphaStop / m_gearPair->drivingGear()->angularPitch());
            }
            percentagePathOfContactCoversPitch.setValue(totalCoverage, dir);
        }
    }
}
