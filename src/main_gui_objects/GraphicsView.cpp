#include "preferences.h"
#include "main_gui_objects/GraphicsView.h"
#include "main_gui_objects/Cursor.h"

GraphicsView::GraphicsView(QWidget *parent) : QGraphicsView(parent) {
    initialize();
}

GraphicsView::GraphicsView(QGraphicsScene *scene, QWidget *parent) : QGraphicsView(scene, parent) {
    initialize();
}

void GraphicsView::initialize() {

    setCacheMode(QGraphicsView::CacheBackground);

    // the render hint QPainter::Antialiasing is for sure lovely, but slows down the rendering extremely!
    // so DO NOT use it!
    setRenderHint(QPainter::SmoothPixmapTransform);

    setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    setMouseTracking(false);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setDragMode(QGraphicsView::RubberBandDrag);
    setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    stopEditing();
}

GraphicsView::~GraphicsView() {
}

void GraphicsView::wheelEvent(QWheelEvent *event) {
    int spinRange = event->angleDelta().y();
    int treshold = 5; //must be a value of 0,..., 10
    if(qAbs(spinRange) >= treshold) {

        qreal scaleFactor = 1.0;
        if(spinRange < 0 && transform().m11() < 15.0) //zoom in! (if not already zoomed in with scaling of 15 or more)
            scaleFactor = 1.1 - (1 / (spinRange + (10 - treshold))); //ensures that values are in [1.0, 1.1)
        else if(spinRange > 0 && transform().m11() > 0.2) //zoom out! (if not already zoomed out with scaling of 0.2 or less)
            scaleFactor = 0.9 + (1 / (spinRange + (10 - treshold))); //ensures that values are in (0.9, 1.0]

        scale(scaleFactor, scaleFactor);
    }
}

void GraphicsView::mouseReleaseEvent(QMouseEvent *event) {
    if(m_editingState == Editing::ZoomIn)
        zoomIn();
    else if (m_editingState == Editing::ZoomOut)
        zoomOut();
    else
        QGraphicsView::mouseReleaseEvent(event);
}

void GraphicsView::keyPressEvent(QKeyEvent *event) {
    if(event->key() == Qt::Key_Space) {
        setDragMode(QGraphicsView::ScrollHandDrag);
        setInteractive(false); // disables the event from being passed to graphics scene and all the items
    } else {
        QGraphicsView::keyPressEvent(event);
    }
}

void GraphicsView::keyReleaseEvent(QKeyEvent *event) {
    if(event->key() == Qt::Key_Space) {
        setDragMode(QGraphicsView::RubberBandDrag);
        setInteractive(true);  // enables again the event from being passed to graphics scene and all the items
    } else {
        QGraphicsView::keyReleaseEvent(event);
    }
}

void GraphicsView::stopEditing() {
    m_editingState = Editing::NoEditing;
    const QCursor cursor = this->cursor();
    unsetCursor();
}

void GraphicsView::startEditing(Editing::State editingState) {
    //The following two lines result in a correct display of the mouse cursor
    //Probably Qt has a bug here, as different similar problems are reported.
    //Without the two lines the mouse curser has its 'normal' pointer shape after
    //a mouse over with a different cursor shape happens.
    setDragMode(QGraphicsView::ScrollHandDrag);
    setDragMode(QGraphicsView::RubberBandDrag);
    if(editingState == Editing::ZoomIn)
        setCursor(CursorRental::zoomIn());
    else if(editingState == Editing::ZoomOut)
        setCursor(CursorRental::zoomOut());
    else if(editingState == Editing::Pen) {
        setCursor(CursorRental::pen());
    }
    else
        unsetCursor();

    m_editingState = editingState;
}

void GraphicsView::executeEditingAction(Editing::Action editingAction) {
    if(editingAction == Editing::ZoomDefault)
        zoomToDefault();
}

void GraphicsView::zoomIn() {
    qreal scaleFactor = 1.1;
    scale(scaleFactor, scaleFactor);
}

void GraphicsView::zoomOut() {
    qreal scaleFactor = 0.9;
    scale(scaleFactor, scaleFactor);
}

void GraphicsView::zoomToDefault() {
    fitInView(-0.5 * Preferences::ZoomDefaultX,
            -0.5 * Preferences::ZoomDefaultY,
            Preferences::ZoomDefaultX,
            Preferences::ZoomDefaultY,
            Qt::KeepAspectRatio);
    centerOn(0, 0);
}
