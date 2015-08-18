#include "preferences.h"
#include "main_gui_objects/GraphicsScene.h"
#include "main_gui_objects/Cursor.h"
#include <cmath>

//****************************************************************************
//********** PUBLIC **********************************************************
//****************************************************************************

GraphicsScene::GraphicsScene(QObject *parent) : QGraphicsScene(parent) {
    initialize();
}

GraphicsScene::GraphicsScene(qreal x, qreal y, qreal width, qreal height, QObject *parent) : QGraphicsScene(x, y, width, height, parent) {
    initialize();
}

void GraphicsScene::initialize() {
    m_selectionModel = 0;
    m_selectionChangeInProgress = false;
    m_editingState = Editing::NoEditing;

    setForegroundBrush(QColor(50, 50, 50, 50));
    setSceneRect(-700, -1000, 2700, 2000);
}

GraphicsScene::~GraphicsScene() {
}

void GraphicsScene::setSelectionModel(SelectionModel *selectionModel) {
    m_selectionModel = selectionModel;
    connect(this, SIGNAL(selectionChanged()), this, SLOT(informModelOfSelectionChange()));
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

QList<GraphicsItem*> GraphicsScene::selectedGraphicsItems() const {
    QList<GraphicsItem*> list;
    foreach(QGraphicsItem *qItem, selectedItems()) {
        if(GraphicsItem::isGraphicsItem(qItem)) {
            list << static_cast<GraphicsItem*>(qItem);
        }
    }
    return list;
}

//Before every startEditing a stopEditing is called.
//So don't stop things again!
void GraphicsScene::startEditing(Editing::State editingState) {
    m_editingState = editingState;
    if(m_editingState == Editing::Pointer)
        setAllItemsCursor(CursorRental::pointer());
    else if(m_editingState == Editing::Eraser)
        setAllItemsCursor(CursorRental::eraser());
}

void GraphicsScene::stopEditing() {
    m_editingState = Editing::NoEditing;
    unsetAllItemsCursor();
}

void GraphicsScene::executeEditingAction(Editing::Action editingAction) {
    Q_UNUSED(editingAction);
    stopEditing();
}

//****************************************************************************
//********** PRIVATE SLOTS ****************************************************
//****************************************************************************

void GraphicsScene::informModelOfSelectionChange() {
    if(m_selectionModel)
        m_selectionModel->sceneSelectionChanged(this);
}

//****************************************************************************
//********** PROTECTED *********************************************************
//****************************************************************************

void GraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) {
    m_clickedPoint = mouseEvent->scenePos();
    if(Editing::demandsWholeCanvas(m_editingState)) {
        mouseEvent->accept();
        return;
    }
    QGraphicsScene::mousePressEvent(mouseEvent);
}

void GraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent) {
    if(Editing::demandsWholeCanvas(m_editingState) && m_clickedPoint == mouseEvent->scenePos()) {
        //Following: Hack to select the select the suitable QGraphicsItem, if clicked on one
        //This is necessary, as the selection happens on the mousePressEvent,
        //but if it is selected, then the mouse is released, an editing click is send
        //to the graphics item, which was meant to be a selection click
        if(selectedGraphicsItems().isEmpty()) {
            QGraphicsScene::mousePressEvent(mouseEvent);
        } else {
            sendClickToSelectedItems();
            mouseEvent->accept();
            return;
        }
    }
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}

//****************************************************************************
//********** PRIVATE *********************************************************
//****************************************************************************
void GraphicsScene::setAllItemsCursor(const QCursor &cursor) {
    foreach(QGraphicsItem* item, items()) {
        item->setCursor(cursor);
    }
}

void GraphicsScene::unsetAllItemsCursor() {
    foreach(QGraphicsItem* item, items()) {
        item->unsetCursor();
    }
}

void GraphicsScene::sendClickToSelectedItems() {
    foreach(GraphicsItem *item, selectedGraphicsItems()) {
        item->receivedClickOn(m_clickedPoint);
    }
}
