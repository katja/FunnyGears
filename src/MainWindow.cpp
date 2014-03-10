#include "MainWindow.h"
#include "ObjectSchedule.h"

MainWindow::MainWindow(QWidget *parent, Qt::WindowFlags flags) : QMainWindow(parent, flags), projectChanged(true) {

    std::cout << "MainWindow is created" << std::endl;

    setGeometry(50, 20, 1000, 800);
    setWindowTitle("Funny Gears");
    setWindowIcon(QIcon("Gear.icns"));
    setUnifiedTitleAndToolBarOnMac(true);

    createActions(); // actions have to be created before toolbars and menus!
    createToolbars();
    createDialogs();
    createLayout();
    createMenus(); //Must be executed after createToolbars()

    statusBar()->showMessage(tr("Ready"));

}

MainWindow::~MainWindow() {
    std::cout << "MainWindow is deleted" << std::endl;
}

QIcon MainWindow::icon() const {
    return windowIcon();
};

void MainWindow::createActions() {
    connect(createAction(m_exitAction,  tr("E&xit"),  tr("quit program"), QKeySequence::Quit),
        SIGNAL(triggered()), this, SLOT(quitProgram()));
    connect(createAction(m_callAction,  tr("&say hello"),  tr("say hello")),
        SIGNAL(triggered()),  this, SLOT(sayHello()));
    connect(createAction(m_aboutAction, tr("&About"), tr("about the program")),
        SIGNAL(triggered()), this, SLOT(about()));
    connect(createAction(m_newAction,   tr("&New"),   tr("create new project"), QKeySequence::New),
        SIGNAL(triggered()), this, SLOT(newProject()));
    connect(createAction(m_saveAction,  tr("&Save"),  tr("save current objects"), QKeySequence::Save),
        SIGNAL(triggered()), this, SLOT(saveProject()));
    connect(createAction(m_saveAsAction,tr("Save As"),  tr("save current objects in new file"), QKeySequence::SaveAs),
        SIGNAL(triggered()), this, SLOT(saveAsProject()));
    connect(createAction(m_openAction,  tr("&Open"),  tr("open saved objects"), QKeySequence::Open),
        SIGNAL(triggered()), this, SLOT(openProject()));
    connect(createAction(m_toggleColorDialogAction, tr("Color Dialog"), tr("Show or open color dialog")),
        SIGNAL(triggered()), this, SLOT(openColorDialog()));
    m_toggleColorDialogAction->setCheckable(true);
    m_toggleColorDialogAction->setChecked(false);
};

QAction* MainWindow::createAction(QAction *&action, QString name, QString statusTip, QKeySequence shortcut) {

    QString iconFileName = QString("images/" + name + ".png");
    QFile iconFile(iconFileName);
    if (iconFile.exists()) {
        action = new QAction(QIcon(iconFileName), name, this);
    } else {
        action = new QAction(name, this);
    }
    action->setStatusTip(statusTip);
    if(shortcut != NULL)
        action->setShortcut(shortcut);
    return action;
};

void MainWindow::createMenus() {
    m_programMenu = menuBar()->addMenu(tr("Funny Gears"));
    m_programMenu->addAction(m_aboutAction);
    m_programMenu->addSeparator();
    m_programMenu->addAction(m_exitAction);

    m_fileMenu = menuBar()->addMenu(tr("&File"));
    m_fileMenu->addAction(m_newAction);
    m_fileMenu->addAction(m_openAction);
    m_fileMenu->addAction(m_saveAction);
    m_fileMenu->addAction(m_saveAsAction);

    m_editMenu = menuBar()->addMenu(tr("&Edit"));
    m_editMenu->addAction(m_callAction);
    m_editMenu->addAction(m_aboutAction);

    m_viewMenu = menuBar()->addMenu(tr("&View"));
    m_viewMenu->addAction(m_toggleFileToolBarAction);
    m_viewMenu->addAction(m_toggleEditToolBarAction);
    m_viewMenu->addSeparator();
    m_viewMenu->addAction(m_toggleColorDialogAction);
    m_viewMenu->addSeparator();
    m_viewMenu->addAction(m_toggleObjectScheduleAction);
    m_viewMenu->addAction(m_toggleObjectAttributesAction);

    m_helpMenu = menuBar()->addMenu(tr("&Help"));
};

void MainWindow::createToolbars() {
    m_fileToolBar = this->addToolBar(tr("File"));
    m_fileToolBar->setIconSize(QSize(16, 16));
    m_fileToolBar->addAction(m_newAction);
    m_fileToolBar->addAction(m_openAction);
    m_fileToolBar->addAction(m_saveAction);
    m_fileToolBar->addAction(m_saveAsAction);

    m_editToolBar = this->addToolBar(tr("Edit"));
    m_editToolBar->addAction(m_callAction);
    m_editToolBar->addAction(m_aboutAction);

    m_toggleFileToolBarAction = m_fileToolBar->toggleViewAction();
    m_toggleEditToolBarAction = m_editToolBar->toggleViewAction();
};

