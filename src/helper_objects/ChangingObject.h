#ifndef CHANGING_OBJECT
#define CHANGING_OBJECT

#include "helper_objects/ChangingObjectListener.h"

/* Should hold a list or something like that to manage all ChangingObjectListeners
 * So if the ChangingObject changes, it can inform them by calling objectChanged(this)
 * on them.
 * Therefore it is essential, that the ChangingObjectListeners call noMoreInformAboutChange(...)
 * when their destructor is called!
 */
class ChangingObject {

public:
    virtual ~ChangingObject() {}
    virtual void informAboutChange(ChangingObjectListener *listener) = 0;
    virtual void noMoreInformAboutChange(ChangingObjectListener *listener) = 0;
};

#endif // CHANGING_OBJECT
