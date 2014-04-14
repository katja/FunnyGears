#include "EditingToolBar.h"

EditingToolBar::EditingToolBar(QWidget *parent) : QToolBar(tr("Edit"), parent) {
    setIconSize(QSize(16,16));

    createEditingStateActions();
    createEditingActionActions();
}

EditingToolBar::~EditingToolBar() {
}

bool EditingToolBar::addListener(ToolBarListener *listener) {
    if(m_listenerList.contains(listener))
        return false;
    m_listenerList << listener;
    return true;
}

bool EditingToolBar::removeListener(ToolBarListener *listener) {
    if(!m_listenerList.contains(listener))
        return false;
    m_listenerList.removeOne(listener);
    return true;
}

void EditingToolBar::toolChanged(bool checked) {
    QAction *action = m_editActionGroup->checkedAction();
    if(checked && action) {
        stopEditing();
        startEditing(m_editingStatesHash.key(action, Editing::NoEditing));
    } else if(!checked && !action) {
        stopEditing();
    }
}

void EditingToolBar::createEditingStateActions() {

    for(int i = 0; i < Editing::STATE_SIZE; ++i) {
        QAction *action = new QAction(this);
        action->setCheckable(true);
        connect(action, SIGNAL(toggled(bool)), this, SLOT(toolChanged(bool)));
        m_editingStatesHash.insert(static_cast<Editing::State>(i), action);
    }

    equipAction(m_editingStatesHash.value(Editing::NoEditing), tr("Pointer"), tr("select and move"));
    equipAction(m_editingStatesHash.value(Editing::Pen), tr("Pen"), tr("draw something new"));
    equipAction(m_editingStatesHash.value(Editing::Eraser), tr("Eraser"), tr("erase from object"));
    equipAction(m_editingStatesHash.value(Editing::ZoomIn), tr("ZoomIn"), tr("zoom in at clicked position"));
    equipAction(m_editingStatesHash.value(Editing::ZoomOut), tr("ZoomOut"), tr("zoom out at clicked position"));

    m_editActionGroup = new QActionGroup(this);
    m_editActionGroup->setExclusive(true);

    for(int i = 0; i < Editing::STATE_SIZE; ++i) {
        m_editActionGroup->addAction(m_editingStatesHash.value(static_cast<Editing::State>(i)));
    }

    addActions(m_editActionGroup->actions());
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

void EditingToolBar::startEditing(Editing::State editingState) {
    foreach(ToolBarListener *listener, m_listenerList) {
        listener->startEditing(editingState);
    }
}

void EditingToolBar::stopEditing() {
    foreach(ToolBarListener *listener, m_listenerList) {
        listener->stopEditing();
    }
}

void EditingToolBar::executeZoomDefault() {
    foreach(ToolBarListener *listener, m_listenerList) {
        listener->executeEditingAction(Editing::ZoomDefault);
    }
}
