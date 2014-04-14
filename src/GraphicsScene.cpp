#include "GraphicsScene.h"
#include "ConnectionModel.h"
#include "preferences.h"
#include <cmath>

GraphicsScene::GraphicsScene(QObject *parent) : QGraphicsScene(parent) {
    initialize();
}

GraphicsScene::GraphicsScene(qreal x, qreal y, qreal width, qreal height, QObject *parent) : QGraphicsScene(x, y, width, height, parent) {
    initialize();
}

void GraphicsScene::initialize() {
    std::cout << "GraphicsScene is created" << std::endl;
    m_currentState = GraphicsScene::VIEW;
    m_currentItem = 0;
    setForegroundBrush(QColor(50, 50, 50, 50));
    setSceneRect(-1000, -1000, 2000, 2000);

    connect(this, SIGNAL(selectionChanged()), this, SLOT(informModelOfSelectionChange()));
}

GraphicsScene::~GraphicsScene() {
    std::cout << "GraphicsScene is deleted" << std::endl;
}

void GraphicsScene::setConnectionModel(ConnectionModel *selectionModel) {
    m_selectionModel = selectionModel;
    connect(m_selectionModel, SIGNAL(updateRegionInScene(const QList<QGraphicsItem*>&)), this, SLOT(updateItems(const QList<QGraphicsItem*>&)));
    connect(m_selectionModel, SIGNAL(startEditingItem(QGraphicsItem*)), this, SLOT(startEditingItem(QGraphicsItem*)));
}

void GraphicsScene::drawBackground(QPainter *painter, const QRectF &rect) {

    QPen pen(Preferences::BackgroundGridColor);
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

    QPen pen(Preferences::CoordinateAxisColor, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
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
    updateItem(item);
}

void GraphicsScene::removeItem(QGraphicsItem *item) {
    if(m_currentState == GraphicsScene::EDITING && item == m_currentItem) {
        stopEditing();
        m_currentItem = 0;
    }
    QGraphicsScene::removeItem(item);
}

QList<GraphicsItem*> GraphicsScene::selectedGraphicsItems() const {
    QList<GraphicsItem*> list;
    foreach(QGraphicsItem *qitem, selectedItems()) {

        if(GraphicsItem::isGraphicsItem(qitem)) {
            list << static_cast<GraphicsItem*>(qitem);
        }
    }
    return list;

}

void GraphicsScene::updateItem(QGraphicsItem *item) {
    QList<QRectF> updateRegions;
    updateRegions.append(item->sceneBoundingRect());
    updateAllViews(updateRegions);
}

void GraphicsScene::updateItems(const QList<QGraphicsItem*> &changedItems) {
    QList<QRectF> updateRegions;
    foreach(QGraphicsItem* item, changedItems) {
        update(item->sceneBoundingRect());
        updateRegions << item->sceneBoundingRect();
    }
    updateAllViews(updateRegions);
}

void GraphicsScene::informModelOfSelectionChange() {
    m_selectionModel->sceneSelectionChanged(this);
}

void GraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) {
    QGraphicsScene::mousePressEvent(mouseEvent);
    if(m_currentState == GraphicsScene::VIEW) {
        informModelOfSelectionChange();
    } else if(m_currentState == GraphicsScene::EDITING && !mouseGrabberItem()) {
        m_clickedPoint = mouseEvent->scenePos();
    }
}

void GraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent) {
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
    if(m_currentState == GraphicsScene::VIEW) {
        informModelOfSelectionChange();
    } else if(m_currentState == GraphicsScene::EDITING && !mouseGrabberItem()) {
        if(m_clickedPoint == mouseEvent->scenePos()) {
            m_currentItem->clickReceived(m_clickedPoint);
            updateItem(m_currentItem);
        }
    }
}

void GraphicsScene::keyReleaseEvent(QKeyEvent *keyEvent) {
    if(m_currentState == GraphicsScene::VIEW) {
        QGraphicsScene::keyReleaseEvent(keyEvent);
    } else if(m_currentState == GraphicsScene::EDITING) {
        if(keyEvent->key() == Qt::Key_Escape)
            stopEditing();
    }
}

void GraphicsScene::updateAllViews(const QList<QRectF> &updateRegions) const {
    QList<QGraphicsView*> views = QGraphicsScene::views();
    foreach(QGraphicsView *view, views) {
        view->updateScene(updateRegions);
    }
}

void GraphicsScene::stopEditing() {
    setAllItemsEnabled(true);
    m_currentState = GraphicsScene::VIEW;
    if(m_currentItem)
        m_currentItem->setToState(Editing::NoEditing);
    clearFocus();
}

void GraphicsScene::startEditing(Editing::State editingStyle) {
    if(!isGraphicsItemEditing(editingStyle))
        return;
    if(!selectedGraphicsItems().empty()) {
        if(selectedGraphicsItems().size() > 1)
            reduceSelection(1);
        m_currentItem = selectedGraphicsItems().first();
        startEditingItem(m_currentItem, editingStyle);
    }
}

void GraphicsScene::executeEditingAction(Editing::Action editingAction) {
    Q_UNUSED(editingAction);
    stopEditing();
}

void GraphicsScene::startEditingItem(QGraphicsItem *item, Editing::State editingStyle) {
    if(!GraphicsItem::isGraphicsItem(item))
        return;
    m_currentItem = static_cast<GraphicsItem*>(item);
    m_currentState = GraphicsScene::EDITING;
    setAllItemsEnabled(false);
    m_currentItem->setEnabled(true);
    m_currentItem->setToState(editingStyle);
    setFocus(Qt::OtherFocusReason);
}

void GraphicsScene::setAllItemsEnabled(bool enabled) {
    QList<QGraphicsItem*> allItems = items();
    foreach(QGraphicsItem* item, allItems) {
        item->setEnabled(enabled);
    }
}

void GraphicsScene::reduceSelection(int leaveBehind) {
    QList<GraphicsItem*> selectedItems = selectedGraphicsItems();
    for(int i = leaveBehind; i <= selectedItems.size(); ++i) {
        selectedItems.last()->setSelected(false);
    }
}

bool GraphicsScene::isGraphicsItemEditing(Editing::State editingStyle) const {
    if(editingStyle == Editing::NoEditing ||
        editingStyle == Editing::Pen ||
        editingStyle == Editing::Eraser)
        return true;
    return false;
}
