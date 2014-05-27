#ifndef GRAPHICS_VIEW
#define GRAPHICS_VIEW

#include "stable.h"
#include "definitions.h"
#include "tool_bars/ToolBarListener.h"
#include "tool_bars/EditingToolBar.h"

class GraphicsView : public QGraphicsView, public ToolBarListener {

Q_OBJECT

public:
    GraphicsView(QWidget *parent = 0);
    GraphicsView(QGraphicsScene *scene, QWidget *parent = 0);
    ~GraphicsView();

    void wheelEvent(QWheelEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

    void stopEditing();
    void startEditing(Editing::State editingState);
    void executeEditingAction(Editing::Action editingAction);

    void zoomToDefault();
    void zoomIn();
    void zoomOut();

private:
    void initialize();

    Editing::State m_editingState;
};

#endif //GRAPHICS_VIEW
