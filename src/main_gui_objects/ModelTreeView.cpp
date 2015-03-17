#include "graphics_objects/GraphicsScheduleItem.h"
#include "graphics_objects/GraphicsSpline.h"
#include "graphics_objects/GraphicsSplineGear.h"
#include "graphics_objects/GraphicsGearPair.h"
#include "main_gui_objects/ModelTreeView.h"
#include "main_gui_objects/YesNoDelegate.h"

ModelTreeView::ModelTreeView(Model *model, QWidget *parent) : QTreeView(parent), m_model(model) {
    setHeaderHidden(false);
    setItemsExpandable(true);
    setRootIsDecorated(true);
    setSelectionMode(QAbstractItemView::ExtendedSelection); //this mode allows a selection of multiple items with mouse, ctrl-key and shift-key
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setEditTriggers(QAbstractItemView::DoubleClicked);
    setItemDelegate(new YesNoDelegate(this));
    setModel(m_model);

    m_rightArrow = new QPixmap(QString(":/images/ArrowRight.png"));
    m_downArrow = new QPixmap(QString(":/images/ArrowDown.png"));
    m_spline = new QPixmap(QString(":/images/Spline.png"));
    m_gear = new QPixmap(QString(":/images/Gear.png"));
    m_gearingClosed = new QPixmap(QString(":/images/GearingClosed.png"));
    m_gearingOpened = new QPixmap(QString(":/images/GearingOpened.png"));
    m_circle = new QPixmap(QString(":/images/Circle.png"));
}

ModelTreeView::~ModelTreeView() {
    delete m_rightArrow;
    delete m_downArrow;
    delete m_spline;
    delete m_gear;
    delete m_gearingClosed;
    delete m_gearingOpened;
    delete m_circle;
}


QModelIndexList ModelTreeView::selectedIndexes() const {
    return QTreeView::selectedIndexes();
}

void ModelTreeView::drawBranches(QPainter *painter, const QRect &rect, const QModelIndex &index) const {
    double scale = 0.65f;
    double h = rect.height() * scale;
    double dy = (rect.height() - h) / 2.0;
    double dx = rect.width() - h;
    if(dx < 0) {
        h = h - dx;
        dx = 0;
    }
    QRectF targetRect(dx + rect.x(), dy + rect.y(), h, h);

    GraphicsScheduleItem *g = m_model->getItemFromIndex(index);

    if(m_model->hasChildren(index)) {
        if(GraphicsGearPair::isGraphicsGearPairItem(g)) {
            if(isExpanded(index))
                painter->drawPixmap(targetRect, *m_gearingOpened, m_gearingOpened->rect());
            else
                painter->drawPixmap(targetRect, *m_gearingClosed, m_gearingClosed->rect());
        } else {
            if(isExpanded(index))
                painter->drawPixmap(targetRect, *m_downArrow, m_downArrow->rect());
            else
                painter->drawPixmap(targetRect, *m_rightArrow, m_rightArrow->rect());
        }
    } else {
        if(GraphicsSpline::isGraphicsSplineItem(g))
            painter->drawPixmap(targetRect, *m_spline, m_spline->rect());
        else if(GraphicsSplineGear::isGraphicsSplineGearItem(g))
            painter->drawPixmap(targetRect, *m_gear, m_gear->rect());
        else
            painter->drawPixmap(targetRect, *m_circle, m_circle->rect());
    }
}

void ModelTreeView::resizeEvent(QResizeEvent *event) {
    setColumnWidth(Model::NAME, event->size().width() - 30);
    setColumnWidth(Model::VISIBILITY, 30);
    QTreeView::resizeEvent(event);
}
