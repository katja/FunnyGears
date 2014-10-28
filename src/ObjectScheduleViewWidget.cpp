#include "ObjectScheduleViewWidget.h"

#include "YesNoDelegate.h"

#include "graphics_objects/GraphicsSpline.h"
#include "graphics_objects/GraphicsSimpleGear.h"
#include "graphics_objects/GraphicsInvoluteGear.h"

ObjectScheduleViewWidget::ObjectScheduleViewWidget(SceneTreeModel *model, QWidget *parent)
: QDockWidget("Object Schedule", parent, Qt::Widget), m_sceneTreeModel(model) {
    std::cout << "ObjectScheduleViewWidget is created" << std::endl;

    m_treeView = new TreeView(this);
    setTreeViewLayout();

    m_addItemButton      = new QPushButton(QIcon("images/Add.png"), "", this);
    m_removeItemButton   = new QPushButton(QIcon("images/Remove.png"), "", this);

    setWidgetLayout();

    QAction *addSplineAction = new QAction("Create own spline curve", this);
    QAction *addGearToothAction = new QAction("Create own gear", this);
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

ConnectionModel* ObjectScheduleViewWidget::setConnectionModel(ConnectionModel *model) {
    m_treeView->setSelectionModel(model);
    ConnectionModel *oldModel = m_connectionModel;
    m_connectionModel = model;
    return oldModel;
}

void ObjectScheduleViewWidget::addSpline() {
    GraphicsSpline *spline = new GraphicsSpline();
    m_sceneTreeModel->addItem(spline);
}

void ObjectScheduleViewWidget::addGearTooth() {
    GraphicsSimpleGear *gear = new GraphicsSimpleGear();
    m_sceneTreeModel->addItem(gear);
}

void ObjectScheduleViewWidget::addInvoluteGear() {
    GraphicsInvoluteGear *gear = new GraphicsInvoluteGear();
    m_sceneTreeModel->addItem(gear);
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
            if(markedColumnsCounter >= m_sceneTreeModel->columnCount()) {
                m_sceneTreeModel->remove(index);
            }
        }
    }
}

void ObjectScheduleViewWidget::setTreeViewLayout() {
    m_treeView->setHeaderHidden(false);
    m_treeView->setIndentation(0);
    m_treeView->setSelectionMode(QAbstractItemView::ExtendedSelection); //this mode allows a selection of multiple items with mouse, ctrl-key and shift-key
    m_treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_treeView->setEditTriggers(QAbstractItemView::DoubleClicked);
    m_treeView->setItemDelegate(new YesNoDelegate(this));
    m_treeView->setModel(m_sceneTreeModel);

    m_treeView->setColumnWidth(SceneTreeModel::NAME, 150);
    m_treeView->setColumnWidth(SceneTreeModel::VISIBILITY, 30);
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
