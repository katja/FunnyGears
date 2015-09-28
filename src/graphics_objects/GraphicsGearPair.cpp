#include "preferences.h"
#include "basic_objects/ContactPoint.h"
#include "graphics_objects/GraphicsGearPair.h"

const int GraphicsGearPair::Type = GraphicsItem::UserType + Type::GraphicsGearPairType;

bool GraphicsGearPair::isGraphicsGearPairItem(QGraphicsItem *item) {
    if(item && item->type() == Type)
        return true;
    return false;
}

uint cutOutColorValue(uint c) {
    if(c < 0x100)
        return 0;
    else if(c >= 0x200)
        return 0xFF;
    else
        return c - 0x100;
}

QColor getColorFor(uint step) {
    uint x = (step << 3);
    uint r, g, b;
    b = (x + 0x300) % 0x600;
    g = (x + 0x100) % 0x600;
    r = (x + 0x500) % 0x600;
    if(b > 0x300)
        b = 0x600 - (b + 1);
    if(g > 0x300)
        g = 0x600 - (g + 1);
    if(r > 0x300)
        r = 0x600 - (r + 1);
    return QColor(cutOutColorValue(r), cutOutColorValue(g), cutOutColorValue(b));
}

GraphicsGearPair::GraphicsGearPair(GearPair *gearPair) :
    m_gearPair(gearPair),
    m_drivingGear(nullptr),
    m_drivenGear(nullptr),
    m_gearPairInformationWidget(new GearPairInformationWidget(gearPair)),
    m_isPitchPointVisible(false),
    m_isDrivingGearSamplingVisible(true),
    m_isDrivenGearSamplingVisible(false),
    m_isForbiddenAreaInDrivingGearVisible(false),
    m_isForbiddenAreaInDrivenGearVisible(false),
    m_areNoneContactPointsVisible(false),
    m_isSelectionOfGearPointsVisible(false),
    m_isPathOfPossibleContactVisible(false),
    m_isPathOfRealContactVisible(true),
    m_isPathOfBestContactVisible(false),
    m_arePitchesVisible(false),
    m_arePitchCirclesVisible(false),
    m_isRotating(false),
    m_rotationVelocity(0.0),
    m_rotationDegreeDrivingGear(0.0),
    m_rotationDegreeDrivenGear(0.0),
    m_finePencilUsed(false)
{
    std::cout << "GraphicsGearPair is created" << std::endl;
    m_drivingGear = new GraphicsMatingSplineGear(m_gearPair->drivingGear(), this); // driving gear inserted as child
    m_drivenGear = new GraphicsMatingSplineGear(m_gearPair->drivenGear(), this); // driven gear inserted as child

    m_drivingGear->setReferenceCircleVisibility(false);
    m_drivenGear->setReferenceCircleVisibility(false);
    m_drivingGear->setBasePitchVisibility(false);
    m_drivenGear->setBasePitchVisibility(false);
    m_drivingGear->setVisibleControlPolygon(false);
    m_drivenGear->setVisibleControlPolygon(false);

    m_drivingGear->informAboutChange(this);
    informAboutChange(m_gearPairInformationWidget);

    m_drivenGear->setTransform(QTransform().translate(m_gearPair->getDistanceOfCenters(), 0));
    updateBoundingRect();
}

GraphicsGearPair::~GraphicsGearPair() {
    //m_drivenGear and m_drivenGear are not deleted, as they are expected to be listed
    //in the model as correct GraphicsScheduleItems
    //=> they are deleted by the model
    //TODO: maybe better ask for if they still exist?
    //everywhere: delete... + ...=0
    //ask if(m_...) delete ...
    std::cout << "GraphicsGearPair is ..." << std::endl;
    m_gearPairInformationWidget->close();
    delete m_gearPairInformationWidget;
    delete m_gearPair;
    std::cout << "deleted" << std::endl;
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
    if(m_drivingGear == object) {
        prepareGeometryChange();
        m_gearPair->calculateAgainWithAllAttributes();
        changed(); //informs all ChangingObjectListeners, so f. ex. GraphicsGearPairAttributesWidget, which will call update() here
    }
}

