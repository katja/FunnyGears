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

    reduceNumberOfNoneContactPoints();
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

void ContactPointSortingList::reduceNumberOfNoneContactPoints() {
    //TODO !!!
}

void ContactPointSortingList::findAllCoveredPoints() {
    // Find all covered points
    // This loop is very expensive but it is quite complicated to reduce the cicles, as no constraint is known for the points.
    for(std::list< PointsWithPosition* >::iterator listIt = m_pointsWithPositionList.begin(), listEnd = m_pointsWithPositionList.end(); listIt != listEnd; ++listIt) {
        for(vector<ContactPoint*>::iterator it = ((*listIt)->points).begin(), end = --((*listIt)->points).end(); it != end; ++it) {

            for(std::list< PointsWithPosition* >::iterator list2It = m_pointsWithPositionList.begin(), list2End = m_pointsWithPositionList.end(); list2It != list2End; ++list2It) {
                for(vector<ContactPoint*>::iterator it2 = ((*listIt)->points).begin(), end2 = ((*listIt)->points).end(); it2 != end2; ++it2) {

                    if(it != it2 && (it+1) != it2) {//don't compare points itself!
                        if(contactPointIsCovered(**it2, **it, **(it+1))) {
                            (*it2)->isCovered = true;
                        }
                    }

                }
            }

        }
    }
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

int ContactPointSortingList::whichPositionBehindAngularPitch(ContactPoint *contactPoint, const vec2 &stopPitch) {
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

int ContactPointSortingList::whichPositionBeforeAngularPitch(ContactPoint *contactPoint, const vec2 &startPitch) {
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

bool ContactPointSortingList::contactPointIsCovered(const ContactPoint &candidate, const ContactPoint &a, const ContactPoint &b) {
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

bool ContactPointSortingList::intersectLines(vec2& intersection, vec2 lineAStart, vec2 lineAEnd, vec2 lineBStart, vec2 lineBEnd) {
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
bool ContactPointSortingList::isPointInTriangle(vec2 point, vec2 a, vec2 b, vec2 c) {
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
    m_triangles.push_back(Triangle({a, b, c, point, isInTriangle}));
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
