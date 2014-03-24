#ifndef SPLINE_ATTRIBUTES_WIDGET
#define SPLINE_ATTRIBUTES_WIDGET

#include "stable.h"
#include "definitions.h"
// #include "RealValuedSlider.h"

class SplineAttributesWidget : public QWidget {

Q_OBJECT

public:
    SplineAttributesWidget(QDataWidgetMapper *mapper, QWidget *parent = 0);
    ~SplineAttributesWidget();

private slots:
    void changed(int);
    // void changed(real);

signals:
    void somethingChanged();

private:
    QDataWidgetMapper *m_mapper;
    QSpinBox    *m_degreeSpinBox;
    QLabel      *m_degreeLabel;
    QCheckBox   *m_tornToEdgesCheckBox;
    QLabel      *m_tornToEdgesLabel;
    QCheckBox   *m_drawTangentCheckBox;
    QSlider     *m_tangentValueSlider;

};

#endif // SPLINE_ATTRIBUTES_WIDGET
