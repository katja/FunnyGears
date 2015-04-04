#include "helpers.h"
#include "basic_objects/ContactPointManager.h"
#include "basic_objects/Ray.h"
#include "glm/glm.hpp"
#include "glm/gtx/rotate_vector.hpp"

ContactPointManager::ContactPointManager() :
    m_numberOfInsertedCPs(0),
    m_currentSmallerValueList(new list<ContactPoint*>()),
    m_currentLargerValueList(new list<ContactPoint*>()),
    m_gearPointsCreated(GearState<bool>(false, false)),
    m_gearBestContact(Directions< vector<ContactPoint*> *>(nullptr, nullptr)),
    m_notTranslatedBestContact(Directions< vector<ContactPoint*> *>(nullptr, nullptr)),
    m_gearOutlineConstructionFailed(false)
{

}

ContactPointManager::~ContactPointManager() {
    deleteSortingLists();
    clearAllGearLists(); // delete the WCPs
    clear(); // delete the m_insertedCPsLists and m_noneContactPointList
}

void ContactPointManager::clear() {
    if(m_currentSmallerValueList != nullptr && !m_currentSmallerValueList->empty()) {
        m_insertedCPsLists.push_back(m_currentSmallerValueList); // delete list with m_insertedCPsLists
        m_currentSmallerValueList = new list<ContactPoint*>();
    }
    if(m_currentLargerValueList != nullptr && !m_currentLargerValueList->empty()) {
        m_insertedCPsLists.push_back(m_currentLargerValueList); // delete list with m_insertedCPsLists
        m_currentLargerValueList = new list<ContactPoint*>();
    }
    deleteInsertedContactPoints();
    deleteNoneContactPoints();

    m_validGearShapeFound = true;
    m_gearPointsCreated = GearState<bool>(false, false);
    m_numberOfInsertedCPs = 0;
}

void ContactPointManager::insert(ContactPoint *cp) {
    if(cp == nullptr) {
        m_validGearShapeFound = false;
        return;
    }
    cp->evaluationStep = m_numberOfInsertedCPs;
    ++m_numberOfInsertedCPs;
    if(cp->usedLargerValue) {
        m_currentLargerValueList->push_back(cp);
        //finish other list:
        if(!m_currentSmallerValueList->empty()) {
            m_insertedCPsLists.push_back(m_currentSmallerValueList);
            m_currentSmallerValueList = new list<ContactPoint*>();
        }
    } else {
        m_currentSmallerValueList->push_back(cp);
        //finish other list:
        if(!m_currentLargerValueList->empty()) {
            m_insertedCPsLists.push_back(m_currentLargerValueList);
            m_currentLargerValueList = new list<ContactPoint*>();
        }
    }
    m_gearPointsCreated = GearState<bool>(false, false);
}

void ContactPointManager::insert(ContactPoint *cpA, ContactPoint *cpB) {
    if(cpA == nullptr) {
        m_validGearShapeFound = false;
        return insert(cpB);
    }
    if(cpB == nullptr) {
        m_validGearShapeFound = false;
        return insert(cpA);
    }
    assert(cpA->evaluationValue == cpB->evaluationValue);
    assert(cpA->usedLargerValue != cpB->usedLargerValue);
    cpA->evaluationStep = m_numberOfInsertedCPs;
    cpB->evaluationStep = m_numberOfInsertedCPs;
    ++m_numberOfInsertedCPs;

    if(cpA->usedLargerValue) {
        m_currentLargerValueList->push_back(cpA);
        m_currentSmallerValueList->push_back(cpB);
    }
    else {
        m_currentLargerValueList->push_back(cpB);
        m_currentSmallerValueList->push_back(cpA);
    }
    m_gearPointsCreated = GearState<bool>(false, false);
}

void ContactPointManager::insert(NoneContactPoint *ncp) {
    if(ncp == nullptr) {
        m_validGearShapeFound = false;
        return;
    }
    // point on original gear has no equivalent on mating gear => examine the whole path of the point later
    ncp->evaluationStep = m_numberOfInsertedCPs;
    ++m_numberOfInsertedCPs;
    m_noneContactPointList.push_back(ncp);
    m_gearPointsCreated = GearState<bool>(false, false);
}

void ContactPointManager::finishInsertion() {
    m_insertedCPsLists.push_back(m_currentSmallerValueList);
    m_currentSmallerValueList = new list<ContactPoint*>();
    m_insertedCPsLists.push_back(m_currentLargerValueList);
    m_currentLargerValueList = new list<ContactPoint*>();

    eraseEmptyAndOneEntryLists();
}

void ContactPointManager::sortLists() {
    bool sorted = true;

    do {
        sorted = true;
        list< list<ContactPoint*> *>::iterator itPrev = m_insertedCPsLists.begin();
        list< list<ContactPoint*> *>::iterator itCurr = m_insertedCPsLists.begin();
        ++itCurr;
        list< list<ContactPoint*> *>::iterator end = m_insertedCPsLists.end();
        // sort bigger lists to front
        while(sorted && itCurr != end) {
            if((*itPrev)->size() < (*itCurr)->size()) {
                list<ContactPoint*> *tempList = (*itPrev);
                (*itPrev) = (*itCurr);
                (*itCurr) = tempList;
                sorted = false;
            } else {
                ++itPrev;
                ++itCurr;
            }
        }
    } while(!sorted);
}

void ContactPointManager::processPointsToGear(uint numberOfTeeth, real pitchRadius, bool isDescribedClockwise) {
    if(m_insertedCPsLists.empty() || numberOfNoneErrorContactPoints() == 0)
        return;

    m_pitchRadiusInDrivenGear = pitchRadius;

    setAngularPitch(numberOfTeeth, isDescribedClockwise);
        //=> m_angularPitchRotation
    sortLists(); // Sort lists of m_insertedCPsLists by decreasing size
    if(!setExaminedPitch())
        return;
    copyPointsInSuitableLists();
        //=> m_contactPointsWithPositionList
        //=> m_examinedPitchStartDirection, m_examinedPitchStopDirection, m_examinedPitchStartDirectionInDrivingGear
        //=> m_examinedPitchLengthInDrivenGear, m_examinedPitchLengthInDrivingGear
    reduceNumberOfNoneContactPoints();
    copyNoneContactPointsInRelevantPitches();
    findAllCoveredPoints();
        //=> m_gearPoints, m_gearCPs, m_gearWCPs,
        //=> m_gearPointsInformation, m_gearPointsInformationIndex
        //=> m_gearBestContact (may be nullptr!), m_gearBestContactStart, m_gearBestContactStop
}

