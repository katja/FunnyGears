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
    void toggleLiveUpdating(bool);
    void toggleDrivingGearSamplingVisibility(bool);
    void toggleDrivenGearSamplingVisibility(bool);
    void toggleNoneContactPointsVisibility(bool);
    void toggleSelectedGearPointsVisibility(bool);
    void toggleForbiddenAreaInDrivingGear(bool);
    void toggleForbiddenAreaInDrivenGear(bool);
    void togglePathOfPossibleContactVisibility(bool);
    void togglePathOfRealContactVisibility(bool);
    void togglePitchesVisibility(bool);
    void togglePitchCirclesVisibility(bool);
    void toggleRotationOfGears(bool);
    void changeRotationVelocity(real);
    void changeSamplingRate(int);
    void changeMaxDriftAngle(int);

private:
    GraphicsGearPair *m_currentGearPair;

    QCheckBox   *m_liveUpdatingCheckBox,
                *m_drivingGearSamplingCheckBox,
                *m_drivenGearSamplingCheckBox,
                *m_noneContactPointsCheckBox,
                *m_selectedGearPointsCheckBox,
                *m_drivingGearForbiddenAreaCheckBox,
                *m_drivenGearForbiddenAreaCheckBox,
                *m_pathOfPossibleContactCheckBox,
                *m_pathOfRealContactCheckBox,
                *m_pitchesCheckBox,
                *m_pitchCirclesCheckBox,
                *m_rotationCheckBox;

    RealValuedSlider *m_rotationVelocitySlider;

    QSpinBox *m_samplingRateSpinBox,
             *m_maxDriftAngleSpinBox;

    bool m_isLiveUpdating;
    int m_samplingRate;
    int m_maxDriftAngle; //in degree

};

#endif // GRAPHICS_GEAR_PAIR_ATTRIBUTES_WIDGET
