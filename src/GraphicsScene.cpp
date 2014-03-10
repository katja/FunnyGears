#include "GraphicsScene.h"
#include <cmath>

GraphicsScene::GraphicsScene(QObject *parent) : QGraphicsScene(parent) {
    initialize();
}

GraphicsScene::GraphicsScene(qreal x, qreal y, qreal width, qreal height, QObject *parent) : QGraphicsScene(x, y, width, height, parent) {
    initialize();
}

void GraphicsScene::initialize() {
    std::cout << "GraphicsScene is created" << std::endl;
    setForegroundBrush(QColor(50, 50, 50, 50));
    setSceneRect(-1000, -1000, 2000, 2000);

    connect(this, SIGNAL(selectionChanged()), this, SLOT(informModelOfSelectionChange()));
}

GraphicsScene::~GraphicsScene() {
    std::cout << "GraphicsScene is deleted" << std::endl;
}

void GraphicsScene::setSelectionModel(ConnectionSelectionModel *selectionModel) {
    m_selectionModel = selectionModel;
}

void GraphicsScene::drawBackground(QPainter *painter, const QRectF &rect) {

    QPen pen(Qt::lightGray);
    pen.setWidth(0); // => cosmetic pen (always one pixel wide, independent of transformation)
    painter->setPen(pen);
    painter->setRenderHint(QPainter::Antialiasing);

    int gridWidth = 10;

    // Only draw the lines in the visible rect.
    // This is much faster than drawing all the 200 lines.
    // Always align the grid with a cross at the origin.
    qreal rx1, rx2, rwidth, rheight;
    rect.getRect(&rx1, &rx2, &rwidth, &rheight);
    int x1, x2, width, height;
    x1 = static_cast<int>(floor(rx1));
    x2 = static_cast<int>(floor(rx2));
    width = static_cast<int>(ceil(rwidth));
    height = static_cast<int>(ceil(rheight));

    int x1Start, x2Start;
    if(x1 < 0)
        x1Start = x1 - gridWidth + (qAbs(x1) % gridWidth);
    else
        x1Start = x1 + gridWidth - (x1 % gridWidth);

    if(x2 < 0)
        x2Start = x2 - gridWidth + (qAbs(x2) % gridWidth);
    else
        x2Start = x2 + gridWidth - (x2 % gridWidth);


    for(int i = x1Start; i <= width + x1; i += gridWidth) {
        painter->drawLine(i, x2, i, x2 + height);
    }
    for(int i = x2Start; i <= height + x2; i += gridWidth) {
        painter->drawLine(x1, i, x1 + width, i);
    }
}

void GraphicsScene::drawForeground(QPainter *painter, const QRectF &rect) {
    Q_UNUSED(rect);

    QPen pen(Qt::gray, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    painter->setPen(pen);
    painter->drawLine(-50, 0, 150, 0);
    painter->drawLine(0, -50, 0, 150);
    painter->drawLine(140, 10, 150, 0);
    painter->drawLine(140, -10, 150, 0);
    painter->drawLine(10, 140, 0, 150);
    painter->drawLine(-10, 140, 0, 150);
    painter->drawEllipse(-5, -5, 10, 10);
}

void GraphicsScene::addItem(QGraphicsItem *item) {
    QGraphicsScene::addItem(item);
    QList<QRectF> updateRegions;
    updateRegions.append(item->sceneBoundingRect());
    updateAllViews(updateRegions);
}

void GraphicsScene::informModelOfSelectionChange() {
    m_selectionModel->sceneSelectionChanged(this);
}

void GraphicsScene::updateAllViews(const QList<QRectF> &updateRegions) const {
    QList<QGraphicsView*> views = QGraphicsScene::views();
    foreach(QGraphicsView *view, views) {
        view->updateScene(updateRegions);
    }
}