//translate gear points with less than angle angleInDegree between their normal and line of centers
void ContactPointManager::translateForBottomClearance(real bottomClearance, real angleInDegree) {
    if(!m_gearPointsCreated.state(CalculationState::Simple) || m_gearPoints.empty())
        return;
    clearAllTranslatedGearLists();

    real maxAngle = absolute(angleInDegree * M_PI / 180.0);
    int normalDir = 1; //normals on the right side of point described direction
    if(m_angularPitchRotation > 0)
        normalDir = -1; //normals on the left side

    ContactPoint *lastContactPoint = nullptr;
    uint foundCuttings = 0;
    uint i = 0;
    uint firstContactPoint = m_gearPoints.size() - 1;

    while(i != firstContactPoint) {
        OriginInformation origin = m_gearPointsInformation[i];
        if(origin == OriginInformation::Cut || origin == OriginInformation::SomethingElse) {
            ++foundCuttings;

        } else {
            int index = m_gearPointsInformationIndex[i];
            ContactPoint *currentContactPoint =
                (origin == OriginInformation::CP) ? m_gearCPs[index] : m_gearWCPs[index];
            if(lastContactPoint == nullptr)
                firstContactPoint = i;
            else
                fillLastCuttingPoints(foundCuttings, i, lastContactPoint, currentContactPoint, bottomClearance, maxAngle);

            vec2 normalInContact = currentContactPoint->normalInContact;

            if(i == firstContactPoint && lastContactPoint != nullptr) {
                //do nothing, as this point was already examined as first point in first iteration

            } else if(absolute(normalInContact.y) < sin(maxAngle)) {
                //translate point
                real t = 1.0 - (asin(absolute(normalInContact.y)) / maxAngle);
                m_translatedGearPoints.push_back(currentContactPoint->point - t * bottomClearance * currentContactPoint->normal); // walk in opposite direction of the normal

            } else {
                //do not translate
                m_translatedGearPoints.push_back(currentContactPoint->point);
                if(origin == OriginInformation::CP)
                    m_notTranslatedGearCPs.push_back(m_gearCPs[index]);
                else
                    m_notTranslatedGearWCPs.push_back(m_gearWCPs[index]);
            }

            foundCuttings = 0;
            lastContactPoint = currentContactPoint;
        }
        ++i;
        if(i >= m_gearPoints.size() && i != firstContactPoint) { // if reached m_gearPoints end and firstContactPoint was set
            // start once again until firstContactPoint
            i = 0;
        }
    }
    findBestPathOfContact(CalculationState::BottomClearance);

    m_gearPointsCreated.bottomClearance = true;
}

const list< list<ContactPoint*>* >& ContactPointManager::foundPoints() const {
    return m_insertedCPsLists;
}

const list<ContactPointsWithPosition*>& ContactPointManager::contactPointsWithPositions() const {
    return m_contactPointsWithPositionList;
}

const list<NoneContactPoint*>& ContactPointManager::noneContactPoints() const {
    return m_noneContactPointList;
}

bool ContactPointManager::gearPointsWereCreated(CalculationState s) const {
    return m_gearPointsCreated.state(s);
}

const vector<vec2>& ContactPointManager::gearPoints(CalculationState state) const {
    if(state == CalculationState::Simple)
        return m_gearPoints;
    else //state == CalculationState::BottomClearance
        return m_translatedGearPoints;
}

const vector<ContactPoint*>& ContactPointManager::gearContactPoints(CalculationState state) const {
    if(state == CalculationState::Simple)
        return m_gearCPs;
    else //state == CalculationState::BottomClearance
        return m_notTranslatedGearCPs;
}

const vector<WrongContactPoint*>& ContactPointManager::gearWrongContactPoints(CalculationState state) const {
    if(state == CalculationState::Simple)
        return m_gearWCPs;
    else //state == CalculationState::BottomClearance
        return m_notTranslatedGearWCPs;
}

vector<ContactPoint*>* ContactPointManager::gearConsecutivelyContactPoints(TurningDirection direction, CalculationState state) const {
    if(state == CalculationState::Simple)
        return m_gearBestContact.direction(direction);
    else //state == CalculationState::BottomClearance
        return m_notTranslatedBestContact.direction(direction);
}

vec2 ContactPointManager::gearConsecutivelyContactPointsStart(TurningDirection direction, CalculationState state) const {
    if(state == CalculationState::Simple)
        return m_gearBestContactStart.direction(direction);
    else //state == CalculationState::BottomClearance
        return m_translatedGearBestContactStart.direction(direction);
}

vec2 ContactPointManager::gearConsecutivelyContactPointsStop(TurningDirection direction, CalculationState state) const {
    if(state == CalculationState::Simple)
        return m_gearBestContactStop.direction(direction);
    else //state == CalculationState::BottomClearance
        return m_translatedGearBestContactStop.direction(direction);
}

vec2 ContactPointManager::startOfExaminedPitchInDrivenGear() const {
    return m_examinedPitchStartDirection;
}

vec2 ContactPointManager::endOfExaminedPitchInDrivenGear() const {
    return m_examinedPitchStopDirection;
}

vec2 ContactPointManager::startOfExaminedPitchInDrivingGear () const {
    return m_examinedPitchStartDirectionInDrivingGear;
}

real ContactPointManager::lengthOfPitchesInDrivenGear() const {
    return m_examinedPitchLengthInDrivenGear;
}

real ContactPointManager::lengthOfPitchesInDrivingGear() const {
    return m_examinedPitchLengthInDrivingGear;
}

real ContactPointManager::usedAngularPitch() const {
    return m_angularPitchRotation;
}

bool ContactPointManager::validGearShapeFound() const {
    return m_validGearShapeFound;
}

bool ContactPointManager::gearOutlineConstructionFailed() const {
    return m_gearOutlineConstructionFailed;
}

///////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
// METHODS FOR ALGORITHM //////////////////////////////

void ContactPointManager::setAngularPitch(uint numberOfTeeth, bool isDescribedClockwise) {
    real turningDirection = ((isDescribedClockwise) ? 1.0 : -1.0);
    m_angularPitchRotation = (2.0 * M_PI * turningDirection) / (real)numberOfTeeth;
}

bool ContactPointManager::setExaminedPitch() {
    if(m_insertedCPsLists.empty())
        return false;
    list<ContactPoint*>::iterator a = ++(m_insertedCPsLists.front()->begin()),
                                  b = m_insertedCPsLists.front()->begin(),
                                  end = m_insertedCPsLists.front()->end();

    bool errorProne = true;
    while(errorProne && a != end) {
        if((*a)->error == ErrorCode::NO_ERROR && (*b)->error == ErrorCode::NO_ERROR)
            errorProne = false;
        ++a; ++b;
    }
    if(errorProne)
        return false;
    m_examinedPitchStartDirection = normalize(((*a)->point + (*b)->point) * 0.5);
    m_examinedPitchLengthInDrivenGear = glm::length(((*a)->point + (*b)->point) * 0.5);
    m_examinedPitchStartDirectionInDrivingGear = normalize(((*a)->originPoint + (*b)->originPoint) * 0.5);
    m_examinedPitchLengthInDrivingGear = glm::length(((*a)->originPoint + (*b)->originPoint) * 0.5);
    m_examinedPitchStopDirection = glm::rotate(m_examinedPitchStartDirection, m_angularPitchRotation);
    return true;
}

