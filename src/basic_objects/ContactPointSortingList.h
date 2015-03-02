#ifndef CONTACT_POINT_SORTING_LIST
#define CONTACT_POINT_SORTING_LIST

#include "definitions.h"
#include "basic_objects/ContactPoint.h"
#include "basic_objects/ContactPointHelpers.h"
#include "basic_objects/ContactPointIterator.h"
#include "stable.h"

class ContactPointSortingList : public std::list<ContactPoint*> {

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

    // Most important method:
    void createCoveringLists(uint numberOfTeeth, bool isDescribedClockwise);
    void sort();
    void clear();

    //TODO: delete following few lines!!!!
    const std::list<PointsWithPosition*>& pointsWithPositions() const;
    const std::list<NoneContactPoint*>& noneContactPoints() const;
    const std::list<Triangle>& triangles(); //TODO: is not filled at the moment!!!
    vector<vec2> gearPoints() const;
    vec2 startOfExaminedPitch() const;
    vec2 endOfExaminedPitch() const;
    real usedAngularPitch() const;
    //until here???

private:
    real                            m_angularPitchRotation;
    std::list<PointsWithPosition*>  m_pointsWithPositionList;
    std::list<NoneContactPoint*>    m_noneContactPointList;
    std::list<Triangle>             m_triangles; //TODO: is not filled at the moment!!!
    vec2                            m_examinedPitchStartDirection;
    vec2                            m_examinedPitchStopDirection;
    vector<vec2>                    m_gearPoints;
    vector<ContactPoint*>           m_gearCPs;

    void setAngularPitch(uint numberOfTeeth, bool isDescribedClockwise);
    bool setExaminedPitch();
    void copyPointsInSuitableLists();
    void reduceNumberOfNoneContactPoints();
    void copyNoneContactPointsInRelevantPitches();
    void findAllCoveredPoints();

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
    ContactPoint* getFirstNoneErrorContactPoint() const;

    bool getFirstNoneErrorCPIterator(vector<ContactPoint*>::iterator &it, vector<ContactPoint*> *&points);

    uint howManyContactPointsCoverPoint(const ContactPointIterator &it, std::vector<CPcutting> &cpCuttingsList) const;
    uint howManyNoneContactPointsCoverPoint(const ContactPointIterator &it, std::vector<NCPcutting> &ncpCuttingsList) const;

    // merely inserts the ContactPoint in the list, but if it has another position
    // than the list is used for, the @p list is pushed to m_pointsWithPositionList and a new list
    // is created to which @p list now points to
    // the point is inserted there as first object, too.
    void copyInCorrectList(const ContactPoint &point, int position, PointsWithPosition *&list);

    int pitchNumberOfPoint(vec2 point) const;
    int whichPositionBehindAngularPitch(ContactPoint *contactPoint, const vec2 &stopPitch) const;
    int whichPositionBeforeAngularPitch(ContactPoint *contactPoint, const vec2 &startPitch) const;

    bool pointIsCovered(const vec2 &candidate, const ContactPoint &a, const ContactPoint &b) const;
    bool contactPointIsCovered(const ContactPoint &candidate, const ContactPoint &a, const ContactPoint &b) const;
    bool intersectLines(real &intersectionValue, vec2 &intersection, vec2 startLine, vec2 stopLine, vec2 startTestLine, vec2 stopTestLine) const;
    bool intersectLines(vec2& intersection, vec2 lineAStart, vec2 lineAEnd, vec2 lineBStart, vec2 lineBEnd) const;
    bool isPointInTriangle(vec2 point, vec2 a, vec2 b, vec2 c) const;

    void setBackLists(); //deletes the ContactPoints, NoneContactPoints and PointsWithPositions of m_pointsWithPositionList and m_noneContactPointList;
    void deleteSortingLists(); // deletes the lists saved in m_pointsWithPositionList
    void deleteNoneContactPoints(); // deletes the NoneContactPoints saved in m_noneContactPointList


    // bool intersectLines(hpvec2& intersection, hpvec2 lineAStart, hpvec2 lineAEnd, hpvec2 lineBStart, hpvec2 lineBEnd);

    // bool isPointInTriangle(hpvec2 point, hpvec2 a, hpvec2 b, hpvec2 c);

    // bool ContactPointIsCovered(ContactPoint point, ContactPoint a, ContactPoint b);

    // void turnPointsOfList(vector<ContactPoint>* list, PointPosition positionOfListPoints);

};

#endif //CONTACT_POINT_SORTING_LIST
