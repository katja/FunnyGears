#include "SceneTreeModel.h"

SceneTreeModel::SceneTreeModel(GraphicsScene *scene, QObject *parent) : QAbstractItemModel(parent), m_graphicsScene(scene) {
    std::cout << "SceneTreeModel is created" << std::endl;

    QHash<Qt::ItemDataRole, QString> *dataHash = new QHash<Qt::ItemDataRole, QString>();

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

    dataHash = new QHash<Qt::ItemDataRole, QString>();
    dataHash->insert(Qt::DisplayRole, "Translation");
    dataHash->insert(Qt::ToolTipRole, "Holds the position of the graphicsItem.");
    m_headHash.insert(TRANSLATION, *dataHash);

    dataHash = new QHash<Qt::ItemDataRole, QString>();
    dataHash->insert(Qt::DisplayRole, "Rotation");
    dataHash->insert(Qt::ToolTipRole, "Holds the rotation of the graphicsItem");
    m_headHash.insert(ROTATION, *dataHash);

    dataHash = new QHash<Qt::ItemDataRole, QString>();
    dataHash->insert(Qt::DisplayRole, "Geometry");
    dataHash->insert(Qt::ToolTipRole, "Object, displayed in the Scene");
    m_headHash.insert(GRAPHIC, *dataHash);

    m_rootItem = new SceneTreeItem(m_graphicsScene);
    emit dataChanged(QModelIndex(), QModelIndex());
}

SceneTreeModel::~SceneTreeModel() {
    std::cout << "SceneTreeModel is deleted" << std::endl;
    delete m_rootItem;
}

SceneTreeItem* SceneTreeModel::setRoot(SceneTreeItem *rootItem) {
    SceneTreeItem *oldRoot = m_rootItem;
    emit layoutAboutToBeChanged();
    m_rootItem = rootItem;
    //read QAbstractItemModel - layoutChanged() TODO: include GraphicsScene here!
    //maybe more is necessary here!!!
    emit layoutChanged();
    return oldRoot;
}

QModelIndex SceneTreeModel::index(int row, int column, const QModelIndex &parent) const {
    if(row < 0)
        return QModelIndex();
    SceneTreeItem *parentItem = findItemBy(parent);
    if(parentItem && parentItem->numberOfChildren() > row) {
        return createIndex(row, column, parentItem->child(row));
    } else {
        return QModelIndex();
    }
}

QModelIndex SceneTreeModel::parent(const QModelIndex &index) const {
    SceneTreeItem *childItem = findItemBy(index);
    if(childItem) {
        if(childItem == m_rootItem | childItem->parent() == m_rootItem)
            return QModelIndex();
        else {
            SceneTreeItem *parentItem = childItem->parent();
            return createIndex(parentItem->childNumber(), 0, parentItem);
        }
    }
    return QModelIndex();
} //When reimplementing this function in a subclass, be careful to avoid calling QModelIndex member functions, such as QModelIndex::parent(), since indexes belonging to your model will simply call your implementation, leading to infinite recursion.

bool SceneTreeModel::hasChildren(const QModelIndex &parent) const {
    SceneTreeItem *parentItem = findItemBy(parent);
    if(!parentItem)
        return false;
    else
        return (parentItem->numberOfChildren() > 0);
}

int SceneTreeModel::rowCount(const QModelIndex &parent) const {
    SceneTreeItem *parentItem = findItemBy(parent);
    if(parentItem)
        return parentItem->numberOfChildren();
    else
        return 0;
}

int SceneTreeModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return m_headHash.size();
}

QVariant SceneTreeModel::data(const QModelIndex &index, int role) const {
    if(!index.isValid())
        return QVariant();
    SceneTreeItem *item = findItemBy(index);
    if(item) {
        if(role == Qt::DisplayRole || role == Qt::ToolTipRole) { //TODO: do something more suitable for ToolTipRole
            switch(index.column()) {
                case NAME:
                    return item->name();
                // case VISIBILITY: //TODO: maybe it would be good to give this boolean value here, too, but disabling it in the used QAbstractItemView or better the QAbstractItemDelegate (YesNoDelegate)
                //     return item->isVisible();
                case TYPE:
                    return item->type();
                // case TRANSLATION:
                //     return ...
                // case ROTATION
                //     return ...
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

bool SceneTreeModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if(role != Qt::EditRole)
        return false;
    SceneTreeItem *item = findItemBy(index);
    if(item) {
        if (index.column() == NAME) {
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

QModelIndex SceneTreeModel::itemWithGraphicsItem(const GraphicsItem *graphicsItem) {
    SceneTreeItem *graphicsTreeItem = m_rootItem->itemWithGraphicsItem(graphicsItem);
    if(!graphicsTreeItem) {
        return QModelIndex();
    }
    int row = graphicsTreeItem->childNumber();
    return createIndex(row, GRAPHIC, graphicsTreeItem);
}

bool SceneTreeModel::toggleValue(const QModelIndex &index) {
    SceneTreeItem *item = findItemBy(index);
    if(item) {
        if(index.column() == VISIBILITY)
            item->toggleVisibility();
        emit dataChanged(index, index);
        return true;
    }
    return false;
}

QVariant SceneTreeModel::headerData(int section, Qt::Orientation orientation, int role) const {
    Q_UNUSED(orientation);
    if(section < 0 || section >= m_headHash.size())
        return QVariant();
    if(m_headHash.contains(Data(section)) && m_headHash[Data(section)].contains(Qt::ItemDataRole(role)))
        return m_headHash[Data(section)][Qt::ItemDataRole(role)];
    else
        return QVariant();
}

Qt::ItemFlags SceneTreeModel::flags(const QModelIndex &index) const {
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

bool SceneTreeModel::addItem(GraphicsItem *graphicsItem) {
    int lastRow = rowCount(QModelIndex());

    beginInsertRows(QModelIndex(), lastRow, 1);
    //TODO: is the "1" in row above correct?
        // m_graphicsScene->addItem(graphicsItem);
        m_rootItem->addChild(graphicsItem);
    endInsertRows();
    return true;
}

bool SceneTreeModel::remove(QModelIndex index) {
    if(!index.isValid())
        return false;
    SceneTreeItem *item = findItemBy(index);
    if(!item->isRemovable())
        return false;

    m_graphicsScene->removeItem(item->graphicsItem());

    beginRemoveRows(index.parent(), index.row(), index.row() + 1);
    item->parent()->removeChild(index.row());
    endRemoveRows();
    return true;
}

SceneTreeItem* SceneTreeModel::findItemBy(const QModelIndex &index) const {
    if(index.isValid()) {
        SceneTreeItem *item = static_cast<SceneTreeItem*>(index.internalPointer());
        if(item)
            return item;
    }
    return m_rootItem;
}
