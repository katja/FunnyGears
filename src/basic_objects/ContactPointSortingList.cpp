#include "ContactPointSortingList.h"

#include "glm/glm.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "helpers.h"

#include <iostream>

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
        //=> m_pointsWithPositionList, m_noneContactPointList, m_examinedPitchStartDirection, m_examinedPitchStopDirection
    reduceNumberOfNoneContactPoints(); //needs a filled m_noneContactPointList
    copyNoneContactPointsInRelevantPitches();
    // rotatePointsWithPositionToOnePitch();
    findAllCoveredPoints();
        //=> m_gearPoints, m_gearCPs
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
    } while(!sorted);
}

void ContactPointSortingList::clear() {
    for(ContactPoint *cp : (*this))
        delete cp;
    std::list<ContactPoint*>::clear();
}

const std::list<PointsWithPosition*>& ContactPointSortingList::pointsWithPositions() const {
    return m_pointsWithPositionList;
}

const std::list<NoneContactPoint*>& ContactPointSortingList::noneContactPoints() const {
    return m_noneContactPointList;
}

const std::list<Triangle>& ContactPointSortingList::triangles() {
    return m_triangles;
}

vector<vec2> ContactPointSortingList::gearPoints() const {
    return m_gearPoints;
}

vec2 ContactPointSortingList::startOfExaminedPitch() const {
    return m_examinedPitchStartDirection;
}

vec2 ContactPointSortingList::endOfExaminedPitch() const {
    return m_examinedPitchStopDirection;
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
    m_examinedPitchStopDirection = glm::rotate(m_examinedPitchStartDirection, m_angularPitchRotation);
    return true;
}

