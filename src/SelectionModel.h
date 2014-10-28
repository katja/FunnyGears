#ifndef SELECTION_MODEL
#define SELECTION_MODEL

#include "stable.h"
#include "graphics_objects/GraphicsItem.h"
class GraphicsScene;
class SceneTreeModel;

class SelectionModel : public QItemSelectionModel {

Q_OBJECT

public:
    SelectionModel(SceneTreeModel *model);
    ~SelectionModel();

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

#endif // SELECTION_MODEL