void GraphicsGearPair::update() {
    prepareGeometryChange();
    m_rotationDegreeDrivingGear = 0.0;
    m_rotationDegreeDrivenGear = 0.0;
    m_drivingGear->setRotation(m_rotationDegreeDrivingGear);
    m_drivenGear->setRotation(m_rotationDegreeDrivenGear);
    m_drivenGear->setTransform(QTransform().translate(m_gearPair->getDistanceOfCenters(), 0));
    updateBoundingRect();
}

void GraphicsGearPair::showGearPairInformationWidget() {
    m_gearPairInformationWidget->activateWindow();
    m_gearPairInformationWidget->show();
    m_gearPairInformationWidget->raise();
}

real GraphicsGearPair::module() const {
    return m_gearPair->module();
}

void GraphicsGearPair::setNumberOfTeethOfDrivenGear(uint toothCount) {
    prepareGeometryChange();
    m_gearPair->setNumberOfTeethOfDrivenGear(toothCount);
    m_gearPair->calculateAgainWithAllAttributes();
    changed(); //informs all ChangingObjectListeners, so f. ex. GraphicsGearPairAttributesWidget, which will call update() here
}

uint GraphicsGearPair::numberOfTeethOfDrivenGear() const {
    return m_gearPair->numberOfTeethOfDrivenGear();
}

void GraphicsGearPair::setDrivingGearEnabled(bool enabled) {
    prepareGeometryChange();
    if(enabled)
        m_drivingGear->enableUserInteraction();
    else
        m_drivingGear->disableUserInteraction();
    m_drivingGear->setVisibleControlPolygon(enabled);
}

bool GraphicsGearPair::isDrivingGearEnabled() {
    return m_drivingGear->isUserInteractionEnabled();
}

void GraphicsGearPair::useBottomClearance(bool useIt) {
    prepareGeometryChange();
    m_gearPair->useBottomClearance(useIt);
}

bool GraphicsGearPair::isBottomClearanceUsed() const {
    return m_gearPair->isBottomClearanceUsed();
}

void GraphicsGearPair::setBottomClearance(int bottomClearance, int startAngle) {
    prepareGeometryChange();
    m_gearPair->setBottomClearance((real)bottomClearance, (real)startAngle);
}

int GraphicsGearPair::bottomClearance() const {
    return (int)m_gearPair->bottomClearance();
}

int GraphicsGearPair::bottomClearanceStartAngle() const {
    return (int)m_gearPair->bottomClearanceStartAngle();
}

void GraphicsGearPair::setVisibilityOfPitchPoint(bool visible) {
    prepareGeometryChange();
    m_isPitchPointVisible = visible;
}

bool GraphicsGearPair::visibilityOfPitchPoint() const {
    return m_isPitchPointVisible;
}

void GraphicsGearPair::setVisibilityOfDrivingGearSampling(bool visible) {
    prepareGeometryChange();
    m_isDrivingGearSamplingVisible = visible;
}

bool GraphicsGearPair::visibilityOfDrivingGearSampling() const {
    return m_isDrivingGearSamplingVisible;
}

void GraphicsGearPair::setVisibilityOfDrivenGearSampling(bool visible) {
    prepareGeometryChange();
    m_isDrivenGearSamplingVisible = visible;
}

bool GraphicsGearPair::visibilityOfDrivenGearSampling() const {
    return m_isDrivenGearSamplingVisible;
}

void GraphicsGearPair::setVisibilityOfForbiddenAreaInDrivingGear(bool visible) {
    prepareGeometryChange();
    m_isForbiddenAreaInDrivingGearVisible = visible;
}

bool GraphicsGearPair::visibilityOfForbiddenAreaInDrivingGear() const {
    return m_isForbiddenAreaInDrivingGearVisible;
}

void GraphicsGearPair::setVisibilityOfForbiddenAreaInDrivenGear(bool visible) {
    prepareGeometryChange();
    m_isForbiddenAreaInDrivenGearVisible = visible;
}

