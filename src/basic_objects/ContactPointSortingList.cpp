#include "basic_objects/ContactPointSortingList.h"

#include "glm/glm.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "helpers.h"
#include "basic_objects/Ray.h"

ContactPointSortingList::ContactPointSortingList() {}

ContactPointSortingList::~ContactPointSortingList() {
    deleteSortingLists();
    deleteNoneContactPoints();
    for(std::list<ContactPoint*>::iterator it = begin(); it != end(); ++it)
        delete *it;
}

void ContactPointSortingList::createCoveringLists(uint numberOfTeeth, bool isDescribedClockwise) {
    if(size() == 0 || numberOfNoneErrorContactPoints() == 0)
        return;

    setBackLists();
    setAngularPitch(numberOfTeeth, isDescribedClockwise);
        //=> m_angularPitchRotation
    sort(); // Sort all points, normally they should already be sorted, but this also inserts the sequential number
        //=> each CP has cp.evaluationStep
    if(!setExaminedPitch())
        return;
    copyPointsInSuitableLists();
        //=> m_contactPointsWithPositionList, m_noneContactPointList,
        //=> m_examinedPitchStartDirection, m_examinedPitchStopDirection, m_examinedPitchStartDirectionInDrivingGear
        //=> m_examinedPitchLengthInDrivenGear, m_examinedPitchLengthInDrivingGear
    reduceNumberOfNoneContactPoints(); //needs a filled m_noneContactPointList
    copyNoneContactPointsInRelevantPitches();
    findAllCoveredPoints();
        //=> m_gearPoints, m_gearCPs, m_gearNotCorrectCPCounter
}

void ContactPointSortingList::sort() {
    bool sorted = true;

    do {
        iterator itPrev = this->begin();
        iterator itCurr = this->begin();
        ++itCurr;
        iterator end = this->end();
        uint number = 0;

        while(sorted && itCurr != end) {
            if((*itPrev)->evaluationValue > (*itCurr)->evaluationValue) { // not sorted ascending! Swap them
                ContactPoint *temp = (*itPrev);
                (*itPrev) = (*itCurr);
                (*itCurr) = temp;
                sorted = false;
            } else {
                (*itPrev)->evaluationStep = number;
                ++number;
                ++itPrev;
                ++itCurr;
            }
        }
        (*itPrev)->evaluationStep = number; //last ContactPoint needs number, too
    } while(!sorted);
}

void ContactPointSortingList::clear() {
    for(ContactPoint *cp : (*this))
        delete cp;
    std::list<ContactPoint*>::clear();
}

const std::list<ContactPointsWithPosition*>& ContactPointSortingList::contactPointsWithPositions() const {
    return m_contactPointsWithPositionList;
}

const std::list<NoneContactPoint*>& ContactPointSortingList::noneContactPoints() const {
    return m_noneContactPointList;
}

const vector<vec2>& ContactPointSortingList::gearPoints() const {
    return m_gearPoints;
}

const vector<ContactPoint*>& ContactPointSortingList::gearContactPoints() const {
    return m_gearCPs;
}

uint ContactPointSortingList::numberOfNotCorrectContactPoints() const {
    return m_gearNotCorrectCPCounter;
}

vec2 ContactPointSortingList::startOfExaminedPitchInDrivenGear() const {
    return m_examinedPitchStartDirection;
}

vec2 ContactPointSortingList::endOfExaminedPitchInDrivenGear() const {
    return m_examinedPitchStopDirection;
}

vec2 ContactPointSortingList::startOfExaminedPitchInDrivingGear () const {
    return m_examinedPitchStartDirectionInDrivingGear;
}

real ContactPointSortingList::lengthOfPitchesInDrivenGear() const {
    return m_examinedPitchLengthInDrivenGear;
}

real ContactPointSortingList::lengthOfPitchesInDrivingGear() const {
    return m_examinedPitchLengthInDrivingGear;
}

real ContactPointSortingList::usedAngularPitch() const {
    return m_angularPitchRotation;
}

///////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
// METHODS FOR ALGORITHM //////////////////////////////

void ContactPointSortingList::setAngularPitch(uint numberOfTeeth, bool isDescribedClockwise) {
    real turningDirection = ((isDescribedClockwise) ? 1.0 : -1.0);
    m_angularPitchRotation = (2.0 * M_PI * turningDirection) / (real)numberOfTeeth;
}

