#ifndef GRAPHICS_SPLINE_GEAR_ATTRIBUTES_WIDGET
#define GRAPHICS_SPLINE_GEAR_ATTRIBUTES_WIDGET

#include "stable.h"
#include "definitions.h"
#include "graphics_widgets/GraphicsItemAttributesWidget.h"
#include "graphics_widgets/GraphicsSplineAttributesWidget.h"
#include "graphics_objects/GraphicsSplineGear.h"

class GraphicsSplineGearAttributesWidget : public GraphicsItemAttributesWidget {

Q_OBJECT

public:
    GraphicsSplineGearAttributesWidget(QWidget *parent = 0);
    ~GraphicsSplineGearAttributesWidget();

    void setItem(QGraphicsItem *graphicsItem) override; // from GraphicsItemAttributesWidget
    bool worksOnItem(QGraphicsItem *graphicsItem) override; // from GraphicsItemAttributesWidget

private slots:
    void changeRadius(real);
    void changeNumberOfTeeth(int);

private:
    GraphicsSplineGear *m_currentGear;

    GraphicsSplineAttributesWidget *m_splineAttributesWidget;

    QSpinBox *m_numberOfTeethSpinBox;
    QLabel   *m_numberOfTeethLabel;

    RealValuedSlider *m_radiusSlider;

    void updateAttributes();

};


#endif // GRAPHICS_SPLINE_GEAR_ATTRIBUTES_WIDGET
