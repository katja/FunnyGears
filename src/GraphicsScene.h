#ifndef GRAPHICS_SCENE
#define GRAPHICS_SCENE

#include "stable.h"
#include "tool_bars/EditingToolBarListener.h"
#include "tool_bars/EditingStatesAndActions.h"
#include "SelectionModel.h"
#include "graphics_objects/GraphicsItem.h"

class GraphicsScene : public QGraphicsScene, public EditingToolBarListener {

Q_OBJECT

public:
    GraphicsScene(QObject *parent = 0);
    GraphicsScene(qreal x, qreal y, qreal width, qreal height, QObject *parent = 0);
    ~GraphicsScene();

    void setSelectionModel(SelectionModel *selectionModel);

    void drawForeground(QPainter *painter, const QRectF &rect);
    void drawBackground(QPainter *painter, const QRectF &rect);

    QList<GraphicsItem*> selectedGraphicsItems() const;

    void startEditing(Editing::State editingState);
    void stopEditing();
    void executeEditingAction(Editing::Action editingAction);

private slots:
    void reactOnSelectionChange();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);

private:
    Editing::State m_editingState;
    QPointF m_clickedPoint;

    bool m_selectionChangeInProgress;
    SelectionModel *m_selectionModel;

    void initialize();

    void setAllItemsCursor(const QCursor &cursor);
    void unsetAllItemsCursor();
    void setCursorInViews(const QCursor &cursor);
    void unsetCursorInViews();
    void sendClickToSelectedItems();
};

#endif // GRAPHICS_SCENE
