#include "main_gui_objects/ObjectScheduleViewWidget.h"
#include "main_gui_objects/YesNoDelegate.h"

#include "graphics_objects/GraphicsSpline.h"
#include "graphics_objects/GraphicsSplineGear.h"
#include "graphics_objects/GraphicsInvoluteGear.h"
#include "graphics_objects/GraphicsGearPair.h"


ObjectScheduleViewWidget::ObjectScheduleViewWidget(Model *model, QWidget *parent)
: QDockWidget("Object Schedule", parent, Qt::Widget), m_model(model) {

    m_treeView = new ModelTreeView(m_model);

    m_addItemButton      = new QPushButton(QIcon(":/images/Add.png"), "", this);
    m_removeItemButton   = new QPushButton(QIcon(":/images/Remove.png"), "", this);

    setWidgetLayout();

    QAction *addSplineAction = new QAction("Create spline curve", this);
    addSplineAction->setShortcut(QKeySequence(Qt::Key_S)); // S like Spline, a combination is not working, f... !

    m_curveAsGearAction = new QAction("Use curve as gear tooth", this);
    m_curveAsGearAction->setShortcut(QKeySequence(Qt::Key_G)); // G like Gear, a combination is not working, f... !
    m_curveAsGearAction->setEnabled(false);
    m_spline = nullptr;

    m_pairGearAction = new QAction("Construct a mating gear", this);
    m_pairGearAction->setShortcut(QKeySequence(Qt::Key_M)); // M like Mating gear
    m_pairGearAction->setEnabled(false);
    m_graphicsSplineGear = nullptr;

    QAction *addInvoluteGearAction = new QAction("Add involute gear", this);

    m_addMenu = new QMenu(this);
    m_addMenu->addAction(addSplineAction);
    m_addMenu->addAction(m_curveAsGearAction);
    m_addMenu->addAction(m_pairGearAction);
    m_addMenu->addAction(addInvoluteGearAction);

    m_addItemButton->setMenu(m_addMenu);

    connect(addSplineAction,       SIGNAL(triggered()), this, SLOT(addSpline()));
    connect(m_curveAsGearAction,   SIGNAL(triggered()), this, SLOT(addGearTooth()));
    connect(m_pairGearAction,      SIGNAL(triggered()), this, SLOT(addGearPair()));
    connect(addInvoluteGearAction, SIGNAL(triggered()), this, SLOT(addInvoluteGear()));
    connect(m_removeItemButton,    SIGNAL(clicked()),   this, SLOT(removeItems()));
}

ObjectScheduleViewWidget::~ObjectScheduleViewWidget() {
}

SelectionModel* ObjectScheduleViewWidget::setSelectionModel(SelectionModel *model) {
    m_treeView->setSelectionModel(model);

    SelectionModel *oldModel = m_selectionModel;
    m_selectionModel = model;

    connect(m_selectionModel, SIGNAL(oneGraphicsItemSelected(GraphicsItem*)), this, SLOT(addActionsFor(GraphicsItem*)));
    connect(m_selectionModel, SIGNAL(noneOrManyGraphicsItemsSelected()), this, SLOT(removeAdditionalActions()));

    return oldModel;
}

void ObjectScheduleViewWidget::addActionsFor(GraphicsItem *graphicsItem) {
    m_curveAsGearAction->setEnabled(false);
    m_pairGearAction->setEnabled(false);
    m_spline = nullptr;
    m_graphicsSplineGear = nullptr;

    if(GraphicsSpline::isGraphicsSplineItem(graphicsItem)) {
        m_curveAsGearAction->setEnabled(true);
        GraphicsSpline *graphicsSpline = static_cast<GraphicsSpline*>(graphicsItem);
        if(graphicsSpline)
            m_spline = graphicsSpline->spline();
        else
            m_spline = nullptr;
    } else if(GraphicsSplineGear::isGraphicsSplineGearItem(graphicsItem)) {
        m_pairGearAction->setEnabled(true);
        m_graphicsSplineGear = static_cast<GraphicsSplineGear*>(graphicsItem);
        if(!m_graphicsSplineGear)
            m_graphicsSplineGear = nullptr;
    }
}

void ObjectScheduleViewWidget::removeAdditionalActions() {
    m_curveAsGearAction->setEnabled(false);
    m_spline = nullptr;
    m_pairGearAction->setEnabled(false);
    m_graphicsSplineGear = nullptr;
}

void ObjectScheduleViewWidget::addSpline() {
    GraphicsSpline *spline = new GraphicsSpline();
    m_model->addItem(spline);
    m_model->clearSelection();
    spline->setSelected(true);
}

void ObjectScheduleViewWidget::addGearTooth() {
    if(m_spline == nullptr)
        return;
    SplineGear *gear = new SplineGear(*m_spline);
    GraphicsSplineGear *graphicsGear = new GraphicsSplineGear(gear);
    m_model->addItem(graphicsGear);
    m_model->clearSelection();
    graphicsGear->setSelected(true);
}

void ObjectScheduleViewWidget::addGearPair() {
    if(m_graphicsSplineGear == nullptr)
        return;
    GraphicsGearPair *graphicsgearPair = new GraphicsGearPair(m_graphicsSplineGear->constructAMatingGear());
    m_model->addItem(graphicsgearPair);
    m_model->clearSelection();
    graphicsgearPair->setSelected(true);
}

void ObjectScheduleViewWidget::addInvoluteGear() {
    GraphicsInvoluteGear *gear = new GraphicsInvoluteGear();
    m_model->addItem(gear);
}

//Helpermethod for removeItems()
bool isBelow(QModelIndex &a, QModelIndex &b) {
    if(a.row() < b.row())
        return false;
    else
        return true;
}
void ObjectScheduleViewWidget::removeItems() {
    QModelIndexList selectedItems = m_treeView->selectedIndexes();

    //Warn the user with a message box:
    if(selectedItems.empty())
        return;

    QString messageText = "Do you really want to delete the selected ";
    if(selectedItems.size() > 1)
        messageText.append("items?");
    else
        messageText.append("item?");

    QMessageBox askPermissionDialog(this);
    askPermissionDialog.setText(messageText);
    askPermissionDialog.setInformativeText("Selected items will have gone until doomsday! You can't undo this action!");
    askPermissionDialog.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
    askPermissionDialog.setDefaultButton(QMessageBox::Yes);
    int answer = askPermissionDialog.exec();
    if(answer == QMessageBox::Yes) { // = 1 => dialog accepted => delete items

        //sort items from bottom to top, otherwise row number will be useless after first rows are deleted
        qSort(selectedItems.begin(), selectedItems.end(), isBelow);

        //only delete entries, whose line (row) was marked fully!
        foreach(QModelIndex index, selectedItems) {
            int markedColumnsCounter = 1;
            foreach(QModelIndex index2, selectedItems) {
                if(index.row() == index2.row() && index.column() < index2.column()) {
                    ++markedColumnsCounter;
                }
            }
            if(markedColumnsCounter >= m_model->columnCount()) {
                m_model->remove(index);
            }
        }
    }
}

void ObjectScheduleViewWidget::setWidgetLayout() {
    QWidget *mainWidget  = new QWidget(this);

    QGridLayout *gLayout = new QGridLayout(mainWidget);
    gLayout->addWidget(m_treeView,        0, 0, 1, 2); // addWidget(widget, fromRow, fromColumn, rowSpan, columnSpan, alignment = 0)
    gLayout->addWidget(m_addItemButton,   2, 0);
    gLayout->addWidget(m_removeItemButton,2, 1);

    mainWidget->setLayout(gLayout);
    setWidget(mainWidget);
}