bool GraphicsGearPair::visibilityOfForbiddenAreaInDrivenGear() const {
    return m_isForbiddenAreaInDrivenGearVisible;
}

void GraphicsGearPair::setVisibilityOfNoneContactPoints(bool visible) {
    prepareGeometryChange();
    m_areNoneContactPointsVisible = visible;
}

bool GraphicsGearPair::visibilityOfNoneContactPoints() const {
    return m_areNoneContactPointsVisible;
}

void GraphicsGearPair::setVisibilityOfSelectedGearPoints(bool visible) {
    prepareGeometryChange();
    m_isSelectionOfGearPointsVisible = visible;
}

bool GraphicsGearPair::visibilityOfSelectedGearPoints() const {
    return m_isSelectionOfGearPointsVisible;
}

void GraphicsGearPair::setVisibilityOfPathOfPossibleContact(bool visible) {
    prepareGeometryChange();
    m_isPathOfPossibleContactVisible = visible;
}

bool GraphicsGearPair::visibilityOfPathOfPossibleContact() const {
    return m_isPathOfPossibleContactVisible;
}

void GraphicsGearPair::setVisibilityOfPathOfRealContact(bool visible) {
    prepareGeometryChange();
    m_isPathOfRealContactVisible = visible;
}

bool GraphicsGearPair::visibilityOfPathOfRealContact() const {
    return m_isPathOfRealContactVisible;
}

void GraphicsGearPair::setVisibilityOfPathOfBestContact(bool visible) {
    prepareGeometryChange();
    m_isPathOfBestContactVisible = visible;
}

bool GraphicsGearPair::visibilityOfPathOfBestContact() const {
    return m_isPathOfBestContactVisible;
}

void GraphicsGearPair::setVisibilityOfPitches(bool visible) {
    prepareGeometryChange();
    m_arePitchesVisible = visible;
}

bool GraphicsGearPair::visibilityOfPitches() const {
    return m_arePitchesVisible;
}

void GraphicsGearPair::setVisibilityOfPitchCircles(bool visible) {
    prepareGeometryChange();
    m_arePitchCirclesVisible = visible;
}

bool GraphicsGearPair::visibilityOfPitchCircles() const {
    return m_arePitchCirclesVisible;
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
    m_gearPair->calculateAgainWithUnchangedAttributes();
}

int GraphicsGearPair::samplingRate() const {
    return (int)(m_gearPair->samplingRate());
}

void GraphicsGearPair::setMaxDriftAngleInDegree(int maxDriftAngle) {
    if(maxDriftAngle <= 0)
        return;
    prepareGeometryChange();
    m_gearPair->setMaxDriftAngleInDegree(maxDriftAngle);
    m_gearPair->calculateAgainWithUnchangedAttributes();
}

int GraphicsGearPair::maxDriftAngleInDegree() const {
    return static_cast<int>(m_gearPair->maxDriftAngleInDegree());
}

void GraphicsGearPair::setFinePencil(bool isFinePencil) {
    prepareGeometryChange();
    m_finePencilUsed = isFinePencil;
}

bool GraphicsGearPair::finePencilUsed() const {
    return m_finePencilUsed;
}

