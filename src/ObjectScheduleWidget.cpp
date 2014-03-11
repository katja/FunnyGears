#include "ObjectScheduleWidget.h"

ObjectScheduleWidget::ObjectScheduleWidget(ObjectScheduleView *objectSchedule, QWidget *parent)
: QDockWidget("Object Schedule", parent, Qt::Widget), m_objectSchedule(objectSchedule) {
    std::cout << "ObjectScheduleWidget is created" << std::endl;

    QWidget *mainWidget  = new QWidget(this);

    m_addItemButton      = new QPushButton("+");
    m_removeItemButton   = new QPushButton("-");

    connect(m_addItemButton, SIGNAL(clicked()), m_objectSchedule, SLOT(addItem()));
    connect(m_removeItemButton, SIGNAL(clicked()), m_objectSchedule, SLOT(removeItems()));

    QGridLayout *gLayout = new QGridLayout(mainWidget);
    gLayout->addWidget(m_objectSchedule,  0, 0, 1, 2); // addWidget(widget, fromRow, fromColumn, rowSpan, columnSpan, alignment = 0)
    gLayout->addWidget(m_addItemButton,   2, 0);
    gLayout->addWidget(m_removeItemButton,2, 1);

    mainWidget->setLayout(gLayout);
    setWidget(mainWidget);
}

ObjectScheduleWidget::~ObjectScheduleWidget() {
    std::cout << "ObjectScheduleWidget is deleted" << std::endl;
}
