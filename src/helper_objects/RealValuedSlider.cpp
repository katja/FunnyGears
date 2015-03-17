#include "helper_objects/RealValuedSlider.h"

RealValuedSlider::RealValuedSlider(const QString &title, QWidget *parent) : QGroupBox(title, parent) {

    m_precision = 3; //default

    m_minLabel = new QLabel(this);
    m_minLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    m_maxLabel = new QLabel(this);
    m_maxLabel->setAlignment(Qt::AlignTop | Qt::AlignRight);

    m_valueLabel = new QLabel(this);
    m_valueLabel->setAlignment(Qt::AlignTop | Qt::AlignCenter);

    m_slider = new QSlider(Qt::Horizontal, this);
    m_slider->setToolTip(title);
    m_slider->setTracking(false);
    m_slider->setFocusPolicy(Qt::StrongFocus); //reach it through clicking or tabbing

    //uses the default values for QSlider where possible
    m_slider->setMinimum(0);
    m_slider->setMaximum(99); //default is 99
    m_slider->setSingleStep(1);
    m_slider->setPageStep(10);

    m_minLabel->setBuddy(m_slider);
    m_maxLabel->setBuddy(m_slider);
    m_valueLabel->setBuddy(m_slider);

    QGridLayout *gridLayout = new QGridLayout(this);
    gridLayout->setContentsMargins(4, 0, 4, 4);
    gridLayout->setHorizontalSpacing(0);
    gridLayout->setVerticalSpacing(3);
    gridLayout->addWidget(m_slider,     0, 0, 1, 3);
    gridLayout->addWidget(m_minLabel,   1, 0, Qt::AlignTop);
    gridLayout->addWidget(m_valueLabel, 1, 1, Qt::AlignBottom);
    gridLayout->addWidget(m_maxLabel,   1, 2, Qt::AlignTop);

    connect(m_slider, SIGNAL(valueChanged(int)), this, SLOT(sliderValueChanged(int)));
}

real RealValuedSlider::value() const {
    return m_value;
}

int RealValuedSlider::precision() const {
    return m_precision;
}

void RealValuedSlider::setPrecision(int precision) {
    m_precision = precision;
    updateLabels();
}

void RealValuedSlider::setValue(real value) {
    m_value = value;
    m_intValue = toSlidersIntegerFromValue(m_value);
    m_slider->setValue(m_intValue);
    updateLabels();
    emit valueChanged(m_value);
}

void RealValuedSlider::setValues(real value, real min, real max) {
    m_minValue = min;
    m_maxValue = max;
    m_value = value;
    m_intValue = toSlidersIntegerFromValue(m_value);
    m_slider->setValue(m_intValue);
    updateLabels();
    emit valueChanged(m_value);
}

void RealValuedSlider::setRange(real min, real max) {
    m_minValue = min;
    m_maxValue = max;
    m_intValue = toSlidersIntegerFromValue(m_value);
    m_slider->setValue(m_intValue);
    updateLabels();
}

void RealValuedSlider::updateLabels() {
    QString str;
    m_minLabel->setText(str.setNum(m_minValue, 'g', m_precision));
    m_maxLabel->setText(str.setNum(m_maxValue, 'g', m_precision));
    m_valueLabel->setText(str.setNum(m_value, 'g', m_precision));
}

void RealValuedSlider::sliderValueChanged(int value) {
    //Only if saved int value is another one than the slider gives here,
    //the slider was moved from a user and therefore we have to update our values.
    if(m_intValue != value) {
       m_intValue = value;
       m_value = toValueFromSlidersInteger(value);
       QString str;
       m_valueLabel->setText(str.setNum(m_value, 'g', m_precision));
       emit valueChanged(m_value);
    }
}

real RealValuedSlider::toValueFromSlidersInteger(int sliderValue) const {
    int resolution = m_slider->maximum() - m_slider->minimum(); // normally resolution = 100
    return m_minValue + (m_maxValue - m_minValue) * sliderValue / resolution;
}

int RealValuedSlider::toSlidersIntegerFromValue(real value) const {
    int resolution = m_slider->maximum() - m_slider->minimum(); // normally resolution = 100
    return static_cast<int>((value - m_minValue) * resolution / (m_maxValue - m_minValue));
}
