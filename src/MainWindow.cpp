#include "MainWindow.h"
#include "ObjectSchedule.h"

MainWindow::MainWindow(QWidget *parent, Qt::WindowFlags flags) : QMainWindow(parent, flags) {

    setGeometry(50, 20, 1000, 800);
    setWindowTitle("Funny Gears");
    setWindowIcon(QIcon("Gear.icns"));
    setUnifiedTitleAndToolBarOnMac(true);

// DOCK WIDGET OBJECT SCHEDULE
    QDockWidget *dockWidgetObjectSchedule   = new QDockWidget(tr("Object Schedule"), this, Qt::Widget);
    QWidget     *objectScheduleWidget       = new QWidget(dockWidgetObjectSchedule);

    ObjectSchedule  *objectSchedule         = new ObjectSchedule(this);
    QPushButton     *addItemButton          = new QPushButton("+");
    QPushButton     *removeItemButton       = new QPushButton("-");

    connect(addItemButton, SIGNAL(clicked()), objectSchedule, SLOT(addItem()));
    connect(removeItemButton, SIGNAL(clicked()), objectSchedule, SLOT(removeItems()));

    QGridLayout *gLayout = new QGridLayout(objectScheduleWidget);
    gLayout->addWidget(objectSchedule,      0, 0, 1, 2); // addWidget(widget, fromRow, fromColumn, rowSpan, columnSpan, alignment = 0)
    gLayout->addWidget(addItemButton,       2, 0);
    gLayout->addWidget(removeItemButton,    2, 1);

    objectScheduleWidget->setLayout(gLayout);
    dockWidgetObjectSchedule->setWidget(objectScheduleWidget);

    addDockWidget(Qt::RightDockWidgetArea, dockWidgetObjectSchedule);
    dockWidgetObjectSchedule->show();

// DOCK WIDGET OBJECT ATTRIBUTES
    QDockWidget *dockWidgetObjectAttributes = new QDockWidget(tr("Object Attributes"), this, Qt::Widget);
    QWidget     *objectAttributesWidget     = new QWidget(dockWidgetObjectAttributes);

    QLabel *label = new QLabel(tr("Object Attributes should go here!"), objectAttributesWidget);
    QVBoxLayout *vLayout = new QVBoxLayout(objectAttributesWidget);
    vLayout->addWidget(label);

    objectAttributesWidget->setLayout(vLayout);
    dockWidgetObjectAttributes->setWidget(objectAttributesWidget);

    addDockWidget(Qt::RightDockWidgetArea, dockWidgetObjectAttributes);
    dockWidgetObjectAttributes->show();

// GRAPHICS

    scene = new GraphicsScene();
    view = new GraphicsView(scene);
    setCentralWidget(view);

    statusBar()->showMessage(tr("Ready"));

}

MainWindow::~MainWindow() {

}
