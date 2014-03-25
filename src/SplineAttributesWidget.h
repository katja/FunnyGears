#ifndef SPLINE_ATTRIBUTES_WIDGET
#define SPLINE_ATTRIBUTES_WIDGET

#include "stable.h"
#include "definitions.h"
#include "GraphicsItemAttributesWidget.h"
#include "GraphicsSpline.h"
#include "RealValuedSlider.h"

class SplineAttributesWidget : public GraphicsItemAttributesWidget {

Q_OBJECT

public:
    SplineAttributesWidget(QWidget *parent = 0);
    ~SplineAttributesWidget();

    void setItem(QGraphicsItem *graphicsItem);

private slots:
    void changeDegree(int value);
    void toggleTornToEdges(bool state);
    void toggleTangentDrawn(bool state);
    void changedTangentValue(real value);
    void refineSpline();

private:
    GraphicsSpline *m_currentSpline;

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

#endif // SPLINE_ATTRIBUTES_WIDGET