void ContactPointSortingList::copyPointsInSuitableLists() {
    //Examine the position of each point relative to the examined pitch
    //The points, which have a cut with the reference circle, are inserted in the first
    //list of m_pointsWithPositionList
    //Afterwards this list is copied and each point rotated, when one of the points do
    //not lie in the examined pitch

    PointsWithPosition *firstPositionList = new PointsWithPosition();
    firstPositionList->position = 0; // in examined angular pitch
    vector<int> foundPositions; //every position besides "0" is inserted here

    for(ContactPoint *cp : (*this)) {
        if(cp->error != ErrorCode::NO_CUT_WITH_REFERENCE_RADIUS) {
            int pitchNumber = pitchNumberOfPoint(cp->point);
            if(pitchNumber != 0) {
                bool alreadyFound = false;
                for(uint i = 0; i < foundPositions.size() && !alreadyFound; ++i) {
                    if(pitchNumber == foundPositions[i])
                        alreadyFound = true;
                }
                if(!alreadyFound)
                    foundPositions.push_back(pitchNumber);
            }
            firstPositionList->points.push_back(new ContactPoint(*cp));

        } else { // point on original gear has no equivalent on mating gear => examine the whole path of the point later
            m_noneContactPointList.push_back(new NoneContactPoint(*(static_cast<NoneContactPoint*>(cp))));
        }
    }
    m_pointsWithPositionList.push_back(firstPositionList); //insert last list, too. Otherwise will be lost

    std::cout << "copyPointsInSuitableLists had found the positions: " << foundPositions << std::endl;

    for(uint i = 0; i < foundPositions.size(); ++i) {
        PointsWithPosition *pointsWithPosition = new PointsWithPosition();
        pointsWithPosition->position = foundPositions[i];
        for(ContactPoint *cp : firstPositionList->points) {
            ContactPoint *copy = new ContactPoint(*cp);
            copy->rotate(m_angularPitchRotation * foundPositions[i]);
            pointsWithPosition->points.push_back(copy);
        }
        m_pointsWithPositionList.push_back(pointsWithPosition);
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

void ContactPointSortingList::rotatePointsWithPositionToOnePitch() {
    // Turn the points of the lists infront and behind of the examined angular pitch
    // Afterwards all lists are mainly in the angular pitch
    for(PointsWithPosition *pointsWithPosition : m_pointsWithPositionList) {
        if(pointsWithPosition->position != 0) {
            real rotationAngle = m_angularPitchRotation * (real)pointsWithPosition->position;
            for(vector<ContactPoint*>::iterator it = pointsWithPosition->points.begin(), end = pointsWithPosition->points.end(); it != end; ++it) {
                (*it)->rotate(rotationAngle);
            }
        }
    }
}

void ContactPointSortingList::findAllCoveredPoints() {

    m_gearPoints.clear();
    m_gearCPs.clear();

    vector<ContactPoint*>::iterator firstIterator;
    vector<ContactPoint*> *firstPoints = nullptr;
    if(!getFirstNoneErrorCPIterator(firstIterator, firstPoints))
        return;

    vec2 startPitch = glm::normalize((*firstIterator)->point);
    vec2 stopPitch = glm::rotate(startPitch, m_angularPitchRotation);
    m2x2 betweenStartStop = glm::inverse(m2x2(startPitch, stopPitch));

    ContactPointIterator it;
    it.startWith(firstIterator, firstPoints);
    //TODO: this may be rubbish, if first point is covered!!!
    m_gearPoints.push_back((*firstIterator)->point);
    m_gearCPs.push_back(*firstIterator);
    ++it;

    // Do while loop preparations, the conditions for the termination
    bool notYetAtOriginCondition;
    bool notAtListEndCondition;
    bool securityBreakCondition;
    uint securityBreak = 0;
    uint securityBreakTreshold = 0;
    if(!m_noneContactPointList.empty())
        securityBreakTreshold += m_noneContactPointList.size() * m_noneContactPointList.front()->points.size(); //every NoneContactPoint has same amound of points
    for(PointsWithPosition *pointsWithPosition : m_pointsWithPositionList) {
        securityBreakTreshold += 2 * pointsWithPosition->points.size();
    }

    do {
        std::cout << "in do-while-loop with securityBreak = " << securityBreak << std::endl;
        std::cout << "Current Iterator State: " << it << std::endl;
        std::vector<CPcutting> cpCuttingsList;
        std::vector<NCPcutting> ncpCuttingsList;
        cpCuttingsList.reserve(this->size()); //only an estimation
        ncpCuttingsList.reserve(4 * m_noneContactPointList.size()); // each path for a NoneContactPoint may in an unfortunate event have four cuttings with a line (cause of the form of the path like a loop)

        uint occurencesCP = howManyContactPointsCoverPoint(it, cpCuttingsList);
        uint occurencesNCP = howManyNoneContactPointsCoverPoint(it, ncpCuttingsList);

        if(occurencesCP + occurencesNCP == 0) {
            //point is not covered => it is a gear point
            m_gearPoints.push_back(it.currentPoint());
            m_gearCPs.push_back(it.currentCP());
            ++it;
            std::cout << "          did NOT find any cutting points ++it called" << std::endl;

        } else {
            //find first cutting
            uint firstCP = 0;
            for(uint i = 1; i < cpCuttingsList.size(); ++i) {
                if(cpCuttingsList[i].t < cpCuttingsList[firstCP].t)
                    firstCP = i;
                std::cout << "first = " << firstCP << std::endl;
            }
            uint firstNCP = 0;
            for(uint i = 1; i < ncpCuttingsList.size(); ++i) {
                if(ncpCuttingsList[i].t < ncpCuttingsList[firstNCP].t)
                    firstNCP = i;
                std::cout << "firstNCP = " << firstNCP << std::endl;
            }

            if(occurencesNCP == 0 || (occurencesCP > 0 && cpCuttingsList[firstCP].t < ncpCuttingsList[firstNCP].t)) {
                //nearest cutting point is one of m_pointsWithPositionList
                std::cout << "          DID find cutting points with first one one of CP (number of CP cuttings: " << occurencesCP << ", NCP cuttings: " << occurencesNCP << ")" << std::endl;
                m_gearPoints.push_back(cpCuttingsList[firstCP].cuttingPoint);
                std::cout << " CP Cutting continue with  : " << cpCuttingsList[firstCP] << std::endl;
                it.continueWith(cpCuttingsList[firstCP]);

            } else { // nearest cutting point is one of m_noneContactPointList
                std::cout << "          DID find cutting points with first one one of NCP (number of CP cuttings: " << occurencesCP << ", NCP cuttings: " << occurencesNCP << ")" << std::endl;
                m_gearPoints.push_back(ncpCuttingsList[firstNCP].cuttingPoint);
                std::cout << "NCP Cutting continue with  : " << ncpCuttingsList[firstNCP] << std::endl;
                it.continueWith(ncpCuttingsList[firstNCP]);
            }
        }

        ++securityBreak;
        securityBreakCondition = (securityBreak <= securityBreakTreshold);
        notAtListEndCondition = !it.reachedEnd();
        if(notAtListEndCondition) {
            vec2 baryz = betweenStartStop * it.currentPoint();
            bool isStillBetween = (baryz.x > 0) && (baryz.y > 0);
            real epsilon = glm::length(m_gearPoints[0] - glm::rotate(it.currentPoint(), -m_angularPitchRotation));
            notYetAtOriginCondition = (isStillBetween || epsilon > 1);
        } else {
            notYetAtOriginCondition = true;
        }

    } while(securityBreakCondition && notYetAtOriginCondition && notAtListEndCondition);

    std::cout << "After while loop and securityBreakCondition:  " << securityBreakCondition << "   securityBreak = " << securityBreak << " of treshold: " << securityBreakTreshold << std::endl;
    std::cout << "                 and notYetAtOriginCondition: " << notYetAtOriginCondition << std::endl;
    std::cout << "                 and notAtListEndCondition:   " << notAtListEndCondition << std::endl;
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

bool ContactPointSortingList::getFirstNoneErrorCPIterator(vector<ContactPoint*>::iterator &it, vector<ContactPoint*> *&points) {
    std::list<PointsWithPosition*>::iterator listIt = m_pointsWithPositionList.begin();

    while(listIt != m_pointsWithPositionList.end()) {

        vector<ContactPoint*>::iterator cpIt = (*listIt)->points.begin();
        while((*cpIt)->error != ErrorCode::NO_ERROR && cpIt != (*listIt)->points.end()) {
            ++cpIt;
        }

        if(cpIt != (*listIt)->points.end()) {
            it = cpIt;
            points = &((*listIt)->points);
            return true;
        }
    }
    //to reach this line is normally not possible!
    return false;
}

uint ContactPointSortingList::howManyContactPointsCoverPoint(const ContactPointIterator &it, std::vector<CPcutting> &cpCuttingsList) const {
    uint foundCoverings = 0;

    for(PointsWithPosition* pointsWithPosition : m_pointsWithPositionList) {
        for(vector<ContactPoint*>::iterator previous = pointsWithPosition->points.begin(),
                                            current = ++(pointsWithPosition->points.begin()),
                                            end = pointsWithPosition->points.end();
            current != end;
            ++previous, ++current)
        {
            if(!it.belongsToQuad(*previous, *current)) { // do not test the point with its own quad!

                real t;
                vec2 intersection;
                //test ground:
                if(intersectLines(t, intersection, it.previousPoint(), it.currentPoint(), (*previous)->point, (*current)->point)) {
                    cpCuttingsList.push_back(CPcutting{t, intersection, previous, &(pointsWithPosition->points), IterationLocation::Ground});
                    ++foundCoverings;
                    std::cout << " CP Cutting on Ground found: " << cpCuttingsList.back() << std::endl;
                }
                //test top:
                if(intersectLines(t, intersection, it.previousPoint(), it.currentPoint(), (*previous)->forbiddenAreaEndPoint, (*current)->forbiddenAreaEndPoint)) {
                    cpCuttingsList.push_back(CPcutting{t, intersection, previous, &(pointsWithPosition->points), IterationLocation::Top});
                    ++foundCoverings;
                    std::cout << " CP Cutting on Top found: " << cpCuttingsList.back() << std::endl;
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
                    std::cout << "NCP Cutting on Ground found: " << ncpCuttingsList.back() << std::endl;
                }
                if(intersectLines(t, intersection, it.previousPoint(), it.currentPoint(), previousEndPoint, currentEndPoint)) {
                    ncpCuttingsList.push_back(NCPcutting{t, intersection, ncp, i - 1, IterationLocation::Top});
                    ++foundCoverings;
                    std::cout << "NCP Cutting on Top found: " << ncpCuttingsList.back() << std::endl;
                }

            }
        }
    }
    return foundCoverings;
}

void ContactPointSortingList::copyInCorrectList(const ContactPoint &cp, int position, PointsWithPosition *&list) {
    //TODO: at the moment every point is inserted also in the first list, this is not correct!
    //      it is only here for testing reasons.

    bool isFirstList = m_pointsWithPositionList.empty();
    if(!isFirstList)
            m_pointsWithPositionList.front()->points.push_back(new ContactPoint(cp)); //insert every cp in first list, undo that!

    list->points.push_back(new ContactPoint(cp));

    if(list->position != position) { // open a new list and insert the contact point cp and his predecessor to the list

        bool hasPredecessor = list->points.size() > 1;
        ContactPoint *predecessor = nullptr;
        if(hasPredecessor) {
            predecessor = new ContactPoint(**(list->points.end() - 2)); // second to last contact point, as we have already inserted the current one
        }
        m_pointsWithPositionList.push_back(list);
        list = new PointsWithPosition();
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
        std::cout << "in pitchNumberOfPoint calculated: " << (beta / angularPitch) << " and returned " << (int)(beta / angularPitch) << std::endl;
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
    // m_triangles.push_back(Triangle({a, b, c, point, isInTriangle}));
    return isInTriangle;
}

void ContactPointSortingList::setBackLists() {
    deleteSortingLists();
    deleteNoneContactPoints();
    m_triangles.clear();
}

void ContactPointSortingList::deleteSortingLists() {
    for(PointsWithPosition *list : m_pointsWithPositionList) {
        for(ContactPoint *point : list->points) {
            delete point;
        }
        list->points.clear();
        delete list;
    }
    m_pointsWithPositionList.clear();
}

void ContactPointSortingList::deleteNoneContactPoints() {
    for(std::list<NoneContactPoint*>::iterator it = m_noneContactPointList.begin(); it != m_noneContactPointList.end(); ++it)
        delete *it;
    m_noneContactPointList.clear();
}
