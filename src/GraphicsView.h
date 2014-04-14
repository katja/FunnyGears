#ifndef GRAPHICS_VIEW
#define GRAPHICS_VIEW

#include "stable.h"
#include "definitions.h"
#include "ToolBarListener.h"
#include "EditingToolBar.h"

class GraphicsView : public QGraphicsView, public ToolBarListener {

Q_OBJECT

public:
    GraphicsView(QWidget *parent = 0);
    GraphicsView(QGraphicsScene *scene, QWidget *parent = 0);
    ~GraphicsView();

    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

    void stopEditing();
    void startEditing(Editing::State editingState);
    void executeEditingAction(Editing::Action editingAction);

private:
    void initialize();
    void zoomToDefault();

    Editing::State m_editingState;
};

#endif //GRAPHICS_VIEW