void ContactPointManager::copyPointsInSuitableLists() {
    //Examine the position of each point relative to the examined pitch
    //The points, which have a cut with the reference circle, are inserted in the first
    //list of m_contactPointsWithPositionList
    //Afterwards this list is copied and each point rotated, when one of the points do
    //not lie in the examined pitch

    //delete old points, if any exist
    deleteSortingLists();

    for(list<ContactPoint*> *l : m_insertedCPsLists) {
        ContactPointsWithPosition *firstPositionList = new ContactPointsWithPosition();
        firstPositionList->position = 0; // in examined angular pitch
        vector<int> foundPositions; //every position besides "0" is inserted here

        for(ContactPoint *cp : *l) {
            for(uint i = 0; i < 2; ++i) { //once for each: point and forbiddenAreaEndPoint
                vec2 examinedPoint = (i == 0) ? cp->point : cp->forbiddenAreaEndPoint;
                int pitchNumber = pitchNumberOfPoint(examinedPoint);

                if(pitchNumber != 0) {
                    bool alreadyFound = false;
                    for(uint i = 0; i < foundPositions.size() && !alreadyFound; ++i) {
                        if(pitchNumber == foundPositions[i])
                            alreadyFound = true;
                    }
                    if(!alreadyFound)
                        foundPositions.push_back(pitchNumber);
                }
            }
            firstPositionList->points.push_back(new ContactPoint(*cp));
        }
        m_contactPointsWithPositionList.push_back(firstPositionList); //insert last list, too, otherwise would be lost

        for(uint i = 0; i < foundPositions.size(); ++i) {
            ContactPointsWithPosition *contactPointsWithPosition = new ContactPointsWithPosition();
            contactPointsWithPosition->position = foundPositions[i];
            for(ContactPoint *cp : firstPositionList->points) {
                ContactPoint *copy = new ContactPoint(*cp);
                copy->rotate(m_angularPitchRotation * foundPositions[i]);
                contactPointsWithPosition->points.push_back(copy);
            }
            m_contactPointsWithPositionList.push_back(contactPointsWithPosition);
        }
    }
}

void ContactPointManager::reduceNumberOfNoneContactPoints() {
    if(m_noneContactPointList.empty())
        return;
    real epsilon = 3;
    list<NoneContactPoint*>::iterator   itAhead = m_noneContactPointList.begin(),
                                        itBehind = m_noneContactPointList.begin(),
                                        itEnd = m_noneContactPointList.end();
    uint middle = static_cast<uint>((*itAhead)->points.size() / 2);

    ++itAhead;
    while(itAhead != itEnd ) {
        vec2 distance = ((*itAhead)->points[middle] - (*itBehind)->points[middle]);
        if(glm::length(distance) < epsilon) {
            delete *itAhead;
            *itAhead = nullptr;
            itAhead = m_noneContactPointList.erase(itAhead); // => itAhead now points to next element!
        } else {
            itBehind = itAhead;
            ++itAhead;
        }
    }
}

void ContactPointManager::copyNoneContactPointsInRelevantPitches() {
    if(m_noneContactPointList.empty())
        return;
    list<NoneContactPoint*>::iterator it = m_noneContactPointList.begin();
    uint currentSize = m_noneContactPointList.size();
    uint middle = static_cast<uint>((*it)->points.size() / 2);
    if(middle < 2)
        return;

    for(uint i = 0; i < currentSize; ++i) {
        vec2 nearestPointToCenter = (*it)->points[middle];

        int pitch = pitchNumberOfPoint(nearestPointToCenter);

        for(int pitchAround = -1; pitchAround <= 1; ++pitchAround) {
            NoneContactPoint *copy = new NoneContactPoint(**it);
            copy->rotate(m_angularPitchRotation * (pitch + pitchAround));
            m_noneContactPointList.push_back(copy);
        }
        ++it;
    }
}

