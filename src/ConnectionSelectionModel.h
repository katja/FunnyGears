#ifndef CONNECTION_SELECTION_MODEL
#define CONNECTION_SELECTION_MODEL

#include "stable.h"
class GraphicsScene;
class SceneTreeModel;

class ConnectionSelectionModel : public QItemSelectionModel {

Q_OBJECT

public:
    ConnectionSelectionModel(SceneTreeModel *model);
    ~ConnectionSelectionModel();

signals:
    void updateRegionInScene(const QList<QGraphicsItem*> &deselect);

public slots:
    void sceneSelectionChanged(GraphicsScene *scene);
    void modelSelectionChanged(const QModelIndex &index, QItemSelectionModel::SelectionFlags command);
    void select(const QModelIndex &index, QItemSelectionModel::SelectionFlags command);
    void select(const QItemSelection &selection, QItemSelectionModel::SelectionFlags command);

private:
    enum Connection {
        MODEL, SCENE
    };

    SceneTreeModel *m_sceneTreeModel;

    QList<QGraphicsItem*> m_selectedItems;
    QList<QGraphicsItem*> m_changedItems;

    void clearPreviousSelectionIn(ConnectionSelectionModel::Connection destination);
    void addToModelSelection(GraphicsScene *scene);
    void addToSceneSelection(QGraphicsItem *graphicsItem);
    void removeFromSceneSelection(QGraphicsItem *graphicsItem);
    void selectItemInModel(QGraphicsItem *item);

    QGraphicsItem* graphicOfRow(const QModelIndex &index);
    bool clearingDemandedIn(QItemSelectionModel::SelectionFlags command);
    bool geometryInSelection(const QModelIndex &index, QItemSelectionModel::SelectionFlags command);


};

#endif // CONNECTION_SELECTION_MODEL
