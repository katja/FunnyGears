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
    void addItem();
    void addNewSplineItem();
    void removeItems();

protected slots:
    void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int>());

private:
    SceneTreeModel *m_sceneModel;
    int m_numberOfHiddenColumns;
    ConnectionModel* m_connectionModel;
};


#endif // OBJECT_SCHEDULE_VIEW
