#include "GraphicsGearPair.h"
#include "preferences.h"

#include "basic_objects/ContactPoint.h"

const int GraphicsGearPair::Type = GraphicsItem::UserType + Type::GraphicsGearPairType;

bool GraphicsGearPair::isGraphicsGearPair(QGraphicsItem *item) {
    if(item && item->type() == Type)
        return true;
    return false;
}

GraphicsGearPair::GraphicsGearPair(GearPair *gearPair) :
    m_gearPair(gearPair),
    m_drivingGear(nullptr),
    m_drivenGear(nullptr),
    m_sampledGearToothIsVisible(false),
    m_sampledGearToothSamplingIsVisible(false),
    m_noneContactPointsAreVisible(false),
    m_samplingWidthInDrivingGearIsVisible(false),
    m_forbiddenAreaIsVisible(false),
    m_filledForbiddenAreaIsVisible(false),
    m_pathOfContactIsVisible(false),
    m_pitchesAreVisible(false),
    m_pitchCirclesAreVisible(false),
    m_isRotating(false),
    m_rotationVelocity(0.0),
    m_rotationDegree(0.0)
{
    m_drivingGear = new GraphicsMatingSplineGear(m_gearPair->drivingGear(), this); // driving gear inserted as child
    m_drivenGear = new GraphicsMatingSplineGear(m_gearPair->drivenGear(), this); // driven gear inserted as child
    m_drivenGear->setTransform(QTransform().translate(gearPair->getDistanceOfCenters(), 0));
}

GraphicsGearPair::~GraphicsGearPair() {
}

int GraphicsGearPair::type() const {
    return Type;
}


QString GraphicsGearPair::defaultName() const {
    return "Gear Pair";
}
void GraphicsGearPair::receivedClickOn(QPointF scenePos) {
    Q_UNUSED(scenePos);
}

void GraphicsGearPair::setVisibilityOfSampledGearTooth(bool visible) {
    prepareGeometryChange();
    m_sampledGearToothIsVisible = visible;
}

bool GraphicsGearPair::visibilityOfSampledGearTooth() const {
    return m_sampledGearToothIsVisible;
}

void GraphicsGearPair::setVisibilityOfNoneContactPoints(bool visible) {
    prepareGeometryChange();
    m_noneContactPointsAreVisible = visible;
}

bool GraphicsGearPair::visibilityOfNoneContactPoints() const {
    return m_noneContactPointsAreVisible;
}

void GraphicsGearPair::setVisibilityOfSamplingWidthInDrivingGear(bool visible) {
    prepareGeometryChange();
    m_samplingWidthInDrivingGearIsVisible = visible;
}

bool GraphicsGearPair::visibilityOfSamplingWidthInDrivingGear() const {
    return m_samplingWidthInDrivingGearIsVisible;
}

void GraphicsGearPair::setVisibilityOfForbiddenArea(bool visible) {
    prepareGeometryChange();
    m_forbiddenAreaIsVisible = visible;
}

bool GraphicsGearPair::visibilityOfForbiddenArea() const {
    return m_forbiddenAreaIsVisible;
}

void GraphicsGearPair::setVisibilityOfFilledForbiddenArea(bool visible) {
    prepareGeometryChange();
    m_filledForbiddenAreaIsVisible = visible;
}

bool GraphicsGearPair::visibilityOfFilledForbiddenArea() const {
    return m_filledForbiddenAreaIsVisible;
}

void GraphicsGearPair::setVisibilityOfPathOfContact(bool visible) {
    prepareGeometryChange();
    m_pathOfContactIsVisible = visible;
}

bool GraphicsGearPair::visibilityOfPathOfContact() const {
    return m_pathOfContactIsVisible;
}

void GraphicsGearPair::setVisibilityOfPitches(bool visible) {
    prepareGeometryChange();
    m_pitchesAreVisible = visible;
}

