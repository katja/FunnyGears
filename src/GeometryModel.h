#ifndef GEOMETRY_MODEL
#define GEOMETRY_MODEL

#include "stable.h"
#include "SceneTreeModel.h"

class GeometryModel : public QAbstractTableModel {

Q_OBJECT

public:
    GeometryModel(SceneTreeModel *model, QObject *parent = 0);
    ~GeometryModel();

    virtual bool operatesOn(QGraphicsItem *graphicsItem) = 0;
    virtual int indexOf(QGraphicsItem *graphicsItem) = 0;

public slots:
    virtual void geometryAdded(QGraphicsItem *graphicsItem) = 0;

protected:
    SceneTreeModel *m_sceneTreeModel;

};

#endif // GEOMETRY_MODEL
