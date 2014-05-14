#ifndef OBJECT_SCHEDULE_VIEW
#define OBJECT_SCHEDULE_VIEW

#include "stable.h"

#include "SceneTreeModel.h"
#include "ConnectionModel.h"

class ObjectScheduleView : public QTreeView {

Q_OBJECT

public:
    ObjectScheduleView(SceneTreeModel *sceneModel, QWidget *parent = 0);
    ~ObjectScheduleView();

    ConnectionModel* setConnectionModel(ConnectionModel *model);

public slots:
    void addNewSplineItem();
    void removeItems();

private:
    SceneTreeModel *m_sceneModel;
    int m_numberOfHiddenColumns;
    ConnectionModel* m_connectionModel;
};


#endif // OBJECT_SCHEDULE_VIEW
