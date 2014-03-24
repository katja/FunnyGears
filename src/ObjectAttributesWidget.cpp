#include "ObjectAttributesWidget.h"
#include "SplineAttributesWidget.h"

ObjectAttributesWidget::ObjectAttributesWidget(SceneTreeModel *sceneTreeModel, ConnectionModel *connectionModel, QWidget *parent)
: QDockWidget("Object Attributes", parent, Qt::Widget), m_connectionModel(connectionModel), m_currentItem(0) {
    std::cout << "ObjectAttributesWidget is created" << std::endl;

    SplineModel *splineModel = new SplineModel(sceneTreeModel, this);
    SplineAttributesWidget *splineWidget = new SplineAttributesWidget(this);
    splineWidget->hide();

    m_modelList << (splineModel);
    m_modelWidgetsHash.insert(splineModel, splineWidget);

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
            GraphicsItemAttributesWidget *currentWidget = m_modelWidgetsHash[model];
            currentWidget->setItem(item);
            setWidget(currentWidget);
        }
    }
}

void ObjectAttributesWidget::deselect() {
    setWidget(m_noContentWidget);
    m_currentItem = 0;
}
