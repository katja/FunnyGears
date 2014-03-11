#include "GraphicsView.h"

GraphicsView::GraphicsView(QWidget *parent) : QGraphicsView(parent) {
    initialize();
}

GraphicsView::GraphicsView(QGraphicsScene *scene, QWidget *parent) : QGraphicsView(scene, parent) {
    initialize();
}

void GraphicsView::initialize() {
    std::cout << "GraphicsView is created" << std::endl;
    // movingEnabled = false;
    setRenderHint(QPainter::Antialiasing);
    setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    setMouseTracking(false);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setDragMode(QGraphicsView::RubberBandDrag);
    setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
}

GraphicsView::~GraphicsView() {
    std::cout << "GraphicsView is deleted" << std::endl;
}

void GraphicsView::wheelEvent(QWheelEvent *event) {
    int spinRange = event->angleDelta().y();
    int treshold = 5; //must be a value of 0,..., 10
    if(qAbs(spinRange) >= treshold) {

        qreal scaleFactor = 1.0f;
        if(spinRange < 0 && transform().m11() < 15.0f) //zoom in! (if not already zoomed in with scaling of 15 or more)
            scaleFactor = 1.1f - (1 / (spinRange + (10 - treshold))); //ensures that values are in [1.0, 1.1)
        else if(spinRange > 0 && transform().m11() > 0.2f) //zoom out! (if not already zoomed out with scaling of 0.2 or less)
            scaleFactor = 0.9f + (1 / (spinRange + (10 - treshold))); //ensures that values are in (0.9, 1.0]

        scale(scaleFactor, scaleFactor);
    }
}

void GraphicsView::keyPressEvent(QKeyEvent *event) {
    if(event->key() == Qt::Key_Space){
        setDragMode(QGraphicsView::ScrollHandDrag);
    } else {
        QGraphicsView::keyPressEvent(event);
    }
}

void GraphicsView::keyReleaseEvent(QKeyEvent *event) {
    if(event->key() == Qt::Key_Space){
        setDragMode(QGraphicsView::RubberBandDrag);
    } else {
        QGraphicsView::keyReleaseEvent(event);
    }
}

// void GraphicsView::enableMoving() {
//     setDragMode(QGraphicsView::ScrollHandDrag);
//     movingEnabled = true;
// }

// void GraphicsView::disableMoving() {
//     // setCursor(Qt::ArrowCursor);
//     setDragMode(QGraphicsView::RubberBandDrag);
//     movingEnabled = false;
// }
