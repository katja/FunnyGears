#ifndef CHANGING_OBJECT
#define CHANGING_OBJECT

#include "ChangingObjectListener.h"

class ChangingObject {

public:
    virtual ~ChangingObject() {}
    virtual void informAboutChange(ChangingObjectListener *listener) = 0;
    virtual void noMoreInformAboutChange(ChangingObjectListener *listener) = 0;
};

#endif // CHANGING_OBJECT
