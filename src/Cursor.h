#ifndef CURSOR
#define CURSOR

#include "stable.h"

class CursorRental {

private:
    static CursorRental *m_cursorRental;
    CursorRental();

    QCursor *m_pointerCursor,
            *m_eraserCursor,
            *m_penCursor,
            *m_zoomInCursor,
            *m_zoomOutCursor;

public:
    ~CursorRental();

    static const QCursor& pointer();
    static const QCursor& pen();
    static const QCursor& eraser();
    static const QCursor& zoomIn();
    static const QCursor& zoomOut();
};

#endif // CURSOR
