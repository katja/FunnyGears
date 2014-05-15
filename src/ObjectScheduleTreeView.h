#ifndef OBJECT_SCHEDULE_TREE_VIEW
#define OBJECT_SCHEDULE_TREE_VIEW

#include "stable.h"

#include "SceneTreeModel.h"
#include "ConnectionModel.h"

class ObjectScheduleTreeView : public QTreeView {

Q_OBJECT

public:
    ObjectScheduleTreeView(SceneTreeModel *sceneModel, QWidget *parent = 0);
    ~ObjectScheduleTreeView();

    ConnectionModel* setConnectionModel(ConnectionModel *model);

public slots:
    void addNewSplineItem();
    void removeItems();

private:
    SceneTreeModel *m_sceneModel;
    int m_numberOfHiddenColumns;
    ConnectionModel* m_connectionModel;
};


#endif // OBJECT_SCHEDULE_TREE_VIEW
