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

    const std::list<ContactPointsWithPosition*>& contactPointsWithPositions() const;
    const std::list<NoneContactPoint*>& noneContactPoints() const;
    const vector<vec2>& gearPoints() const;
    const vector<ContactPoint*>& gearContactPoints() const;
    uint numberOfNotCorrectContactPoints() const;
    vec2 startOfExaminedPitchInDrivenGear() const;
    vec2 endOfExaminedPitchInDrivenGear() const;
    real lengthOfPitchesInDrivenGear() const;

    vec2 startOfExaminedPitchInDrivingGear () const;
    real lengthOfPitchesInDrivingGear() const;

    real usedAngularPitch() const;

private:
    real                            m_angularPitchRotation; //is negative, if driven gear tooth is described counter clockwise (in screen representation), and positive otherwise
    std::list<ContactPointsWithPosition*>  m_contactPointsWithPositionList;
    std::list<NoneContactPoint*>    m_noneContactPointList;
    vec2                            m_examinedPitchStartDirection;
    vec2                            m_examinedPitchStopDirection;
    vec2                            m_examinedPitchStartDirectionInDrivingGear;
    real                            m_examinedPitchLengthInDrivenGear;
    real                            m_examinedPitchLengthInDrivingGear;
    vector<vec2>                    m_gearPoints;
    vector<ContactPoint*>           m_gearCPs;
    uint                            m_gearNotCorrectCPCounter;

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

    uint findStartPointForGearPoints(CPcutting &cpCutting, NCPcutting &ncpCutting);

    uint howManyContactPointsCoverPoint(const ContactPointIterator &it, std::vector<CPcutting> &cpCuttingsList) const;
    uint howManyNoneContactPointsCoverPoint(const ContactPointIterator &it, std::vector<NCPcutting> &ncpCuttingsList) const;

    // merely inserts the ContactPoint in the list, but if it has another position
    // than the list is used for, the @p list is pushed to m_contactPointsWithPositionList and a new list
    // is created to which @p list now points to
    // the point is inserted there as first object, too.
    void copyInCorrectList(const ContactPoint &point, int position, ContactPointsWithPosition *&list);

    int pitchNumberOfPoint(vec2 point) const;
    int whichPositionBehindAngularPitch(ContactPoint *contactPoint, const vec2 &stopPitch) const;
    int whichPositionBeforeAngularPitch(ContactPoint *contactPoint, const vec2 &startPitch) const;

    bool pointIsCovered(const vec2 &candidate, const ContactPoint &a, const ContactPoint &b) const;
    bool contactPointIsCovered(const ContactPoint &candidate, const ContactPoint &a, const ContactPoint &b) const;
    bool intersectLines(real &intersectionValue, vec2 &intersection, vec2 startLine, vec2 stopLine, vec2 startTestLine, vec2 stopTestLine) const;
    bool intersectLines(vec2& intersection, vec2 lineAStart, vec2 lineAEnd, vec2 lineBStart, vec2 lineBEnd) const;
    bool isPointInTriangle(vec2 point, vec2 a, vec2 b, vec2 c) const;

    void setBackLists(); //deletes the ContactPoints, NoneContactPoints and ContactPointsWithPositions of m_contactPointsWithPositionList and m_noneContactPointList;
    void deleteSortingLists(); // deletes the lists saved in m_contactPointsWithPositionList
    void deleteNoneContactPoints(); // deletes the NoneContactPoints saved in m_noneContactPointList

};

#endif //CONTACT_POINT_SORTING_LIST
