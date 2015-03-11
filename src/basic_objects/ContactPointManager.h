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

    void clear(); //deletes all ContactPoint* and list* of m_firstCPLists

    void insert(ContactPoint *cp); // only one direction important => list of not used direction is finished
    void insert(ContactPoint *cpA, ContactPoint *cpB); // same originPoint, but ray in different directions
    void insert(NoneContactPoint *ncp); // inserted in m_noneContactPointList
    void finishInsertion(); // call when all needed points were inserted

    void sortLists(); // sorts the lists of m_firstCPLists by their size in decreasing order. ContactPoints in each list must already be sorted!!!

    // Most important method:
    void processPointsToGear(uint numberOfTeeth, bool isDescribedClockwise);
    void translateForBottomClearance(real bottomClearance, real angleInDegree); //translate gear points with less than angle angleInDegree between their normal and line of centers. Needs valid creation of all gear points etc. before (so call createCoveringLists() before!)

    const list< list<ContactPoint*>* >& foundPoints() const;
    const list<ContactPointsWithPosition*>& contactPointsWithPositions() const;
    const list<NoneContactPoint*>& noneContactPoints() const;
    const vector<vec2>& gearPoints() const;
    const vector<ContactPoint*>& gearContactPoints() const;
    const vector<WrongContactPoint*>& gearWrongContactPoints() const;
    const vector<vec2>& translatedGearPoints() const;
    const vector<ContactPoint*>& translatedStillContactPoints() const;
    const vector<WrongContactPoint*>& translatedStillWrongContactPoints() const;

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
    uint                            m_numberOfInsertedCPs; // is equal or less as all collected ContactPoints in m_insertedCPsLists (if all ContactPoints have different evaluationStep, should be equal, if not deleted some ContactPoints in eraseEmptyAndOneEntryLists())
    list<NoneContactPoint*>         m_noneContactPointList; // also used for further collection
    list<ContactPoint*>            *m_currentSmallerValueList; // is nullptr after finishInsertion()!
    list<ContactPoint*>            *m_currentLargerValueList; // is nullptr after finishInsertion()!

    // sorted, copied and in appropriate pitches
    // (set back/deleted, when copyPointsInSuitableLists() called)
    list<ContactPointsWithPosition*> m_contactPointsWithPositionList;

    // chosen points for gear
    // (set back, when findAllCoveredPoints() called)
    vector<vec2>                    m_gearPoints; // all points for shape of tooth of gear, includes cutting points, NoneContactPoints and forbiddenAreaEndPoints
    vector<OriginInformation>       m_gearPointsInformation; // information about the origin of m_gearPoints (cutting, CP or WCP (from NCP))
    vector<int>                     m_gearPointsInformationIndex; // information how m_gearPoints, m_gearCPs and m_gearWCPs are linked: positive number => is CP or WCP and number is index of m_gearCPs/m_gearWCPs, negative number => -1 <-> cutting , -2 <-> forbiddenAreaEndPoint
    vector<ContactPoint*>           m_gearCPs; // really good ContactPoints for the gear (gear is best, if only such points exist)
    vector<WrongContactPoint*>      m_gearWCPs; // is arisen from a NCP in special position, bad point for the gear, as it results in a violation of the basic requirement of a gear tooth system

    // chosen points if already translated
    vector<vec2>                    m_translatedGearPoints; // all gear points after translation
    vector<ContactPoint*>           m_notTranslatedGearCPs; // not translated and therefore still in contact, only references! No Copies!
    vector<WrongContactPoint*>      m_notTranslatedGearWCPs; // not translated and therefore still in contact, only references! No Copies!
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

    // WrongContactPoint* wrongContactPointWhenTurnedBack(vec2 point, vec2 normal, real pitchRadiusDrivenGear);
    uint numberOfNoneErrorContactPoints() const;

    uint findStartPointForGearPoints(CPcutting &cpCutting, NCPcutting &ncpCutting);
    bool tryToSwitchToOtherList(ContactPointIterator &it);

    uint howManyContactPointsCutPreviousLine(const ContactPointIterator &it, vector<CPcutting> &cpCuttingsList, vector<ContinuationType> &cpCuttingsTypes) const;
    uint howManyNoneContactPointsCutPreviousLine(const ContactPointIterator &it, vector<NCPcutting> &ncpCuttingsList, vector<ContinuationType> &ncpCuttingsTypes) const;

    int pitchNumberOfPoint(vec2 point) const;
    int whichPositionBehindAngularPitch(ContactPoint *contactPoint, const vec2 &stopPitch) const;
    int whichPositionBeforeAngularPitch(ContactPoint *contactPoint, const vec2 &startPitch) const;

    void fillLastCuttingPoints(uint foundCuttings, uint currentIndex, ContactPoint *lastContactPoint, ContactPoint *currentContactPoint, real bottomClearance, real maxAngle);

    bool pointIsCovered(const vec2 &candidate, const ContactPoint &a, const ContactPoint &b) const;
    bool contactPointIsCovered(const ContactPoint &candidate, const ContactPoint &a, const ContactPoint &b) const;
    bool intersectLines(real &intersectionValue, vec2 &intersection, vec2 startLine, vec2 stopLine, vec2 startTestLine, vec2 stopTestLine) const;
    bool intersectLines(vec2& intersection, vec2 lineAStart, vec2 lineAEnd, vec2 lineBStart, vec2 lineBEnd) const;
    bool isPointInTriangle(vec2 point, vec2 a, vec2 b, vec2 c) const;

    void eraseEmptyAndOneEntryLists(); // deletes lists of m_insertedCPsLists with no entrys or with only one entry (which then is deleted, too)
    void deleteInsertedContactPoints(); //deletes the ContactPoints saved in m_insertedCPsLists
    void deleteNoneContactPoints(); // deletes the NoneContactPoints saved in m_noneContactPointList
    void deleteSortingLists(); // deletes the lists saved in m_contactPointsWithPositionList
    void clearAllGearLists(); //deletes the m_gearWCPs and sets back all m_gear... things
    void clearAllTranslatedGearLists();
};

#endif //CONTACT_POINT_MANAGER
