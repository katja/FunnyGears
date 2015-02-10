#ifndef GRAPHICS_SPLINE_ATTRIBUTES_WIDGET
#define GRAPHICS_SPLINE_ATTRIBUTES_WIDGET

#include "stable.h"
#include "definitions.h"
#include "graphics_widgets/GraphicsItemAttributesWidget.h"
#include "graphics_objects/GraphicsSpline.h"
#include "RealValuedSlider.h"
#include "ChangingObjectListener.h"

class GraphicsSplineAttributesWidget : public GraphicsItemAttributesWidget, public ChangingObjectListener {

Q_OBJECT

public:
    GraphicsSplineAttributesWidget(QWidget *parent = 0);
    ~GraphicsSplineAttributesWidget();

    void setItem(QGraphicsItem *graphicsItem) override; // from GraphicsItemAttributesWidget
    bool worksOnItem(QGraphicsItem *graphicsItem)override; // from GraphicsItemAttributesWidget

    void objectChanged(ChangingObject *object) override; // from ChangingObjectListener

private slots:
    void toggleVisibilityOfControlPolygon(bool);
    void changeDegree(int value);
    void toggleTornToEdges(bool state);
    void toggleTangentDrawn(bool state);
    void changedTangentValue(real value);
    void refineSpline();

private:
    GraphicsSpline *m_currentSpline;

    QCheckBox   *m_showControlPolygonCheckBox;

    QSpinBox    *m_degreeSpinBox;
    QLabel      *m_degreeLabel;
    int          m_degree;

    QCheckBox   *m_tornToEdgesCheckBox;
    QLabel      *m_tornToEdgesLabel;

    QCheckBox   *m_drawTangentCheckBox;

    RealValuedSlider *m_tangentValueSlider;

    QPushButton *m_refineButton;

    void updateAttributes();

};

#endif // GRAPHICS_SPLINE_ATTRIBUTES_WIDGET
