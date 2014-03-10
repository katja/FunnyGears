#ifndef CONNECTION_SELECTION_MODEL
#define CONNECTION_SELECTION_MODEL

#include "stable.h"
class GraphicsScene;
class SceneTreeModel;

class ConnectionSelectionModel : public QItemSelectionModel {

public:
    ConnectionSelectionModel(SceneTreeModel *model);
    ~ConnectionSelectionModel();

public slots:
    void sceneSelectionChanged(GraphicsScene *scene);

    // selectGeometries(QList<QGraphicsItem*>);
private:
    SceneTreeModel *m_sceneTreeModel;

};

#endif // CONNECTION_SELECTION_MODEL
