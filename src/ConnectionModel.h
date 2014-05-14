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

signals:
    //signals a connected GraphicsScene needs to handle:
    void selectOnly(QGraphicsItem*);
    void selectAlso(QGraphicsItem*);
    void selectNothing();
    void selectNoMore(QGraphicsItem*);

    void oneGraphicsItemSelected(QGraphicsItem *item);
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

    QList<QGraphicsItem*> m_selectedItems;
    QList<QGraphicsItem*> m_selectedRubberBandItems;
    QList<QGraphicsItem*> m_changedItems;

    void changeSelectionInModel(QGraphicsItem *item, QItemSelectionModel::SelectionFlags command = QItemSelectionModel::Select);
    void reportSelectionCount();
    QGraphicsItem* graphicOfRow(const QModelIndex &index);
    bool selectionIncludesGraphicsItem(const QModelIndex &index, QItemSelectionModel::SelectionFlags command);


};

#endif // CONNECTION_MODEL
