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

    void setVisibilityOfSampledGearTooth(bool visible);
    bool visibilityOfSampledGearTooth() const;

    void setVisibilityOfNoneContactPoints(bool visible);
    bool visibilityOfNoneContactPoints() const;

    void setVisibilityOfSamplingWidthInDrivingGear(bool visible);
    bool visibilityOfSamplingWidthInDrivingGear() const;

    void setVisibilityOfForbiddenArea(bool visible);
    bool visibilityOfForbiddenArea() const;

    void setVisibilityOfFilledForbiddenArea(bool visible);
    bool visibilityOfFilledForbiddenArea() const;

    void setVisibilityOfPathOfContact(bool visible);
    bool visibilityOfPathOfContact() const;

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

    QRectF boundingRect() const override; // from QGraphicsItem
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override; // from QGraphicsItem
    void paintAdditionals(QPainter *painter, GraphicsMatingSplineGear *gear) const;
    void paintSampledGearTooth(QPainter *painter) const;
    void paintSampledGearToothSamplingPoints(QPainter *painter) const;
    void paintSampledContactPoints(QPainter *painter, bool, bool, bool) const;
    void paintContactPoint(const ContactPoint &point, QPainter *painter, QColor color, bool, bool, bool) const;

    void paintSampledContactPointsDrivingGear(QPainter *painter) const;
    void paintSampledContactPointsDrivenGear(QPainter *painter) const;
    void paintPathOfContact(QPainter *painter) const;
    void paintNoneContactPoints(QPainter *painter, bool, bool) const;
    void paintFilledForbiddenArea(QPainter *painter) const;
    void paintGearPoints(QPainter *painter) const;

private:
    GearPair *m_gearPair;
    GraphicsMatingSplineGear *m_drivingGear;
    GraphicsMatingSplineGear *m_drivenGear;

    bool m_sampledGearToothIsVisible;
    bool m_sampledGearToothSamplingIsVisible;
    bool m_noneContactPointsAreVisible;
    bool m_samplingWidthInDrivingGearIsVisible;
    bool m_forbiddenAreaIsVisible;
    bool m_filledForbiddenAreaIsVisible;
    bool m_pathOfContactIsVisible;
    bool m_pitchesAreVisible;
    bool m_pitchCirclesAreVisible;
    bool m_isRotating;
    real m_rotationVelocity;
    real m_rotationDegree;

};
#endif // GRAPHICS_GEAR_PAIR
