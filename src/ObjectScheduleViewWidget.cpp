#include "ObjectScheduleViewWidget.h"
#include "YesNoDelegate.h"

#include "graphics_objects/GraphicsSpline.h"
#include "graphics_objects/GraphicsSplineGear.h"
#include "graphics_objects/GraphicsInvoluteGear.h"

#include "basic_objects/SplineGear.h"
#include "basic_objects/SplineToothProfile.h"

ObjectScheduleViewWidget::ObjectScheduleViewWidget(Model *model, QWidget *parent)
: QDockWidget("Object Schedule", parent, Qt::Widget), m_model(model) {
    std::cout << "ObjectScheduleViewWidget is created" << std::endl;

    m_treeView = new ModelTreeView(m_model);

    m_addItemButton      = new QPushButton(QIcon(":/images/Add.png"), "", this);
    m_removeItemButton   = new QPushButton(QIcon(":/images/Remove.png"), "", this);

    setWidgetLayout();

    QAction *addSplineAction = new QAction("Create spline curve", this);
    QAction *addGearToothAction = new QAction("Use curve as gear tooth", this);
    QAction *addInvoluteGearAction = new QAction("Add involute gear", this);

    m_addMenu = new QMenu(this);
    m_addMenu->addAction(addSplineAction);
    m_addMenu->addAction(addGearToothAction);
    m_addMenu->addAction(addInvoluteGearAction);

    m_addItemButton->setMenu(m_addMenu);

    connect(addSplineAction,       SIGNAL(triggered()), this, SLOT(addSpline()));
    connect(addGearToothAction,    SIGNAL(triggered()), this, SLOT(addGearTooth()));
    connect(addInvoluteGearAction, SIGNAL(triggered()), this, SLOT(addInvoluteGear()));
    connect(m_removeItemButton,    SIGNAL(clicked()),   this, SLOT(removeItems()));
}

ObjectScheduleViewWidget::~ObjectScheduleViewWidget() {
    std::cout << "ObjectScheduleViewWidget is deleted" << std::endl;
}

SelectionModel* ObjectScheduleViewWidget::setSelectionModel(SelectionModel *model) {
    m_treeView->setSelectionModel(model);
    SelectionModel *oldModel = m_selectionModel;
    m_selectionModel = model;
    return oldModel;
}

void ObjectScheduleViewWidget::addSpline() {
    GraphicsSpline *spline = new GraphicsSpline();
    m_model->addItem(spline);
    m_model->clearSelection();
    spline->setSelected(true);
}

void ObjectScheduleViewWidget::addGearTooth() {
    SplineGear *gear = new SplineGear(new SplineToothProfile());
    // GraphicsSpline *graphicsGear = new GraphicsSpline(gear);
    GraphicsSplineGear *graphicsGear = new GraphicsSplineGear(gear);
    m_model->addItem(graphicsGear);
    m_model->clearSelection();
    graphicsGear->setSelected(true);
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
    gLayout->addWidget(m_treeView,  0, 0, 1, 2); // addWidget(widget, fromRow, fromColumn, rowSpan, columnSpan, alignment = 0)
    gLayout->addWidget(m_addItemButton,   2, 0);
    gLayout->addWidget(m_removeItemButton,2, 1);

    mainWidget->setLayout(gLayout);
    setWidget(mainWidget);
}
