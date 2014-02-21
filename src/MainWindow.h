#ifndef MAIN_WINDOW
#define MAIN_WINDOW

#include "stable.h"
#include "GraphicsView.h"
#include "GraphicsScene.h"

class MainWindow : public QMainWindow {

Q_OBJECT

public:
    MainWindow(QWidget *parent = 0, Qt::WindowFlags flags = 0);
    ~MainWindow();

    QIcon icon() const;

private slots:
    void quitProgram();
    void about();
    void newProject();
    bool saveProject();
    bool saveAsProject();
    void openProject();
    void openColorDialog();
    void sayHello();


private:
    GraphicsView  *m_view;
    GraphicsScene *m_scene;

    QDockWidget *m_objectScheduleDockWidget,
                *m_objectAttributesDockWidget;

    QMenu       *m_programMenu,
                *m_fileMenu,
                *m_editMenu,
                *m_viewMenu,
                *m_helpMenu;
    QToolBar    *m_fileToolBar,
                *m_editToolBar;
    QColorDialog *m_colorDialog;
    QAction     *m_exitAction,
                *m_newAction,
                *m_saveAction,
                *m_saveAsAction,
                *m_openAction,
                *m_aboutAction,
                *m_callAction,
                *m_toggleFileToolBarAction,
                *m_toggleEditToolBarAction,
                *m_toggleColorDialogAction,
                *m_toggleObjectAttributesAction,
                *m_toggleObjectScheduleAction;

    bool projectChanged;

    void createActions();
    QAction* createAction(QAction *&action, QString name, QString statusTip, QKeySequence shortcut = NULL);
    void createMenus();
    void createToolbars();
    void createDialogs();
    void createLayout();
    bool savedProjectOrOk();
};


#endif // MAIN_WINDOW
