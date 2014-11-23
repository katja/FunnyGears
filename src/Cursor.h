#ifndef CURSOR
#define CURSOR

#include "stable.h"

class CursorRental {

public:
    virtual ~CursorRental();

    static CursorRental* getCursorRental();

    static const QCursor& pointer();
    static const QCursor& pen();
    static const QCursor& eraser();
    static const QCursor& zoomIn();
    static const QCursor& zoomOut();

private:
    static CursorRental *m_uniqueCursorRental;
    CursorRental();

    QCursor *m_pointerCursor,
            *m_penCursor,
            *m_eraserCursor,
            *m_zoomInCursor,
            *m_zoomOutCursor;

};

#endif // CURSOR
