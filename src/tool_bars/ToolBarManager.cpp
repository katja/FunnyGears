#include "ToolBarManager.h"

ToolBarManager* ToolBarManager::getToolBarManager() {
    if(m_uniqueManager == 0)
        m_uniqueManager = new ToolBarManager();
    return m_uniqueManager;
}

ToolBarManager::~ToolBarManager() {
    //delete ToolBars???
}

void ToolBarManager::control(ToolBar *toolBar) {
    if(toolBar && !m_toolBars.contains(toolBar))
        m_toolBars.append(toolBar);
}

QList<ToolBar*> ToolBarManager::managedToolBars(ToolBarListener *listener) {
    if(listener == 0)
        return m_toolBars;

    QList<ToolBar*> listenerToolBars;
    for(ToolBar* toolBar : m_toolBars) {
        if(toolBar->hasListener(listener))
            listenerToolBars.append(toolBar);
    }
    return listenerToolBars;
}

void ToolBarManager::addListenerToToolBars(ToolBarListener *listener, ToolBarListener *listensTo) {
    if(!listener)
        return;

    for(ToolBar* toolBar : m_toolBars) {
        if(!listensTo || toolBar->hasListener(listensTo))
            toolBar->addListener(listener);
    }
}

void ToolBarManager::removeListenerFromToolBars(ToolBarListener *listener) {
    if(!listener)
        return;

    QList<ToolBar*> toolBars = managedToolBars(listener);
    for(ToolBar *toolBar : toolBars) {
        toolBar->removeListener(listener);
    }
}

ToolBarManager::ToolBarManager() {}

ToolBarManager *ToolBarManager::m_uniqueManager = 0;
