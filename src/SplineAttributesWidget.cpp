#include "SplineAttributesWidget.h"
#include "SplineModel.h"

SplineAttributesWidget::SplineAttributesWidget(QDataWidgetMapper *mapper, QWidget *parent) : QWidget(parent), m_mapper(mapper) {

    m_degreeLabel   = new QLabel(tr("Degree"));
    m_degreeSpinBox = new QSpinBox(this);
    m_degreeSpinBox->setMinimum(1);
    m_degreeLabel->setBuddy(m_degreeSpinBox);
    connect(m_degreeSpinBox, SIGNAL(valueChanged(int)), this, SLOT(changed(int)));

    m_tornToEdgesLabel = new QLabel(tr("Is torn to edges"));
    m_tornToEdgesCheckBox = new QCheckBox(tr("Is torn to edges"), this);
    m_tornToEdgesLabel->setBuddy(m_tornToEdgesCheckBox);
    connect(m_tornToEdgesCheckBox, SIGNAL(stateChanged(int)), this, SLOT(changed(int)));

    m_drawTangentCheckBox = new QCheckBox(tr("Draw tangent?"), this);
    connect(m_drawTangentCheckBox, SIGNAL(stateChanged(int)), this, SLOT(changed(int)));
    // m_tangentValueSlider = new RealValuedSlider("Tangent at value...", this);
    // connect(m_tangentValueSlider, SIGNAL(valueChanged(real)), this, SLOT(changed(real)));

    m_tangentValueSlider = new QSlider(Qt::Horizontal, this);
    connect(m_tangentValueSlider, SIGNAL(valueChanged(int)), this, SLOT(changed(int)));

    QGridLayout *layout = new QGridLayout(this);
    layout->addWidget(m_degreeLabel,        0, 0, 1, 1);
    layout->addWidget(m_degreeSpinBox,      0, 1, 1, 1);
    layout->addWidget(m_tornToEdgesLabel,   1, 0, 1, 1);
    layout->addWidget(m_tornToEdgesCheckBox,1, 1, 1, 1);
    layout->addWidget(m_drawTangentCheckBox,2, 0, 3, 1);
    layout->addWidget(m_tangentValueSlider, 2, 1, 3, 1);
    setLayout(layout);

    m_mapper->addMapping(m_degreeSpinBox, SplineModel::DEGREE);
    m_mapper->addMapping(m_tornToEdgesCheckBox, SplineModel::TORN_TO_EDGES);
    m_mapper->addMapping(m_drawTangentCheckBox, SplineModel::TANGENT_DRAWN);
    m_mapper->addMapping(m_tangentValueSlider, SplineModel::TANGENT_VALUE, "value");
    m_mapper->addMapping(m_tangentValueSlider, SplineModel::MIN_VALUE, "minimum");
    m_mapper->addMapping(m_tangentValueSlider, SplineModel::MAX_VALUE, "maximum");
    m_mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);

}

SplineAttributesWidget::~SplineAttributesWidget() {
}

void SplineAttributesWidget::changed(int) {
    std::cout << "SplineAttributesWidget::changed(int) was called and mapper will now submit" << std::endl;
    m_mapper->submit();
}

// void SplineAttributesWidget::changed(real) {
//     m_mapper->submit();
// }
