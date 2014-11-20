#ifndef SELECTION_MODEL
#define SELECTION_MODEL

#include "stable.h"
#include "graphics_objects/GraphicsItem.h"
#include "graphics_objects/GraphicsScheduleItem.h"

class GraphicsScene;
class Model;

class SelectionModel : public QItemSelectionModel {

Q_OBJECT

public:
    SelectionModel(Model *model);
    ~SelectionModel();

signals:
    void oneGraphicsItemSelected(GraphicsItem *item);
    void noneOrManyGraphicsItemsSelected();

public slots:
    void sceneSelectionChanged(GraphicsScene *scene);

private slots:
    void updateSelection(const QItemSelection &selected, const QItemSelection &deselected);

private:
    Model *m_model;

    QList<GraphicsScheduleItem*> m_selectedItems;

    void changeSelectionInModel(GraphicsScheduleItem *item, QItemSelectionModel::SelectionFlags command = QItemSelectionModel::Select);
    void reportSelectionCount();

};

#endif // SELECTION_MODEL
