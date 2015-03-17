#ifndef MAIN_WINDOW
#define MAIN_WINDOW

#include "stable.h"
#include "main_gui_objects/GraphicsView.h"
#include "main_gui_objects/GraphicsScene.h"
#include "main_gui_objects/Model.h"
#include "main_gui_objects/ObjectScheduleViewWidget.h"
#include "main_gui_objects/ObjectAttributesWidget.h"

class MainWindow : public QMainWindow {

Q_OBJECT

public:
    MainWindow(QWidget *parent = 0, Qt::WindowFlags flags = 0);
    ~MainWindow();

    QIcon icon() const;

protected:
    void keyPressEvent(QKeyEvent *event);

private:
    GraphicsView                *m_view;
    GraphicsScene               *m_scene;

    Model                       *m_model;
    ObjectScheduleViewWidget    *m_objectSchedule; // is a QDockWidget, has a TreeView
    ObjectAttributesWidget      *m_objectAttributesWidget; // is a QDockWidget

    QMenu       *m_programMenu,
                *m_fileMenu,
                *m_viewMenu;
    QToolBar    *m_fileToolBar;
    EditingToolBar *m_editToolBar;
    QColorDialog *m_colorDialog;
    QAction     *m_exitAction,
                *m_aboutAction,
                *m_newAction,
                *m_openAction,
                *m_saveAction,
                *m_saveAsAction,
                *m_toggleFileToolBarAction,
                *m_toggleEditToolBarAction,
                *m_toggleColorDialogAction,
                *m_toggleObjectAttributesAction,
                *m_toggleObjectScheduleAction;

    bool projectChanged;

    void createMainProgramPart();
    void createMenusAndToolbars();

    void createProgramMenu();
    void createFileMenuAndToolbar();
    void createEditToolbar();
    void createViewMenu();

    void createActions();
    QAction* createAction(QAction *&action, QString name, QString toolTip, QKeySequence shortcut = 0);

    bool savedProjectOrOk();

private slots:
    void quitProgram();
    void about();
    void newProject();
    bool saveProject();
    bool saveAsProject();
    void openProject();
    void openColorDialog();
};


#endif // MAIN_WINDOW