void GraphicsGearPair::updateBoundingRect() {
    if(!m_gearPair->isValid())
        return;
    list< list<ContactPoint*> *> foundPoints = m_gearPair->contactPointManager().foundPoints();
    m_largestDistanceToPoint = 0;
    m_largestNormalOfPoint = 0;
    vec2 pathOfContactBottomLeft, pathOfContactTopRight;
    pathOfContactBottomLeft = pathOfContactTopRight = foundPoints.front()->front()->contactPosition;
    for(list<ContactPoint*> *l : foundPoints) {
        for(ContactPoint *cp : *l) {
            real distToPoint = glm::length(cp->point);
            if(distToPoint > m_largestDistanceToPoint)
                m_largestDistanceToPoint = distToPoint;

            if(cp->forbiddenAreaLength > m_largestNormalOfPoint)
                m_largestNormalOfPoint = cp->forbiddenAreaLength;
            if(cp->contactPosition.x < pathOfContactTopRight.x)
                pathOfContactTopRight.x = cp->contactPosition.x;
            if(cp->contactPosition.y < pathOfContactTopRight.y)
                pathOfContactTopRight.y = cp->contactPosition.y;
            if(cp->contactPosition.x > pathOfContactBottomLeft.x)
                pathOfContactBottomLeft.x = cp->contactPosition.x;
            if(cp->contactPosition.y > pathOfContactBottomLeft.y)
                pathOfContactBottomLeft.y = cp->contactPosition.y;
        }
    }
    QPointF topRight = QPointF(pathOfContactTopRight.x - 20, pathOfContactTopRight.y - 20);
    QPointF bottomLeft = QPointF(pathOfContactBottomLeft.x + 20, pathOfContactBottomLeft.y + 20);
    m_boundingRect = QRectF(topRight, bottomLeft); // normals of path of contact should be in area, too
    m_largestDistanceToNCP = 0;
    m_largestNormalOfNCP = 0;
    list<NoneContactPoint*> ncps = m_gearPair->contactPointManager().noneContactPoints();
    if(!ncps.empty()) {
        NoneContactPoint *ncp = ncps.front();
        for(uint i = 0; i < ncp->points.size(); ++i) {
            real distToPoint = glm::length(ncp->points[i]);
            if(distToPoint > m_largestDistanceToNCP)
                m_largestDistanceToNCP = distToPoint;
            real normalLength = glm::length(ncp->normals[i]);
            if(normalLength > m_largestNormalOfNCP)
                m_largestNormalOfNCP = normalLength;
        }
    }
}

QRectF GraphicsGearPair::boundingRect(const GraphicsMatingSplineGear *gear) const {
    if(gear == m_drivingGear) {
        return gear->GraphicsSplineGear::normalBoundingRect(5);
    } else { // gear == m_drivenGear
        real max = m_largestDistanceToPoint;
        if(m_isForbiddenAreaInDrivenGearVisible)
            max += m_largestNormalOfPoint;
        if(m_areNoneContactPointsVisible) {
            real maxNCP = m_largestDistanceToNCP;
            if(m_isForbiddenAreaInDrivenGearVisible)
                maxNCP += m_largestNormalOfNCP;
            if(maxNCP > max)
                max = maxNCP;
        }
        max += 5; //extra space for thick lines, point radii etc.
        return QRectF(-max, -max, 2 * max, 2 * max);
    }
}

QRectF GraphicsGearPair::boundingRect() const {
    return m_boundingRect;
}

void GraphicsGearPair::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);
    if(!m_gearPair->isValid())
        return;

    if(m_finePencilUsed) {
        changePenWidth(painter, 0);
    }

    if(m_isRotating) {
        m_rotationDegreeDrivingGear += m_rotationVelocity;
        m_rotationDegreeDrivenGear += m_rotationVelocity / m_gearPair->transmissionRatio();
        m_drivingGear->setRotation(m_rotationDegreeDrivingGear);
        m_drivenGear->setRotation(m_rotationDegreeDrivenGear);
    }

    if(m_isPathOfBestContactVisible)
        paintBestConsecutivePathOfContact(painter);
    if(m_isPathOfPossibleContactVisible)
        paintPathOfPossibleContact(painter);
    if(m_isPathOfRealContactVisible)
        paintPathOfRealContact(painter);
    if(m_isPitchPointVisible)
        paintPitchPoint(painter);
}

