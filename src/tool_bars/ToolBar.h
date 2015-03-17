#ifndef TOOL_BAR
#define TOOL_BAR

#include "tool_bars/ToolBarListener.h"

class ToolBar {
public:
    virtual bool addListener(ToolBarListener *listener) = 0;
    virtual bool removeListener(ToolBarListener *listener) = 0;
    virtual bool hasListener(ToolBarListener *listener) = 0;
    virtual ~ToolBar() {}
};

#endif // TOOL_BAR
