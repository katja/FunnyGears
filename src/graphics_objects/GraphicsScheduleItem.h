#ifndef GRAPHICS_SCHEDULE_ITEM
#define GRAPHICS_SCHEDULE_ITEM

#include "stable.h"
#include "GraphicsItem.h"

/** Abstract class for all objects which should be rendered and displayed in the object schedule.
 *  This class is intended to be used in combination with the Model and the ObjectSchedule class.
 *  It stores another tree, as each item has a parent and can have children. But only items
 *  of subclasses of this class are tracked in the tree and therefore in the Model, as the
 *  overridden itemChange() method tracks only objects, which inherit from GraphicsScheduleItem.
 */
class GraphicsScheduleItem : public GraphicsItem {

public:
    static const int Type;// = GraphicsItem::UserType + Type::GraphicsScheduleItemType;
    static bool isGraphicsScheduleItem(QGraphicsItem *item);

    GraphicsScheduleItem();
    virtual ~GraphicsScheduleItem();

    int type() const override;

    /** Returns the parent item or 0, if it is a top-level item or if it does not have a parent
     */
    GraphicsScheduleItem* parent() const;

    /** Returns a pointer to the child number 'number'
     *  To receive correct values, number must be >= 0 and < numberOfChildren
     *  If this is not the case, a default constructed GraphicsScheduleItem* will be returned
     */
    GraphicsScheduleItem* child(int number) const;

    /** Returns the number of children, which are GraphicsItems
     *  Use childItems() to get a list of ALL children, not only GraphicsItems ones.
     */
    int numberOfChildren() const;

    /** The number returned specifies, at which position this item is placed in its parents child items
     *  This method is necessary f.ex. for the model to calculate the correct index or more precisely the correct row
     */
    int childNumber() const;

    /** The default name, intended for giving a sign to the user, what type of item
     *  this item is. It is used f. ex. in the tree schedule to have a first name.
     */
    virtual QString defaultName() const;

    /** Sets the internal saved name to @param name
     */
    void setName(QString name);

    /** Returns a name to facilitate the recognizability for the user.
     *  This method is used f. ex. in the model to display something in the NAME column
     *  The method is not const, as it changes the name, if saved name is an empty String
     *  like it is the case when just created. It is set to "Unnamed <defaultName()>"
     *  With this approach the default name of subclasses can be handled, which is not
     *  possible, if the first name construction would sit in the constructor.
     */
    virtual QString name();

    /** Shows the item, if it has been hidden,
     *  and hides it, if it was shown until now.
     */
    void toggleVisibility();

protected:
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &var) override;

private:
    GraphicsScheduleItem *m_parent;
    QList<GraphicsScheduleItem*> m_children;

    QString m_name;

    void handleNewParent(QGraphicsItem *newParent);
    void handleChildAdded(QGraphicsItem *newChild);
    void handleChildRemoved(QGraphicsItem *oldChild);
    void handleSceneChange(QGraphicsScene *newScene);

};

#endif // GRAPHICS_SCHEDULE_ITEM
