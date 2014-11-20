#ifndef EDITING_STATES_AND_ACTIONS
#define EDITING_STATES_AND_ACTIONS

namespace Editing {
    enum State : int {
        NoEditing = 0,
        Pointer = 1,
        Pen,
        Eraser,
        ZoomIn,
        ZoomOut,
        STATE_SIZE
    };

    enum Action : int {
        ZoomDefault = 0,
        ACTION_SIZE
    };

    bool demandsWholeCanvas(State s);
}

#endif // EDITING_STATES_AND_ACTIONS
