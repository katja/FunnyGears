#ifndef EDITING_STATES_AND_ACTIONS
#define EDITING_STATES_AND_ACTIONS

namespace Editing {
    enum State : int {
        NoEditing = 0,
        Pen = 1,
        Eraser = 2,
        ZoomIn,
        ZoomOut,
        STATE_SIZE
    };

    enum Action : int {
        ZoomDefault = 0,
        ACTION_SIZE
    };
}

#endif // EDITING_STATES_AND_ACTIONS
