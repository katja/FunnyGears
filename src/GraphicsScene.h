#ifndef GRAPHICS_SCENE
#define GRAPHICS_SCENE

#include "stable.h"
#include "tool_bars/ToolBarListener.h"
#include "SelectionModel.h"
#include "graphics_objects/GraphicsSpline.h"
#include "graphics_objects/GraphicsItem.h"

class GraphicsScene : public QGraphicsScene, public ToolBarListener {

Q_OBJECT

public:
    GraphicsScene(QObject *parent = 0);
    GraphicsScene(qreal x, qreal y, qreal width, qreal height, QObject *parent = 0);
    ~GraphicsScene();

    void setSelectionModel(SelectionModel *selectionModel);

    void drawForeground(QPainter *painter, const QRectF &rect);
    void drawBackground(QPainter *painter, const QRectF &rect);

    void removeItem(QGraphicsItem *item);

    QList<GraphicsItem*> graphicsItems() const;
    QList<GraphicsItem*> selectedGraphicsItems() const;

    void startEditing(Editing::State editingState);
    void stopEditing();
    void executeEditingAction(Editing::Action editingAction);

private slots:
    void reactOnSelectionChange();
    void selectOnly(GraphicsItem*);
    void selectAlso(GraphicsItem*);
    void selectNoMore(GraphicsItem*);
    void selectNothing();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);

private:
    enum class SelectionState {
        OneItemRequested,
        OneItem,
        AllItems
    };
    SelectionState m_selectionState;
    Editing::State m_editingState;

    bool m_selectionChangeInProgress;

    QPointF m_clickedPoint;

    SelectionModel *m_selectionModel;

    void initialize();
    void updateAllViews(const QList<QRectF> &updateRegions) const;

    void switchToDefaultState();
    void switchToPointerState();
    void switchToPenState();
    void switchToEraserState();

    void activateAllItemSelection();
    void activateOneItemRequestedSelection();
    void activateOneItemSelection();
    void activateItem(GraphicsItem *item);

    void swapActiveItemTo(GraphicsItem *newActiveItem);

    void setAllItemsEnabled(bool enabled = true, bool selectedOnesExcluded = false);
    void setAllItemsFlags(QGraphicsItem::GraphicsItemFlags);
    void expandAllItemsFlags(QGraphicsItem::GraphicsItemFlags);
    void setAllItemsCursor(const QCursor &cursor);
    void unsetAllItemsCursor();
    void setCursorInViews(const QCursor &cursor);
    void unsetCursorInViews();
    void sendClickToSelectedItems();
    void reduceSelection(int leaveBehind);
    bool isGraphicsItemEditing(Editing::State editingStyle) const;

};

#endif // GRAPHICS_SCENE
