#ifndef SPLINE_ATTRIBUTES_WIDGET
#define SPLINE_ATTRIBUTES_WIDGET

#include "stable.h"

class SplineAttributesWidget : public QWidget {

Q_OBJECT

public:
    SplineAttributesWidget(QDataWidgetMapper *mapper, QWidget *parent = 0);
    ~SplineAttributesWidget();

private slots:
    void changed(int);

signals:
    void somethingChanged();

private:
    QDataWidgetMapper *m_mapper;
    QSpinBox    *m_degreeSpinBox;
    QLabel      *m_degreeLabel;
    QCheckBox   *m_tornToEdgesCheckBox;
    QLabel      *m_tornToEdgesLabel;

};

#endif // SPLINE_ATTRIBUTES_WIDGET
