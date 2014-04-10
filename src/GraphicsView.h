#ifndef GRAPHICS_VIEW
#define GRAPHICS_VIEW

#include "stable.h"
#include "definitions.h"

class GraphicsView : public QGraphicsView {

Q_OBJECT

public:
    GraphicsView(QWidget *parent = 0);
    GraphicsView(QGraphicsScene *scene, QWidget *parent = 0);
    ~GraphicsView();

    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

    void changeView(EditingState editingState);
    void changeViewToDefault();
    void stopChangeView();

private:
    void initialize();
};

#endif //GRAPHICS_VIEW