void ContactPointManager::findAllCoveredPoints() {
    clearAllGearLists();

    ContactPointIterator it;

    CPcutting cpCutting; NCPcutting ncpCutting;
    int cutting = findStartPointForGearPoints(cpCutting, ncpCutting);
    int normalDirection = (m_angularPitchRotation > 0.0) ? -1 : 1;
    if(cutting == 0) {
        it.startWith(cpCutting, normalDirection);
        m_gearPoints.push_back(cpCutting.cuttingPoint);
    } else {
        it.startWith(ncpCutting, normalDirection);
        m_gearPoints.push_back(ncpCutting.cuttingPoint);
    }
    m_gearPointsInformation.push_back(OriginInformation::Cut);
    m_gearPointsInformationIndex.push_back(-1);


    // Do while loop preparations, the conditions for the termination
    bool reachedOriginAgainCondition;
    bool reachedAListEndCondition;
    bool reachedSecurityBreakCondition;
    uint securityBreak = 0;
    uint securityBreakTreshold = 0;
    if(!m_noneContactPointList.empty())
        securityBreakTreshold += m_noneContactPointList.size() * m_noneContactPointList.front()->points.size(); //every NoneContactPoint has same amound of points
    for(ContactPointsWithPosition *contactPointsWithPosition : m_contactPointsWithPositionList) {
        securityBreakTreshold += contactPointsWithPosition->points.size();
    }
    Ray rayOfPitchEnd(vec2(0, 0), glm::rotate(m_examinedPitchStartDirection, m_angularPitchRotation));

    do {
        vector<CPcutting> cpCuttingsList;
        vector<ContinuationType> cpCuttingsTypes;
        vector<NCPcutting> ncpCuttingsList;
        vector<ContinuationType> ncpCuttingsTypes;
        cpCuttingsList.reserve(m_numberOfInsertedCPs); //only an estimation
        cpCuttingsTypes.reserve(m_numberOfInsertedCPs);
        ncpCuttingsList.reserve(4 * m_noneContactPointList.size()); // each path for a NoneContactPoint may in an unfortunate event have four cuttings with a line (cause of the form of the path like a loop)
        ncpCuttingsTypes.reserve(4 * m_noneContactPointList.size());

        uint occurencesCP = howManyContactPointsCutPreviousLine(it, cpCuttingsList, cpCuttingsTypes);
        uint occurencesNCP = howManyNoneContactPointsCutPreviousLine(it, ncpCuttingsList, ncpCuttingsTypes);

        if(occurencesCP + occurencesNCP == 0) {
            //point is not covered => it is a gear point
            m_gearPoints.push_back(it.currentPoint());
            if(it.isCurrentlyInCorrectCP()) {
                m_gearPointsInformation.push_back(OriginInformation::CP);
                m_gearPointsInformationIndex.push_back(m_gearCPs.size());
                m_gearCPs.push_back(it.currentCP());
            } else if(it.isCurrentlyInCorrectNCP()) {
                m_gearPointsInformation.push_back(OriginInformation::WCP);
                m_gearPointsInformationIndex.push_back(m_gearWCPs.size());
                m_gearWCPs.push_back(new WrongContactPoint(*(it.currentNCP())));
            } else {
                std::cout << "ATTENTION: found point with NOT CORRECT NCP AND NOT CORRECT CP!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
                m_gearPointsInformation.push_back(OriginInformation::SomethingElse);
                m_gearPointsInformationIndex.push_back(-2); //cutting
            }
            ++it;

        } else {
            //find first cutting
            uint firstCP = 0;
            for(uint i = 1; i < cpCuttingsList.size(); ++i) {
                if(cpCuttingsList[i].t < cpCuttingsList[firstCP].t)
                    firstCP = i;
            }
            uint firstNCP = 0;
            for(uint i = 1; i < ncpCuttingsList.size(); ++i) {
                if(ncpCuttingsList[i].t < ncpCuttingsList[firstNCP].t)
                    firstNCP = i;
            }

            if(occurencesNCP == 0 || (occurencesCP > 0 && cpCuttingsList[firstCP].t < ncpCuttingsList[firstNCP].t)) {
                //nearest cutting point is one of m_contactPointsWithPositionList
                m_gearPoints.push_back(cpCuttingsList[firstCP].cuttingPoint);
                it.continueWith(cpCuttingsList[firstCP], cpCuttingsTypes[firstCP]);

            } else { // nearest cutting point is one of m_noneContactPointList
                m_gearPoints.push_back(ncpCuttingsList[firstNCP].cuttingPoint);
                it.continueWith(ncpCuttingsList[firstNCP], ncpCuttingsTypes[firstNCP]);
            }
            m_gearPointsInformation.push_back(OriginInformation::Cut);
            m_gearPointsInformationIndex.push_back(-1); //cutting
        }

        if(it.reachedEnd()) {
            if(!it.tryToContinueWithOtherList(m_contactPointsWithPositionList) && it.isCurrentlyInCorrectCP()) {
                tryToSwitchToOtherList(it);
            }
        }

        ++securityBreak;
        reachedSecurityBreakCondition = (securityBreak > securityBreakTreshold);
        reachedAListEndCondition = it.reachedEnd();

        real epsilon = glm::length(m_gearPoints[0] - glm::rotate(it.previousPoint(), -m_angularPitchRotation));
        if(!reachedAListEndCondition) {
            vec2 intersection;
            if(rayOfPitchEnd.intersect(it.previousPoint(), it.currentPoint(), intersection, 0.0001)) {
                real intersectionEpsilon = glm::length(m_gearPoints[0] - glm::rotate(intersection, -m_angularPitchRotation));
                if(intersectionEpsilon < epsilon)
                    epsilon = intersectionEpsilon;
            }
        }
        reachedOriginAgainCondition = (epsilon < 1.5);

    } while(!reachedSecurityBreakCondition && !reachedOriginAgainCondition && !reachedAListEndCondition);

    assert(m_gearPoints.size() == m_gearPointsInformation.size());
    assert(m_gearPoints.size() == m_gearPointsInformationIndex.size());

    findBestPathOfContact(CalculationState::Simple);

    if(!reachedOriginAgainCondition)
        m_gearOutlineConstructionFailed = true;
    else
        m_gearOutlineConstructionFailed = false;

    m_gearPointsCreated.simple = true;
}

void ContactPointManager::findBestPathOfContact(CalculationState calcState) {
    vector<ContactPoint*> *contactPoints;
    Directions< vector<ContactPoint*> *> *bestContactPoints;
    Directions<vec2> *start;
    Directions<vec2> *stop;
    if(calcState == CalculationState::Simple) {
        contactPoints = &m_gearCPs;
        bestContactPoints = &m_gearBestContact;
        start = &m_gearBestContactStart;
        stop = &m_gearBestContactStop;

    } else { //CalculationState::BottomClearance
        contactPoints = &m_notTranslatedGearCPs;
        bestContactPoints = &m_notTranslatedBestContact;
        start = &m_translatedGearBestContactStart;
        stop = &m_translatedGearBestContactStop;
    }

    *bestContactPoints = Directions< vector<ContactPoint*> *>(nullptr, nullptr);
    real bestInClockCoverageAngle = 0;
    real bestCounterClockCoverageAngle = 0;

    if(contactPoints->empty())
        return;

    //Find an appropriate starting point
    uint startWith = 0;
    if(m_numberOfInsertedCPs > 0) {
        ContactPoint *previous = contactPoints->back();
        ContactPoint *next = contactPoints->front();
        while(((previous->evaluationStep + 1) % m_numberOfInsertedCPs == next->evaluationStep)
            && (cross(normalize(previous->contactPosition), previous->normalInContact)
                * cross(normalize(next->contactPosition), next->normalInContact) > 0.0) // same directions of normals
            && (startWith + 2 < contactPoints->size()))
        {
            previous = contactPoints->at(startWith);
            ++startWith;
            next = contactPoints->at(startWith);
        }
    }

    vector<ContactPoint*> currentDirectionPoints;
    ContactPoint *cp = contactPoints->at(startWith);
    currentDirectionPoints.push_back(cp);
    real direction = cross(normalize(cp->contactPosition), cp->normalInContact);
    vec2 begin, end;
    TurningDirection currentDirection = turningDirectionOf(direction); // 1 => normal points in clock direction (=> clockwise), -1 => normal points in counter clock direction, (always from view of driving gear)

    for(uint i = (startWith + 1) % contactPoints->size(); i != startWith; i = (i + 1) % contactPoints->size()) {
        ContactPoint *lastCP = cp;
        cp = contactPoints->at(i);
        TurningDirection direction = turningDirectionOf(cross(normalize(cp->contactPosition), cp->normalInContact));
        if((cp->evaluationStep == lastCP->evaluationStep + 1)
            && (direction == currentDirection))
        {
            currentDirectionPoints.push_back(cp);

        } else {
            // Direction or list changed => inspect currently used list and maybe use it as a new best list
            real currentCoverageAngle = contactPositionCoverageAngle(currentDirectionPoints, begin, end);

            // Current direction is clockwise
            if(currentDirection == TurningDirection::Clockwise && currentCoverageAngle > bestInClockCoverageAngle) {
                // replace best list with current list
                bestInClockCoverageAngle = currentCoverageAngle;
                if(bestContactPoints->clockwise != nullptr) {
                    bestContactPoints->clockwise->clear();
                    delete bestContactPoints->clockwise;
                }
                bestContactPoints->clockwise = new vector<ContactPoint*>(currentDirectionPoints);
                start->clockwise = begin;
                stop->clockwise = end;

            // Current direction is counter clockwise
            } else if(currentDirection == TurningDirection::CounterClockwise && currentCoverageAngle > bestCounterClockCoverageAngle) {
                // replace best list with current list
                bestCounterClockCoverageAngle = currentCoverageAngle;
                if(bestContactPoints->counterClockwise != nullptr) {
                    bestContactPoints->counterClockwise->clear();
                    delete bestContactPoints->counterClockwise;
                }
                bestContactPoints->counterClockwise = new vector<ContactPoint*>(currentDirectionPoints);
                start->counterClockwise = begin;
                stop->counterClockwise = end;
            }

            // Initialize for new list/direction
            currentDirectionPoints.clear();
            currentDirectionPoints.push_back(cp);
            currentDirection = direction;
        }
    }

    // Inspect last created currentDirectionPoints list
    real currentCoverageAngle = contactPositionCoverageAngle(currentDirectionPoints, begin, end);
    if(currentDirection == TurningDirection::Clockwise
        && (currentCoverageAngle > bestInClockCoverageAngle || bestContactPoints->clockwise == nullptr)) {
        bestInClockCoverageAngle = currentCoverageAngle;
        if(bestContactPoints->clockwise != nullptr) {
            bestContactPoints->clockwise->clear();
            delete bestContactPoints->clockwise;
        }
        bestContactPoints->clockwise = new vector<ContactPoint*>(currentDirectionPoints);
        start->clockwise = begin;
        stop->clockwise = end;
    } else if(currentDirection == TurningDirection::CounterClockwise
        && (currentCoverageAngle > bestCounterClockCoverageAngle || bestContactPoints->counterClockwise == nullptr)) {
        bestCounterClockCoverageAngle = currentCoverageAngle;
        if(bestContactPoints->counterClockwise != nullptr) {
            bestContactPoints->counterClockwise->clear();
            delete bestContactPoints->counterClockwise;
        }
        bestContactPoints->counterClockwise = new vector<ContactPoint*>(currentDirectionPoints);
        start->counterClockwise = begin;
        stop->counterClockwise = end;
    }
}