void MainWindow::createDialogs() {
    m_colorDialog = new QColorDialog(this);
    m_colorDialog->setOption(QColorDialog::ShowAlphaChannel);
}

void MainWindow::createLayout() {
// GRAPHICS
    m_scene = new GraphicsScene(this);
    m_view = new GraphicsView(m_scene);
    setCentralWidget(m_view);

    m_sceneModel = new SceneTreeModel(m_scene, this);

// DOCK WIDGET OBJECT SCHEDULE
    m_objectScheduleDockWidget          = new QDockWidget(tr("Object Schedule"), this, Qt::Widget);
    QWidget *objectScheduleWidget       = new QWidget(m_objectScheduleDockWidget);

    ObjectSchedule  *objectSchedule     = new ObjectSchedule(m_sceneModel, this);
    QPushButton     *addItemButton      = new QPushButton("+");
    QPushButton     *removeItemButton   = new QPushButton("-");




    ConnectionSelectionModel *selectionModel = new ConnectionSelectionModel(m_sceneModel);
    m_scene->setSelectionModel(selectionModel);
    objectSchedule->setSelectionModel(selectionModel);




    connect(addItemButton, SIGNAL(clicked()), objectSchedule, SLOT(addItem()));
    connect(removeItemButton, SIGNAL(clicked()), objectSchedule, SLOT(removeItems()));

    QGridLayout *gLayout = new QGridLayout(objectScheduleWidget);
    gLayout->addWidget(objectSchedule,  0, 0, 1, 2); // addWidget(widget, fromRow, fromColumn, rowSpan, columnSpan, alignment = 0)
    gLayout->addWidget(addItemButton,   2, 0);
    gLayout->addWidget(removeItemButton,2, 1);

    objectScheduleWidget->setLayout(gLayout);
    m_objectScheduleDockWidget->setWidget(objectScheduleWidget);

    addDockWidget(Qt::RightDockWidgetArea, m_objectScheduleDockWidget);
    m_objectScheduleDockWidget->show();

    m_toggleObjectScheduleAction = m_objectScheduleDockWidget->toggleViewAction();

// DOCK WIDGET OBJECT ATTRIBUTES
    m_objectAttributesDockWidget        = new QDockWidget(tr("Object Attributes"), this, Qt::Widget);
    QWidget *objectAttributesWidget     = new QWidget(m_objectAttributesDockWidget);

    QLabel *label = new QLabel(tr("Object Attributes should go here!"), objectAttributesWidget);
    QVBoxLayout *vLayout = new QVBoxLayout(objectAttributesWidget);
    vLayout->addWidget(label);

    objectAttributesWidget->setLayout(vLayout);
    m_objectAttributesDockWidget->setWidget(objectAttributesWidget);

    addDockWidget(Qt::RightDockWidgetArea, m_objectAttributesDockWidget);
    m_objectAttributesDockWidget->show();

    m_toggleObjectAttributesAction = m_objectAttributesDockWidget->toggleViewAction();

};

void MainWindow::quitProgram() {
    close();
};

void MainWindow::about() {
     QMessageBox::about(this, tr("About Funny Gears"),
            tr("<p><b>Funny Gears</b> wants to demonstrate the challenge of gear construction. </p>"
               "<p><small>Copyright: Katja Renner 2013</small></p>"));
};

void MainWindow::newProject() {
    //TODO: Test if state already saved...
    //TODO: Open new file
};

bool MainWindow::saveProject() {
    return true;
};

bool MainWindow::saveAsProject() {
    return true;
};

void MainWindow::openProject() {
    if((projectChanged && savedProjectOrOk()) || !projectChanged) {
        QString fileName = QFileDialog::getOpenFileName(this, tr("Choose a funny gear project"), QString(), tr(".fg"));
        if(!fileName.isEmpty());
            // loadFile(fileName);
    }
};

void MainWindow::openColorDialog() {
    if(m_colorDialog->isVisible())
        m_colorDialog->close();
    else
        m_colorDialog->open();
    m_toggleColorDialogAction->setChecked(m_colorDialog->isVisible());
}

bool MainWindow::savedProjectOrOk() {
    QMessageBox::StandardButton userRequest;
    userRequest = QMessageBox::warning(this, tr("Save Changes?"),
        tr("<p>You have not saved your project in the current state. Do you want to save it before closing?</p>"
        "<p><small>Your changes will be lost otherwise!</small></p>"),
        (QMessageBox::Cancel | QMessageBox::Discard | QMessageBox::Save),
        QMessageBox::Save);
    if(userRequest == QMessageBox::Save)
        return saveProject();
    else if(userRequest == QMessageBox::Cancel)
        return false;
    else
        return true;
};

void MainWindow::sayHello() {
    std::cout << "MainWindow::sayHello" << std::endl;
};
