#ifndef TOOL_BAR_LISTENER
#define TOOL_BAR_LISTENER

#include "tool_bars/EditingStatesAndActions.h"

class ToolBarListener {
public:
    virtual void stopEditing() = 0;
    virtual void startEditing(Editing::State) = 0;
    virtual void executeEditingAction(Editing::Action) = 0;
};

#endif // TOOL_BAR_LISTENER