real ContactPointManager::contactPositionCoverageAngle(vector<ContactPoint*> &contactPoints, vec2 &start, vec2 &end) const {
    if(contactPoints.size() <= 1)
        return 0.0;

    //Search for first and last point taken the normal direction as reference
    ContactPoint *firstPoint = contactPoints.front();
    real direction = cross(normalize(firstPoint->contactPosition), firstPoint->normalInContact);;
    int listDirection = (direction < 0.0) ? -1 : 1;  //1 => clockwise, -1 => counter clockwise

    for(ContactPoint *cp : contactPoints) {
        if(cp != firstPoint) {
            if(cp->contactPosition.y * listDirection < firstPoint->contactPosition.y * listDirection) {
                firstPoint = cp;
            }
        }
    }
    ContactPoint *lastPoint = contactPoints.back();
    for(ContactPoint *cp : contactPoints) {
        if(cp != lastPoint) {
            if(cp->contactPosition.y * listDirection > lastPoint->contactPosition.y * listDirection) {
                lastPoint = cp;
            }
        }
    }
    start = firstPoint->contactPosition;
    end = lastPoint->contactPosition;
    return angleBetween(start, end);

}

uint ContactPointManager::numberOfNoneErrorContactPoints() const {
    uint foundPoints = 0;
    for(list<ContactPoint*> *l : m_insertedCPsLists) {
        for(ContactPoint *cp : *l) {
            if(cp->error == ErrorCode::NO_ERROR)
                ++foundPoints;
        }
    }
    return foundPoints;
}

uint ContactPointManager::findStartPointForGearPoints(CPcutting &cpCutting, NCPcutting &ncpCutting) {
    Ray ray(vec2(0, 0), m_examinedPitchStartDirection);

    vector<CPcutting> cpCuttingsList;
    for(ContactPointsWithPosition *contactPointsWithPosition : m_contactPointsWithPositionList) {
        for(vector<ContactPoint*>::iterator previous = contactPointsWithPosition->points.begin(),
                                            current = ++(contactPointsWithPosition->points.begin()),
                                            end = contactPointsWithPosition->points.end();
            current != end;
            ++previous, ++current)
        {
            vec2 intersection;
            //test ground:
            if(ray.intersect((*previous)->point, (*current)->point, intersection, 0.0001)) {
                cpCuttingsList.push_back(CPcutting{glm::length(intersection), intersection, previous, &(contactPointsWithPosition->points), IterationLocation::Ground});
            }
        }
    }
    uint firstCP = 0;
    for(uint i = 1; i < cpCuttingsList.size(); ++i) {
        if(cpCuttingsList[i].t < cpCuttingsList[firstCP].t)
            firstCP = i;
    }

    vector<NCPcutting> ncpCuttingsList;
    for(NoneContactPoint *ncp : m_noneContactPointList) {
        for(uint i = 1; i < ncp->points.size(); ++i) {
            vec2 previous = ncp->points[i - 1];
            vec2 current = ncp->points[i];

            vec2 intersection;
            //test ground:
            if(ray.intersect(previous, current, intersection, 0.0001)) {
                ncpCuttingsList.push_back(NCPcutting{glm::length(intersection), intersection, ncp, i - 1, IterationLocation::Ground});
            }
        }
    }
    uint firstNCP = 0;
    for(uint i = 1; i < ncpCuttingsList.size(); ++i) {
        if(ncpCuttingsList[i].t < ncpCuttingsList[firstNCP].t)
            firstNCP = i;
    }

    if(ncpCuttingsList.size() == 0 || cpCuttingsList[firstCP].t < ncpCuttingsList[firstNCP].t) {
        //correct first point is of CP
        cpCutting = cpCuttingsList[firstCP];
        return 0;
    } else {
        //correct first point is of NCP
        ncpCutting = ncpCuttingsList[firstNCP];
        return 1;
    }
}

