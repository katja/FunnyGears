#ifndef CHANGING_OBJECT_LISTENER
#define CHANGING_OBJECT_LISTENER

class ChangingObject;

class ChangingObjectListener {

public:
    virtual ~ChangingObjectListener() {}
    virtual void objectChanged(ChangingObject *object) = 0;
};

#endif // CHANGING_OBJECT_LISTENER
