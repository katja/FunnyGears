#include "ObjectAttributesWidget.h"
#include "graphics_widgets/GraphicsSplineAttributesWidget.h"
#include "graphics_widgets/GraphicsSplineGearAttributesWidget.h"
#include "graphics_widgets/GraphicsGearPairAttributesWidget.h"

ObjectAttributesWidget::ObjectAttributesWidget(SelectionModel *selectionModel, QWidget *parent)
: QDockWidget("Object Attributes", parent, Qt::Widget), m_selectionModel(selectionModel) {
    std::cout << "ObjectAttributesWidget is created" << std::endl;

    GraphicsItemAttributesWidget *widget;

    widget = new GraphicsSplineAttributesWidget(this);
    widget->hide();
    m_graphicsItemWidgets << widget;

    widget = new GraphicsSplineGearAttributesWidget(this);
    widget->hide();
    m_graphicsItemWidgets << widget;

    widget = new GraphicsGearPairAttributesWidget(this);
    widget->hide();
    m_graphicsItemWidgets << widget;

    m_noContentWidget = new QWidget(this);
    setWidget(m_noContentWidget);

    connect(m_selectionModel, SIGNAL(oneGraphicsItemSelected(GraphicsItem*)), this, SLOT(selectItem(GraphicsItem*)));
    connect(m_selectionModel, SIGNAL(noneOrManyGraphicsItemsSelected()), this, SLOT(deselect()));
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
