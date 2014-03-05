#ifndef GRAPHICS_SCENE
#define GRAPHICS_SCENE

#include "stable.h"

class GraphicsScene : public QGraphicsScene {

public:
    GraphicsScene(QObject *parent = 0);
    GraphicsScene(qreal x, qreal y, qreal width, qreal height, QObject *parent = 0);
    ~GraphicsScene();

    void drawForeground(QPainter *painter, const QRectF &rect);
    void drawBackground(QPainter *painter, const QRectF &rect);

    void addItem(QGraphicsItem *item);

private:

    void updateAllViews(const QList<QRectF> &updateRegions) const;
    void initialize();

};

#endif // GRAPHICS_SCENE
