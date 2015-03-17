#ifndef GEAR_PAIR_INFORMATION_WIDGET
#define GEAR_PAIR_INFORMATION_WIDGET

#include "stable.h"
#include "basic_objects/GearPair.h"
#include "basic_objects/GearPairInformation.h"

class GearPairInformationWidget : public QWidget, public ChangingObjectListener {

public:
    GearPairInformationWidget(GearPair *gearPair);
    virtual ~GearPairInformationWidget();

    void updateInformation();

    void objectChanged(ChangingObject *object) override; // from ChangingObjectListener

protected:
    void showEvent(QShowEvent *event) override; // from QWidget

private:
    GearPair            *m_gearPair;
    GearPairInformation *m_gearPairInfo;

    QGroupBox *m_attentionBox;
    QVBoxLayout *m_attentionLayout;
    QVBoxLayout *m_layout;

    bool m_showInvalidGearShape;
    bool m_showIncorrectGearOutline;

    QLabel  *m_invalidGearShape,
            *m_incorrectGearOutline,
            *m_module,
            *m_drivingGearNumberOfTeeth,
            *m_drivenGearNumberOfTeeth,
            *m_transmissionRatio,
            *m_basicRequireC,
            *m_basicRequireCC,
            *m_basicFirstRequireC,
            *m_basicFirstRequireCC,
            *m_basicSecondRequireC,
            *m_basicSecondRequireCC;

    void updateAttentionBoxVisibility(bool invalidGearShapeVisible, bool incorrectGearOutlineVisible);

};

#endif // GEAR_PAIR_INFORMATION_WIDGET
