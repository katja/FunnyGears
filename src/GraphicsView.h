#ifndef GRAPHICS_VIEW
#define GRAPHICS_VIEW

#include "stable.h"
#include "definitions.h"
#include "tool_bars/EditingToolBarListener.h"
#include "tool_bars/EditingToolBar.h"

class GraphicsView : public QGraphicsView, public EditingToolBarListener {

Q_OBJECT

public:
    GraphicsView(QWidget *parent = 0);
    GraphicsView(QGraphicsScene *scene, QWidget *parent = 0);
    ~GraphicsView();

    // This method is only for test reasons
    // Turn it on to measure rendering speed
    // void paintEvent(QPaintEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

    void stopEditing() override;
    void startEditing(Editing::State editingState) override;
    void executeEditingAction(Editing::Action editingAction) override;

    void zoomToDefault();
    void zoomIn();
    void zoomOut();

private:
    void initialize();

    Editing::State m_editingState;
};

#endif //GRAPHICS_VIEW