void GraphicsGearPair::paintAdditionals(QPainter *painter, GraphicsMatingSplineGear *gear) const {
    if(!m_gearPair->isValid())
        return;
    if(m_finePencilUsed) {
        changePenWidth(painter, 0);
    }
    if(gear == m_drivingGear) {
        if(m_arePitchCirclesVisible)
            painter->drawPath(circlePath(m_gearPair->drivingGearPitchRadius()));
        if(m_arePitchesVisible)
            painter->drawPath(pitchesPath(m_drivingGear->numberOfTeeth(),
                                            m_gearPair->contactPointManager().startOfExaminedPitchInDrivingGear(),
                                            m_gearPair->contactPointManager().lengthOfPitchesInDrivingGear()));
        if(m_isPathOfBestContactVisible)
            paintHighlightOfStartAndEndOfPathOfContact(painter);
        if(m_areNoneContactPointsVisible)
            paintNoneContactPoints(painter, true, false);
        if(m_isDrivingGearSamplingVisible)
            paintSampledContactPointsDrivingGear(painter);

    } else if (gear == m_drivenGear) {
        if(m_arePitchCirclesVisible)
            painter->drawPath(circlePath(m_gearPair->drivenGearPitchRadius()));
        if(m_arePitchesVisible)
            painter->drawPath(pitchesPath(m_drivenGear->numberOfTeeth(),
                                            m_gearPair->contactPointManager().startOfExaminedPitchInDrivenGear(),
                                            m_gearPair->contactPointManager().lengthOfPitchesInDrivenGear()));
        if(m_areNoneContactPointsVisible)
            paintNoneContactPoints(painter, false, true);
        if(m_isDrivenGearSamplingVisible)
            paintSampledContactPointsDrivenGear(painter);
        if(m_isSelectionOfGearPointsVisible)
            paintSelectedGearPoints(painter);

    } // other things should not happen ^^
}

void GraphicsGearPair::paintSampledContactPointsDrivingGear(QPainter *painter) const {
    if(!m_gearPair->isValid())
        return;
    list< list<ContactPoint*> *> contactPointsLists = m_gearPair->contactPointManager().foundPoints();

    painter->save();

    for(list<ContactPoint*> *l : contactPointsLists) {
        for(ContactPoint *cp : *l) {
            changePenColor(painter, getColorFor(cp->evaluationStep));

            // originPoint, originNormal
            drawCircle(painter, cp->originPoint);
            if(m_isForbiddenAreaInDrivingGearVisible) {
                vec2 endNormal = cp->originPoint - cp->originNormal * cp->forbiddenAreaLength;
                drawLine(painter, cp->originPoint, endNormal);
            }
        }
    }
    painter->restore();
}

void GraphicsGearPair::paintSampledContactPointsDrivenGear(QPainter *painter) const {
    if(!m_gearPair->isValid())
        return;
    list<ContactPointsWithPosition*> contactPointsWithPositions = m_gearPair->contactPointManager().contactPointsWithPositions();

    painter->save();
    real lightUp = 1.0 / contactPointsWithPositions.size();

    for(ContactPointsWithPosition *list : contactPointsWithPositions) {
        ContactPoint *lastCP = list->points.front();

        for(ContactPoint *cp : list->points) {

            QColor c = getColorFor(cp->evaluationStep);
            if(list->position > 0)
                c = lightUpColor(c, list->position * lightUp);
            else if(list->position < 0)
                c = lightUpColor(c, -(list->position * lightUp));
            changePenColor(painter, c);

            // point (and forbidden area)
            drawCircle(painter, cp->point);
            if(m_finePencilUsed)
                drawCircle(painter, cp->point, Preferences::SmallPointRadius);
            if(m_isForbiddenAreaInDrivenGearVisible) {
                vec2 endNormal = cp->point + cp->normal * cp->forbiddenAreaLength;
                drawLine(painter, cp->point, endNormal);
            }
            // merge following list points by a line:
            if(lastCP != cp) { //not on first point on list
                drawLine(painter, lastCP->point, cp->point);
                if(m_isForbiddenAreaInDrivenGearVisible)
                    drawLine(painter, lastCP->forbiddenAreaEndPoint, cp->forbiddenAreaEndPoint);
                lastCP = cp;
            }
        }
    }
    painter->restore();
}

