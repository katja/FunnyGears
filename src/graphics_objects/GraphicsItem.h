#ifndef GRAPHICS_ITEM
#define GRAPHICS_ITEM

#include "stable.h"
#include "definitions.h"
#include "tool_bars/EditingToolBarListener.h"
#include "helper_objects/ChangingObject.h"
#include "helper_objects/ChangingObjectResponder.h"

/* GraphicsItem provides two sorts of tracking of changes: One for immediate reaction (ChangingObject)
and one for asking for reaction (ChangingObjectResponder).
In changed() both variants are used. All ChangingObjectListener are informed and the intern
status of GraphicsItem is set to changed. */

class GraphicsItem : public QGraphicsItem, public EditingToolBarListener, public ChangingObject, public ChangingObjectResponder {

public:
    static const int Type;// = GraphicsItem::UserType + Type::GraphicsItemType;
    static bool isGraphicsItem(QGraphicsItem *item);

    GraphicsItem(GraphicsItem *parent = 0);
    virtual ~GraphicsItem() {}

    virtual int type() const override; // from QGraphicsItem
    virtual void receivedClickOn(QPointF scenePos) = 0;

    void setParentItem(QGraphicsItem *newParent); // (not real) override from QGraphicsItem

    bool noItemInSceneSelected();

    virtual void stopEditing() override {} // from EditingToolBarListener
    virtual void startEditing(Editing::State) override {}; // from EditingToolBarListener
    virtual void executeEditingAction(Editing::Action) override {}; // from EditingToolBarListener

    void informAboutChange(ChangingObjectListener *listener) override; // from ChangingObject
    void noMoreInformAboutChange(ChangingObjectListener *listener) override; // from ChangingObject
    void changed();
    bool hasChanged() const override; // from ChangingObjectResponder
    void clearChanges() override; // from ChangingObjectResponder

protected:
    list<ChangingObjectListener*> m_changingListeners;
    bool m_changed;

};

#endif // GRAPHICS_ITEM
