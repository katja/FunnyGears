#ifndef SCENE_TREE_ITEM
#define SCENE_TREE_ITEM

#include "stable.h"
#include "GraphicsScene.h"

class SceneTreeItem {

public:
    SceneTreeItem(GraphicsScene *graphicsScene);
    SceneTreeItem(GraphicsItem *graphicsItem, SceneTreeItem *parent);
    ~SceneTreeItem();

    bool            isRoot() const;

    SceneTreeItem*  addChild(GraphicsItem *graphicsItem);
    bool            removeChild(SceneTreeItem *child);
    bool            removeChild(int childIndex);

    SceneTreeItem*  parent() const;
    SceneTreeItem*  child(int number) const;

    int             numberOfChildren() const;
    int             childNumber() const;

    void            setName(QString name);
    QString         name() const;
    void            toggleVisibility();
    bool            isVisible() const;
    bool            isRemovable() const;


    QString         type() const;
    GraphicsItem*   graphicsItem() const;
    SceneTreeItem*  itemWithGraphicsItem(const GraphicsItem *graphicsItem) const;

private:
    bool m_isRoot;
    GraphicsScene *m_graphicsScene;

    SceneTreeItem *m_parent;
    QList<SceneTreeItem*> m_children;

    QString m_name;
    GraphicsItem *m_graphicsItem;
    bool m_isVisible;

};

#endif //SCENE_TREE_ITEM