bool ContactPointManager::tryToSwitchToOtherList(ContactPointIterator &it) {
    // Test the last possible CP in the list if its forbiddenArea has cuttings with a
    // connection of two other consecutive CPs or with two other consecutive points of a NCP
    // If so, take first cutting point and continue with that one.

    ContactPoint *cp = it.previousCP();
    //test with CPs:
    vector<CPcutting> cpCuttingsList;
    for(ContactPointsWithPosition *contactPointsWithPosition : m_contactPointsWithPositionList) {
        for(vector<ContactPoint*>::iterator previous = contactPointsWithPosition->points.begin(),
                                            current = ++(contactPointsWithPosition->points.begin()),
                                            end = contactPointsWithPosition->points.end();
            current != end;
            ++previous, ++current)
        {
            if(cp != *previous && cp != *current) { // do not test with same point
                real t;
                vec2 intersection;
                if(intersectLines(t, intersection, cp->point, cp->forbiddenAreaEndPoint, (*previous)->point, (*current)->point)) {
                    cpCuttingsList.push_back(CPcutting{t, intersection, previous, &(contactPointsWithPosition->points), IterationLocation::Ground});
                }
            }
        }
    }
    //test with NCPs:
    vector<NCPcutting> ncpCuttingsList;
    for(NoneContactPoint *ncp : m_noneContactPointList) {
        for(uint i = 1; i < ncp->points.size(); ++i) {
            real t;
            vec2 intersection;
            if(intersectLines(t, intersection, cp->point, cp->forbiddenAreaEndPoint, ncp->points[i - 1], ncp->points[i])) {
                ncpCuttingsList.push_back(NCPcutting{t, intersection, ncp, i - 1, IterationLocation::Ground});
            }
        }
    }
    if(cpCuttingsList.empty() && ncpCuttingsList.empty())
        return false;

    //find first cutting
    uint firstCP = 0;
    for(uint i = 1; i < cpCuttingsList.size(); ++i) {
        if(cpCuttingsList[i].t < cpCuttingsList[firstCP].t)
            firstCP = i;
    }
    uint firstNCP = 0;
    for(uint i = 1; i < ncpCuttingsList.size(); ++i) {
        if(ncpCuttingsList[i].t < ncpCuttingsList[firstNCP].t)
            firstNCP = i;
    }

    if(ncpCuttingsList.empty() || (!cpCuttingsList.empty() && cpCuttingsList[firstCP].t < ncpCuttingsList[firstNCP].t)) {
        //nearest cutting point is one of m_contactPointsWithPositionList
        m_gearPoints.push_back(cpCuttingsList[firstCP].cuttingPoint);
        it.continueWith(cpCuttingsList[firstCP], ContinuationType::HopOn);

    } else { // nearest cutting point is one of m_noneContactPointList
        m_gearPoints.push_back(ncpCuttingsList[firstNCP].cuttingPoint);
        it.continueWith(ncpCuttingsList[firstNCP], ContinuationType::HopOn);
    }
    m_gearPointsInformation.push_back(OriginInformation::Cut);
    m_gearPointsInformationIndex.push_back(-1); //cutting
    return true;
}


uint ContactPointManager::howManyContactPointsCutPreviousLine(const ContactPointIterator &it, vector<CPcutting> &cpCuttingsList, vector<ContinuationType> &cpCuttingsTypes) const {
    uint foundCoverings = 0;

    for(ContactPointsWithPosition *contactPointsWithPosition : m_contactPointsWithPositionList) {

        real t;
        vec2 intersection;
        ContactPoint *outermost;
        if(!it.belongsToQuad(*(contactPointsWithPosition->points.begin()), *(++(contactPointsWithPosition->points.begin())))) {
             outermost = contactPointsWithPosition->points.front();
            if(intersectLines(t, intersection, it.previousPoint(), it.currentPoint(), outermost->point, outermost->forbiddenAreaEndPoint)) {
                cpCuttingsList.push_back(CPcutting{t, intersection, contactPointsWithPosition->points.begin(), &(contactPointsWithPosition->points), IterationLocation::Ground});
                cpCuttingsTypes.push_back(ContinuationType::StartAgain);
                ++foundCoverings;
            }
        }
        if(!it.belongsToQuad(*(contactPointsWithPosition->points.end() - 1), *(contactPointsWithPosition->points.end() - 2))) {
            outermost = contactPointsWithPosition->points.back();
            if(intersectLines(t, intersection, it.previousPoint(), it.currentPoint(), outermost->point, outermost->forbiddenAreaEndPoint)) {
                cpCuttingsList.push_back(CPcutting{t, intersection, --(contactPointsWithPosition->points.end()), &(contactPointsWithPosition->points), IterationLocation::Ground});
                cpCuttingsTypes.push_back(ContinuationType::StartAgain);
                ++foundCoverings;
            }
        }

        for(vector<ContactPoint*>::iterator previous = contactPointsWithPosition->points.begin(),
                                            current = ++(contactPointsWithPosition->points.begin()),
                                            end = contactPointsWithPosition->points.end();
            current != end;
            ++previous, ++current)
        {
            if(!it.belongsToQuad(*previous, *current)) { // do not test the point with its own quad!

                real t;
                vec2 intersection;
                //test ground:
                if(intersectLines(t, intersection, it.previousPoint(), it.currentPoint(), (*previous)->point, (*current)->point)) {
                    cpCuttingsList.push_back(CPcutting{t, intersection, previous, &(contactPointsWithPosition->points), IterationLocation::Ground});
                    cpCuttingsTypes.push_back(ContinuationType::Default);
                    ++foundCoverings;
                }
            }
        }
    }
    return foundCoverings;
}

uint ContactPointManager::howManyNoneContactPointsCutPreviousLine(const ContactPointIterator &it, vector<NCPcutting> &ncpCuttingsList, vector<ContinuationType> &ncpCuttingsTypes) const {
    uint foundCoverings = 0;

    for(NoneContactPoint *ncp : m_noneContactPointList) {

        real t;
        vec2 intersection;
        vec2 outerMostForbiddenEndPoint;
        if(!it.belongsToQuad(ncp, 0, 1)) {
            outerMostForbiddenEndPoint = ncp->points[0] + ncp->normals[0] * ncp->forbiddenAreaLength;
            if(intersectLines(t, intersection, it.previousPoint(), it.currentPoint(), ncp->points[0], outerMostForbiddenEndPoint)) {
                ncpCuttingsList.push_back(NCPcutting{t, intersection, ncp, 0, IterationLocation::Ground});
                ncpCuttingsTypes.push_back(ContinuationType::StartAgain);
                ++foundCoverings;
            }
        }
        uint lastIndex = ncp->points.size() - 1;
        if(!it.belongsToQuad(ncp, lastIndex - 1, lastIndex)) {
            outerMostForbiddenEndPoint = ncp->points[lastIndex] + ncp->normals[lastIndex] * ncp->forbiddenAreaLength;
            if(intersectLines(t, intersection, it.previousPoint(), it.currentPoint(), ncp->points[lastIndex], outerMostForbiddenEndPoint)) {
                ncpCuttingsList.push_back(NCPcutting{t, intersection, ncp, lastIndex, IterationLocation::Ground});
                ncpCuttingsTypes.push_back(ContinuationType::StartAgain);
                ++foundCoverings;
            }
        }

        for(uint i = 1; i < ncp->points.size(); ++i) {
            if(!it.belongsToQuad(ncp, i - 1, i)) { // do not test the point with its own quad!
                real t;
                vec2 intersection;
                //test ground
                if(intersectLines(t, intersection, it.previousPoint(), it.currentPoint(), ncp->points[i - 1], ncp->points[i])) {
                    ncpCuttingsList.push_back(NCPcutting{t, intersection, ncp, i - 1, IterationLocation::Ground});
                    ncpCuttingsTypes.push_back(ContinuationType::Default);
                    ++foundCoverings;
                }

            }
        }
    }
    return foundCoverings;
}

    //  2 => 2 angular pitches before the examined one
    //  1 => 1 angular pitch   before the examined one
    //  0 => in the examined pitch
    // -1 => 1 angular pitch   behind the examined one
    // -2 => 2 angular pitches behind the examined one
