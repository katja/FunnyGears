#include "ObjectAttributesWidget.h"
#include "SplineAttributesWidget.h"

ObjectAttributesWidget::ObjectAttributesWidget(SceneTreeModel *sceneTreeModel, ConnectionModel *connectionModel, QWidget *parent)
: QDockWidget("Object Attributes", parent, Qt::Widget), m_connectionModel(connectionModel), m_currentItem(0) {
    std::cout << "ObjectAttributesWidget is created" << std::endl;

    SplineModel *splineModel = new SplineModel(sceneTreeModel, this);
    QDataWidgetMapper *splineMapper = new QDataWidgetMapper(this);
    splineMapper->setModel(splineModel);
    SplineAttributesWidget *splineWidget = new SplineAttributesWidget(splineMapper, this);
    splineWidget->hide();
    connect(splineWidget, SIGNAL(somethingChanged()), this, SLOT(updateCurrentSceneItem()));

    m_modelList << (splineModel);
    m_modelWidgetsHash.insert(splineModel, splineWidget);
    m_modelMapperHash.insert(splineModel, splineMapper);

    m_noContentWidget = new QWidget(this);
    setWidget(m_noContentWidget);

    connect(m_connectionModel, SIGNAL(oneGraphicsItemSelected(QGraphicsItem*)), this, SLOT(selectItem(QGraphicsItem*)));
    connect(m_connectionModel, SIGNAL(manyOrNoneGraphicsItemSelected()), this, SLOT(deselect()));
}

ObjectAttributesWidget::~ObjectAttributesWidget() {
    std::cout << "ObjectAttributesWidget is deleted" << std::endl;
}

void ObjectAttributesWidget::selectItem(QGraphicsItem *item) {
    foreach(GraphicsItemModel *model, m_modelList) {
        if(model->operatesOn(item)) {
            m_currentItem = item;
            setWidget(m_modelWidgetsHash[model]);
            (m_modelMapperHash[model])->setCurrentIndex(model->indexOf(item));
        }
    }
}

void ObjectAttributesWidget::deselect() {
    setWidget(m_noContentWidget);
    m_currentItem = 0;
}

void ObjectAttributesWidget::updateCurrentSceneItem() {
    if(!m_currentItem)
        return;
    m_connectionModel->graphicsItemChanged(m_currentItem);
}
