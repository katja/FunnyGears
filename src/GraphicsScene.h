#ifndef GRAPHICS_SCENE
#define GRAPHICS_SCENE

#include "stable.h"
#include "ConnectionModel.h"
#include "GraphicsSpline.h"
#include "GraphicsItem.h"

class GraphicsScene : public QGraphicsScene {

Q_OBJECT

public:
    GraphicsScene(QObject *parent = 0);
    GraphicsScene(qreal x, qreal y, qreal width, qreal height, QObject *parent = 0);
    ~GraphicsScene();

    void setConnectionModel(ConnectionModel *selectionModel);

    void drawForeground(QPainter *painter, const QRectF &rect);
    void drawBackground(QPainter *painter, const QRectF &rect);

    void addItem(QGraphicsItem *item);
    void removeItem(QGraphicsItem *item);

    QList<GraphicsItem*> selectedGraphicsItems() const;

    void startEditing(EditingState editingStyle);

public slots:
    void stopEditing();

private slots:
    void updateItem(QGraphicsItem *item);
    void updateItems(const QList<QGraphicsItem*> &changedItems);
    void informModelOfSelectionChange();
    void startEditingItem(QGraphicsItem *item, EditingState editingStyle = EditingState::Add);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void keyReleaseEvent(QKeyEvent *keyEvent);

private:
    enum State {
        VIEW, EDITING
    };
    State m_currentState;
    GraphicsItem *m_currentItem;
    GraphicsSpline *m_currentSpline;
    QPointF m_clickedPoint;

    ConnectionModel *m_selectionModel;

    void initialize();
    void updateAllViews(const QList<QRectF> &updateRegions) const;
    void setAllItemsEnabled(bool enabled = true);
    void reduceSelection(int leaveBehind);

};

#endif // GRAPHICS_SCENE