int ContactPointManager::pitchNumberOfPoint(vec2 point) const {
    vec2 pointN = normalize(point);
    real alpha = angleBetweenN(m_examinedPitchStartDirection, pointN);
    real beta = angleBetweenN(m_examinedPitchStopDirection, pointN);
    real angularPitch = absolute(m_angularPitchRotation);
    // 3 cases:
    //      - in examined pitch, when alpha & beta both smaller than abs(m_angularPitchRotation)
    //      - before examined pitch, when beta > alpha (and not first case)
    //      - after examined pitch, when alpha > beta (an not first case)
    // If there is an uneven number of teeth, the points of the pitch opposite
    // of the examined one, will split, but that points out there should not happen anyway
    if(alpha < angularPitch && beta < angularPitch)
        return 0;
    if(alpha < beta) { // before angular pitch
        return (int)(beta / angularPitch);
    } else { // after angular pitch
        return -((int)(alpha / angularPitch));
    }

}

int ContactPointManager::whichPositionBehindAngularPitch(ContactPoint *contactPoint, const vec2 &stopPitch) const {
    vec2 a = stopPitch;
    vec2 b = glm::rotate(a, m_angularPitchRotation);

    bool pitchFound = false;
    int i = 0;
    do {
        i -= 1;
        m2x2 betweenAB = glm::inverse(m2x2(a, b));
        vec2 baryz = contactPoint->point * betweenAB;
        pitchFound = glm::all(glm::greaterThan(baryz, vec2(0, 0)));
        a = b;
        b = glm::rotate(a, m_angularPitchRotation);
    } while(!pitchFound);
    return i;
}

int ContactPointManager::whichPositionBeforeAngularPitch(ContactPoint *contactPoint, const vec2 &startPitch) const {
    vec2 b = startPitch;
    vec2 a = glm::rotate(b, -m_angularPitchRotation);

    bool pitchFound = false;
    int i = 0;
    do {
        i += 1;
        m2x2 betweenAB = glm::inverse(m2x2(a, b));
        vec2 baryz = contactPoint->point * betweenAB;
        pitchFound = glm::all(glm::greaterThan(baryz, vec2(0, 0)));
        b = a;
        a = glm::rotate(b, -m_angularPitchRotation);
    } while(!pitchFound);
    return i;
}

void ContactPointManager::fillLastCuttingPoints(
    uint foundCuttings,
    uint currentIndex,
    ContactPoint *lastContactPoint,
    ContactPoint *currentContactPoint,
    real bottomClearance,
    real maxAngle)
{
    if(foundCuttings == 0 || lastContactPoint == nullptr)
        return;

    bool lastWasTranslated = absolute(lastContactPoint->normalInContact.y) < sin(maxAngle);
    bool currentWasTranslated = absolute(currentContactPoint->normalInContact.y) < sin(maxAngle);

    if(!lastWasTranslated && !currentWasTranslated) {
        //all cutting points between stay the same as before (will not be translated)
        int startIndex = (int)currentIndex - (int)foundCuttings;
        for(uint i = 0; i < foundCuttings; ++i) {
            uint index;
            if(startIndex + (int)i >= 0)
                index = startIndex + i;
            else
                index = m_gearPoints.size() - (startIndex + i);
            m_translatedGearPoints.push_back(m_gearPoints[index]);
        }
    } else {
        //at least some of the cutting points will be translated
        real angleOfLast = asin(absolute(lastContactPoint->normalInContact.y));
        real angleOfCurrent = asin(absolute(currentContactPoint->normalInContact.y));

        vector<real> length(foundCuttings);
        real completeLength = 0.0;
        int startIndex = (int)currentIndex - (int)foundCuttings - 1;
        for(uint i = 0; i <= foundCuttings; ++i) {
            uint index;
            if(startIndex + (int)i >= 0)
                index = startIndex + i;
            else
                index = m_gearPoints.size() - (startIndex + i);
            completeLength += glm::length(m_gearPoints[index + 1] - m_gearPoints[index]);
            if(i < foundCuttings)
                length.push_back(completeLength);
        }

        real angleCoefficient = (angleOfCurrent - angleOfLast) / completeLength;
        vec2 normalCoefficient = (currentContactPoint->normal - lastContactPoint->normal) * (1.0 / completeLength);
        startIndex = (int)currentIndex - (int)foundCuttings;
        for(uint i = 0; i < foundCuttings; ++i) {
            uint index;
            if(startIndex + (int)i >= 0)
                index = startIndex + i;
            else
                index = m_gearPoints.size() - (startIndex + i);
            real angle = angleOfLast + angleCoefficient * length[i];
            if(absolute(angle) < maxAngle) {
                //translate point
                real t = 1.0 - (absolute(angle) / maxAngle);
                vec2 normal = lastContactPoint->normal + normalCoefficient * length[i];
                m_translatedGearPoints.push_back(m_gearPoints[index] - t * bottomClearance * normal); // walk in opposite direction of the normal
            } else {
                //do not translate
                m_translatedGearPoints.push_back(m_gearPoints[index]);
            }
        }
    }
}

bool ContactPointManager::pointIsCovered(const vec2 &candidate, const ContactPoint &a, const ContactPoint &b) const {
    // in each case two tests to make
    vec2 intersection;
    if(intersectLines(intersection, a.point, a.forbiddenAreaEndPoint, b.point, b.forbiddenAreaEndPoint)) {
        return isPointInTriangle(candidate, a.point, intersection, b.point)
            || isPointInTriangle(candidate, a.forbiddenAreaEndPoint, intersection, b.forbiddenAreaEndPoint);
    } else {
        return isPointInTriangle(candidate, b.forbiddenAreaEndPoint, a.point, a.forbiddenAreaEndPoint)
            || isPointInTriangle(candidate, b.point, a.point, b.forbiddenAreaEndPoint);
        }
}