bool GraphicsGearPair::visibilityOfPitches() const {
    return m_pitchesAreVisible;
}

void GraphicsGearPair::setVisibilityOfPitchCircles(bool visible) {
    prepareGeometryChange();
    m_pitchCirclesAreVisible = visible;
}

bool GraphicsGearPair::visibilityOfPitchCircles() const {
    return m_pitchCirclesAreVisible;
}

void GraphicsGearPair::setRotating(bool isRotating) {
    prepareGeometryChange();
    m_isRotating = isRotating;
}

bool GraphicsGearPair::isRotating() const {
    return m_isRotating;
}

void GraphicsGearPair::setRotationVelocity(real velocity) {
    prepareGeometryChange();
    m_rotationVelocity = velocity;
}

real GraphicsGearPair::rotationVelocity() const {
    return m_rotationVelocity;
}

void GraphicsGearPair::setSamplingRate(int samplingRate) {
    if(samplingRate <= 0)
        return;
    prepareGeometryChange();
    m_gearPair->setSamplingRate((uint)samplingRate);
    m_gearPair->doCalculation();
}

int GraphicsGearPair::samplingRate() const {
    return (int)(m_gearPair->samplingRate());
}

void GraphicsGearPair::setMaxDriftAngleInDegree(int maxDriftAngle) {
    if(maxDriftAngle <= 0)
        return;
    prepareGeometryChange();
    m_gearPair->setMaxDriftAngleInDegree(maxDriftAngle);
    m_gearPair->doCalculation();
}

int GraphicsGearPair::maxDriftAngleInDegree() const {
    return static_cast<int>(m_gearPair->maxDriftAngleInDegree());
}

QRectF GraphicsGearPair::boundingRect() const {
    return QRectF(QPointF(-300, -500), QPointF(1000, 500));
    // std::list<ContactPoint> *points = m_gearPair->foundPoints();
    // vec2 topLeft = points->front().point;
    // vec2 bottomRight = topLeft;

    // for(ContactPoint point : (*points)) {
    //     if(topLeft.x > point.point.x)
    //         topLeft = vec2(point.point.x, topLeft.y);
    //     if(topLeft.y > point.point.y)
    //         topLeft = vec2(topLeft.x, point.point.y);
    //     if(bottomRight.x < point.point.x)
    //         bottomRight = vec2(point.point.x, bottomRight.y);
    //     if(bottomRight.y < point.point.y)
    //         bottomRight = vec2(bottomRight.x, point.point.y);
    // }
    // return QRectF(QPointF(topLeft.x - 20.0, topLeft.y - 20.0), QPointF(bottomRight.x + 20.0, bottomRight.y + 20.0)); //20.0 is the normal Length at the moment
}

void GraphicsGearPair::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if(m_isRotating) {
        m_rotationDegree += m_rotationVelocity;
        m_drivingGear->setRotation(m_rotationDegree);
        m_drivenGear->setRotation(-m_rotationDegree * m_gearPair->transmissionRatio());
    }

    if(m_pathOfContactIsVisible)
        paintSampledContactPoints(painter, false, false, true);

    // painter->drawRect(boundingRect());
}

void GraphicsGearPair::paintAdditionals(QPainter *painter, GraphicsMatingSplineGear *gear) const {
    if(gear == m_drivingGear) {
        if(m_pitchesAreVisible)
            painter->drawPath(pitchesPath(m_drivingGear->numberOfTeeth(), m_gearPair->startPoint().originPoint, 250));
        if(m_pitchCirclesAreVisible)
            painter->drawPath(circlePath(m_gearPair->drivingGearPitchRadius()));
        if(m_sampledGearToothIsVisible)
            paintSampledGearTooth(painter);
        if(m_noneContactPointsAreVisible)
            paintNoneContactPoints(painter, true, false);
        paintSampledContactPoints(painter, true, false, false);
        if(m_sampledGearToothSamplingIsVisible)
            paintSampledGearToothSamplingPoints(painter);

    } else if (gear == m_drivenGear) {
        if(m_pitchesAreVisible)
            painter->drawPath(pitchesPath(m_drivenGear->numberOfTeeth(), m_gearPair->startPoint().point, 250));
        if(m_pitchCirclesAreVisible)
            painter->drawPath(circlePath(m_gearPair->drivenGearPitchRadius()));
        if(m_noneContactPointsAreVisible)
            paintNoneContactPoints(painter, false, true);
        paintSampledContactPoints(painter, false, true, false);
        if(m_filledForbiddenAreaIsVisible)
            paintFilledForbiddenArea(painter);

    } // other things should not happen ^^
}

