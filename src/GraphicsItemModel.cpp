#include "GraphicsItemModel.h"

GraphicsItemModel::GraphicsItemModel(SceneTreeModel *sceneModel, QObject *parent) : QAbstractTableModel(parent), m_sceneTreeModel(sceneModel) {
    connect(m_sceneTreeModel, SIGNAL(graphicsItemAdded(QGraphicsItem*)), this, SLOT(graphicsItemAdded(QGraphicsItem*)));
    connect(m_sceneTreeModel, SIGNAL(graphicsItemRemoved(QGraphicsItem*)), this, SLOT(graphicsItemRemoved(QGraphicsItem*)));
}

GraphicsItemModel::~GraphicsItemModel() {
}
