#ifndef GRAPHICS_SCENE
#define GRAPHICS_SCENE

#include "stable.h"
#include "ConnectionModel.h"
#include "GraphicsSpline.h"

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

private slots:
    void updateItem(QGraphicsItem *item);
    void updateItems(const QList<QGraphicsItem*> &changedItems);
    void informModelOfSelectionChange();
    void startEditingItem(QGraphicsItem *item);
    void stopEditing();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void keyReleaseEvent(QKeyEvent *keyEvent);

private:
    enum State {
        VIEW, EDITING
    };
    State m_currentState;
    GraphicsSpline *m_currentSpline;
    QPointF m_clickedPoint;

    ConnectionModel *m_selectionModel;

    void initialize();
    void updateAllViews(const QList<QRectF> &updateRegions) const;
    void setAllItemsEnabled(bool enabled = true);

};

#endif // GRAPHICS_SCENE
