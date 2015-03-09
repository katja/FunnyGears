#ifndef CONTACT_POINT_HELPERS
#define CONTACT_POINT_HELPERS
#include "definitions.h"
#include "basic_objects/ContactPoint.h"

#include "ContactPointHelpers.h"

struct ContactPointsWithPosition {
    int position;
    //  ...
    //  2 => 2 angular pitches before the examined one
    //  1 => 1 angular pitch   before the examined one
    //  0 => in the examined pitch
    // -1 => 1 angular pitch   behind the examined one
    // -2 => 2 angular pitches behind the examined one
    //  ...
    vector<ContactPoint*> points;
};

struct Triangle {
    vec2 a;
    vec2 b;
    vec2 c;
    vec2 point;
    bool pointIsInTriangle;
};

enum class IterationLocation {
    Ground = 0,
    Top = 1
};

enum class IterationState {
    CP = 0,
    NCP = 1
};

enum class IterationDirection {
    Reverse = -1,
    Normal = 1
};

struct CPcutting {
    real t;
    vec2 cuttingPoint;
    vector<ContactPoint*>::iterator placeBeforeCutting;
    vector<ContactPoint*> *list;
    IterationLocation location; //IterationLocation::Ground or IterationLocation::Top
};

struct NCPcutting {
    real t;
    vec2 cuttingPoint;
    NoneContactPoint *ncp;
    uint placeBeforeCutting;
    IterationLocation location; //IterationLocation::Ground or IterationLocation::Top
};

enum class ContinuationType {
    Default,
    HopOn,
    StartAgain
};

enum class OriginInformation {
    WCP = -1,
    CUT = 0,
    CP = 1,
    SOME_THING_ELSE = 999
};

#endif // CONTACT_POINT_HELPERS
