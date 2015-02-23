#include "ContactPointSortingList.h"

#include "glm/glm.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "helpers.h"

#include <iostream>

ContactPointSortingList::ContactPointSortingList() {
    m_angularPitchRotation = nullptr;
    m_sortingLists = nullptr;
}

ContactPointSortingList::~ContactPointSortingList() {
    delete m_angularPitchRotation;
    deleteSortingLists();
    delete m_sortingLists;
}

uint ContactPointSortingList::numberOfNoneErrorContactPoints() const {
    uint foundPoints = 0;
    for(ContactPoint cp : (*this)) {
        if(cp.error == ErrorCode::NO_ERROR)
            ++foundPoints;
    }
    return foundPoints;
}

ContactPoint ContactPointSortingList::getFirstNoneErrorContactPoint() const {
    const_iterator it = begin();
    while(it->error != ErrorCode::NO_ERROR && it != end())
        ++it;
    if(it == end())
        return ContactPoint();
    return *it;
}

void ContactPointSortingList::deleteSortingLists() {
    for(PositionList *list : *m_sortingLists) {
        delete list;
    }
    m_sortingLists->clear();
}

void ContactPointSortingList::sort(uint numberOfTeeth, bool isDescribedClockwise) {
    if(size() == 0 || numberOfNoneErrorContactPoints() == 0)
        return;

    if(m_angularPitchRotation != nullptr)
        delete m_angularPitchRotation;
    if(m_sortingLists != nullptr)
        deleteSortingLists();
    m_triangles.clear();

    real turningDirection = ((isDescribedClockwise) ? 1.0 : -1.0);
    m_angularPitchRotation = new real((2.0 * M_PI * turningDirection) / (real)numberOfTeeth);
    m_sortingLists = new std::list< PositionList* >;

    //Every point has to be inside one angular pitch of the gear!
    //Create lists with ContactPoints which lay either out of the angular pitch region or in it
    //lists with points that lay outside are rotated to the start or respectively to the end
    //of the angular pitch region so that they lay inside the region, too.
    m_startPoint = getFirstNoneErrorContactPoint();
    vec2 startPitch = glm::normalize(getFirstNoneErrorContactPoint().point);
    vec2 stopPitch = glm::rotate(startPitch, (*m_angularPitchRotation));
    m2x2 betweenStartStop = glm::inverse(m2x2(startPitch, stopPitch));

    PositionList *positionList = new PositionList();
    positionList->position = 0; // in examined angular pitch

    for(iterator it = this->begin(), end = this->end(); it != end; ++it) {
        if(it->error != ErrorCode::NO_CUT_WITH_REFERENCE_RADIUS) {
            vec2 baryz = betweenStartStop * it->point;

            if(glm::all(glm::greaterThanEqual(baryz, vec2(0,0)))) {
                insertInCorrectList(*it, 0, positionList);

            } else if (baryz.x > 0.0f && baryz.y < 0.0f) {
                //startPitch still positive, stopPitch negative => point lies infront of start of tooth
                int position = whichPositionBeforeAngularPitch(&(*it), startPitch);
                insertInCorrectList(*it, position, positionList);

            } else if (baryz.x < 0.0f && baryz.y > 0.0f) {
                //startPitch negative, stopPitch still positive => point lies behind end of tooth
                int position = whichPositionBehindAngularPitch(&(*it), stopPitch);
                insertInCorrectList(*it, position, positionList);

            }  //else: point lies nearly opposite tooth

        } //else: point on original gear has no equivalent on mating gear => do nothing with that point!

    }

    m_sortingLists->push_back(positionList); //insert last list, too. Otherwise will be lost

    std::cout << "LISTS:" << std::endl;
    for(PositionList *posList : (*m_sortingLists)) {
        std::cout << "Position: " << posList->position << ", and members: " << posList->list.size() << std::endl;
    }

    // Turn the points of the lists infront and behind of the examined angular pitch
    // Afterwards all lists are mainly in the angular pitch
    for(PositionList *positionList : (*m_sortingLists)) {
        if(positionList->position != 0) {
            real rotationAngle = (*m_angularPitchRotation) * (real)positionList->position;
            for(vector<ContactPoint>::iterator it = positionList->list.begin(), end = positionList->list.end(); it != end; ++it) {
                it->rotate(rotationAngle);
            }
        }
    }

    // Now all points of all lists are compared and only points, which are not covered by any of the rectangles
    // which are constructed by two other points with their forbidden area are chosen for the mating gear.
    // First mark all points as not covered.

    // Find all covered points
    // This loop is very expensive but it is quite complicated to reduce the cicles, as no constraint is known for the points.
    for(std::list< PositionList* >::iterator listIt = m_sortingLists->begin(), listEnd = m_sortingLists->end(); listIt != listEnd; ++listIt) {
        for(vector<ContactPoint>::iterator it = ((*listIt)->list).begin(), end = --((*listIt)->list).end(); it != end; ++it) {

            for(std::list< PositionList* >::iterator list2It = m_sortingLists->begin(), list2End = m_sortingLists->end(); list2It != list2End; ++list2It) {
                for(vector<ContactPoint>::iterator it2 = ((*listIt)->list).begin(), end2 = ((*listIt)->list).end(); it2 != end2; ++it2) {
                    if(it != it2 && (it+1) != it2) {//don't compare points itself!
                        if(contactPointIsCovered(*it2, *it, *(it+1))) {
                            it2->isCovered = true;
                        }
                    }
                }
            }

        }
    }

    // vector<ContactPoint> chosenPoints = vector<ContactPoint>();
    // for(hpuint i = 0; i < positionList.size(); ++i) {
    //     if(positionList[i] == PointPosition::IN_ANGULAR_PITCH) {
    //         for(vector<ContactPoint>::iterator it = lists[i]->begin(), end = lists[i]->end(); it != end; ++it) {
    //             if(!it->isCovered) {
    //                 chosenPoints.push_back(*it);
    //             }
    //         }
    //     }
    // }
    // return new vector<ContactPoint>(chosenPoints);
}

