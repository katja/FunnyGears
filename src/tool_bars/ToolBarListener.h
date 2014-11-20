#ifndef TOOL_BAR_LISTENER
#define TOOL_BAR_LISTENER

class ToolBarListener {

public:
    virtual ~ToolBarListener() {}
    virtual ToolBarListener* toolBarType() = 0;

};

#endif // TOOL_BAR_LISTENER
