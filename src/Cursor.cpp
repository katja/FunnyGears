#include "Cursor.h"

CursorRental* CursorRental::m_uniqueCursorRental = 0;

CursorRental::CursorRental() {
    std::cout << "CursorRental created" << std::endl;
    m_pointerCursor = 0;
    m_penCursor = 0;
    m_eraserCursor = 0;
    m_zoomInCursor = 0;
    m_zoomOutCursor = 0;
}

CursorRental::~CursorRental() {
    std::cout << "CursorRental deleted" << std::endl;
    delete m_pointerCursor;
    delete m_penCursor;
    delete m_eraserCursor;
    delete m_zoomInCursor;
    delete m_zoomOutCursor;
}

CursorRental* CursorRental::getCursorRental() {
    if(m_uniqueCursorRental == 0)
        m_uniqueCursorRental = new CursorRental();
    return m_uniqueCursorRental;
}

const QCursor& CursorRental::pointer() {
    if(getCursorRental()->m_pointerCursor == 0) {
        QBitmap bitmap("images/PointerBitmap.pbm");
        QBitmap bitmapMask("images/PointerMask.pbm");
        getCursorRental()->m_pointerCursor = new QCursor(bitmap, bitmapMask, 8, 2);
    }
    return *(getCursorRental()->m_pointerCursor);
}

const QCursor& CursorRental::pen() {
    if(getCursorRental()->m_penCursor == 0) {
        QBitmap bitmap("images/PenBitmap.pbm");
        QBitmap bitmapMask("images/PenMask.pbm");
        getCursorRental()->m_penCursor = new QCursor(bitmap, bitmapMask, 3, 30);
    }
    return *(getCursorRental()->m_penCursor);
}

const QCursor& CursorRental::eraser() {
    if(getCursorRental()->m_eraserCursor == 0) {
        QBitmap bitmap("images/EraserBitmap.pbm");
        QBitmap bitmapMask("images/EraserMask.pbm");
        getCursorRental()->m_eraserCursor = new QCursor(bitmap, bitmapMask, 9, 22);
    }
    return *(getCursorRental()->m_eraserCursor);
}

const QCursor& CursorRental::zoomIn() {
    if(getCursorRental()->m_zoomInCursor == 0) {
        QBitmap bitmap("images/ZoomInBitmap.pbm");
        QBitmap bitmapMask("images/ZoomMask.pbm");
        getCursorRental()->m_zoomInCursor = new QCursor(bitmap, bitmapMask, 11, 11);
    }
    return *(getCursorRental()->m_zoomInCursor);
}
const QCursor& CursorRental::zoomOut() {
    if(getCursorRental()->m_zoomOutCursor == 0) {
        QBitmap bitmap("images/ZoomOutBitmap.pbm");
        QBitmap bitmapMask("images/ZoomMask.pbm");
        getCursorRental()->m_zoomOutCursor = new QCursor(bitmap, bitmapMask, 11, 11);
    }
    return *(getCursorRental()->m_zoomOutCursor);
}
