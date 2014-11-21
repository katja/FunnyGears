#ifndef GRAPHICS_ROOT_ITEM
#define GRAPHICS_ROOT_ITEM

#include "graphics_objects/GraphicsScheduleItem.h"

/** Only a helper class for the Model */
class GraphicsRootItem : public GraphicsScheduleItem {

public:
    GraphicsRootItem() {
        setFlags(QGraphicsItem::ItemHasNoContents);
    }
    ~GraphicsRootItem() {}

    QString defaultName() const override {
        return QString("Root Item");
    }

    QRectF boundingRect() const override {
        return QRectF();
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override {
        Q_UNUSED(painter);
        Q_UNUSED(option);
        Q_UNUSED(widget);
    }

    void receivedClickOn(QPointF scenePos) override {
        Q_UNUSED(scenePos);
    }
};
#endif // GRAPHICS_ROOT_ITEM