bool ContactPointSortingList::setExaminedPitch() {
    if(empty())
        return false;
    iterator a = ++(begin());
    iterator b = begin();

    bool errorProne = true;
    while(errorProne && a != end()) {
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

void ContactPointSortingList::copyPointsInSuitableLists() {
    //Examine the position of each point relative to the examined pitch
    //The points, which have a cut with the reference circle, are inserted in the first
    //list of m_contactPointsWithPositionList
    //Afterwards this list is copied and each point rotated, when one of the points do
    //not lie in the examined pitch

    ContactPointsWithPosition *firstPositionList = new ContactPointsWithPosition();
    firstPositionList->position = 0; // in examined angular pitch
    vector<int> foundPositions; //every position besides "0" is inserted here

    for(ContactPoint *cp : (*this)) {
        if(cp->error != ErrorCode::NO_CUT_WITH_REFERENCE_RADIUS) {
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

        } else { // point on original gear has no equivalent on mating gear => examine the whole path of the point later
            m_noneContactPointList.push_back(new NoneContactPoint(*(static_cast<NoneContactPoint*>(cp))));
        }
    }
    m_contactPointsWithPositionList.push_back(firstPositionList); //insert last list, too. Otherwise will be lost

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

void ContactPointSortingList::reduceNumberOfNoneContactPoints() {
    if(m_noneContactPointList.empty())
        return;
    real epsilon = 3;
    std::list<NoneContactPoint*>::iterator  itAhead = m_noneContactPointList.begin(),
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

void ContactPointSortingList::copyNoneContactPointsInRelevantPitches() {
    if(m_noneContactPointList.empty())
        return;
    std::list<NoneContactPoint*>::iterator it = m_noneContactPointList.begin();
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

void ContactPointSortingList::findAllCoveredPoints() {
    m_gearPoints.clear();
    m_gearCPs.clear();
    m_gearNotCorrectCPCounter = 0;

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

    // Do while loop preparations, the conditions for the termination
    bool notYetAtOriginCondition;
    bool notAtListEndCondition;
    bool securityBreakCondition;
    uint securityBreak = 0;
    uint securityBreakTreshold = 0;
    if(!m_noneContactPointList.empty())
        securityBreakTreshold += m_noneContactPointList.size() * m_noneContactPointList.front()->points.size(); //every NoneContactPoint has same amound of points
    for(ContactPointsWithPosition *contactPointsWithPosition : m_contactPointsWithPositionList) {
        securityBreakTreshold += 2 * contactPointsWithPosition->points.size();
    }
    Ray rayOfPitchEnd(vec2(0, 0), glm::rotate(m_examinedPitchStartDirection, m_angularPitchRotation));

    do {
        std::vector<CPcutting> cpCuttingsList;
        std::vector<NCPcutting> ncpCuttingsList;
        cpCuttingsList.reserve(this->size()); //only an estimation
        ncpCuttingsList.reserve(4 * m_noneContactPointList.size()); // each path for a NoneContactPoint may in an unfortunate event have four cuttings with a line (cause of the form of the path like a loop)

        uint occurencesCP = howManyContactPointsCoverPoint(it, cpCuttingsList);
        uint occurencesNCP = howManyNoneContactPointsCoverPoint(it, ncpCuttingsList);

        if(occurencesCP + occurencesNCP == 0) {
            //point is not covered => it is a gear point
            m_gearPoints.push_back(it.currentPoint());
            ContactPoint *current = it.currentCorrectInContactPoint();
            if(current != nullptr)
                m_gearCPs.push_back(current);
            else
                ++m_gearNotCorrectCPCounter; //current point is a NCP or a forbiddenAreaEndPoint
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
                it.continueWith(cpCuttingsList[firstCP]);

            } else { // nearest cutting point is one of m_noneContactPointList
                m_gearPoints.push_back(ncpCuttingsList[firstNCP].cuttingPoint);
                it.continueWith(ncpCuttingsList[firstNCP]);
            }
        }

        if(it.reachedEnd())
            it.tryToContinueWithOtherList(m_contactPointsWithPositionList);

        ++securityBreak;
        securityBreakCondition = (securityBreak <= securityBreakTreshold);
        notAtListEndCondition = !it.reachedEnd();

        real epsilon = glm::length(m_gearPoints[0] - glm::rotate(it.previousPoint(), -m_angularPitchRotation));
        if(notAtListEndCondition) {
            vec2 intersection;
            if(rayOfPitchEnd.intersect(it.previousPoint(), it.currentPoint(), intersection, 0.0001)) {
                real intersectionEpsilon = glm::length(m_gearPoints[0] - glm::rotate(intersection, -m_angularPitchRotation));
                if(intersectionEpsilon < epsilon)
                    epsilon = intersectionEpsilon;
            }
        }
        notYetAtOriginCondition = (epsilon > 1.5);

    } while(securityBreakCondition && notYetAtOriginCondition && notAtListEndCondition);

    std::cout << "\n\nAfter while loop and securityBreakCondition:  " << securityBreakCondition << "   securityBreak = " << securityBreak << " of treshold: " << securityBreakTreshold << std::endl;
    std::cout << "                 and notYetAtOriginCondition: " << notYetAtOriginCondition << std::endl;
    std::cout << "                 and notAtListEndCondition:   " << notAtListEndCondition << std::endl;
    std::cout << "        distance from first to last point if rotated: " << glm::length(m_gearPoints[0] - glm::rotate(m_gearPoints[m_gearPoints.size() - 1], -m_angularPitchRotation));
    std::cout << "GearPoints have a size of: " << m_gearPoints.size() << std::endl;

}

uint ContactPointSortingList::numberOfNoneErrorContactPoints() const {
    uint foundPoints = 0;
    for(ContactPoint *cp : (*this)) {
        if(cp->error == ErrorCode::NO_ERROR)
            ++foundPoints;
    }
    return foundPoints;
}

ContactPoint* ContactPointSortingList::getFirstNoneErrorContactPoint() const {
    const_iterator it = begin();
    while((*it)->error != ErrorCode::NO_ERROR && it != end())
        ++it;
    if(it == end())
        return nullptr;
    return *it;
}

uint ContactPointSortingList::findStartPointForGearPoints(CPcutting &cpCutting, NCPcutting &ncpCutting) {
    Ray ray(vec2(0, 0), m_examinedPitchStartDirection);

    std::vector<CPcutting> cpCuttingsList;
    for(ContactPointsWithPosition* contactPointsWithPosition : m_contactPointsWithPositionList) {
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
            //test top:
            if(ray.intersect((*previous)->forbiddenAreaEndPoint, (*current)->forbiddenAreaEndPoint, intersection, 0.0001)) {
                cpCuttingsList.push_back(CPcutting{glm::length(intersection), intersection, previous, &(contactPointsWithPosition->points), IterationLocation::Top});
            }
        }
    }
    uint firstCP = 0;
    for(uint i = 1; i < cpCuttingsList.size(); ++i) {
        if(cpCuttingsList[i].t < cpCuttingsList[firstCP].t)
            firstCP = i;
    }

    std::vector<NCPcutting> ncpCuttingsList;
    for(NoneContactPoint *ncp : m_noneContactPointList) {
        for(uint i = 1; i < ncp->points.size(); ++i) {
            vec2 previous = ncp->points[i - 1];
            vec2 current = ncp->points[i];
            vec2 previousEndPoint = previous + ncp->forbiddenAreaLength * ncp->normals[i - 1];
            vec2 currentEndPoint = current + ncp->forbiddenAreaLength * ncp->normals[i];

            vec2 intersection;
            //test ground:
            if(ray.intersect(previous, current, intersection, 0.0001)) {
                ncpCuttingsList.push_back(NCPcutting{glm::length(intersection), intersection, ncp, i - 1, IterationLocation::Ground});
            }
            //test top:
            if(ray.intersect(previousEndPoint, currentEndPoint, intersection, 0.0001)) {
                ncpCuttingsList.push_back(NCPcutting{glm::length(intersection), intersection, ncp, i - 1, IterationLocation::Top});
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

uint ContactPointSortingList::howManyContactPointsCoverPoint(const ContactPointIterator &it, std::vector<CPcutting> &cpCuttingsList) const {
    uint foundCoverings = 0;

    for(ContactPointsWithPosition* contactPointsWithPosition : m_contactPointsWithPositionList) {
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
                    ++foundCoverings;
                }
                //test top:
                if(intersectLines(t, intersection, it.previousPoint(), it.currentPoint(), (*previous)->forbiddenAreaEndPoint, (*current)->forbiddenAreaEndPoint)) {
                    cpCuttingsList.push_back(CPcutting{t, intersection, previous, &(contactPointsWithPosition->points), IterationLocation::Top});
                    ++foundCoverings;
                }
            }
        }
    }
    return foundCoverings;
}

uint ContactPointSortingList::howManyNoneContactPointsCoverPoint(const ContactPointIterator &it, std::vector<NCPcutting> &ncpCuttingsList) const {
    uint foundCoverings = 0;

    for(NoneContactPoint *ncp : m_noneContactPointList) {
        for(uint i = 1; i < ncp->points.size(); ++i) {
            if(!it.belongsToQuad(ncp, i - 1, i)) { // do not test the point with its own quad!

                vec2 previous = ncp->points[i - 1];
                vec2 current = ncp->points[i];
                vec2 previousEndPoint = previous + ncp->forbiddenAreaLength * ncp->normals[i - 1];
                vec2 currentEndPoint = current + ncp->forbiddenAreaLength * ncp->normals[i];

                real t;
                vec2 intersection;
                if(intersectLines(t, intersection, it.previousPoint(), it.currentPoint(), previous, current)) {
                    ncpCuttingsList.push_back(NCPcutting{t, intersection, ncp, i - 1, IterationLocation::Ground});
                    ++foundCoverings;
                }
                if(intersectLines(t, intersection, it.previousPoint(), it.currentPoint(), previousEndPoint, currentEndPoint)) {
                    ncpCuttingsList.push_back(NCPcutting{t, intersection, ncp, i - 1, IterationLocation::Top});
                    ++foundCoverings;
                }

            }
        }
    }
    return foundCoverings;
}

void ContactPointSortingList::copyInCorrectList(const ContactPoint &cp, int position, ContactPointsWithPosition *&list) {
    //TODO: at the moment every point is inserted also in the first list, this is not correct!
    //      it is only here for testing reasons.

    bool isFirstList = m_contactPointsWithPositionList.empty();
    if(!isFirstList)
            m_contactPointsWithPositionList.front()->points.push_back(new ContactPoint(cp)); //insert every cp in first list, undo that!

    list->points.push_back(new ContactPoint(cp));

    if(list->position != position) { // open a new list and insert the contact point cp and his predecessor to the list

        bool hasPredecessor = list->points.size() > 1;
        ContactPoint *predecessor = nullptr;
        if(hasPredecessor) {
            predecessor = new ContactPoint(**(list->points.end() - 2)); // second to last contact point, as we have already inserted the current one
        }
        m_contactPointsWithPositionList.push_back(list);
        list = new ContactPointsWithPosition();
        list->position = position;
        if(hasPredecessor)
            list->points.push_back(predecessor);
        list->points.push_back(new ContactPoint(cp));
    }
}

    //  2 => 2 angular pitches before the examined one
    //  1 => 1 angular pitch   before the examined one
    //  0 => in the examined pitch
    // -1 => 1 angular pitch   behind the examined one
    // -2 => 2 angular pitches behind the examined one

int ContactPointSortingList::pitchNumberOfPoint(vec2 point) const {
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

int ContactPointSortingList::whichPositionBehindAngularPitch(ContactPoint *contactPoint, const vec2 &stopPitch) const {
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

int ContactPointSortingList::whichPositionBeforeAngularPitch(ContactPoint *contactPoint, const vec2 &startPitch) const {
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

bool ContactPointSortingList::pointIsCovered(const vec2 &candidate, const ContactPoint &a, const ContactPoint &b) const {
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

bool ContactPointSortingList::contactPointIsCovered(const ContactPoint &candidate, const ContactPoint &a, const ContactPoint &b) const {
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

bool ContactPointSortingList::intersectLines(real &intersectionValue, vec2 &intersection, vec2 startLine, vec2 stopLine, vec2 startTestLine, vec2 stopTestLine) const {
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

bool ContactPointSortingList::intersectLines(vec2 &intersection, vec2 lineAStart, vec2 lineAEnd, vec2 lineBStart, vec2 lineBEnd) const {
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

// TODO: is the epsilon value e a suitable one? Maybe it catches too much or too less?
bool ContactPointSortingList::isPointInTriangle(vec2 point, vec2 a, vec2 b, vec2 c) const {
    real e = 0.00f;
    vec2 start = a - b;
    vec2 stop = c - b;
    m2x2 betweenStartStop = glm::inverse(m2x2(start, stop));
    vec2 baryz = betweenStartStop * (point - b);
    bool isInTriangle;
    if(glm::all(glm::greaterThan(baryz, vec2(e,e))) && glm::all(glm::lessThan(baryz, vec2(1.0f - e, 1.0f - e))) && (baryz.x + baryz.y < 1.0f - 2 * e)) {
        isInTriangle = true;
    } else {
        isInTriangle = false;
    }
    return isInTriangle;
}

void ContactPointSortingList::setBackLists() {
    deleteSortingLists();
    deleteNoneContactPoints();
}

void ContactPointSortingList::deleteSortingLists() {
    for(ContactPointsWithPosition *list : m_contactPointsWithPositionList) {
        for(ContactPoint *point : list->points) {
            delete point;
        }
        list->points.clear();
        delete list;
    }
    m_contactPointsWithPositionList.clear();
}

void ContactPointSortingList::deleteNoneContactPoints() {
    for(std::list<NoneContactPoint*>::iterator it = m_noneContactPointList.begin(); it != m_noneContactPointList.end(); ++it)
        delete *it;
    m_noneContactPointList.clear();
}