void GraphicsGearPair::paintSampledGearTooth(QPainter *painter) const {
    Spline *s = m_gearPair->completeToothProfile();
    uint samples = s->numberOfControlPoints() * 2 + 400;
    vector<QPointF> curve(samples);
    s->curve(curve);
    QPainterPath path;
    path.addPolygon(QPolygonF(convertToQVector(curve)));
    painter->drawPath(path);
}

void GraphicsGearPair::paintSampledGearToothSamplingPoints(QPainter *painter) const {
    std::list<ContactPoint> *points = m_gearPair->foundPoints();
    for(ContactPoint point : (*points)) {
        painter->drawEllipse(QPointF(point.originPoint.x, point.originPoint.y), 1.2, 1.2);
    }
}

void GraphicsGearPair::paintSampledContactPoints(QPainter *painter, bool paintOriginPoints, bool paintTargetPoints, bool paintPathOfContact) const {
    std::list< PositionList* > *positionLists = m_gearPair->pointsInSortedLists();

    uint r = 10;
    uint g = 0;
    uint b = 50;
    uint red = r;
    uint green = g;
    uint blue = b;

    for(PositionList *list : (*positionLists)) {
        ContactPoint lastCP = list->list.front();
        for(ContactPoint cp : list->list) {
            if(list->position != 0) {
                red = green = blue = 0;
                // QColor fillColor = QColor(128 + list->position * 10, 0, 0);
                // painter->setBrush(QBrush(fillColor));
            }
            paintContactPoint(cp, painter, QColor(red, green, blue), paintOriginPoints, paintTargetPoints, paintPathOfContact);

            b = (b + 8) & 0x1FF;
            if((b & 0x100) > 0)//count downwards
                blue = 0x100 - ((b & 0xFF) + 1);
            else
                blue = b;

            if((b & 0x180) > 0) {
                r = (r + 8) & 0x1FF;
                if((r & 0x100) > 0)//count downwards
                    red = 0x100 - ((r & 0xFF) + 1);
                else
                    red = r;
            }

            if((r & 0x180) > 0) {
                g = (g + 8) & 0x1FF;
                if((g & 0x100) > 0)//count downwards
                    green = 0x100 - ((g & 0xFF) + 1);
                else
                    green = g;
            }
            if(paintTargetPoints && lastCP.isRotated == cp.isRotated) { //both elements are in normal range, or both somewhere else
                painter->drawLine(lastCP.point.x, lastCP.point.y, cp.point.x, cp.point.y);
                lastCP = cp;
            }
        }
    }
}

