#ifndef GRAPHICS_ITEM_ATTRIBUTES_WIDGET
#define GRAPHICS_ITEM_ATTRIBUTES_WIDGET

#include "stable.h"

class GraphicsItemAttributesWidget : public QWidget {

public:
    GraphicsItemAttributesWidget(QWidget *parent = 0);
    virtual ~GraphicsItemAttributesWidget();

    virtual void setItem(QGraphicsItem *graphicsItem) = 0;
};

#endif // GRAPHICS_ITEM_ATTRIBUTES_WIDGET
