#ifndef OBJECT_SCHEDULE_VIEW
#define OBJECT_SCHEDULE_VIEW

#include "stable.h"

#include "SceneTreeModel.h"

class ObjectScheduleView : public QTreeView {

Q_OBJECT

public:
    ObjectScheduleView(SceneTreeModel *sceneModel, QWidget *parent = 0);
    ~ObjectScheduleView();
public slots:
    void addItem();
    void removeItems();

private:
    SceneTreeModel *m_sceneModel;
    int m_numberOfHiddenColumns;
};


#endif // OBJECT_SCHEDULE_VIEW
