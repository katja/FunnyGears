#ifndef GRAPHICS_GEAR_PAIR
#define GRAPHICS_GEAR_PAIR

#include "stable.h"
#include "definitions.h"
#include "graphics_objects/GraphicsScheduleItem.h"
#include "graphics_objects/GraphicsItem.h"
#include "graphics_objects/GraphicsMatingSplineGear.h"
#include "basic_objects/GearPair.h"
#include "basic_objects/ContactPoint.h"
#include "ChangingObjectListener.h"

class GraphicsGearPair : public GraphicsScheduleItem, public ChangingObjectListener {

public:
    static const int Type; // = GraphicsItem::UserType + Type::GraphicsGearPairType;
    static bool isGraphicsGearPair(QGraphicsItem *item);

    GraphicsGearPair(GearPair *gearPair);
    virtual ~GraphicsGearPair();

    int type() const override; // from QGraphicsItem
    QString defaultName() const override; // GraphicsItem

    void receivedClickOn(QPointF scenePos) override; //from GraphicsItem, does nothing here

    void objectChanged(ChangingObject *object) override; // from ChangingObjectListener

    void update();

    void setDrivingGearEnabled(bool enabled);
    bool isDrivingGearEnabled();

    void setVisibilityOfDrivingGearSampling(bool visible);
    bool visibilityOfDrivingGearSampling() const;

    void setVisibilityOfDrivenGearSampling(bool visible);
    bool visibilityOfDrivenGearSampling() const;

    void setVisibilityOfForbiddenAreaInDrivingGear(bool visible);
    bool visibilityOfForbiddenAreaInDrivingGear() const;

    void setVisibilityOfForbiddenAreaInDrivenGear(bool visible);
    bool visibilityOfForbiddenAreaInDrivenGear() const;

    void setVisibilityOfNoneContactPoints(bool visible);
    bool visibilityOfNoneContactPoints() const;

    void setVisibilityOfSelectedGearPoints(bool visible);
    bool visibilityOfSelectedGearPoints() const;

    void setVisibilityOfPathOfPossibleContact(bool visible);
    bool visibilityOfPathOfPossibleContact() const;

    void setVisibilityOfPathOfRealContact(bool visible);
    bool visibilityOfPathOfRealContact() const;

    void setVisibilityOfPitches(bool visible);
    bool visibilityOfPitches() const;

    void setVisibilityOfPitchCircles(bool visible);
    bool visibilityOfPitchCircles() const;

    void setRotating(bool isRotating);
    bool isRotating() const;

    void setRotationVelocity(real velocity);
    real rotationVelocity() const;

    void setSamplingRate(int samplingRate);
    int samplingRate() const;

    void setMaxDriftAngleInDegree(int maxDriftAngle);
    int maxDriftAngleInDegree() const;

    void setFinePencil(bool isFinePencil);
    bool finePencilUsed() const;

    QRectF boundingRect() const override; // from QGraphicsItem
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override; // from QGraphicsItem
    void paintAdditionals(QPainter *painter, GraphicsMatingSplineGear *gear) const;
    void paintSampledContactPointsDrivingGear(QPainter *painter) const;
    void paintSampledContactPointsDrivenGear(QPainter *painter) const;
    void paintPathOfContact(QPainter *painter) const;
    void paintNoneContactPoints(QPainter *painter, bool, bool) const;
    void paintSelectedGearPoints(QPainter *painter) const;

private:
    GearPair *m_gearPair;
    GraphicsMatingSplineGear *m_drivingGear;
    GraphicsMatingSplineGear *m_drivenGear;

    bool m_drivingGearSamplingIsVisible;
    bool m_drivenGearSamplingIsVisible;
    bool m_forbiddenAreaInDrivingGearIsVisible;
    bool m_forbiddenAreaInDrivenGearIsVisible;
    bool m_noneContactPointsAreVisible;
    bool m_selectionOfGearPointsIsVisible;
    bool m_pathOfPossibleContactIsVisible;
    bool m_pathOfRealContactIsVisible;
    bool m_pitchesAreVisible;
    bool m_pitchCirclesAreVisible;
    bool m_isRotating;
    real m_rotationVelocity;
    real m_rotationDegree;
    bool m_finePencilUsed;

};
#endif // GRAPHICS_GEAR_PAIR