void GraphicsGearPair::paintPitchPoint(QPainter *painter) const {
    painter->save();
    changePenColor(painter, Preferences::AttentionColor);
    drawCircle(painter, vec2(m_gearPair->drivingGearPitchRadius(), 0), Preferences::PointRadius * 2);
    painter->restore();
}

void GraphicsGearPair::paintBestConsecutivePathOfContact(QPainter *painter) const {
    if(!m_gearPair->isValid())
        return;

    CalculationState s = CalculationState::Simple;
    if(isBottomClearanceUsed())
        s = CalculationState::BottomClearance;

    TurningDirection dir = TurningDirection::Clockwise;
    if(m_rotationVelocity < 0.0) {
        dir = TurningDirection::CounterClockwise;
    }

    vector<ContactPoint*> *bestContactSeries = m_gearPair->contactPointManager().gearConsecutivelyContactPoints(dir, s);
    ContactPoint *startSeries = m_gearPair->contactPointManager().gearConsecutivelyContactPointsStart(dir, s);
    ContactPoint *endSeries = m_gearPair->contactPointManager().gearConsecutivelyContactPointsStop(dir, s);
    real coverage = m_gearPair->contactPointManager().gearConsecutivelyContactPointsCoverageAngle(dir, s);

    QColor colorPath = Preferences::InformationColorA;
    QColor colorCoverage = Preferences::InformationColorB;
    real bigLineWidth = m_finePencilUsed ? Preferences::HighlightedLineWidth : 2 * Preferences::HighlightedLineWidth;
    real thinLineWidth = m_finePencilUsed ? 0 : 1;

    painter->save();

    if(bestContactSeries != nullptr && startSeries != nullptr && endSeries != nullptr) {
        if(bestContactSeries->size() == 1) {
            changePen(painter, colorPath, bigLineWidth, Qt::RoundCap);
            drawCircle(painter, bestContactSeries->at(0)->contactPosition, Preferences::PointRadius + 1);
        } else {
            if(cross(startSeries->contactPosition, endSeries->contactPosition) < 0)
                coverage = -coverage;
            //mark lines to start and end of contact path
            changePen(painter, colorPath, thinLineWidth);
            paintLinesToStartAndEndOfBestConsecutivePathOfContact(painter, startSeries->contactPosition, endSeries->contactPosition);
            //highlight start and end of coverage in contact path
            changePen(painter, colorCoverage, thinLineWidth);
            drawCircle(painter, startSeries->contactPosition, Preferences::PointRadius + 2);
            drawCircle(painter, endSeries->contactPosition, Preferences::PointRadius + 2);
            //mark coverage by lines
            changePen(painter, colorCoverage, thinLineWidth);
            vec2 startOnPitch = normalize(startSeries->contactPosition) * m_gearPair->drivingGearPitchRadius();
            drawLine(painter, startOnPitch, vec2(0.0));
            drawLine(painter, glm::rotate(startOnPitch, coverage), vec2(0.0));
            //mark coverage on pitch circle
            changePen(painter, colorCoverage, bigLineWidth, Qt::RoundCap);
            real startAngle = asin(normalize(startOnPitch).y);
            drawArc(painter, startAngle, startAngle + coverage, m_gearPair->drivingGearPitchRadius());
            //draw path of contact
            changePen(painter, colorPath, bigLineWidth, Qt::RoundCap);
            for(uint i = 1; i < bestContactSeries->size(); ++i) {
                drawLine(painter, bestContactSeries->at(i - 1)->contactPosition, bestContactSeries->at(i)->contactPosition);
            }
        }
    }
    painter->restore();
}

