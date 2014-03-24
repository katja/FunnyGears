#ifndef CONNECTION_MODEL
#define CONNECTION_MODEL

#include "stable.h"
class GraphicsScene;
class SceneTreeModel;

class ConnectionModel : public QItemSelectionModel {

Q_OBJECT

public:
    ConnectionModel(SceneTreeModel *model);
    ~ConnectionModel();

    void turnOnEditingOf(QGraphicsItem *item);
    void informModelDataChange(const QModelIndex &topLeft, const QModelIndex &bottomRight);

signals:
    void updateRegionInScene(const QList<QGraphicsItem*> &changedItems);
    void startEditingItem(QGraphicsItem *item);
    void oneGraphicsItemSelected(QGraphicsItem *item);
    void manyOrNoneGraphicsItemSelected();

public slots:
    void sceneSelectionChanged(GraphicsScene *scene);
    void modelSelectionChanged(const QModelIndex &index, QItemSelectionModel::SelectionFlags command);
    void select(const QModelIndex &index, QItemSelectionModel::SelectionFlags command);
    void select(const QItemSelection &selection, QItemSelectionModel::SelectionFlags command);

private:
    enum Connection {
        MODEL, SCENE, ALL
    };

    SceneTreeModel *m_sceneTreeModel;

    QList<QGraphicsItem*> m_selectedItems;
    QList<QGraphicsItem*> m_changedItems;

    void clearPreviousSelectionIn(ConnectionModel::Connection destination);
    void addToModelSelection(GraphicsScene *scene);
    void addToSceneSelection(QGraphicsItem *graphicsItem);
    void removeFromSceneSelection(QGraphicsItem *graphicsItem);
    void selectItemInModel(QGraphicsItem *item);

    QGraphicsItem* graphicOfRow(const QModelIndex &index);
    bool clearingDemandedIn(QItemSelectionModel::SelectionFlags command);
    bool geometryInSelection(const QModelIndex &index, QItemSelectionModel::SelectionFlags command);


};

#endif // CONNECTION_MODEL
