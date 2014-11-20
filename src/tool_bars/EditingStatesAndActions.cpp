#include "EditingStatesAndActions.h"

bool Editing::demandsWholeCanvas(State s) {
    if(s == Pen ||
        s == Eraser
        )
        return true;
    return false;
}
