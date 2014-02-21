#ifndef GRAPHICS_VIEW
#define GRAPHICS_VIEW

#include "stable.h"

class GraphicsView : public QGraphicsView {

public:
    GraphicsView(QWidget *parent = 0);
    GraphicsView(QGraphicsScene *scene, QWidget *parent = 0);
    ~GraphicsView();

    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

private:
    // void enableMoving();
    // void disableMoving();

    // bool movingEnabled;

    void initialize();
};

#endif //GRAPHICS_VIEW
