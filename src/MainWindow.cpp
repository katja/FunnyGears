#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent, Qt::WindowFlags flags) : QMainWindow(parent, flags), projectChanged(true) {
    std::cout << "MainWindow is created" << std::endl;

    setGeometry(50, 20, 1000, 800);
    setWindowTitle("Funny Gears");
    setWindowIcon(QIcon("Gear.icns"));
    setUnifiedTitleAndToolBarOnMac(true);

    createMainProgramPart();
    createMenusAndToolbars();

    statusBar()->showMessage(tr("Ready"));
}

MainWindow::~MainWindow() {
    std::cout << "MainWindow is deleted" << std::endl;
}

QIcon MainWindow::icon() const {
    return windowIcon();
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if(event->key() == Qt::Key_Escape ||
        event->key() == Qt::Key_V ||
        event->key() == Qt::Key_P ||
        event->key() == Qt::Key_E) {
        m_editToolBar->keyPressEvent(event);
        if(event->isAccepted())
            return;
    }
    QMainWindow::keyPressEvent(event);
}

void MainWindow::createMainProgramPart() {
// GRAPHICSSCENE THINGS
    m_scene = new GraphicsScene(this);
    m_view = new GraphicsView(m_scene);
    setCentralWidget(m_view);

// MODEL/VIEW THINGS
    m_sceneModel = new SceneTreeModel(m_scene, this);
    m_objectScheduleView = new ObjectScheduleTreeView(m_sceneModel, this);

// CONNECTION
    ConnectionModel *connectionModel = new ConnectionModel(m_sceneModel);
    m_scene->setConnectionModel(connectionModel);
    m_objectScheduleView->setConnectionModel(connectionModel);
//WIDGETS
    m_objectScheduleWidget  = new ObjectScheduleWidget(m_objectScheduleView, this);
    addDockWidget(Qt::RightDockWidgetArea, m_objectScheduleWidget);

    m_objectAttributesWidget = new ObjectAttributesWidget(connectionModel, this);
    addDockWidget(Qt::RightDockWidgetArea, m_objectAttributesWidget);
}

void MainWindow::createMenusAndToolbars() {
    createProgramMenu();
    createFileMenuAndToolbar();
    createEditToolbar();
    createViewMenu();
}

void MainWindow::createProgramMenu() {
    connect(createAction(m_aboutAction, tr("&About"), tr("about the program")),
        SIGNAL(triggered()), this, SLOT(about()));
    connect(createAction(m_exitAction,  tr("E&xit"),  tr("quit program"), QKeySequence::Quit),
        SIGNAL(triggered()), this, SLOT(quitProgram()));

    m_programMenu = menuBar()->addMenu(tr("Funny Gears"));
    m_programMenu->addAction(m_aboutAction);
    m_programMenu->addSeparator();
    m_programMenu->addAction(m_exitAction);
}

void MainWindow::createFileMenuAndToolbar() {
    connect(createAction(m_newAction,   tr("&New"),   tr("create new project"), QKeySequence::New),
        SIGNAL(triggered()), this, SLOT(newProject()));
    connect(createAction(m_openAction,  tr("&Open"),  tr("open saved objects"), QKeySequence::Open),
        SIGNAL(triggered()), this, SLOT(openProject()));
    connect(createAction(m_saveAction,  tr("&Save"),  tr("save current objects"), QKeySequence::Save),
        SIGNAL(triggered()), this, SLOT(saveProject()));
    connect(createAction(m_saveAsAction,tr("Save As"),  tr("save current objects in new file"), QKeySequence::SaveAs),
        SIGNAL(triggered()), this, SLOT(saveAsProject()));

    m_fileMenu = menuBar()->addMenu(tr("&File"));
    m_fileMenu->addAction(m_newAction);
    m_fileMenu->addAction(m_openAction);
    m_fileMenu->addAction(m_saveAction);
    m_fileMenu->addAction(m_saveAsAction);

    m_fileToolBar = this->addToolBar(tr("File"));
    m_fileToolBar->setIconSize(QSize(16, 16));
    m_fileToolBar->addAction(m_newAction);
    m_fileToolBar->addAction(m_openAction);
    m_fileToolBar->addAction(m_saveAction);
    m_fileToolBar->addAction(m_saveAsAction);
}

void MainWindow::createEditToolbar() {
    m_editToolBar = new EditingToolBar(this);
    m_editToolBar->addListener(m_scene);
    m_editToolBar->addListener(m_view);
    addToolBar(m_editToolBar);

}

void MainWindow::createViewMenu() {
    m_toggleFileToolBarAction = m_fileToolBar->toggleViewAction();
    m_toggleEditToolBarAction = m_editToolBar->toggleViewAction();

    //TODO: remove color dialog things if not necessary!
    m_colorDialog = new QColorDialog(this);
    m_colorDialog->setOption(QColorDialog::ShowAlphaChannel);
    connect(createAction(m_toggleColorDialogAction, tr("Color Dialog"), tr("Show or open color dialog")),
        SIGNAL(triggered()), this, SLOT(openColorDialog()));
    m_toggleColorDialogAction->setCheckable(true);
    m_toggleColorDialogAction->setChecked(false);

    m_toggleObjectScheduleAction = m_objectScheduleWidget->toggleViewAction();
    m_toggleObjectAttributesAction = m_objectAttributesWidget->toggleViewAction();

    m_viewMenu = menuBar()->addMenu(tr("&View"));
    m_viewMenu->addAction(m_toggleFileToolBarAction);
    m_viewMenu->addAction(m_toggleEditToolBarAction);
    m_viewMenu->addSeparator();
    m_viewMenu->addAction(m_toggleColorDialogAction);
    m_viewMenu->addSeparator();
    m_viewMenu->addAction(m_toggleObjectScheduleAction);
    m_viewMenu->addAction(m_toggleObjectAttributesAction);
}

QAction* MainWindow::createAction(QAction *&action, QString name, QString toolTip, QKeySequence shortcut) {
    QString iconFileName = QString("images/" + name + ".png");
    QFile iconFile(iconFileName);
    if(iconFile.exists()) {
        action = new QAction(QIcon(iconFileName), name, this);
    } else {
        action = new QAction(name, this);
    }
    action->setToolTip(toolTip);
    if(shortcut != 0)
        action->setShortcut(shortcut);
    return action;
}

void MainWindow::quitProgram() {
    close();
}

void MainWindow::about() {
     QMessageBox::about(this, tr("About Funny Gears"),
            tr("<p><b>Funny Gears</b> wants to demonstrate the challenge of gear construction. </p>"
               "<p><small>Copyright: Katja Renner 2013</small></p>"));
}

void MainWindow::newProject() {
    //TODO: Test if state already saved...
    //TODO: Open new file
}

void MainWindow::openProject() {
    if((projectChanged && savedProjectOrOk()) || !projectChanged) {
        QString fileName = QFileDialog::getOpenFileName(this, tr("Choose a funny gear project"), QString(), tr(".fg"));
        if(!fileName.isEmpty());
            // TODO: loadFile(fileName);
    }
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
}

bool MainWindow::saveProject() {
    return true;
}

bool MainWindow::saveAsProject() {
    return true;
}

void MainWindow::openColorDialog() {
    if(m_colorDialog->isVisible())
        m_colorDialog->close();
    else
        m_colorDialog->open();
    m_toggleColorDialogAction->setChecked(m_colorDialog->isVisible());
}
