#ifndef CONTACT_POINT_ITERATOR
#define CONTACT_POINT_ITERATOR
#include "basic_objects/ContactPointIterator.h"
#include "basic_objects/ContactPointHelpers.h"
#include "definitions.h"
#include "helpers.h"


class ContactPointIterator {

public:
    ContactPointIterator();
    ~ContactPointIterator();

    ContactPointIterator& operator++(); // only prefix version here!

    bool reachedEnd() const;

    bool tryToContinueWithOtherList(std::list<ContactPointsWithPosition*> &possibleLists);

    void startWith(CPcutting cutting, int standardNormalDirection);
    void startWith(NCPcutting cutting, int standardNormalDirection);

    vec2 currentPoint() const;
    vec2 previousPoint() const;
    ContactPoint* currentCP() const;
    ContactPoint* currentCorrectInContactPoint() const;

    void continueWith(CPcutting cutting);
    void continueWith(NCPcutting cutting);

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


    void setNormalDirection(); //may only be used after startWith(...) is called!
    bool isOnNormalsSide(vec2 point) const;
    vec2 forbiddenAreaEndPoint(NoneContactPoint *ncp, uint position) const;
};

#endif // CONTACT_POINT_ITERATOR
