#ifndef CONTACT_POINT_ITERATOR
#define CONTACT_POINT_ITERATOR
#include "basic_objects/ContactPointIterator.h"
#include "basic_objects/ContactPointHelpers.h"
#include "definitions.h"


class ContactPointIterator {

public:
    ContactPointIterator();
    ~ContactPointIterator();

    ContactPointIterator& operator++(); // only prefix version here!

    bool reachedEnd() const;

    bool tryToContinueWithOtherList(list<ContactPointsWithPosition*> &possibleLists);

    void startWith(CPcutting cutting, int standardNormalDirection);
    void startWith(NCPcutting cutting, int standardNormalDirection);

    vec2 currentPoint() const;
    vec2 previousPoint() const;
    bool isCurrentlyInCorrectCP() const;
    bool isCurrentlyInCorrectNCP() const;
    ContactPoint* currentCP() const; //Returns pointer to current CP, if in CP state, otherwise nullptr!
    ContactPoint* previousCP() const; //Returns pointer to previous CP, if in CP state, otherwise nullptr!
    NoneContactPoint* currentNCP() const; //Returns current pointer to NCP, if in NCP state, otherwise nullptr!

    void continueWith(CPcutting cutting, ContinuationType continueType = ContinuationType::Default); //if Default, takes as reference for normal and therefore stepping on direction the line between previous and current point, if HopOn takes instead the line between previous point and the previous normal end point, if StartAgain start or end of the list has to be given as placeBeforeCutting (if not appropriate placeBeforeCutting is found, nothing is done here) and it is walked in the only possible direction
    void continueWith(NCPcutting cutting, ContinuationType continueType = ContinuationType::Default); //if Default, takes as reference for normal and therefore stepping on direction the line between previous and current point, if HopOn takes instead the line between previous point and the previous normal end point, if StartAgain start or end of the list has to be given as placeBeforeCutting (if not appropriate placeBeforeCutting is found, nothing is done here) and it is walked in the only possible direction

    bool belongsToQuad(ContactPoint *a, ContactPoint *b) const;
    bool belongsToQuad(NoneContactPoint *ncp, uint a, uint b) const;

    std::ostream& currentStateStream(std::ostream &os) const;

private:
    vector<ContactPoint*>::iterator m_cpIterator;
    vector<ContactPoint*>           *m_cpList;
    NoneContactPoint                *m_ncp;
    int                             m_ncpPosition; // as it acts as iteration position and one can iterate to the front, too, it needs to be possible to be negative!

    vec2                            m_previousPoint; // as this may be a calculated cutting point, it is necessary to save it
    int                             m_normalDirection; // 0 => not set yet, -1 => left from direction, 1 => right from direction
    IterationDirection              m_iterationDirection;
    IterationLocation               m_iterationLocation;
    IterationState                  m_iterationState;

    bool isOnNormalsSideOf(vec2 lineStart, vec2 lineEnd, vec2 point) const;
    vec2 previousNormalEnd() const;
    vec2 forbiddenAreaEndPoint(NoneContactPoint *ncp, uint position) const;
};

#endif // CONTACT_POINT_ITERATOR
