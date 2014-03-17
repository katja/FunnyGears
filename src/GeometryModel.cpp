#include "GeometryModel.h"

GeometryModel::GeometryModel(SceneTreeModel *sceneModel, QObject *parent) : QAbstractTableModel(parent), m_sceneTreeModel(sceneModel) {
    connect(m_sceneTreeModel, SIGNAL(geometryAdded(QGraphicsItem*)), this, SLOT(geometryAdded(QGraphicsItem*)));
}

GeometryModel::~GeometryModel() {
}
