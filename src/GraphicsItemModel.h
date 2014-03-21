#ifndef GRAPHICS_ITEM_MODEL
#define GRAPHICS_ITEM_MODEL

#include "stable.h"
#include "SceneTreeModel.h"

class GraphicsItemModel : public QAbstractTableModel {

Q_OBJECT

public:
    GraphicsItemModel(SceneTreeModel *model, QObject *parent = 0);
    ~GraphicsItemModel();

    virtual bool operatesOn(QGraphicsItem *graphicsItem) = 0;
    virtual int indexOf(QGraphicsItem *graphicsItem) = 0;

public slots:
    virtual void graphicsItemAdded(QGraphicsItem *graphicsItem) = 0;
    virtual void graphicsItemRemoved(QGraphicsItem *graphicsItem) = 0;

protected:
    SceneTreeModel *m_sceneTreeModel;

};

#endif // GRAPHICS_ITEM_MODEL
