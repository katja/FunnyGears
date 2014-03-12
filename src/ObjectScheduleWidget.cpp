#include "ObjectScheduleWidget.h"

ObjectScheduleWidget::ObjectScheduleWidget(ObjectScheduleView *objectSchedule, QWidget *parent)
: QDockWidget("Object Schedule", parent, Qt::Widget), m_objectSchedule(objectSchedule) {
    std::cout << "ObjectScheduleWidget is created" << std::endl;

    QWidget *mainWidget  = new QWidget(this);

    m_addItemButton      = new QPushButton(QIcon("images/Add.png"), "", this);
    m_removeItemButton   = new QPushButton(QIcon("images/Remove.png"), "", this);

    QGridLayout *gLayout = new QGridLayout(mainWidget);
    gLayout->addWidget(m_objectSchedule,  0, 0, 1, 2); // addWidget(widget, fromRow, fromColumn, rowSpan, columnSpan, alignment = 0)
    gLayout->addWidget(m_addItemButton,   2, 0);
    gLayout->addWidget(m_removeItemButton,2, 1);

    mainWidget->setLayout(gLayout);
    setWidget(mainWidget);

    QAction *addSplineAction = new QAction("Create own gear tooth", this);
    QAction *addGearAction = new QAction("Add involute gear", this);

    m_addMenu = new QMenu(this);
    m_addMenu->addAction(addSplineAction);
    m_addMenu->addAction(addGearAction);

    m_addItemButton->setMenu(m_addMenu);

    connect(addSplineAction, SIGNAL(triggered()),   this, SLOT(addSpline()));
    connect(addGearAction,   SIGNAL(triggered()),   this, SLOT(addGear()));
    connect(m_removeItemButton, SIGNAL(clicked()),  m_objectSchedule, SLOT(removeItems()));
}

ObjectScheduleWidget::~ObjectScheduleWidget() {
    std::cout << "ObjectScheduleWidget is deleted" << std::endl;
}

void ObjectScheduleWidget::addSpline() {
    m_objectSchedule->addNewSplineItem();
}

void ObjectScheduleWidget::addGear() {

}
