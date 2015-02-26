#include "GraphicsGearPair.h"
#include "preferences.h"

#include "basic_objects/ContactPoint.h"

const int GraphicsGearPair::Type = GraphicsItem::UserType + Type::GraphicsGearPairType;

bool GraphicsGearPair::isGraphicsGearPair(QGraphicsItem *item) {
    if(item && item->type() == Type)
        return true;
    return false;
}

struct Coloring {
    uint r, g, b;
    uint red, green, blue;

    Coloring() {
        setBack();
    }

    void setBack() {
        r = 10;
        g = 0;
        b = 50;
        red = r;
        green = g;
        blue = b;
    }

    QColor nextColor() {
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
        return QColor(red, green, blue);
    }
};

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
    std::cout << "GraphicsGearPair is created" << std::endl;
    m_drivingGear = new GraphicsMatingSplineGear(m_gearPair->drivingGear(), this); // driving gear inserted as child
    m_drivenGear = new GraphicsMatingSplineGear(m_gearPair->drivenGear(), this); // driven gear inserted as child
    m_drivenGear->setTransform(QTransform().translate(gearPair->getDistanceOfCenters(), 0));

    m_drivingGear->informAboutChange(this);
}

GraphicsGearPair::~GraphicsGearPair() {
    delete m_drivingGear;
    delete m_drivenGear;
    std::cout << "GraphicsGearPair is deleted" << std::endl;
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

void GraphicsGearPair::objectChanged(ChangingObject *object) {
    if(m_drivingGear == object)
        GraphicsItem::changed();
}

void GraphicsGearPair::update() {
    prepareGeometryChange();
    m_gearPair->updateDrivingGearChange();
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

    if(m_sampledGearToothIsVisible)
            paintSampledGearTooth(painter);

    if(m_pathOfContactIsVisible)
        paintPathOfContact(painter);

    // painter->drawRect(boundingRect());
}

void GraphicsGearPair::paintAdditionals(QPainter *painter, GraphicsMatingSplineGear *gear) const {
    if(gear == m_drivingGear) {
        if(m_pitchCirclesAreVisible)
            painter->drawPath(circlePath(m_gearPair->drivingGearPitchRadius()));
        if(m_noneContactPointsAreVisible)
            paintNoneContactPoints(painter, true, false);
        paintSampledContactPointsDrivingGear(painter);
        if(m_sampledGearToothSamplingIsVisible)
            paintSampledGearToothSamplingPoints(painter);

    } else if (gear == m_drivenGear) {
        // QPen pen = painter->pen();
        // pen.setWidth(0);
        // painter->setPen(pen);
        if(m_pitchesAreVisible) {
            painter->drawPath(pitchesPath(m_drivenGear->numberOfTeeth(), m_gearPair->startOfExaminedPitch(), 250));

            vec2 start = m_gearPair->startOfExaminedPitch();
            vec2 end = m_gearPair->endOfExaminedPitch();
            painter->save();
            QPen pen = painter->pen();
            pen.setWidth(2);
            painter->setPen(pen);
            painter->drawLine(QPointF(0, 0), QPointF(start.x, start.y));
            pen.setWidth(1);
            painter->setPen(pen);
            painter->drawLine(QPointF(0, 0), QPointF(end.x, end.y));
            painter->restore();
        }
        if(m_pitchCirclesAreVisible)
            painter->drawPath(circlePath(m_gearPair->drivenGearPitchRadius()));
        if(m_noneContactPointsAreVisible)
            paintNoneContactPoints(painter, false, true);
        paintSampledContactPointsDrivenGear(painter);
        if(m_filledForbiddenAreaIsVisible)
            paintFilledForbiddenArea(painter);

        // paintGearPoints(painter);

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
    const std::list<ContactPoint*> points = m_gearPair->foundPoints();
    for(ContactPoint *cp : points) {
        painter->drawEllipse(QPointF(cp->originPoint.x, cp->originPoint.y), 1.2, 1.2);
    }
}

void GraphicsGearPair::paintSampledContactPointsDrivingGear(QPainter *painter) const {
    std::list<PointsWithPosition*> pointsWithPositions = m_gearPair->pointsInSortedLists();

    painter->save();
    Coloring coloring;

    PointsWithPosition *firstList = pointsWithPositions.front();
    coloring.setBack();

    for(ContactPoint *cp : firstList->points) {

        QColor c = coloring.nextColor();
        if(cp->error != ErrorCode::NO_ERROR) {
            c = QColor(220, 0, 20); //red
        }
        QPen pen = painter->pen();
        pen.setColor(c);
        painter->setPen(pen);

        // originPoint, originNormal
        drawCircle(painter, cp->originPoint);
        if(m_samplingWidthInDrivingGearIsVisible) {
            vec2 endNormal = cp->originPoint - cp->originNormal * cp->forbiddenAreaLength;
            drawLine(painter, cp->originPoint, endNormal);
        }
    }
    painter->restore();
}

void GraphicsGearPair::paintSampledContactPointsDrivenGear(QPainter *painter) const {
    std::list<PointsWithPosition*> pointsWithPositions = m_gearPair->pointsInSortedLists();

    painter->save();
    Coloring coloring;
    real lightUp = 1.0 / pointsWithPositions.size();

    for(PointsWithPosition *list : pointsWithPositions) {
        ContactPoint *lastCP = list->points.front();
        coloring.setBack();

        for(ContactPoint *cp : list->points) {

            QColor c = coloring.nextColor();
            if(list->position > 0)
                c = lightUpColor(c, list->position * lightUp);
            else if(list->position < 0)
                c = lightUpColor(c, -(list->position * lightUp));
            if(cp->error != ErrorCode::NO_ERROR) {
                c = QColor(220, 0, 20); //red
            }
            QPen pen = painter->pen();
            pen.setColor(c);
            painter->setPen(pen);

            // point (and forbidden area)
            drawCircle(painter, cp->point);
            // drawCircle(painter, cp->point, 0.15);
            if(m_forbiddenAreaIsVisible) {
                vec2 endNormal = cp->point + cp->normal * cp->forbiddenAreaLength;
                drawLine(painter, cp->point, endNormal);
            }
            // merge following list points by a line:
            if(lastCP != cp) { //not on first point on list
                drawLine(painter, lastCP->point, cp->point);
                if(m_forbiddenAreaIsVisible)
                    drawLine(painter, lastCP->forbiddenAreaEndPoint, cp->forbiddenAreaEndPoint);
                lastCP = cp;
            }
        }
    }
    painter->restore();
}

void GraphicsGearPair::paintPathOfContact(QPainter *painter) const {
    std::list<PointsWithPosition*> pointsWithPositions = m_gearPair->pointsInSortedLists();

    painter->save();
    Coloring coloring;

    PointsWithPosition *firstList = pointsWithPositions.front();
    coloring.setBack();

    for(ContactPoint *cp : firstList->points) {
        QColor c = coloring.nextColor();
        if(cp->error != ErrorCode::NO_ERROR) {
            c = QColor(220, 0, 20); //red
        }
        QPen pen = painter->pen();
        pen.setColor(c);
        pen.setBrush(c);
        painter->setPen(pen);
        drawCircle(painter, cp->contactPosition);
        vec2 endNormal = cp->contactPosition + cp->normalInContact * 20.0;
        drawLine(painter, cp->contactPosition, endNormal);
    }
    painter->restore();
}

void GraphicsGearPair::paintNoneContactPoints(QPainter *painter, bool paintOriginPoints, bool paintTargetPoints) const {
    std::list<NoneContactPoint*> ncps = m_gearPair->noneContactPoints();

    painter->save();
    QColor orange = QColor(210, 180, 0);
    QPen pen = painter->pen();
    pen.setColor(orange);
    painter->setPen(pen);

    for(NoneContactPoint *ncp : ncps) {
        painter->save();
        if(ncp->error == ErrorCode::NO_THICKNESS) {
            pen.setColor(Preferences::AttentionColor);
            painter->setPen(pen);
        }

        //OriginPoint
        if(paintOriginPoints) {
            painter->drawEllipse(QPointF(ncp->originPoint.x, ncp->originPoint.y), Preferences::PointRadius, Preferences::PointRadius);
            if(m_samplingWidthInDrivingGearIsVisible) {
                vec2 endNormal = ncp->originPoint - ncp->originNormal * ncp->forbiddenAreaLength;
                painter->drawLine(QPointF(ncp->originPoint.x, ncp->originPoint.y), QPointF(endNormal.x, endNormal.y));
            }
        }

        //Points (and normals) of NoneContactPoint
        if(paintTargetPoints) {
            vec2 previousPoint;
            vec2 previousEndPoint;
            for(uint i = 0; i < ncp->points.size(); ++i) {
                vec2 p = ncp->points[i];
                vec2 n = ncp->normals[i];

                //connect the points
                if(i > 0)
                    painter->drawLine(QPointF(previousPoint.x, previousPoint.y), QPointF(p.x, p.y));

                // painter->drawEllipse(QPointF(p.x, p.y), Preferences::PointRadius, Preferences::PointRadius);

                if(m_forbiddenAreaIsVisible) {
                    vec2 endPoint = p + n * ncp->forbiddenAreaLength;
                    if(i > 0)
                        painter->drawLine(QPointF(previousEndPoint.x, previousEndPoint.y), QPointF(endPoint.x, endPoint.y));
                    // painter->drawLine(QPointF(p.x, p.y), QPointF(endPoint.x, endPoint.y));
                    painter->drawEllipse(QPointF(endPoint.x, endPoint.y), 0.1, 0.1);
                    previousEndPoint = endPoint;
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

void GraphicsGearPair::paintGearPoints(QPainter *painter) const {
    vector<vec2> gearPoints = m_gearPair->gearPoints();

    painter->save();
    QPen pen = painter->pen();
    pen.setWidth(0);
    pen.setColor(QColor(220, 0, 20)); //red
    painter->setPen(pen);
    for(uint i = 0; i < gearPoints.size(); ++i) {
        painter->drawEllipse(QPointF(gearPoints[i].x, gearPoints[i].y), 2.2, 2.2);
        painter->drawEllipse(QPointF(gearPoints[i].x, gearPoints[i].y), 0.1, 0.1);
        if(i > 0)
            painter->drawLine(QPointF(gearPoints[i - 1].x, gearPoints[i - 1].y), QPointF(gearPoints[i].x, gearPoints[i].y));
    }
    painter->restore();
}