bool ContactPointManager::contactPointIsCovered(const ContactPoint &candidate, const ContactPoint &a, const ContactPoint &b) const {
    // in each case two tests to make!
    vec2 intersection;
    if(intersectLines(intersection, a.point, a.forbiddenAreaEndPoint, b.point, b.forbiddenAreaEndPoint)) {
        return isPointInTriangle(candidate.point, a.point, intersection, b.point)
            || isPointInTriangle(candidate.point, a.forbiddenAreaEndPoint, intersection, b.forbiddenAreaEndPoint);
    } else {
        return isPointInTriangle(candidate.point, b.forbiddenAreaEndPoint, a.point, a.forbiddenAreaEndPoint)
            || isPointInTriangle(candidate.point, b.point, a.point, b.forbiddenAreaEndPoint);
        }
}

bool ContactPointManager::intersectLines(real &intersectionValue, vec2 &intersection, vec2 startLine, vec2 stopLine, vec2 startTestLine, vec2 stopTestLine) const {
    vec2 x = startTestLine - stopTestLine;
    vec2 y = stopLine - startLine;
    vec2 z = startTestLine - startLine;
    m2x2 matrix = m2x2(x, y);
    if(glm::determinant(matrix) != 0) {
        m2x2 invMatrix = glm::inverse(matrix);
        vec2 t = invMatrix * z;
        if (0.0001 < t.x && 0.0001 < t.y && t.x < 0.9999 && t.y < 0.9999) {
            intersectionValue = t.y;
            intersection = startLine + t.y * y;
            return true;
        }
    }
    return false;
}

bool ContactPointManager::intersectLines(vec2 &intersection, vec2 lineAStart, vec2 lineAEnd, vec2 lineBStart, vec2 lineBEnd) const {
    vec2 x = lineAStart - lineAEnd;
    vec2 y = lineBEnd - lineBStart;
    vec2 z = lineAStart - lineBStart;
    m2x2 matrix = m2x2(x, y);
    if(glm::determinant(matrix) != 0) {
        m2x2 invMatrix = glm::inverse(matrix);
        vec2 t = invMatrix * z;
        if (0 <= t.x && 0 <= t.y && t.x <= 1 && t.y <= 1) {
            intersection = lineBStart + t.y * y;
            return true;
        }
    }
    return false;
}

bool ContactPointManager::isPointInTriangle(vec2 point, vec2 a, vec2 b, vec2 c) const {
    vec2 start = a - b;
    vec2 stop = c - b;
    m2x2 betweenStartStop = glm::inverse(m2x2(start, stop));
    vec2 baryz = betweenStartStop * (point - b);
    bool isInTriangle;
    if(glm::all(glm::greaterThan(baryz, vec2(0, 0))) && glm::all(glm::lessThan(baryz, vec2(1.0, 1.0))) && (baryz.x + baryz.y < 1.0)) {
        isInTriangle = true;
    } else {
        isInTriangle = false;
    }
    return isInTriangle;
}

void ContactPointManager::eraseEmptyAndOneEntryLists() {
    for(list< list<ContactPoint*>* >::iterator listIt = m_insertedCPsLists.begin(); listIt != m_insertedCPsLists.end(); ++listIt) {
        if((*listIt)->size() < 2) {
            //clear and delete the list
            if((*listIt)->empty()) {
                delete *listIt;
                *listIt = nullptr;
            } else { //list size == 1
                delete (*listIt)->front(); //delete the ContactPoint
                (*listIt)->clear();
                delete *listIt; //delete the list itself
                *listIt = nullptr;
            }
            listIt = m_insertedCPsLists.erase(listIt); //listIt points to next list now
            --listIt; //as listIt is increased when starting again the loop, decrease it now
        }
    }
}

void ContactPointManager::deleteInsertedContactPoints() {
    for(list<ContactPoint*> *l : m_insertedCPsLists) {
        for(ContactPoint *cp : (*l)) {
            delete cp;
        }
        delete l;
        l = nullptr;
    }
    m_insertedCPsLists.clear();
}

void ContactPointManager::deleteNoneContactPoints() {
    for(list<NoneContactPoint*>::iterator it = m_noneContactPointList.begin(); it != m_noneContactPointList.end(); ++it)
        delete *it;
    m_noneContactPointList.clear();
}

void ContactPointManager::deleteSortingLists() {
    for(ContactPointsWithPosition *list : m_contactPointsWithPositionList) {
        for(ContactPoint *point : list->points) {
            delete point;
        }
        list->points.clear();
        delete list;
        list = nullptr;
    }
    m_contactPointsWithPositionList.clear();
}

void ContactPointManager::clearAllGearLists() {
    m_gearPointsInformationIndex.clear();
    m_gearPointsInformation.clear();
    m_gearPoints.clear();
    m_gearCPs.clear();
    for(WrongContactPoint *wcp : m_gearWCPs) {
        delete wcp;
    }
    m_gearWCPs.clear();
    if(m_gearBestContact.clockwise != nullptr) {
        m_gearBestContact.clockwise->clear();
        delete m_gearBestContact.clockwise;
        m_gearBestContact.clockwise = nullptr;
    }
    if(m_gearBestContact.counterClockwise != nullptr) {
        m_gearBestContact.counterClockwise->clear();
        delete m_gearBestContact.counterClockwise;
        m_gearBestContact.counterClockwise = nullptr;
    }
    m_gearBestContactStart = Directions<vec2>(vec2(0, 0), vec2(0, 0));
    m_gearBestContactStop = Directions<vec2>(vec2(0, 0), vec2(0, 0));
}

void ContactPointManager::clearAllTranslatedGearLists() {
    m_translatedGearPoints.clear();
    m_notTranslatedGearCPs.clear();
    m_notTranslatedGearWCPs.clear();
    if(m_notTranslatedBestContact.clockwise != nullptr) {
        m_notTranslatedBestContact.clockwise->clear();
        delete m_notTranslatedBestContact.clockwise;
        m_notTranslatedBestContact.clockwise = nullptr;
    }
    if(m_notTranslatedBestContact.counterClockwise != nullptr) {
        m_notTranslatedBestContact.counterClockwise->clear();
        delete m_notTranslatedBestContact.counterClockwise;
        m_notTranslatedBestContact.counterClockwise = nullptr;
    }
    m_translatedGearBestContactStart = Directions<vec2>(vec2(0, 0), vec2(0, 0));
    m_translatedGearBestContactStop = Directions<vec2>(vec2(0, 0), vec2(0, 0));
}
