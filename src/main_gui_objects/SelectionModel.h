#ifndef SELECTION_MODEL
#define SELECTION_MODEL

#include "stable.h"
#include "graphics_objects/GraphicsItem.h"
#include "graphics_objects/GraphicsScheduleItem.h"

class GraphicsScene;
class Model;

/** Builds a connection for the GraphicsItems from the Model to the Scene
 *
 *  As a QItemSelectionModel this class is used by Qt to manage the selection of f.ex.
 *  a QTreeView (like the TreeView in ObjectScheduleViewWidget). The QGraphicsScene is
 *  normally not meant to be used together with Qts model/view architecture. Therefore
 *  this class acts as a connection between Qts model/view and Qts Graphics View Framework.
 *  It manages the correct selection of GraphicsItems in both architectures.
 *
 *  SelectionModel provides signals, that can be used by other components to get the current
 *  selection of the model/view architecture. Other components may use the provided slot.
 *  At the moment the connection is only used by the GraphicsScene. To keep things simple
 *  the sceneSelectionChanged() slot expects a GraphicsScene pointer. But one could
 *  introduce an interface with the only method used here (selectedItems()) to connect other
 *  components, too.
 *
 *  The internal QList<GraphicsScheduleItem*> m_selectedItems keeps track of which items
 *  are selected. So every time a selection change happens in the connected Model model,
 *  the private slot updateSelection() is called, which updates m_selectedItems and then
 *  emits the suitable sigal.
 *
 *  Special handling is needed before removing one or especially many GraphicsItems. This
 *  is only allowed by the Model model. While objects are removed, no signals are sent
 *  either way. Afterwards the selection is updated with the current selection of the model.
 */
class SelectionModel : public QItemSelectionModel {

Q_OBJECT

public:
    SelectionModel(Model *model);
    ~SelectionModel();

signals:
    void oneGraphicsItemSelected(GraphicsItem *item);
    void noneOrManyGraphicsItemsSelected();
    void topLevelItemsAreSelected(); //at least one top level item is in current selection
    void noTopLevelItemsAreSelected(); //no top level items are in current selection

public slots:
    void sceneSelectionChanged(GraphicsScene *scene);

private slots:
    void updateSelection(const QItemSelection &selected, const QItemSelection &deselected);
    void prepareRowRemoval();
    void continueAfterRowRemoval();

private:
    Model *m_model;

    QList<GraphicsScheduleItem*> m_selectedItems;

    bool m_connectionsAreActivated;

    void pauseSelectionConnection();
    void continueSelectionConnection();

    void deselectAll();
    void changeSelectionInModel(GraphicsScheduleItem *item, QItemSelectionModel::SelectionFlags command = QItemSelectionModel::Select);
    void reportSelectionCount();
};

#endif // SELECTION_MODEL
