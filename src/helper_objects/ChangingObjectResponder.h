#ifndef CHANGING_OBJECT_RESPONDER
#define CHANGING_OBJECT_RESPONDER

class ChangingObjectResponder {
public:
    virtual ~ChangingObjectResponder() {}
    virtual bool hasChanged() const = 0;
    virtual void clearChanges() = 0;
};

#endif // CHANGING_OBJECT_RESPONDER
