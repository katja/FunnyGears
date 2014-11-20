#ifndef EDITING_TOOL_BAR_LISTENER
#define EDITING_TOOL_BAR_LISTENER

#include "tool_bars/ToolBarListener.h"
#include "tool_bars/EditingStatesAndActions.h"

class EditingToolBarListener : public ToolBarListener {
    static EditingToolBarListener *listener;
public:
    virtual ~EditingToolBarListener();
    virtual void stopEditing() = 0;
    virtual void startEditing(Editing::State) = 0;
    virtual void executeEditingAction(Editing::Action) = 0;

    virtual ToolBarListener* toolBarType();
    static ToolBarListener* toolBarClassType();
};

#endif // EDITING_TOOL_BAR_LISTENER
