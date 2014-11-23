#include "tool_bars/EditingStatesAndActions.h"

bool Editing::demandsWholeCanvas(State s) {
    if(s == Pen)
        return true;
    return false;
}
