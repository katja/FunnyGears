#include "SplineAttributesWidget.h"
#include "SplineModel.h"

SplineAttributesWidget::SplineAttributesWidget(QDataWidgetMapper *mapper, QWidget *parent) : QWidget(parent), m_mapper(mapper) {

    m_degreeLabel   = new QLabel(tr("Degree"));
    m_degreeSpinBox = new QSpinBox(this);
    m_degreeLabel->setBuddy(m_degreeSpinBox);
    connect(m_degreeSpinBox, SIGNAL(valueChanged(int)), this, SLOT(changed(int)));

    m_tornToEdgesLabel = new QLabel(tr("Is torn to Edges"));
    m_tornToEdgesCheckBox = new QCheckBox(tr("Is torn to Edges"), this);
    m_tornToEdgesLabel->setBuddy(m_tornToEdgesCheckBox);
    connect(m_tornToEdgesCheckBox, SIGNAL(stateChanged(int)), this, SLOT(changed(int)));

    QGridLayout *layout = new QGridLayout(this);
    layout->addWidget(m_degreeLabel, 0, 0, 1, 1);
    layout->addWidget(m_degreeSpinBox, 0, 1, 1, 1);
    layout->addWidget(m_tornToEdgesLabel, 1, 0, 1, 1);
    layout->addWidget(m_tornToEdgesCheckBox, 1, 1, 1, 1);
    setLayout(layout);

    m_mapper->addMapping(m_degreeSpinBox, SplineModel::DEGREE);
    m_mapper->addMapping(m_tornToEdgesCheckBox, SplineModel::TORN_TO_EDGES);
    m_mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);

}

SplineAttributesWidget::~SplineAttributesWidget() {
}

void SplineAttributesWidget::changed(int) {
    emit somethingChanged();
    m_mapper->submit();
}