std::list< PositionList* >* ContactPointSortingList::positionLists() {
    return m_sortingLists;
}

std::list< Triangle > ContactPointSortingList::triangles() {
    return m_triangles;
}

ContactPoint ContactPointSortingList::startPoint() const {
    return m_startPoint;
}

void ContactPointSortingList::insertInCorrectList(const ContactPoint &point, int position, PositionList *&list) {
    if(list->position == position) {
        list->list.push_back(point);
        if(!m_sortingLists->empty() && list != m_sortingLists->front())
            m_sortingLists->front()->list.push_back(point); //insert every point in first list, undo that!
    }
    else {
        //TODO: Maybe better to create a new ContactPoint at the cutting line
        //      of angular pitch with the line between this and the last ContactPoint?
        list->list.push_back(ContactPoint(point));
        ContactPoint previousPoint;
        bool isFirstList = m_sortingLists->empty();
        if(!isFirstList) {
            previousPoint = ContactPoint(m_sortingLists->back()->list.back());
        }
        m_sortingLists->push_back(list);
        if(!m_sortingLists->empty() && list != m_sortingLists->front())
            m_sortingLists->front()->list.push_back(point); //insert every point in first list, undo that!
        list = new PositionList();
        list->position = position;
        if(!isFirstList)
            list->list.push_back(previousPoint);
        list->list.push_back(point);
    }

//TODO: following was normal code, so uncomment this and delete lines above!!!
    // if(list->position == position)
    //     list->list.push_back(point);
    // else {
    //     //TODO: Maybe better to create a new ContactPoint at the cutting line
    //     //      of angular pitch with the line between this and the last ContactPoint?
    //     list->list.push_back(point);
        // ContactPoint previousPoint;
        // bool isFirstList = m_sortingLists->empty();
        // if(!isFirstList) {
        //     ContactPoint previousPoint = ContactPoint(m_sortingLists->back()->list.back());
        // }

    //     m_sortingLists->push_back(list);
    //     list = new PositionList();
    //     list->position = position;
        // if(!isFirstList)
        //     list->list.push_back(previousPoint);
    //     list->list.push_back(point);
    // }
}

int ContactPointSortingList::whichPositionBehindAngularPitch(ContactPoint *contactPoint, const vec2 &stopPitch) {
    vec2 a = stopPitch;
    vec2 b = glm::rotate(a, (*m_angularPitchRotation));

    bool pitchFound = false;
    int i = 0;
    do {
        i -= 1;
        m2x2 betweenAB = glm::inverse(m2x2(a, b));
        vec2 baryz = contactPoint->point * betweenAB;
        pitchFound = glm::all(glm::greaterThan(baryz, vec2(0, 0)));
        a = b;
        b = glm::rotate(a, (*m_angularPitchRotation));
    } while(!pitchFound);
    return i;
}

int ContactPointSortingList::whichPositionBeforeAngularPitch(ContactPoint *contactPoint, const vec2 &startPitch) {
    vec2 b = startPitch;
    vec2 a = glm::rotate(b, -(*m_angularPitchRotation));

    bool pitchFound = false;
    int i = 0;
    do {
        i += 1;
        m2x2 betweenAB = glm::inverse(m2x2(a, b));
        vec2 baryz = contactPoint->point * betweenAB;
        pitchFound = glm::all(glm::greaterThan(baryz, vec2(0, 0)));
        b = a;
        a = glm::rotate(b, -(*m_angularPitchRotation));
    } while(!pitchFound);
    return i;
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

bool ContactPointSortingList::contactPointIsCovered(ContactPoint candidate, ContactPoint a, ContactPoint b) {
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

// void ContactPointSortingList::insertInList(
//     ContactPoint ContactPoint,
//     PointPosition lastPosition,
//     PointPosition currentPosition,
//     vector< vector<ContactPoint>* >& lists,
//     vector<PointPosition>& positionList) {

//     if(lastPosition != currentPosition && !lists.back()->empty()) {
//         ContactPoint lastContactPoint = lists.back()->back();
//         lists.back()->push_back(ContactPoint);
//         turnPointsOfList(lists.back(), lastPosition);
//         positionList.push_back(lastPosition);
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