void GraphicsGearPair::paintHighlightOfStartAndEndOfPathOfContact(QPainter *painter) const {
    if(!m_gearPair->isValid())
        return;
    CalculationState s = CalculationState::Simple;
    if(isBottomClearanceUsed())
        s = CalculationState::BottomClearance;

    TurningDirection dir = TurningDirection::Clockwise;
    if(m_rotationVelocity < 0.0) {
        dir = TurningDirection::CounterClockwise;
    }

    painter->save();
    changePenColor(painter, Preferences::InformationColorB);
    ContactPoint *startSeries = m_gearPair->contactPointManager().gearConsecutivelyContactPointsStart(dir, s);
    ContactPoint *endSeries = m_gearPair->contactPointManager().gearConsecutivelyContactPointsStop(dir, s);
    if(startSeries != nullptr)
        drawCircle(painter, startSeries->originPoint, Preferences::PointRadius + 2);
    if(endSeries != nullptr)
        drawCircle(painter, endSeries->originPoint, Preferences::PointRadius + 2);
    painter->restore();
}

void GraphicsGearPair::paintLinesToStartAndEndOfBestConsecutivePathOfContact(QPainter *painter, vec2 startPath, vec2 endPath) const {
    bool startInDrivingGear, stopInDrivingGear;
    real alphaStart, alphaStop;

    if(startPath.x < m_gearPair->drivingGearPitchRadius()) {
        startInDrivingGear = true;
        vec2 toContactPosition = normalize(startPath);
        alphaStart = asin(toContactPosition.y);
        drawLine(painter, vec2(0, 0), toContactPosition * m_gearPair->drivingGearPitchRadius());
    } else {
        startInDrivingGear = false;
        vec2 toContactPosition = normalize(startPath - vec2(m_gearPair->getDistanceOfCenters(), 0));
        alphaStart = asin(toContactPosition.y);
        drawLine(painter, vec2(m_gearPair->getDistanceOfCenters(), 0), vec2(m_gearPair->getDistanceOfCenters(), 0) + toContactPosition * m_gearPair->drivenGearPitchRadius());
    }

    if(endPath.x < m_gearPair->drivingGearPitchRadius()) {
        stopInDrivingGear = true;
        vec2 toContactPosition = normalize(endPath);
        alphaStop = asin(toContactPosition.y);
        drawLine(painter, vec2(0, 0), toContactPosition * m_gearPair->drivingGearPitchRadius());
    } else {
        stopInDrivingGear = false;
        vec2 toContactPosition = normalize(endPath - vec2(m_gearPair->getDistanceOfCenters(), 0));
        alphaStop = asin(toContactPosition.y);
        drawLine(painter, vec2(m_gearPair->getDistanceOfCenters(), 0), vec2(m_gearPair->getDistanceOfCenters(), 0) + toContactPosition * m_gearPair->drivenGearPitchRadius());
    }

    if(startInDrivingGear) {
        real stop = stopInDrivingGear ? alphaStop : 0;
        drawArc(painter, alphaStart, stop, m_gearPair->drivingGearPitchRadius());
    } else {
        real stop = stopInDrivingGear ? 0 : alphaStop;
        drawArc(painter, M_PI - alphaStart, M_PI - stop, m_gearPair->drivenGearPitchRadius(), vec2(m_gearPair->getDistanceOfCenters(), 0));
    }
    if(startInDrivingGear != stopInDrivingGear) {
        if(stopInDrivingGear)
            drawArc(painter, 0, alphaStop, m_gearPair->drivingGearPitchRadius());
        else
            drawArc(painter, M_PI, M_PI - alphaStop, m_gearPair->drivenGearPitchRadius(), vec2(m_gearPair->getDistanceOfCenters(), 0));
    }
}

void GraphicsGearPair::paintPathOfPossibleContact(QPainter *painter) const {
    if(!m_gearPair->isValid())
        return;

    list< list<ContactPoint*> *> contactPointsLists = m_gearPair->contactPointManager().foundPoints();

    painter->save();

    if(m_finePencilUsed)
        changePenColor(painter, Qt::black);
    else
        changePenWidth(painter, 0);

    for(list<ContactPoint*> *l : contactPointsLists) {
        for(ContactPoint *cp : *l) {

            if(!m_finePencilUsed)
                changePenColor(painter, getColorFor(cp->evaluationStep));

            drawCircle(painter, cp->contactPosition);
            if(m_finePencilUsed)
                drawCircle(painter, cp->contactPosition, Preferences::SmallPointRadius);
            vec2 endNormal = cp->contactPosition + cp->normalInContact * 20.0;
            drawLine(painter, cp->contactPosition, endNormal);
        }
    }
    painter->restore();
}

