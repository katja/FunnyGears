#include "ObjectAttributesWidget.h"
#include "SplineAttributesWidget.h"

ObjectAttributesWidget::ObjectAttributesWidget(SceneTreeModel *sceneTreeModel, ConnectionModel *connectionModel, QWidget *parent) : QDockWidget("Object Attributes", parent, Qt::Widget) {
    std::cout << "ObjectAttributesWidget is created" << std::endl;


    SplineModel *splineModel = new SplineModel(sceneTreeModel, this);
    QDataWidgetMapper *splineMapper = new QDataWidgetMapper(this);
    splineMapper->setModel(splineModel);
    SplineAttributesWidget *splineWidget = new SplineAttributesWidget(splineMapper, this);

    m_modelList << (splineModel);
    m_modelWidgetsHash.insert(splineModel, splineWidget);
    m_modelMapperHash.insert(splineModel, splineMapper);

    m_noContentWidget = new QWidget(this);
    deselect();

    connect(connectionModel, SIGNAL(oneGraphicsItemSelected(QGraphicsItem*)), this, SLOT(selectItem(QGraphicsItem*)));
    connect(connectionModel, SIGNAL(manyOrNoneGraphicsItemSelected()), this, SLOT(deselect()));
}

ObjectAttributesWidget::~ObjectAttributesWidget() {
}

void ObjectAttributesWidget::selectItem(QGraphicsItem *item) {
    std::cout << "ObjectAttributesWidget::selectItem" << std::endl;
    foreach(GeometryModel *model, m_modelList) {
        if(model->operatesOn(item)) {
            setWidget(m_modelWidgetsHash[model]);
            (m_modelMapperHash[model])->setCurrentIndex(model->indexOf(item));
        }
    }
}

void ObjectAttributesWidget::deselect() {
    setWidget(m_noContentWidget);
}
