#ifndef GRAPHICS_SPLINE_GEAR_ATTRIBUTES_WIDGET
#define GRAPHICS_SPLINE_GEAR_ATTRIBUTES_WIDGET

#include "stable.h"
#include "definitions.h"
#include "graphics_widgets/GraphicsItemAttributesWidget.h"
#include "graphics_widgets/GraphicsSplineAttributesWidget.h"
#include "graphics_objects/GraphicsSplineGear.h"

class GraphicsSplineGearAttributesWidget : public GraphicsItemAttributesWidget {

public:
    GraphicsSplineGearAttributesWidget(QWidget *parent = 0);
    ~GraphicsSplineGearAttributesWidget();

    void setItem(QGraphicsItem *graphicsItem) override; // from GraphicsItemAttributesWidget
    bool worksOnItem(QGraphicsItem *graphicsItem) override; // from GraphicsItemAttributesWidget

private:
    GraphicsSplineGear *m_currentGear;

    GraphicsSplineAttributesWidget *m_splineAttributesWidget;

};


#endif // GRAPHICS_SPLINE_GEAR_ATTRIBUTES_WIDGET
