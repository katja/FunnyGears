#ifndef GRAPHICS_SCENE
#define GRAPHICS_SCENE

#include "stable.h"
#include "ConnectionSelectionModel.h"

class GraphicsScene : public QGraphicsScene {

Q_OBJECT

public:
    GraphicsScene(QObject *parent = 0);
    GraphicsScene(qreal x, qreal y, qreal width, qreal height, QObject *parent = 0);
    ~GraphicsScene();

    void setSelectionModel(ConnectionSelectionModel *selectionModel);

    void drawForeground(QPainter *painter, const QRectF &rect);
    void drawBackground(QPainter *painter, const QRectF &rect);

    void addItem(QGraphicsItem *item);

private slots:
    void updateItems(const QList<QGraphicsItem*> &changedItems);
    void informModelOfSelectionChange();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);

private:

    ConnectionSelectionModel *m_selectionModel;

    void updateAllViews(const QList<QRectF> &updateRegions) const;
    void initialize();

};

#endif // GRAPHICS_SCENE
