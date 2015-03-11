#ifndef GRAPHICS_GEAR_PAIR_ATTRIBUTES_WIDGET
#define GRAPHICS_GEAR_PAIR_ATTRIBUTES_WIDGET

#include "stable.h"
#include "definitions.h"
#include "graphics_widgets/GraphicsItemAttributesWidget.h"
#include "ChangingObjectListener.h"
#include "graphics_objects/GraphicsGearPair.h"
#include "GearPairInformationWidget.h"
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
    void changeNumberOfTeethOfDrivenGear(int);
    void toggleDrivingGearEnabled(bool);
    void toggleUseBottomClearance(bool);
    void changeBottomClearance(int);
    void changeBottomClearanceStartAngle(int);
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
    void showGearPairInformation();
    void changeSamplingRate(int);
    void changeMaxDriftAngle(int);
    void togglePencilWidth(bool);

private:
    GraphicsGearPair *m_currentGearPair;

    QCheckBox   *m_drivingGearEnabledCheckBox,
                *m_useBottomClearanceCheckBox,
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
                *m_rotationCheckBox,
                *m_smallPencilWidthCheckBox;

    RealValuedSlider *m_rotationVelocitySlider;

    QSpinBox *m_toothCountDrivenGearSpinBox,
             *m_bottomClearanceSpinBox,
             *m_bottomClearanceStartAngleSpinBox,
             *m_samplingRateSpinBox,
             *m_maxDriftAngleSpinBox;

    QLabel *m_moduleLabel;

    GearPairInformationWidget *m_gearPairInformationWidget;

    int m_bottomClearance;
    int m_bottomClearanceStartAngle;
    int m_samplingRate;
    int m_maxDriftAngle; //in degree
};

#endif // GRAPHICS_GEAR_PAIR_ATTRIBUTES_WIDGET
