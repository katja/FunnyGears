#include "GraphicsScene.h"
#include "Cursor.h"
#include "preferences.h"
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
    std::cout << "GraphicsScene is created" << std::endl;
    m_selectionModel = 0;
    m_selectionChangeInProgress = false;
    m_editingState = Editing::NoEditing;

    setForegroundBrush(QColor(50, 50, 50, 50));
    setSceneRect(-1000, -1000, 2000, 2000);
}

GraphicsScene::~GraphicsScene() {
    std::cout << "GraphicsScene is deleted" << std::endl;
    //TODO: I think the Graphicsitems should be deleted by the scene itself, so no removing before has to take place
    //      Like that they are automatically deleted from QGraphicsScene.
    // foreach(GraphicsItem *graphicsItem, graphicsItems()) {
    //     removeItem(graphicsItem);
    // }
}

void GraphicsScene::setSelectionModel(SelectionModel *selectionModel) {
    m_selectionModel = selectionModel;
    connect(this, SIGNAL(selectionChanged()), this, SLOT(reactOnSelectionChange()));
    connect(m_selectionModel, SIGNAL(selectOnly(GraphicsItem*)), this, SLOT(selectOnly(GraphicsItem*)));
    connect(m_selectionModel, SIGNAL(selectAlso(GraphicsItem*)), this, SLOT(selectAlso(GraphicsItem*)));
    connect(m_selectionModel, SIGNAL(selectNoMore(GraphicsItem*)), this, SLOT(selectNoMore(GraphicsItem*)));
    connect(m_selectionModel, SIGNAL(selectNothing()), this, SLOT(selectNothing()));
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
}

void GraphicsScene::stopEditing() {
    m_editingState = Editing::NoEditing;
}

void GraphicsScene::executeEditingAction(Editing::Action editingAction) {
    Q_UNUSED(editingAction);
    stopEditing();
}

//****************************************************************************
//********** PRIVATE SLOTS ****************************************************
//****************************************************************************

void GraphicsScene::reactOnSelectionChange() {
    if(m_selectionModel && !m_selectionChangeInProgress) //TODO: delete m_selectionChangeInProgress, if it is not longer needed!!!
        m_selectionModel->sceneSelectionChanged(this);
}

//****************************************************************************
//********** PROTECTED *********************************************************
//****************************************************************************

void GraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) {
    std::cout << "GraphicsScene::mousePressEvent tracked" << std::endl;
    m_clickedPoint = mouseEvent->scenePos();
    if(Editing::demandsWholeCanvas(m_editingState)) {
        mouseEvent->accept();
        std::cout << "          and accepted" << std::endl;
        return;
    }
    QGraphicsScene::mousePressEvent(mouseEvent);
}

void GraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent) {
    std::cout << "GraphicsScene::mouseReleaseEvent tracked" << std::endl;
    if(Editing::demandsWholeCanvas(m_editingState) && m_clickedPoint == mouseEvent->scenePos()) {
        //Following: Hack to select the select the suitable QGraphicsItem, if clicked on one
        //This is necessary, as the selection happens on the mousePressEvent,
        //but if it is selected, then the mouse is released, an editing click is send
        //to the graphics item, which was meant to be a selection click
        if(selectedGraphicsItems().isEmpty()) {
            std::cout << "          and is now send away with mousePressEvent" << std::endl;
            QGraphicsScene::mousePressEvent(mouseEvent);
        } else {
            std::cout << "          and is sent to selected items now:" << std::endl;
            sendClickToSelectedItems();
            mouseEvent->accept();
            std::cout << "          and accepted" << std::endl;
            return;
        }
    }
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}

//****************************************************************************
//********** PRIVATE *********************************************************
//****************************************************************************
void GraphicsScene::setAllItemsCursor(const QCursor &cursor) {
    //TODO: actually here an GraphicsItem should be used instead of QGraphicsItem!!!
    //But as items() is probably much faster than graphicsItems(), I use this variant up to now.
    foreach(QGraphicsItem* item, items()) {
        item->setCursor(cursor);
    }
}

void GraphicsScene::unsetAllItemsCursor() {
    //TODO: actually here an GraphicsItem should be used instead of QGraphicsItem!!!
    //But as items() is probably much faster than graphicsItems(), I use this variant up to now.
    foreach(QGraphicsItem* item, items()) {
        item->unsetCursor();
    }
}

void GraphicsScene::setCursorInViews(const QCursor &cursor) {
    foreach(QGraphicsView *view, views()) {
        view->setCursor(cursor);
    }
}

void GraphicsScene::unsetCursorInViews() {
    foreach(QGraphicsView *view, views()) {
        view->unsetCursor();
    }
}

void GraphicsScene::sendClickToSelectedItems() {
    foreach(GraphicsItem *item, selectedGraphicsItems()) {
        item->receivedClickOn(m_clickedPoint);
    }
}
