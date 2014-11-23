#include "Model.h"
#include "graphics_objects/GraphicsRootItem.h"

Model::Model(QGraphicsScene *scene, QObject *parent) : QAbstractItemModel(parent), m_scene(scene) {
    std::cout << "Model is created" << std::endl;

    m_rootItem = new GraphicsRootItem();
    m_scene->addItem(m_rootItem);

    QHash<Qt::ItemDataRole, QString> *dataHash;

    dataHash = new QHash<Qt::ItemDataRole, QString>();
    dataHash->insert(Qt::DisplayRole, "Name");
    dataHash->insert(Qt::ToolTipRole, "Your given name for the item. You can change it.");
    m_headHash.insert(NAME, *dataHash);

    dataHash = new QHash<Qt::ItemDataRole, QString>();
    dataHash->insert(Qt::DisplayRole, "Vis.");
    dataHash->insert(Qt::ToolTipRole, "The item can be visible or not, which is shown here.");
    m_headHash.insert(VISIBILITY, *dataHash);

    dataHash = new QHash<Qt::ItemDataRole, QString>();
    dataHash->insert(Qt::DisplayRole, "Type");
    dataHash->insert(Qt::ToolTipRole, "Holds information about the whole graphicsItem.");
    m_headHash.insert(TYPE, *dataHash);

    emit dataChanged(QModelIndex(), QModelIndex());
}

Model::~Model() {
    std::cout << "Model is deleted" << std::endl;
}

QModelIndex Model::index(int row, int column, const QModelIndex &parentIndex) const {
    if(row < 0 || column < 0 || column > columnCount())
        return QModelIndex();
    GraphicsScheduleItem *parent = getInternItemFromIndex(parentIndex);
    if(parent && parent->numberOfChildren() > row) {
        return createIndex(row, column, parent->child(row));
    } else {
        return QModelIndex();
    }
}

QModelIndex Model::parent(const QModelIndex &index) const {
    GraphicsScheduleItem *child = getInternItemFromIndex(index);
    if(child) {
        GraphicsScheduleItem *parent = child->parent();
        if(parent == m_rootItem)
            return QModelIndex();
        else
            return createIndex(parent->childNumber(), 0, parent);
    }
    return QModelIndex();
}

bool Model::hasChildren(const QModelIndex &parentIndex) const {
    GraphicsScheduleItem *parent = getInternItemFromIndex(parentIndex);
    if(!parent) {
        return false;
    } else {
        return (parent->numberOfChildren() > 0);
    }
}

int Model::rowCount(const QModelIndex &parentIndex) const {
    GraphicsScheduleItem *parent = getInternItemFromIndex(parentIndex);
    if(parent)
        return parent->numberOfChildren();
    else
        return 0;
}

int Model::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return m_headHash.size();
}

QVariant Model::data(const QModelIndex &index, int role) const {
    if(!index.isValid()) {
        return QVariant();
    }
    GraphicsScheduleItem *item = getInternItemFromIndex(index);
    if(item) {
        if(role == Qt::DisplayRole || role == Qt::ToolTipRole) { //TODO: do something more suitable for ToolTipRole
            switch(index.column()) {
                case NAME:
                    return item->name();
                case TYPE:
                    return item->type();
                default:
                    return QVariant();
            }
        } else if (role == Qt::DecorationRole && index.column() == VISIBILITY) {
            if(item->isVisible())
                return QIcon("images/EyeOpen.png");
            else
                return QIcon("images/EyeClosed.png");
        }
    }
    return QVariant();
}

bool Model::setData(const QModelIndex &index, const QVariant &value, int role) {
    if(role != Qt::EditRole)
        return false;
    GraphicsScheduleItem *item = getInternItemFromIndex(index);
    if(item) {
        if(index.column() == NAME) {
            item->setName(value.toString());
        } else {
            return false;
        }
    } else {
        return false;
    }
    emit dataChanged(index, index);
    return true;
}

bool Model::toggleValue(const QModelIndex &index) {
    GraphicsScheduleItem *item = getInternItemFromIndex(index);
    if(item) {
        if(index.column() == VISIBILITY)
            item->toggleVisibility();
        emit dataChanged(index, index);
        return true;
    }
    return false;
}

QVariant Model::headerData(int column, Qt::Orientation orientation, int role) const {
    Q_UNUSED(orientation);
    if(column < 0 || column >= m_headHash.size())
        return QVariant();
    if(m_headHash.contains(Data(column)) && m_headHash[Data(column)].contains(Qt::ItemDataRole(role)))
        return m_headHash[Data(column)][Qt::ItemDataRole(role)];
    else
        return QVariant();
}

Qt::ItemFlags Model::flags(const QModelIndex &index) const {
    if(index.isValid()) {
        if(index.column() == NAME)
            return Qt::ItemIsEditable | QAbstractItemModel::flags(index);
        if(index.column() == VISIBILITY)
            return Qt::ItemIsUserCheckable | QAbstractItemModel::flags(index);
        if(index.column() == TYPE)
            return Qt::ItemIsSelectable | QAbstractItemModel::flags(index);
        return Qt::NoItemFlags;
    } else {
        return Qt::NoItemFlags;
    }
}

bool Model::addItem(GraphicsScheduleItem *newItem, GraphicsScheduleItem *parent) {
    if(parent == 0) { //parent is rootItem
        int lastRow = m_rootItem->numberOfChildren();
        beginInsertRows(QModelIndex(), lastRow, lastRow + 1);
        newItem->setParentItem(m_rootItem);
        endInsertRows();
    } else {
        int row = parent->numberOfChildren();
        QModelIndex parentIndex = createIndex(parent->childNumber(), 0, parent);
        beginInsertRows(parentIndex, row, row + 1);
        newItem->setParentItem(parent);
        endInsertRows();
    }
    return true;
}

bool Model::remove(QModelIndex index) {
    if(!index.isValid())
        return false;
    GraphicsScheduleItem *item = getInternItemFromIndex(index);

    beginRemoveRows(index.parent(), index.row(), index.row() + 1);
    m_scene->removeItem(item);
    delete item;
    endRemoveRows();
    return true;
}

void Model::clearSelection() {
    m_rootItem->clearSelection();
}

GraphicsScheduleItem* Model::getItemFromIndex(const QModelIndex &index) const {
    GraphicsScheduleItem *item = getInternItemFromIndex(index);
    if(item == m_rootItem)
        return 0;
    return item;
}

QModelIndex Model::getIndexFromItem(GraphicsScheduleItem *item) const {
    return createIndex(item->childNumber(), 0, item);
}

GraphicsScheduleItem* Model::getInternItemFromIndex(const QModelIndex &index) const {
    if(index.isValid()) {
        GraphicsScheduleItem *item = static_cast<GraphicsScheduleItem*>(index.internalPointer());
        if(item)
            return item;
    }
    return m_rootItem;
}
