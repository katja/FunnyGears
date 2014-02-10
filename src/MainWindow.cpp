#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent, Qt::WindowFlags flags) : QMainWindow(parent, flags) {

    setGeometry(50, 20, 1000, 800);
    setWindowTitle("Funny Gears");
    setWindowIcon(QIcon("Gear.icns"));
    setUnifiedTitleAndToolBarOnMac(true);

// DOCK WIDGET OBJECT SCHEDULE
    QDockWidget *dockWidgetObjectSchedule = new QDockWidget(tr("Object Schedule"), this, Qt::Widget);

    QPushButton *addGeometryButton = new QPushButton("+");
    QPushButton *removeGeometryButton = new QPushButton("-");

    QGridLayout *gLayout = new QGridLayout(dockWidgetObjectSchedule);
    gLayout->addWidget(addGeometryButton, 0, 0);
    gLayout->addWidget(removeGeometryButton, 0, 1);

    QWidget *objectScheduleWidget = new QWidget(dockWidgetObjectSchedule);
    objectScheduleWidget->setLayout(gLayout);
    dockWidgetObjectSchedule->setWidget(objectScheduleWidget);

    addDockWidget(Qt::RightDockWidgetArea, dockWidgetObjectSchedule);
    dockWidgetObjectSchedule->show();

// DOCK WIDGET OBJECT ATTRIBUTES
    QDockWidget *dockWidgetObjectAttributes = new QDockWidget(tr("Object Attributes"), this, Qt::Widget);

    QWidget *objectAttributesWidget = new QWidget(dockWidgetObjectAttributes);
    QLabel *label = new QLabel(tr("Object Attributes should go here!"), objectAttributesWidget);
    QVBoxLayout *vLayout = new QVBoxLayout(dockWidgetObjectAttributes);
    vLayout->addWidget(label);
    objectAttributesWidget->setLayout(vLayout);
    dockWidgetObjectAttributes->setWidget(objectAttributesWidget);
    addDockWidget(Qt::RightDockWidgetArea, dockWidgetObjectAttributes);
    dockWidgetObjectAttributes->show();


    statusBar()->showMessage(tr("Ready"));

}

MainWindow::~MainWindow() {

}