void GraphicsGearPair::paintPathOfRealContact(QPainter *painter) const {
    if(!m_gearPair->isValid())
        return;

    CalculationState s = CalculationState::Simple;
    if(m_gearPair->isBottomClearanceUsed())
        s = CalculationState::BottomClearance;

    vector<ContactPoint*> selectedContactPoints = m_gearPair->contactPointManager().gearContactPoints(s);
    vector<WrongContactPoint*> selectedWrongContactPoints = m_gearPair->contactPointManager().gearWrongContactPoints(s);

    painter->save();
    for(ContactPoint *cp : selectedContactPoints) {
        QColor c = getColorFor(cp->evaluationStep);
        changePenColor(painter, c);
        painter->setBrush(QBrush(c));

        drawCircle(painter, cp->contactPosition);
        if(m_finePencilUsed)
            drawCircle(painter, cp->contactPosition, Preferences::SmallPointRadius);
        vec2 endNormal = cp->contactPosition + cp->normalInContact * 20.0;
        drawLine(painter, cp->contactPosition, endNormal);
    }

    changePenColor(painter, Preferences::AttentionColor);
    painter->setBrush(QBrush(Preferences::AttentionColor));

    for(ContactPoint *cp : selectedWrongContactPoints) {
        drawCircle(painter, cp->contactPosition);
        if(m_finePencilUsed)
            drawCircle(painter, cp->contactPosition, Preferences::SmallPointRadius);
        vec2 endNormal = cp->contactPosition + cp->normalInContact * 20.0;
        drawLine(painter, cp->contactPosition, endNormal);
    }
    painter->restore();
}

void GraphicsGearPair::paintNoneContactPoints(QPainter *painter, bool paintOriginPoints, bool paintTargetPoints) const {
    if(!m_gearPair->isValid())
        return;

    list<NoneContactPoint*> ncps = m_gearPair->contactPointManager().noneContactPoints();

    painter->save();
    changePenColor(painter, Preferences::InformationColorC);

    for(NoneContactPoint *ncp : ncps) {
        //OriginPoint
        if(paintOriginPoints) {
            drawCircle(painter, ncp->originPoint);
            if(m_isForbiddenAreaInDrivingGearVisible) {
                vec2 endNormal = ncp->originPoint - ncp->originNormal * ncp->forbiddenAreaLength;
                drawLine(painter, ncp->originPoint, endNormal);
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
                    drawLine(painter, previousPoint, p);
                if(m_finePencilUsed)
                    drawCircle(painter, p, Preferences::SmallPointRadius);

                if(m_isForbiddenAreaInDrivenGearVisible) {
                    vec2 endPoint = p + n * ncp->forbiddenAreaLength;
                    if(i > 0)
                        drawLine(painter, previousEndPoint, endPoint);
                    drawLine(painter, p, endPoint);
                    if(m_finePencilUsed)
                        drawCircle(painter, endPoint, Preferences::SmallPointRadius);
                    previousEndPoint = endPoint;
                }
                previousPoint = p;
            }
        }
    }
    painter->restore();
}

void GraphicsGearPair::paintSelectedGearPoints(QPainter *painter) const {
    if(!m_gearPair->isValid())
        return;

    vector<vec2> gearPoints = m_gearPair->contactPointManager().gearPoints();

    painter->save();
    changePen(painter, Preferences::AttentionColor, 0);
    for(uint i = 0; i < gearPoints.size(); ++i) {
        drawCircle(painter, gearPoints[i], Preferences::PointRadius + 0.3);
        if(m_finePencilUsed)
            drawCircle(painter, gearPoints[i], Preferences::SmallPointRadius);
        if(i > 0)
            drawLine(painter, gearPoints[i - 1], gearPoints[i]);
    }
    painter->restore();
}
