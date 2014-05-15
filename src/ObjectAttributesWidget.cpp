#include "ObjectAttributesWidget.h"
#include "GraphicsSplineAttributesWidget.h"

ObjectAttributesWidget::ObjectAttributesWidget(ConnectionModel *connectionModel, QWidget *parent)
: QDockWidget("Object Attributes", parent, Qt::Widget), m_connectionModel(connectionModel) {
    std::cout << "ObjectAttributesWidget is created" << std::endl;

    GraphicsSplineAttributesWidget *splineWidget = new GraphicsSplineAttributesWidget(this);
    splineWidget->hide();
    m_graphicsItemWidgets << splineWidget;

    m_noContentWidget = new QWidget(this);
    setWidget(m_noContentWidget);

    connect(m_connectionModel, SIGNAL(oneGraphicsItemSelected(GraphicsItem*)), this, SLOT(selectItem(GraphicsItem*)));
    connect(m_connectionModel, SIGNAL(noneOrManyGraphicsItemsSelected()), this, SLOT(deselect()));
}

ObjectAttributesWidget::~ObjectAttributesWidget() {
    std::cout << "ObjectAttributesWidget is deleted" << std::endl;
}

void ObjectAttributesWidget::selectItem(GraphicsItem *item) {
    GraphicsItemAttributesWidget *currentWidget = 0;
    int i = 0;
    while(currentWidget == 0 && i < m_graphicsItemWidgets.size()) {
        if(m_graphicsItemWidgets.at(i)->worksOnItem(item)) {
            currentWidget = m_graphicsItemWidgets.at(i);
            currentWidget->setItem(item);
            setWidget(currentWidget);
        }
        ++i;
    }
    if(currentWidget == 0)
        deselect();
}

void ObjectAttributesWidget::deselect() {
    setWidget(m_noContentWidget);
}
