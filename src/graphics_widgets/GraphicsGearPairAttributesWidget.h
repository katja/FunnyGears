#ifndef GRAPHICS_GEAR_PAIR_ATTRIBUTES_WIDGET
#define GRAPHICS_GEAR_PAIR_ATTRIBUTES_WIDGET

#include "stable.h"
#include "definitions.h"
#include "graphics_widgets/GraphicsItemAttributesWidget.h"
#include "ChangingObjectListener.h"
#include "graphics_objects/GraphicsGearPair.h"
#include "RealValuedSlider.h"

class GraphicsGearPairAttributesWidget : public GraphicsItemAttributesWidget, public ChangingObjectListener {

Q_OBJECT

public:
    GraphicsGearPairAttributesWidget(QWidget *parent = 0);
    ~GraphicsGearPairAttributesWidget();

    void setItem(QGraphicsItem *graphicsItem) override; // from GraphicsItemAttributesWidget
    bool worksOnItem(QGraphicsItem *graphicsItem) override; // from GraphicsItemAttributesWidget

    void updateAttributes();

    void objectChanged(ChangingObject *object) override; // from ChangingObjectListener

private slots:
    void toggleSampledGearToothVisibility(bool);
    void toggleNoneContactPointsVisibility(bool);
    void toggleSamplingWidthVisibilityInDrivingGear(bool);
    void toggleForbiddenAreaInDrivenGear(bool);
    void toggleFilledForbiddenAreaVisibility(bool);
    void togglePathOfContactVisibility(bool);
    void togglePitchesVisibility(bool);
    void togglePitchCirclesVisibility(bool);
    void toggleRotationOfGears(bool);
    void changeRotationVelocity(real);
    void changeSamplingRate(int);
    void changeMaxDriftAngle(int);
    void toggleLiveUpdating(bool);

private:
    GraphicsGearPair *m_currentGearPair;

    QCheckBox   *m_sampledGearToothCheckBox,
                *m_noneContactPointsCheckBox,
                *m_drivingGearWidthSamplingCheckBox,
                *m_drivenGearForbiddenAreaCheckBox,
                *m_filledForbiddenAreaCheckBox,
                *m_pathOfContactCheckBox,
                *m_pitchesCheckBox,
                *m_pitchCirclesCheckBox,
                *m_rotationCheckBox,
                *m_liveUpdatingCheckBox;

    RealValuedSlider *m_rotationVelocitySlider;

    QSpinBox *m_samplingRateSpinBox,
             *m_maxDriftAngleSpinBox;

    bool m_isLiveUpdating;
    int m_samplingRate;
    int m_maxDriftAngle; //in degree

};

#endif // GRAPHICS_GEAR_PAIR_ATTRIBUTES_WIDGET
