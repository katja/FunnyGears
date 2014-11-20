#ifndef EDITING_TOOL_BAR
#define EDITING_TOOL_BAR

#include "stable.h"
#include "tool_bars/ToolBar.h"
#include "tool_bars/EditingToolBarListener.h"
#include "tool_bars/EditingStatesAndActions.h"

class EditingToolBar : public QToolBar, public ToolBar {

Q_OBJECT

public:
    EditingToolBar(QWidget *parent = 0);
    virtual ~EditingToolBar();

    virtual bool addListener(ToolBarListener *listener) override; //from ToolBar
    virtual bool removeListener(ToolBarListener *listener) override; //from ToolBar
    virtual bool hasListener(ToolBarListener *listener) override; //from ToolBar

    void keyPressEvent(QKeyEvent *event);

    Editing::State currentSelection() const;

private slots:
    void toolChanged(bool);
    void executeZoomDefault();

private:
    QHash<Editing::State, QAction*>     m_editingStatesHash;
    QHash<Editing::Action, QAction*>    m_editingActionsHash;
    QActionGroup                       *m_editingStatesActionGroup;
    QList<EditingToolBarListener*>      m_listenerList;
    ToolBarListener*                    m_tempListener;

    void createEditingStateActions();
    void createEditingActionActions();
    void equipAction(QAction *action, QString name, QString toolTip, QKeySequence shortcut = 0);
    QAction* editingStateActionWithKeyEvent(QKeyEvent *event) const;

    void stopEditing();
    void startEditing(Editing::State);

    bool listensToMe(ToolBarListener *listener) const;

};

#endif // EDITING_TOOL_BAR
