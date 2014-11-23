#include "tool_bars/EditingToolBar.h"

EditingToolBar::EditingToolBar(QWidget *parent) : QToolBar(tr("Edit"), parent) {
    setIconSize(QSize(16,16));

    createEditingStateActions();
    createEditingActionActions();
}

EditingToolBar::~EditingToolBar() {
}

bool EditingToolBar::addListener(ToolBarListener *listener) {
    //EditingToolBar accepts only EditingToolBarListeners.
    if(listensToMe(listener)) {
        EditingToolBarListener *editingListener = static_cast<EditingToolBarListener*>(listener);
        if(m_listenerList.contains(editingListener))
            return false;
        m_listenerList << editingListener;
        editingListener->startEditing(currentSelection()); //inform new listener of current editing state
        return true;
    }
    return false;
}

bool EditingToolBar::removeListener(ToolBarListener *listener) {
    if(listensToMe(listener)) {
        EditingToolBarListener *editingListener = static_cast<EditingToolBarListener*>(listener);
        if(!m_listenerList.contains(editingListener))
            return false;
        m_listenerList.removeOne(editingListener);
        return true;
    }
    return false;
}

bool EditingToolBar::hasListener(ToolBarListener *listener) {
    for(ToolBarListener *l : m_listenerList) {
        if(l == listener)
            return true;
    }
    return false;
}

void EditingToolBar::keyPressEvent(QKeyEvent *event) {
    if(event->key() == Qt::Key_Escape && m_editingStatesActionGroup->checkedAction()) {
        m_editingStatesActionGroup->checkedAction()->setChecked(false);
        event->accept();
    } else {
        QAction *action = editingStateActionWithKeyEvent(event);
        if(action) {
            action->setChecked(true);
            event->accept();
        } else {
            QToolBar::keyPressEvent(event);
        }
    }
}

Editing::State EditingToolBar::currentSelection() const {
    QAction *action = m_editingStatesActionGroup->checkedAction();
    if(action)
        return m_editingStatesHash.key(action, Editing::NoEditing); //If action is not found, Editing::NoEditing is returned
    return Editing::NoEditing; //if no action checked
}

void EditingToolBar::toolChanged(bool checked) {
    QAction *action = m_editingStatesActionGroup->checkedAction();
    if(checked && action) {
        stopEditing();
        startEditing(m_editingStatesHash.key(action, Editing::NoEditing));
    } else if(!checked && !action) {
        stopEditing();
    }
}

void EditingToolBar::createEditingStateActions() {

    for(int i = 1; i < Editing::STATE_SIZE; ++i) {
        QAction *action = new QAction(this);
        action->setCheckable(true);
        connect(action, SIGNAL(toggled(bool)), this, SLOT(toolChanged(bool)));
        m_editingStatesHash.insert(static_cast<Editing::State>(i), action);
    }

    equipAction(m_editingStatesHash.value(Editing::Pointer), tr("Pointer"), tr("select and move"), QKeySequence(Qt::Key_V));
    equipAction(m_editingStatesHash.value(Editing::Pen), tr("Pen"), tr("draw something new"), QKeySequence(Qt::Key_P));
    equipAction(m_editingStatesHash.value(Editing::Eraser), tr("Eraser"), tr("erase from object"), QKeySequence(Qt::Key_E));
    equipAction(m_editingStatesHash.value(Editing::ZoomIn), tr("ZoomIn"), tr("zoom in at clicked position"));
    equipAction(m_editingStatesHash.value(Editing::ZoomOut), tr("ZoomOut"), tr("zoom out at clicked position"));

    m_editingStatesActionGroup = new QActionGroup(this);
    m_editingStatesActionGroup->setExclusive(true);

    for(int i = 1; i < Editing::STATE_SIZE; ++i) {
        m_editingStatesActionGroup->addAction(m_editingStatesHash.value(static_cast<Editing::State>(i)));
    }

    addActions(m_editingStatesActionGroup->actions());
}

void EditingToolBar::createEditingActionActions() {
    QAction *action = new QAction(this);
    action->setCheckable(false);
    equipAction(action, tr("ZoomDefault"), tr("zoom to default"));
    connect(action, SIGNAL(triggered()), this, SLOT(executeZoomDefault()));
    addAction(action);
}

void EditingToolBar::equipAction(QAction *action, QString name, QString toolTip, QKeySequence shortcut) {
    action->setText(name);
    action->setToolTip(toolTip);

    QString iconFileName = QString("images/" + name + ".png");
    QFile iconFile(iconFileName);
    if(iconFile.exists())
        action->setIcon(QIcon(iconFileName));

    if(shortcut != 0)
        action->setShortcut(shortcut);
};

QAction* EditingToolBar::editingStateActionWithKeyEvent(QKeyEvent *event) const {
    int key = event->key();
    if(event->modifiers() != Qt::NoModifier || key == 0 || key == Qt::Key_unknown)
        return 0;

    QKeySequence keySequence(key);
    foreach(QAction *action, m_editingStatesActionGroup->actions()) {
        if(action->shortcut() == keySequence)
            return action;
    }
    return 0; //no action with shortcut of key event found
}

void EditingToolBar::startEditing(Editing::State editingState) {
    foreach(EditingToolBarListener *listener, m_listenerList) {
        listener->startEditing(editingState);
    }
}

void EditingToolBar::stopEditing() {
    foreach(EditingToolBarListener *listener, m_listenerList) {
        listener->stopEditing();
    }
}

void EditingToolBar::executeZoomDefault() {
    foreach(EditingToolBarListener *listener, m_listenerList) {
        listener->executeEditingAction(Editing::ZoomDefault);
    }
}

bool EditingToolBar::listensToMe(ToolBarListener *listener) const {
    return listener->toolBarType() == EditingToolBarListener::toolBarClassType();
}
