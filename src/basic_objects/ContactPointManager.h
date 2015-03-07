#ifndef CONTACT_POINT_MANAGER
#define CONTACT_POINT_MANAGER

#include "definitions.h"
#include "basic_objects/ContactPoint.h"
#include "basic_objects/ContactPointHelpers.h"
#include "basic_objects/ContactPointIterator.h"
#include "stable.h"

class ContactPointManager {

public:
    ContactPointManager();

    // * @brief Initializes a ContactPointManager with given angular pitch
    //  *  and turning direction
    //  *
    //  *  @param numberOfTeeth number of teeth of the driven gear (with this information
    //  *                       the angular pitch is given/calculated)
    //  *  @param isToothDescribedInClockDirection the direction the tooth profile of the
    //  *                       driven gear is described in. This will be the other way
    //  *                       round than the driving gear is described in.

    ~ContactPointManager();

    void insert(ContactPoint *cp); // only one direction important => list of not used direction is finished
    void insert(ContactPoint *cpA, ContactPoint *cpB); // same originPoint, but ray in different directions
    void insert(NoneContactPoint *ncp); // inserted in m_noneContactPointList
    void finishInsertion(); // call when all needed points were inserted
    void sortLists(); // sorts the lists of m_firstCPLists by their size in decreasing order. ContactPoints in each list must already be sorted!!!
    // Most important method:
    void processPointsToGear(uint numberOfTeeth, bool isDescribedClockwise);
    void clear(); //deletes all ContactPoint* and list* of m_firstCPLists

    const list< list<ContactPoint*>* >& foundPoints() const;
    const list<ContactPointsWithPosition*>& contactPointsWithPositions() const;
    const list<NoneContactPoint*>& noneContactPoints() const;
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

    // first collection
    // (set back/deleted when clear() is called)
    list< list<ContactPoint*>* >    m_insertedCPsLists;
    list<NoneContactPoint*>         m_noneContactPointList; //also used for further collection
    list<ContactPoint*>            *m_currentSmallerValueList; //is nullptr after finishInsertion()!
    list<ContactPoint*>            *m_currentLargerValueList; //is nullptr after finishInsertion()!
    uint                            m_numberOfFirstCPs;

    // sorted, copied and in appropriate pitches
    // (set back/deleted, when copyPointsInSuitableLists() called)
    list<ContactPointsWithPosition*> m_contactPointsWithPositionList;

    // chosen points for gear
    // (set back, when findAllCoveredPoints() called)
    vector<vec2>                    m_gearPoints;
    vector<ContactPoint*>           m_gearCPs; // NOT newly created (copied)!
    uint                            m_gearNotCorrectCPCounter;

    // other (pitch and rotation) attributes:
    real                            m_angularPitchRotation; //is negative, if driven gear tooth is described counter clockwise (in screen representation), and positive otherwise
    vec2                            m_examinedPitchStartDirection;
    vec2                            m_examinedPitchStopDirection;
    vec2                            m_examinedPitchStartDirectionInDrivingGear;
    real                            m_examinedPitchLengthInDrivenGear;
    real                            m_examinedPitchLengthInDrivingGear;

    void setAngularPitch(uint numberOfTeeth, bool isDescribedClockwise);
    bool setExaminedPitch();
    void copyPointsInSuitableLists();
    void reduceNumberOfNoneContactPoints();
    void copyNoneContactPointsInRelevantPitches();
    void findAllCoveredPoints();

    uint numberOfNoneErrorContactPoints() const;

    uint findStartPointForGearPoints(CPcutting &cpCutting, NCPcutting &ncpCutting);

    uint howManyContactPointsCoverPoint(const ContactPointIterator &it, std::vector<CPcutting> &cpCuttingsList) const;
    uint howManyNoneContactPointsCoverPoint(const ContactPointIterator &it, std::vector<NCPcutting> &ncpCuttingsList) const;

    int pitchNumberOfPoint(vec2 point) const;
    int whichPositionBehindAngularPitch(ContactPoint *contactPoint, const vec2 &stopPitch) const;
    int whichPositionBeforeAngularPitch(ContactPoint *contactPoint, const vec2 &startPitch) const;

    bool pointIsCovered(const vec2 &candidate, const ContactPoint &a, const ContactPoint &b) const;
    bool contactPointIsCovered(const ContactPoint &candidate, const ContactPoint &a, const ContactPoint &b) const;
    bool intersectLines(real &intersectionValue, vec2 &intersection, vec2 startLine, vec2 stopLine, vec2 startTestLine, vec2 stopTestLine) const;
    bool intersectLines(vec2& intersection, vec2 lineAStart, vec2 lineAEnd, vec2 lineBStart, vec2 lineBEnd) const;
    bool isPointInTriangle(vec2 point, vec2 a, vec2 b, vec2 c) const;

    void eraseEmptyAndOneEntryLists(); // deletes lists of m_insertedCPsLists with no entrys or with only one entry (which then is deleted, too)
    void deleteSortingLists(); // deletes the lists saved in m_contactPointsWithPositionList
    void deleteNoneContactPoints(); // deletes the NoneContactPoints saved in m_noneContactPointList
    void deleteInsertedContactPoints(); //deletes the ContactPoints saved in m_insertedCPsLists

};

#endif //CONTACT_POINT_MANAGER
