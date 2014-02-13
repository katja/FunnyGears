#ifndef SCENE_TREE_ITEM
#define SCENE_TREE_ITEM

#include "stable.h"
#include "Geometry.h"

class SceneTreeItem
{
public:
    SceneTreeItem();
    SceneTreeItem(Geometry *geometry, SceneTreeItem *parent);
    ~SceneTreeItem();

    bool            isRoot() const;

    SceneTreeItem*  addChild(Geometry *geometry);
    bool            removeChild(SceneTreeItem *child);

    SceneTreeItem*  parent() const;
    SceneTreeItem*  child(int number) const;

    int             numberOfChildren() const;
    int             childNumber() const;

    void            setName(QString name);
    QString         name() const;
    void            toggleVisibility();
    bool            isVisible() const;
    bool            isRemovable() const;

private:
    Geometry *m_geometry;

    bool m_isRoot;
    SceneTreeItem *m_parent;
    QList<SceneTreeItem*> m_children;

    QString m_name;
    bool m_isVisible;
};

#endif //SCENE_TREE_ITEM
