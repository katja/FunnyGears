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

const std::list<PointsWithPosition*>& ContactPointSortingList::pointsWithPositions() {
    return m_pointsWithPositionList;
}

const std::list<Triangle>& ContactPointSortingList::triangles() {
    return m_triangles;
}

vector<vec2> ContactPointSortingList::gearPoints() const {
    return m_gearPoints;
}

const ContactPoint& ContactPointSortingList::startPoint() const {
    return *m_startPoint;
}

void ContactPointSortingList::clear() {
    for(ContactPoint *cp : (*this))
        delete cp;
    std::list<ContactPoint*>::clear();
}

void ContactPointSortingList::sort() {
    bool sorted = true;

    do {
        std::cout << "ContactPointSortingList::sort() while" << std::endl;
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

void ContactPointSortingList::createCoveringLists(uint numberOfTeeth, bool isDescribedClockwise) {
    if(size() == 0 || numberOfNoneErrorContactPoints() == 0)
        return;

    setBackLists();
    sort(); // Sort all points, normally they should already be sorted, but this also inserts the sequential number

    copyPointsInSuitableLists(numberOfTeeth, isDescribedClockwise);
    rotatePointsWithPositionToOnePitch();

    // Now all points of all lists are compared and only points, which are not covered by any of the rectangles
    // which are constructed by two other points with their forbidden area are chosen for the mating gear.

    std::cout << "POINTS WITH POSITION: " << std::endl;
    for(PointsWithPosition *points : m_pointsWithPositionList) {
        std::cout << "\nLIST mit position: " << points->position << std::endl;
        for(ContactPoint *cp : points->points) {
            std::cout << *cp << std::endl;
        }
    }

    findAllCoveredPoints();

    // vector<ContactPoint> chosenPoints = vector<ContactPoint>();
    // for(hpuint i = 0; i < pointsWithPosition.size(); ++i) {
    //     if(pointsWithPosition[i] == PointPosition::IN_ANGULAR_PITCH) {
    //         for(vector<ContactPoint>::iterator it = lists[i]->begin(), end = lists[i]->end(); it != end; ++it) {
    //             if(!it->isCovered) {
    //                 chosenPoints.push_back(*it);
    //             }
    //         }
    //     }
    // }
    // return new vector<ContactPoint>(chosenPoints);
}

void ContactPointSortingList::copyPointsInSuitableLists(uint numberOfTeeth, bool isDescribedClockwise) {

    real turningDirection = ((isDescribedClockwise) ? 1.0 : -1.0);
    m_angularPitchRotation = (2.0 * M_PI * turningDirection) / (real)numberOfTeeth;

    //Every point has to be inside one angular pitch of the gear!
    //Create lists with ContactPoints which lay either out of the angular pitch region or in it.
    //Lists with points that lay outside are rotated to the start or respectively to the end
    //of the angular pitch region so that they lay inside the region, too.
    m_startPoint = getFirstNoneErrorContactPoint();
    vec2 startPitch = glm::normalize(m_startPoint->point);
    vec2 stopPitch = glm::rotate(startPitch, m_angularPitchRotation);
    m2x2 betweenStartStop = glm::inverse(m2x2(startPitch, stopPitch));

    PointsWithPosition *pointsWithPosition = new PointsWithPosition();
    pointsWithPosition->position = 0; // in examined angular pitch

    for(ContactPoint *cp : (*this)) {
        if(cp->error != ErrorCode::NO_CUT_WITH_REFERENCE_RADIUS) {
            vec2 baryz = betweenStartStop * cp->point;

            if(glm::all(glm::greaterThanEqual(baryz, vec2(0,0)))) {
                copyInCorrectList(*cp, 0, pointsWithPosition);

            } else if (baryz.x > 0.0f && baryz.y < 0.0f) {
                //startPitch still positive, stopPitch negative => point lies infront of start of tooth
                int position = whichPositionBeforeAngularPitch(cp, startPitch);
                copyInCorrectList(*cp, position, pointsWithPosition);

            } else if (baryz.x < 0.0f && baryz.y > 0.0f) {
                //startPitch negative, stopPitch still positive => point lies behind end of tooth
                int position = whichPositionBehindAngularPitch(cp, stopPitch);
                copyInCorrectList(*cp, position, pointsWithPosition);

            }  //else: point lies nearly opposite tooth

        } else { // point on original gear has no equivalent on mating gear => examine the whole path of the point later
            m_noneContactPointList.push_back(new NoneContactPoint(*(static_cast<NoneContactPoint*>(cp))));
        }
    }
    m_pointsWithPositionList.push_back(pointsWithPosition); //insert last list, too. Otherwise will be lost
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

// void ContactPointSortingList::reduceNumberOfNoneContactPoints() {
//     //TODO !!!
// }

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

    // bool isFirstList = m_pointsWithPositionList.empty();
    // if(!isFirstList)
    //         m_pointsWithPositionList.front()->points.push_back(new ContactPoint(cp)); //insert every cp in first list, undo that!

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

// void ContactPointSortingList::insertInList(
//     ContactPoint ContactPoint,
//     PointPosition lastPosition,
//     PointPosition currentPosition,
//     vector< vector<ContactPoint>* >& lists,
//     vector<PointPosition>& pointsWithPosition) {

//     if(lastPosition != currentPosition && !lists.back()->empty()) {
//         ContactPoint lastContactPoint = lists.back()->back();
//         lists.back()->push_back(ContactPoint);
//         turnPointsOfList(lists.back(), lastPosition);
//         pointsWithPosition.push_back(lastPosition);
//         lists.push_back(new vector<ContactPoint>());
//         lists.back()->push_back(lastContactPoint);
//     }
//     lists.back()->push_back(ContactPoint);
// }

// void ContactPointSortingList::turnPointsOfList(vector<ContactPoint>* list, PointPosition positionOfListPoints) {
//     if(positionOfListPoints == PointPosition::IN_ANGULAR_PITCH) {
//         return;
//     } else {
//         real rotation = 0.0f;
//         if(positionOfListPoints == PointPosition::INFRONT_OF_ANGULAR_PITCH) {
//             rotation = -m_angularPitchRotation;
//         } else { //positionOfListsPoints == PointPosition::BEHIND_ANGULAR_PITCH
//             rotation = m_angularPitchRotation;
//         }
//         for(vector<ContactPoint>::iterator it = list->begin(), end = list->end(); it != end; ++it) {
//             it->rotate(rotation);
//         }
//     }
// }

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
