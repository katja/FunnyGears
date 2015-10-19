#ifndef PROJECT_CHANGES_TRACKER
#define PROJECT_CHANGES_TRACKER
#include "definitions.h"
#include "helper_objects/ChangingObject.h"
#include "helper_objects/ChangingObjectListener.h"
#include "helper_objects/ChangingObjectResponder.h"

class ProjectChangesTracker : public ChangingObjectListener {
public:
    ProjectChangesTracker();
    virtual ~ProjectChangesTracker();
    void trackChangesOf(ChangingObject *object);
    void trackChangesOf(ChangingObjectResponder *object);
    void objectChanged(ChangingObject *object);
    void setBackChanges();
    bool hasChanges();

private:
    bool m_anythingChanged;
    list<ChangingObjectResponder*> m_responderList;
};

#endif // PROJECT_CHANGES_TRACKER
