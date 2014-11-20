#include "EditingToolBarListener.h"
#include "ToolBarManager.h"

//Helper class (null object) to decide if a ToolBarListener object is an EditingToolBarListener
class EditingToolBarNullListener : public EditingToolBarListener {
public:
    EditingToolBarNullListener() {}
    virtual void stopEditing() {};
    virtual void startEditing(Editing::State) {};
    virtual void executeEditingAction(Editing::Action) {};
};

//All STATIC things:
EditingToolBarListener* EditingToolBarListener::listener = 0;

ToolBarListener* EditingToolBarListener::toolBarClassType() {
    if(listener == 0) {
        listener = new EditingToolBarNullListener();
    }
    return listener;
}

//All NONE STATIC things:
EditingToolBarListener::~EditingToolBarListener() {
    ToolBarManager::getToolBarManager()->removeListenerFromToolBars(this);
}

ToolBarListener* EditingToolBarListener::toolBarType() {
    return toolBarClassType();
}
