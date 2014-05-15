#include "GraphicsScene.h"
#include "ConnectionModel.h"
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
    m_connectionModel = 0;
    m_selectionChangeInProgress = false;
    switchToDefaultState();

    setForegroundBrush(QColor(50, 50, 50, 50));
    setSceneRect(-1000, -1000, 2000, 2000);
}

GraphicsScene::~GraphicsScene() {
    std::cout << "GraphicsScene is deleted" << std::endl;
}

void GraphicsScene::setConnectionModel(ConnectionModel *connectionModel) {
    m_connectionModel = connectionModel;
    connect(this, SIGNAL(selectionChanged()), this, SLOT(reactOnSelectionChange()));
    connect(m_connectionModel, SIGNAL(selectOnly(GraphicsItem*)), this, SLOT(selectOnly(GraphicsItem*)));
    connect(m_connectionModel, SIGNAL(selectAlso(GraphicsItem*)), this, SLOT(selectAlso(GraphicsItem*)));
    connect(m_connectionModel, SIGNAL(selectNoMore(GraphicsItem*)), this, SLOT(selectNoMore(GraphicsItem*)));
    connect(m_connectionModel, SIGNAL(selectNothing()), this, SLOT(selectNothing()));
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

void GraphicsScene::removeItem(QGraphicsItem *item) {
    if(mouseGrabberItem() == item && m_selectionState == SelectionState::OneItem) {
        activateAllItemSelection();
        m_selectionState = SelectionState::OneItemRequested;
    }
    QGraphicsScene::removeItem(item);
}

QList<GraphicsItem*> GraphicsScene::graphicsItems() const {
    QList<GraphicsItem*> list;
    foreach(QGraphicsItem *qItem, items()) {
        if(GraphicsItem::isGraphicsItem(qItem)) {
            list << static_cast<GraphicsItem*>(qItem);
        }
    }
    return list;
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
    if(isGraphicsItemEditing(editingState))
        m_editingState = editingState;
    if(editingState == Editing::Pointer)
        switchToPointerState();
    else if (editingState == Editing::Pen)
        switchToPenState();
    else if (editingState == Editing::Eraser)
        switchToEraserState();
    else if (editingState == Editing::NoEditing)
        std::cout << "START EDITING WITH NO_EDITING CALLED! WHY????????" << std::endl;
}

void GraphicsScene::stopEditing() {
    switchToDefaultState();
}

void GraphicsScene::executeEditingAction(Editing::Action editingAction) {
    Q_UNUSED(editingAction);
    stopEditing();
}

//****************************************************************************
//********** PRIVATE SLOTS ****************************************************
//****************************************************************************

void GraphicsScene::reactOnSelectionChange() {
    if(m_connectionModel && !m_selectionChangeInProgress)
        m_connectionModel->sceneSelectionChanged(this);
    if(m_selectionState == SelectionState::OneItemRequested)
        activateOneItemSelection();
}

void GraphicsScene::selectOnly(GraphicsItem *qItem) {
    m_selectionChangeInProgress = true;
    if(m_selectionState == SelectionState::OneItem)
        swapActiveItemTo(static_cast<GraphicsItem*>(qItem));
    else {
        clearSelection();
        if(qItem->flags() & QGraphicsItem::ItemIsSelectable)
            qItem->setSelected(true);
        else
            std::cout << "selection was not possible :-(" << std::endl;
    }
    m_selectionChangeInProgress = false;
    reactOnSelectionChange();
}

void GraphicsScene::selectAlso(GraphicsItem *qItem) {
    if(m_selectionState == SelectionState::OneItem)
        //no more item can be selected => inform connection model
        reactOnSelectionChange();
    else if(qItem->flags() & QGraphicsItem::ItemIsSelectable)
        qItem->setSelected(true);
    else
        std::cout << "selection was not possible :-(" << std::endl;
}

void GraphicsScene::selectNoMore(GraphicsItem *qItem) {
    if(m_selectionState == SelectionState::OneItem && qItem->isSelected()) {
        selectNothing();
    } else {
        qItem->setSelected(false);
    }
}

void GraphicsScene::selectNothing() {
    if(m_selectionState == SelectionState::OneItem) {
        //As all items have to be selectable again, editing has to stop first and then be started again!
        activateAllItemSelection();
        clearSelection();
        activateOneItemSelection();
    } else {
        clearSelection();
    }
}

//****************************************************************************
//********** PROTECTED *********************************************************
//****************************************************************************

void GraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) {
    m_clickedPoint = mouseEvent->scenePos();
    if(m_selectionState == SelectionState::OneItem)
        mouseEvent->accept();
    else
        QGraphicsScene::mousePressEvent(mouseEvent);
}

void GraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent) {
    if(m_selectionState == SelectionState::OneItem
        && m_clickedPoint == mouseEvent->scenePos()) {
        sendClickToSelectedItems();
        mouseEvent->accept();
        return;
    }
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}

//****************************************************************************
//********** PRIVATE *********************************************************
//****************************************************************************

void GraphicsScene::updateAllViews(const QList<QRectF> &updateRegions) const {
    QList<QGraphicsView*> views = QGraphicsScene::views();
    foreach(QGraphicsView *view, views) {
        view->updateScene(updateRegions);
    }
}

void GraphicsScene::switchToDefaultState() {
    m_editingState = Editing::NoEditing;
    activateAllItemSelection();
}

void GraphicsScene::switchToPointerState() {
    m_editingState = Editing::Pointer;
    activateAllItemSelection();
    expandAllItemsFlags(
        QGraphicsItem::ItemIsMovable |
        QGraphicsItem::ItemSendsGeometryChanges);
}

void GraphicsScene::switchToPenState() {
    m_editingState = Editing::Pen;
    activateOneItemSelection();
}

void GraphicsScene::switchToEraserState() {
    m_editingState = Editing::Eraser;
    activateOneItemSelection();
}

void GraphicsScene::activateAllItemSelection() {
    m_selectionState = SelectionState::AllItems;
    if(mouseGrabberItem())
        mouseGrabberItem()->ungrabMouse();
    setAllItemsFlags(QGraphicsItem::ItemIsSelectable);
    setAllItemsEnabled(true);
}

void GraphicsScene::activateOneItemSelection() {
    m_selectionState = SelectionState::OneItemRequested;
    if(selectedGraphicsItems().empty()) {
        return;
    } else {
        if(selectedGraphicsItems().size() > 1)
            reduceSelection(1);
        m_selectionState = SelectionState::OneItem;
        setAllItemsFlags(0);
        setAllItemsEnabled(false, true);
        activateItem(selectedGraphicsItems().first());
        return;
    }
}

void GraphicsScene::activateItem(GraphicsItem *item) {
    item->setFlags(
        QGraphicsItem::ItemIsSelectable |
        QGraphicsItem::ItemIsMovable |
        QGraphicsItem::ItemSendsGeometryChanges);
    item->setEnabled(true);
    item->setSelected(true);
    item->grabMouse();
}

//TODO: method is not used!!!
void GraphicsScene::swapActiveItemTo(GraphicsItem *newActiveItem) {
    if(m_selectionState != SelectionState::OneItem)
        return;
    GraphicsItem *currentItem = selectedGraphicsItems().first();
    currentItem->setFlags(0);
    currentItem->setEnabled(false);
    activateItem(newActiveItem);
}

void GraphicsScene::setAllItemsEnabled(bool enabled, bool selectedOnesExcluded) {
    //TODO: actually here an GraphicsItem should be used instead of QGraphicsItem!!!
    //But as items() is probably much faster than graphicsItems(), I use this variant up to now.
    foreach(QGraphicsItem* item, items()) {
        if(!(selectedOnesExcluded && item->isSelected()))
            item->setEnabled(enabled);
    }
}

void GraphicsScene::setAllItemsFlags(QGraphicsItem::GraphicsItemFlags flags) {
    //TODO: actually here an GraphicsItem should be used instead of QGraphicsItem!!!
    //But as items() is probably much faster than graphicsItems(), I use this variant up to now.
    foreach(QGraphicsItem* item, items()) {
        if(item->isSelected())
            item->setFlags(flags | QGraphicsItem::ItemIsSelectable);
        else
            item->setFlags(flags);
    }
}

void GraphicsScene::expandAllItemsFlags(QGraphicsItem::GraphicsItemFlags flags) {
    //TODO: actually here an GraphicsItem should be used instead of QGraphicsItem!!!
    //But as items() is probably much faster than graphicsItems(), I use this variant up to now.
    foreach(QGraphicsItem* item, items()) {
        item->setFlags(flags | item->flags());
    }
}

void GraphicsScene::sendClickToSelectedItems() {
    foreach(GraphicsItem *item, selectedGraphicsItems()) {
        item->clickReceived(m_clickedPoint, m_editingState);
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