void GraphicsGearPair::paintContactPoint(ContactPoint point, QPainter *painter, QColor color, bool paintOriginPoints, bool paintTargetPoints, bool paintPathOfContact) const {
    painter->save();
    painter->setPen(color);
    if(point.error != ErrorCode::NO_ERROR) {
        painter->setPen(QColor(220, 0, 20)); //red
    }

    //point, normal, originPoint, originNormal
    if(paintTargetPoints) {
        painter->drawEllipse(QPointF(point.point.x, point.point.y), Preferences::PointRadius, Preferences::PointRadius);
        if(m_forbiddenAreaIsVisible) {
            vec2 endNormal = point.point + point.normal * point.forbiddenAreaLength;
            painter->drawLine(point.point.x, point.point.y, endNormal.x, endNormal.y);
        }
    }

    if(paintOriginPoints) {
        painter->drawEllipse(QPointF(point.originPoint.x, point.originPoint.y), Preferences::PointRadius, Preferences::PointRadius);
        if(m_samplingWidthInDrivingGearIsVisible) {
            vec2 endNormal = point.originPoint - point.originNormal * point.forbiddenAreaLength;
            painter->drawLine(point.originPoint.x, point.originPoint.y, endNormal.x, endNormal.y);
        }
    }

    if(paintPathOfContact) {
        painter->setBrush(QBrush(color));
        painter->drawEllipse(QPointF(point.contactPosition.x, point.contactPosition.y), Preferences::PointRadius, Preferences::PointRadius);
        vec2 endNormal = point.contactPosition + point.normalInContact * 20.0;
        painter->drawLine(point.contactPosition.x, point.contactPosition.y, endNormal.x, endNormal.y);
    }

    painter->restore();
}

void GraphicsGearPair::paintNoneContactPoints(QPainter *painter, bool paintOriginPoints, bool paintTargetPoints) const {
    std::vector<NoneContactPoint*> ncps = m_gearPair->noneContactPoints();

    painter->save();
    QColor orange = QColor(210, 180, 0);
    painter->setPen(orange);

    for(NoneContactPoint *ncp : ncps) {
        painter->save();
        if(ncp->error == ErrorCode::NO_THICKNESS) {
            painter->setPen(Preferences::AttentionColor);
        }

        //OriginPoint
        if(paintOriginPoints) {
            painter->drawEllipse(QPointF(ncp->originPoint.x, ncp->originPoint.y), Preferences::PointRadius, Preferences::PointRadius);
            if(m_samplingWidthInDrivingGearIsVisible) {
                vec2 endNormal = ncp->originPoint - ncp->originNormal * ncp->forbiddenAreaLength;
                painter->drawLine(ncp->originPoint.x, ncp->originPoint.y, endNormal.x, endNormal.y);
            }
        }

        //Points (and normals) of NoneContactPoint
        if(paintTargetPoints) {
            vec2 previousPoint;
            for(uint i = 0; i < ncp->points.size(); ++i) {
                vec2 p = ncp->points[i];
                vec2 n = ncp->normals[i];

                //connect the points
                if(i > 0)
                    painter->drawLine(previousPoint.x, previousPoint.y, p.x, p.y);

                painter->drawEllipse(QPointF(p.x, p.y), Preferences::PointRadius, Preferences::PointRadius);

                if(m_forbiddenAreaIsVisible) {
                    vec2 endNormal = p + n * ncp->forbiddenAreaLength;
                    painter->drawLine(p.x, p.y, endNormal.x, endNormal.y);
                }

                previousPoint = p;
            }
        }
        painter->restore();
    }
    painter->restore();
}

void GraphicsGearPair::paintFilledForbiddenArea(QPainter *painter) const {
    std::list<Triangle> triangleList = m_gearPair->triangles();

    painter->save();
    painter->setPen(QPen(QBrush(QColor(80, 80, 120, 150)), 0));
    for(Triangle t : triangleList) {
        QPointF triangle[3] = {QPointF(t.a.x, t.a.y), QPointF(t.b.x, t.b.y), QPointF(t.c.x, t.c.y)};
        painter->drawConvexPolygon(triangle, 3);
        painter->save();
        if(t.pointIsInTriangle) {
            painter->setPen(QColor(220, 0, 20)); //red
            painter->drawEllipse(QPointF(t.point.x, t.point.y), 1, 1);
        } else {
            painter->drawEllipse(QPointF(t.point.x, t.point.y), 3, 3);
        }
        painter->restore();
    }
    painter->restore();
}
