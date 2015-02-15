#ifndef CONTACT_POINT_SORTING_LIST
#define CONTACT_POINT_SORTING_LIST

#include "definitions.h"
#include "basic_objects/ContactPoint.h"
#include "stable.h"

struct PositionList {
    int position;
    //  ...
    //  2 => 2 angular pitches before the examined one
    //  1 => 1 angular pitch   before the examined one
    //  0 => in the examined pitch
    // -1 => 1 angular pitch   behind the examined one
    // -2 => 2 angular pitches behind the examined one
    //  ...
    std::vector<ContactPoint> list;
};

class ContactPointSortingList : public std::list<ContactPoint> {

public:
    ContactPointSortingList();

    // * @brief Initializes a ContactPointSortingList with given angular pitch
    //  *  and turning direction
    //  *
    //  *  @param numberOfTeeth number of teeth of the driven gear (with this information
    //  *                       the angular pitch is given/calculated)
    //  *  @param isToothDescribedInClockDirection the direction the tooth profile of the
    //  *                       driven gear is described in. This will be the other way
    //  *                       round than the driving gear is described in.

    ~ContactPointSortingList();

    /** @brief Returns the currenctly available number of ContactPoints which do not have an Error
     *
     *  Iterates through all inserted ContactPoints and counts the points with
     *  error = ErrorCode::NO_ERROR
     *
     *  @return number of contact points with no error
     */
    uint numberOfNoneErrorContactPoints() const;

    /** @brief Returns the first ContactPoint in list with no error
     *
     *  @warning: This methods requires at least one suitable ContactPoint in the list
     *            otherwise it returns a stub.
     */
    ContactPoint getFirstNoneErrorContactPoint() const;

    void sort(uint numberOfTeeth, bool isDescribedClockwise);

    //TODO: delete this!!!!
    std::list< PositionList* >* positionLists();

private:
    real *m_angularPitchRotation;
    std::list< PositionList* > *m_sortingLists;

    void deleteSortingLists(); // deletes the lists saved in m_sortingLists

    // merely inserts the ContactPoint in the list, but if it has another position
    // than the list is used for, the @p list is pushed to m_sortingLists and a new list
    // is created to which @p list now points to
    // the point is inserted there as first object, too.
    void insertInCorrectList(const ContactPoint &point, int position, PositionList *&list);

    int whichPositionBehindAngularPitch(ContactPoint *contactPoint, const vec2 &stopPitch);
    int whichPositionBeforeAngularPitch(ContactPoint *contactPoint, const vec2 &startPitch);

    // bool intersectLines(hpvec2& intersection, hpvec2 lineAStart, hpvec2 lineAEnd, hpvec2 lineBStart, hpvec2 lineBEnd);

    // bool isPointInTriangle(hpvec2 point, hpvec2 a, hpvec2 b, hpvec2 c);

    // bool ContactPointIsCovered(ContactPoint point, ContactPoint a, ContactPoint b);

    // void turnPointsOfList(std::vector<ContactPoint>* list, PointPosition positionOfListPoints);

};

#endif //CONTACT_POINT_SORTING_LIST
