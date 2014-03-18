#include "SplineModel.h"

SplineModel::SplineModel(SceneTreeModel *sceneModel, QObject *parent) : GeometryModel(sceneModel, parent) {
    std::cout << "SplineModel is created" << std::endl;
}

SplineModel::~SplineModel() {
    std::cout << "SplineModel is deleted" << std::endl;
    //TODO: delete splines (m_splines) here???
}

int SplineModel::rowCount(const QModelIndex &parent) const {
    if(parent.isValid())
        return 0;
    return m_splines.size();
}

int SplineModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return 2; // TODO: better idea?
}

QVariant SplineModel::data(const QModelIndex &index, int role) const {
    Q_UNUSED(role); //TODO: vielleicht doch nutzen???!!! (tooltip etc.)
    if(!index.isValid() || !m_splines.containsIndex(index.row()))
        return QVariant();

    Spline *spline = m_splines.at(index.row());
    switch(index.column()) {
        case DEGREE:
            return spline->degree();
        case TORN_TO_EDGES:
            return spline->isTornToEdges();
        default:
            return QVariant();
    }
}

bool SplineModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    Q_UNUSED(role); //TODO: vielleicht doch noch ändern?
    if(!index.isValid() || !m_splines.containsIndex(index.row()))
        return false;
    Spline *spline = m_splines.at(index.row());
    switch(index.column()) {
        case DEGREE:
            spline->setDegree(value.toInt());
            // m_sceneTreeModel->
            return true;
        case TORN_TO_EDGES:
            spline->setTornToEdges(value.toBool());
            return true;
        default:
            return false;
    }
}

Qt::ItemFlags SplineModel::flags(const QModelIndex &index) const {
    if(!index.isValid())
        return Qt::NoItemFlags;
    if(index.column() == DEGREE)
        return Qt::ItemIsEditable | QAbstractItemModel::flags(index);
    if(index.column() == TORN_TO_EDGES)
        return Qt::ItemIsUserCheckable | QAbstractItemModel::flags(index);
    return Qt::NoItemFlags;
}

bool SplineModel::addSpline(Spline *spline) {
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_splines << spline;
    endInsertRows();
    return true;
}

bool SplineModel::removeSpline(Spline *spline) {
    int index = m_splines.indexOf(spline);
    if(index < 0)
        return false;
    beginRemoveRows(QModelIndex(), index, index);
    m_splines.removeAt(index); //TODO: delete spline???
    endRemoveRows();
    return true;
}

bool SplineModel::operatesOn(QGraphicsItem *graphicsItem) {
    return isSpline(graphicsItem);
}

int SplineModel::indexOf(QGraphicsItem *graphicsItem) {
    if(!isSpline(graphicsItem))
        return -1;
    return m_splines.indexOf((static_cast<GraphicsSpline*>(graphicsItem))->spline());
}

void SplineModel::geometryAdded(QGraphicsItem *graphicsItem) {
    if(!isSpline(graphicsItem))
        return;
    addSpline((static_cast<GraphicsSpline*>(graphicsItem))->spline());
}

bool SplineModel::isSpline(QGraphicsItem *graphicsItem) {
    GraphicsSpline spline;
    if(typeid(*graphicsItem) == typeid(spline))
        return true;
    else
        return false;
}
