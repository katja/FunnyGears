#ifndef TOOL_BAR_MANAGER
#define TOOL_BAR_MANAGER

#include "tool_bars/ToolBar.h"
#include "tool_bars/ToolBarListener.h"
#include "stable.h"

class ToolBarManager {

public:
    static ToolBarManager* getToolBarManager();

    virtual ~ToolBarManager();
    void control(ToolBar *toolBar);
    QList<ToolBar*> managedToolBars(ToolBarListener *listener = 0);
    void addListenerToToolBars(ToolBarListener *listener, ToolBarListener *listensTo = 0);
    void removeListenerFromToolBars(ToolBarListener *listener);

private:
    ToolBarManager();
    static ToolBarManager *m_uniqueManager;
    QList<ToolBar*> m_toolBars;
};

#endif //TOOL_BAR_MANAGER
