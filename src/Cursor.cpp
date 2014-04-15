#include "Cursor.h"

CursorRental* CursorRental::m_cursorRental = new CursorRental();

CursorRental::CursorRental() {
    m_pointerCursor = 0;
    m_penCursor = 0;
    m_eraserCursor = 0;
}

CursorRental::~CursorRental() {
    delete m_pointerCursor;
}

const QCursor& CursorRental::pointer() {
    if(m_cursorRental->m_pointerCursor == 0) {
        QBitmap bitmap("images/PointerBitmap.pbm");
        QBitmap bitmapMask("images/PointerMask.pbm");
        m_cursorRental->m_pointerCursor = new QCursor(bitmap, bitmapMask, 8, 2);
    }
    return *(m_cursorRental->m_pointerCursor);
}

const QCursor& CursorRental::pen() {
    if(m_cursorRental->m_penCursor == 0) {
        QBitmap bitmap("images/PenBitmap.pbm");
        QBitmap bitmapMask("images/PenMask.pbm");
        m_cursorRental->m_penCursor = new QCursor(bitmap, bitmapMask, 3, 30);
    }
    return *(m_cursorRental->m_penCursor);
}

const QCursor& CursorRental::eraser() {
    if(m_cursorRental->m_eraserCursor == 0) {
        QBitmap bitmap("images/EraserBitmap.pbm");
        QBitmap bitmapMask("images/EraserMask.pbm");
        m_cursorRental->m_eraserCursor = new QCursor(bitmap, bitmapMask, 9, 22);
    }
    return *(m_cursorRental->m_eraserCursor);
}

const QCursor& CursorRental::zoomIn() {
    if(m_cursorRental->m_zoomInCursor == 0) {
        QBitmap bitmap("images/ZoomInBitmap.pbm");
        QBitmap bitmapMask("images/ZoomInMask.pbm");
        m_cursorRental->m_zoomInCursor = new QCursor(bitmap, bitmapMask, 11, 11);
    }
    return *(m_cursorRental->m_zoomInCursor);
}
const QCursor& CursorRental::zoomOut() {
    if(m_cursorRental->m_zoomOutCursor == 0) {
        QBitmap bitmap("images/ZoomOutBitmap.pbm");
        QBitmap bitmapMask("images/ZoomMask.pbm");
        m_cursorRental->m_zoomOutCursor = new QCursor(bitmap, bitmapMask, 11, 11);
    }
    return *(m_cursorRental->m_zoomOutCursor);
}
