#ifndef OBJECT_SCHEDULE
#define OBJECT_SCHEDULE

#include "stable.h"

#include "SceneTreeModel.h"

class ObjectSchedule : public QTreeView {

Q_OBJECT

public:
    ObjectSchedule(QWidget *parent = 0);
    ~ObjectSchedule();
public slots:
    void addGeometry();
    void removeGeometry();
private:
    SceneTreeModel *sceneModel;
};


#endif // OBJECT_SCHEDULE
