#ifndef CONNECTION_MODEL
#define CONNECTION_MODEL

#include "stable.h"
#include "GraphicsItem.h"
class GraphicsScene;
class SceneTreeModel;

class ConnectionModel : public QItemSelectionModel {

Q_OBJECT

public:
    ConnectionModel(SceneTreeModel *model);
    ~ConnectionModel();

signals:
    //signals a connected GraphicsScene needs to handle:
    void selectOnly(GraphicsItem*);
    void selectAlso(GraphicsItem*);
    void selectNothing();
    void selectNoMore(GraphicsItem*);

    void oneGraphicsItemSelected(GraphicsItem *item);
    void noneOrManyGraphicsItemsSelected();

public slots:
    void sceneSelectionChanged(GraphicsScene *scene);

private slots:
    void updateSelection(const QItemSelection &selected, const QItemSelection &deselected);

private:
    enum Connection {
        MODEL, SCENE, ALL
    };

    SceneTreeModel *m_sceneTreeModel;

    QList<GraphicsItem*> m_selectedItems;

    void changeSelectionInModel(GraphicsItem *item, QItemSelectionModel::SelectionFlags command = QItemSelectionModel::Select);
    void reportSelectionCount();
    GraphicsItem* graphicOfRow(const QModelIndex &index);
    bool selectionIncludesGraphicsItem(const QModelIndex &index, QItemSelectionModel::SelectionFlags command);


};

#endif // CONNECTION_MODEL
