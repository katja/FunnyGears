#include "GraphicsSplineAttributesWidget.h"

GraphicsSplineAttributesWidget::GraphicsSplineAttributesWidget(QWidget *parent) : GraphicsItemAttributesWidget(parent) {

    m_degreeLabel   = new QLabel(tr("Degree"));
    m_degreeSpinBox = new QSpinBox(this);
    m_degreeSpinBox->setMinimum(1);
    m_degreeLabel->setBuddy(m_degreeSpinBox);
    m_degree = m_degreeSpinBox->value();
    connect(m_degreeSpinBox, SIGNAL(valueChanged(int)), this, SLOT(changeDegree(int)));

    m_tornToEdgesLabel = new QLabel(tr("Is torn to edges"));
    m_tornToEdgesCheckBox = new QCheckBox(tr("Is torn to edges"), this);
    m_tornToEdgesLabel->setBuddy(m_tornToEdgesCheckBox);
    connect(m_tornToEdgesCheckBox, SIGNAL(toggled(bool)), this, SLOT(toggleTornToEdges(bool)));

    m_drawTangentCheckBox = new QCheckBox(tr("Draw tangent?"), this);
    connect(m_drawTangentCheckBox, SIGNAL(toggled(bool)), this, SLOT(toggleTangentDrawn(bool)));
    m_tangentValueSlider = new RealValuedSlider("Tangent at value...", this);
    connect(m_tangentValueSlider, SIGNAL(valueChanged(real)), this, SLOT(changedTangentValue(real)));

    m_refineButton = new QPushButton(tr("Add more control points"), this);
    connect(m_refineButton, SIGNAL(clicked()), this, SLOT(refineSpline()));

    QGridLayout *layout = new QGridLayout(this);
    layout->addWidget(m_degreeLabel,        0, 0, 1, 1);
    layout->addWidget(m_degreeSpinBox,      0, 1, 1, 1);
    layout->addWidget(m_tornToEdgesLabel,   1, 0, 1, 1);
    layout->addWidget(m_tornToEdgesCheckBox,1, 1, 1, 1);
    layout->addWidget(m_drawTangentCheckBox,2, 0, 3, 1);
    layout->addWidget(m_tangentValueSlider, 2, 1, 3, 1);
    layout->addWidget(m_refineButton,       5, 0, 1, 2);
    setLayout(layout);

}

GraphicsSplineAttributesWidget::~GraphicsSplineAttributesWidget() {
}

void GraphicsSplineAttributesWidget::setItem(QGraphicsItem *graphicsItem) {
    m_currentSpline = static_cast<GraphicsSpline*>(graphicsItem);
    updateAttributes();
}

bool GraphicsSplineAttributesWidget::worksOnItem(QGraphicsItem *graphicsItem) {
    if(GraphicsSpline::isGraphicsSplineItem(graphicsItem))
        return true;
    return false;
}

void GraphicsSplineAttributesWidget::updateAttributes() {
    const Spline *spline = m_currentSpline->spline();
    m_degreeSpinBox->setValue(spline->degree());
    m_tornToEdgesCheckBox->setChecked(spline->isTornToEdges());
    if(spline->isValid()) {
        m_drawTangentCheckBox->setEnabled(true);
        m_drawTangentCheckBox->setChecked(m_currentSpline->isTangentDrawn());
        m_tangentValueSlider->setEnabled(m_currentSpline->isTangentDrawn());
        m_tangentValueSlider->setRange(spline->lowerDomainLimit(), spline->upperDomainLimit() - 0.01f);
        m_tangentValueSlider->setValue(m_currentSpline->tangentValue());
        m_refineButton->setEnabled(true);
    } else {
        m_drawTangentCheckBox->setEnabled(false);
        m_tangentValueSlider->setEnabled(false);
        m_refineButton->setEnabled(false);
    }
}

void GraphicsSplineAttributesWidget::changeDegree(int value) {
    //Prevent an update of the other items, when degree is still the same!
    if(m_degree != value) {
        m_degree = value;
        m_currentSpline->changeDegree(m_degree);
        updateAttributes();
    }
}

void GraphicsSplineAttributesWidget::toggleTornToEdges(bool state) {
    m_currentSpline->changeTornToEdges(state);
    updateAttributes();
}

void GraphicsSplineAttributesWidget::toggleTangentDrawn(bool state) {
    m_currentSpline->setTangentDrawn(state);
    m_tangentValueSlider->setEnabled(state);
}

void GraphicsSplineAttributesWidget::changedTangentValue(real value) {
    m_currentSpline->setTangentValue(value);
}

void GraphicsSplineAttributesWidget::refineSpline() {
    m_currentSpline->refineSpline();
    updateAttributes();
}
