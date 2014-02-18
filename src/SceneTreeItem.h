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

    SceneTreeItem*  addChild(Geometry *geometry = new Geometry());
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


    QString         geometry() const;
    void            translate(const QVector2D &translation);
    QVector2D       getTranslation() const;
    void            rotate(float angle);
    float           getRotation() const;


private:
    Geometry *m_geometry;

    bool m_isRoot;
    SceneTreeItem *m_parent;
    QList<SceneTreeItem*> m_children;

    QString m_name;
    bool m_isVisible;

    QVector2D m_translation; //translation
    float m_rotation; //rotation counterclockwise

};

#endif //SCENE_TREE_ITEM
