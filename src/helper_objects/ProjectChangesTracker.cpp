#include "ProjectChangesTracker.h"
#include <iostream>

ProjectChangesTracker::ProjectChangesTracker() : m_anythingChanged(false) {}

ProjectChangesTracker::~ProjectChangesTracker() { //TODO: delete something or first noMoreInformAboutChange()...???
}

void ProjectChangesTracker::objectChanged(ChangingObject *object) {
    m_anythingChanged = true;
    std::cout << "ProjectChangesTracker got objectChanged()!!!!!!!!!!!!" << std::endl;
}

void ProjectChangesTracker::trackChangesOf(ChangingObject *object) {
    object->informAboutChange(this);
}

void ProjectChangesTracker::trackChangesOf(ChangingObjectResponder *object) {
    m_responderList.push_back(object);
}

void ProjectChangesTracker::setBackChanges() {
    m_anythingChanged = false;
    for(ChangingObjectResponder *responder : m_responderList) {
        responder->clearChanges();
    }
    std::cout << "ProjectChangesTracker was set back +++++++++++++++++++" << std::endl;
}

bool ProjectChangesTracker::hasChanges() {
    if(m_anythingChanged) {
        std::cout << "ProjectChangesTracker: m_anythingChanged = true" << std::endl;
        return true;
    }
    for(ChangingObjectResponder *responder : m_responderList) {
        if(responder->hasChanged()) {
            std::cout << "ProjectChangesTracker: m_anythingChanged = false, responder had a change" << std::endl;
            return true;
        }
    }
    return false;
}
