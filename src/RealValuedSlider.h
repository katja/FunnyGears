#ifndef REAL_VALUED_SLIDER
#define REAL_VALUED_SLIDER

#include "stable.h"
#include "definitions.h"

class RealValuedSlider : public QGroupBox {

Q_OBJECT

public:
    RealValuedSlider(const QString &title, QWidget *parent = 0);


    real value() const; //current value

public slots:
    void setRange(real min, real max);
    void setValue(real value);
    void setValues(real value, real min, real max);

signals:
    void valueChanged(real value);

protected:
    QSlider *m_slider;
    QLabel  *m_minLabel,
            *m_maxLabel,
            *m_valueLabel;
    real    m_minValue,
            m_maxValue,
            m_value;
    int     m_intValue;

    void updateLabels();

private:
    int toSlidersIntegerFromValue(real value) const;
    real toValueFromSlidersInteger(int value) const;

private slots:
    void sliderValueChanged(int value);

};

#endif // REAL_VALUED_SLIDER
