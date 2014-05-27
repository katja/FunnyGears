#ifndef EDITING_TOOL_BAR
#define EDITING_TOOL_BAR

#include "stable.h"
#include "tool_bars/ToolBarListener.h"
#include "tool_bars/EditingStatesAndActions.h"

class EditingToolBar : public QToolBar {

Q_OBJECT

public:
    EditingToolBar(QWidget *parent = 0);
    ~EditingToolBar();

    bool addListener(ToolBarListener *listener);
    bool removeListener(ToolBarListener *listener);

    void keyPressEvent(QKeyEvent *event);

private slots:
    void toolChanged(bool);
    void executeZoomDefault();

private:
    QHash<Editing::State, QAction*>     m_editingStatesHash;
    QHash<Editing::Action, QAction*>    m_editingActionsHash;
    QActionGroup                       *m_editActionGroup;
    QList<ToolBarListener*>             m_listenerList;

    void createEditingStateActions();
    void createEditingActionActions();
    void equipAction(QAction *action, QString name, QString toolTip, QKeySequence shortcut = 0);
    QAction* editingStateActionWithKeyEvent(QKeyEvent *event) const;

    void stopEditing();
    void startEditing(Editing::State);

};

#endif // EDITING_TOOL_BAR
