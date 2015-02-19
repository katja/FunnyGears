#include "graphics_widgets/GraphicsSplineAttributesWidget.h"

GraphicsSplineAttributesWidget::GraphicsSplineAttributesWidget(QWidget *parent) : GraphicsItemAttributesWidget(parent) {

    m_showControlPolygonCheckBox = new QCheckBox(tr("Show Control Polygon"), this);
    connect(m_showControlPolygonCheckBox, SIGNAL(toggled(bool)), this, SLOT(toggleVisibilityOfControlPolygon(bool)));

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

    QGroupBox *splineWidget = new QGroupBox(tr("Spline Curve Attributes"), this);

    QGridLayout *splineLayout = new QGridLayout(splineWidget);
    splineLayout->addWidget(m_showControlPolygonCheckBox, 0, 0, 1, 2);
    splineLayout->addWidget(m_degreeLabel,        1, 0);
    splineLayout->addWidget(m_degreeSpinBox,      1, 1);
    splineLayout->addWidget(m_tornToEdgesLabel,   2, 0);
    splineLayout->addWidget(m_tornToEdgesCheckBox,2, 1);
    splineLayout->addWidget(m_drawTangentCheckBox,3, 0);
    splineLayout->addWidget(m_tangentValueSlider, 3, 1);
    splineLayout->addWidget(m_refineButton,       4, 0, 1, 2);
    splineLayout->setRowStretch(5, 1); // As row 5 is not filled with a widget, the layout fills with empty space, if it is larger than needed

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(4, 0, 4, 4); //use small margins at left, right and bottom
    layout->addWidget(splineWidget);
}

GraphicsSplineAttributesWidget::~GraphicsSplineAttributesWidget() {
}

void GraphicsSplineAttributesWidget::setItem(QGraphicsItem *graphicsItem) {
    m_currentSpline = static_cast<GraphicsSpline*>(graphicsItem);
    if(m_currentSpline) {
        updateAttributes();
        m_currentSpline->informAboutChange(this);
    }
    else {
        m_currentSpline = nullptr;
        std::cerr << "ERRRRORRRRR!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
    }
}

bool GraphicsSplineAttributesWidget::worksOnItem(QGraphicsItem *graphicsItem) {
    if(GraphicsSpline::isGraphicsSplineItem(graphicsItem))
        return true;
    return false;
}

void GraphicsSplineAttributesWidget::objectChanged(ChangingObject *object) {
    if(m_currentSpline == object)
        updateAttributes();
}

void GraphicsSplineAttributesWidget::updateAttributes() {
    const Spline *spline = m_currentSpline->spline();
    m_degreeSpinBox->setValue(spline->degree());
    m_tornToEdgesCheckBox->setChecked(spline->isTornToEdges());
    m_showControlPolygonCheckBox->setChecked(m_currentSpline->isControlPolygonVisible());
    if(spline->isValid()) {
        m_drawTangentCheckBox->setEnabled(true);
        m_drawTangentCheckBox->setChecked(m_currentSpline->isTangentDrawn());
        m_tangentValueSlider->setEnabled(m_currentSpline->isTangentDrawn());
        m_tangentValueSlider->setRange(spline->lowerDomainLimit(), spline->upperDomainLimit());
        m_tangentValueSlider->setValue(m_currentSpline->tangentValue());
        m_refineButton->setEnabled(true);
    } else {
        m_drawTangentCheckBox->setEnabled(false);
        m_tangentValueSlider->setEnabled(false);
        m_refineButton->setEnabled(false);
    }
}

void GraphicsSplineAttributesWidget::toggleVisibilityOfControlPolygon(bool isVisible) {
    m_currentSpline->setVisibleControlPolygon(isVisible);
    updateAttributes();
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
