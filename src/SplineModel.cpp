#include "SplineModel.h"

SplineModel::SplineModel(SceneTreeModel *sceneModel, QObject *parent) : GraphicsItemModel(sceneModel, parent) {
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
    return PROPERTIES; // TODO: better idea?
}

QVariant SplineModel::data(const QModelIndex &index, int role) const {
    std::cout << "\nDATA IS ASKED" << std::endl;
    Q_UNUSED(role); //TODO: vielleicht doch nutzen???!!! (tooltip etc.)
    if(!index.isValid() || !m_splines.containsIndex(index.row()))
        return QVariant();

    GraphicsSpline *graphicsSpline = m_splines.at(index.row());
    switch(index.column()) {
        case DEGREE:
            std::cout << "degree" << std::endl;
            return graphicsSpline->spline()->degree();
        case TORN_TO_EDGES:
            std::cout << "torn to edges?" << std::endl;
            return graphicsSpline->spline()->isTornToEdges();
        case TANGENT_DRAWN:
            std::cout << "tangent drawn?" << std::endl;
            return graphicsSpline->isTangentDrawn();
        case TANGENT_VALUE:
            std::cout << "tangent value" << std::endl;
            return graphicsSpline->tangentValue();
        case MIN_VALUE:
            std::cout << "min value " << std::endl;
            return 3;
        case MAX_VALUE:
            std::cout << "max value " << std::endl;
            return 3;
        default:
            return QVariant();
    }
}

bool SplineModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    std::cout << "\nDATA IS SET" << std::endl;
    Q_UNUSED(role); //TODO: vielleicht doch noch ändern?
    if(!index.isValid() || !m_splines.containsIndex(index.row()))
        return false;

    GraphicsSpline *graphicsSpline = m_splines.at(index.row());
    if(index.column() == DEGREE) {
        std::cout << "SplineModel::setData degree change     with " << value.toInt() << std::endl;
        graphicsSpline->changeDegree(value.toInt());
    }
    else if(index.column() == TORN_TO_EDGES) {
        std::cout << "SplineModel::setData changeTornToEdges with " << value.toBool() << std::endl;
        graphicsSpline->changeTornToEdges(value.toBool());
    }
    else if(index.column() == TANGENT_DRAWN) {
        std::cout << "SplineModel::setData tangent drawn?    with " << value.toBool() << std::endl;
        graphicsSpline->setTangentDrawn(value.toBool());
    }
    else if(index.column() == TANGENT_VALUE) {
        std::cout << "SplineModel::setData tangent value     with " << value.toInt() << std::endl;
        graphicsSpline->setTangentValue(value.toInt());
    }
    else
        return false;
    std::cout << "DATA IS SET - READY with true" << std::endl;
    return true;
}

Qt::ItemFlags SplineModel::flags(const QModelIndex &index) const {
    if(!index.isValid())
        return Qt::NoItemFlags;

    if(index.column() == DEGREE ||
        index.column() == TANGENT_VALUE)
        return Qt::ItemIsEditable | QAbstractItemModel::flags(index);

    if(index.column() == TORN_TO_EDGES ||
        index.column() == TANGENT_DRAWN)
        return Qt::ItemIsUserCheckable | QAbstractItemModel::flags(index);

    if(index.column() == MIN_VALUE ||
        index.column() == MAX_VALUE)
        return Qt::NoItemFlags | QAbstractItemModel::flags(index);

    return Qt::NoItemFlags;
}

bool SplineModel::addSpline(GraphicsSpline *graphicsSpline) {
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_splines << graphicsSpline;
    endInsertRows();
    return true;
}

bool SplineModel::removeSpline(GraphicsSpline *graphicsSpline) {
    int index = m_splines.indexOf(graphicsSpline);
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
    return m_splines.indexOf(static_cast<GraphicsSpline*>(graphicsItem));
}

void SplineModel::graphicsItemAdded(QGraphicsItem *graphicsItem) {
    if(!isSpline(graphicsItem))
        return;
    addSpline(static_cast<GraphicsSpline*>(graphicsItem));
}

void SplineModel::graphicsItemRemoved(QGraphicsItem *graphicsItem) {
    if(!isSpline(graphicsItem))
        return;
    removeSpline(static_cast<GraphicsSpline*>(graphicsItem));
}

bool SplineModel::isSpline(QGraphicsItem *graphicsItem) {
    GraphicsSpline spline;
    if(typeid(*graphicsItem) == typeid(spline))
        return true;
    else
        return false;
}
