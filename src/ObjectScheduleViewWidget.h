#ifndef OBJECT_SCHEDULE_VIEW_WIDGET
#define OBJECT_SCHEDULE_VIEW_WIDGET

#include "stable.h"
#include "SceneTreeModel.h"
#include "ConnectionModel.h"

class TreeView;
class ObjectScheduleViewWidget : public QDockWidget {

Q_OBJECT

public:
    ObjectScheduleViewWidget(SceneTreeModel *model, QWidget *parent = 0);
    virtual ~ObjectScheduleViewWidget();

    ConnectionModel* setConnectionModel(ConnectionModel *model);

private slots:
    void addSpline();
    void addGearTooth();
    void addInvoluteGear();
    void removeItems();

private:
    TreeView *m_treeView;

    QMenu *m_addMenu;

    QPushButton *m_addItemButton,
                *m_removeItemButton;

    SceneTreeModel *m_sceneTreeModel;
    ConnectionModel* m_connectionModel;

    void setTreeViewLayout();
    void setWidgetLayout();
};

class TreeView : public QTreeView {
public:
    TreeView(QWidget *parent = 0) : QTreeView(parent) {}
    virtual ~TreeView() {}
    QModelIndexList selectedIndexes() const {
        return QTreeView::selectedIndexes();
    }
};
#endif // OBJECT_SCHEDULE_VIEW_WIDGET
