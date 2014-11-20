#include "ModelTreeView.h"
#include "YesNoDelegate.h"
#include "graphics_objects/GraphicsScheduleItem.h"
#include "graphics_objects/GraphicsSpline.h" //delete this <<<

ModelTreeView::ModelTreeView(Model *model, QWidget *parent) : QTreeView(parent), m_model(model) {
    setHeaderHidden(false);
    // setIndentation(0);
    setItemsExpandable(true);
    setRootIsDecorated(true);
    setSelectionMode(QAbstractItemView::ExtendedSelection); //this mode allows a selection of multiple items with mouse, ctrl-key and shift-key
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setEditTriggers(QAbstractItemView::DoubleClicked);
    setItemDelegate(new YesNoDelegate(this));
    setModel(m_model);

    setColumnWidth(Model::NAME, 150);
    setColumnWidth(Model::VISIBILITY, 30);

}

ModelTreeView::~ModelTreeView() {}


QModelIndexList ModelTreeView::selectedIndexes() const {
    return QTreeView::selectedIndexes();
}

void ModelTreeView::drawBranches(QPainter *painter, const QRect &rect, const QModelIndex &index) const {
    QPixmap *m_rightArrow = new QPixmap(QString("images/ArrowRight.png"));
    QPixmap *m_downArrow = new QPixmap(QString("images/ArrowDown.png"));
    QPixmap *m_spline = new QPixmap(QString("images/Spline.png"));
    QPixmap *m_circle = new QPixmap(QString("images/Circle.png"));
    double scale = 0.65f;
    double h = rect.height() * scale;
    double dy = (-h + rect.height()) / 2.0f;
    double dx = rect.width() - h;
    if(dx < 0) {
        h = h - dx;
        dx = 0;
    }

    GraphicsScheduleItem *g = m_model->getItemFromIndex(index);

    QRectF targetRect(dx + rect.x(), dy + rect.y(), h, h);

    if(m_model->hasChildren(index)) {
        if(isExpanded(index))
            painter->drawPixmap(targetRect, *m_downArrow, m_downArrow->rect());
        else
            painter->drawPixmap(targetRect, *m_rightArrow, m_rightArrow->rect());
    } else {
        if(GraphicsSpline::isGraphicsSplineItem(g))
            painter->drawPixmap(targetRect, *m_spline, m_spline->rect());
        else
            painter->drawPixmap(targetRect, *m_circle, m_circle->rect());
    }
}
