#ifndef OBJECT_ATTRIBUTES_WIDGET
#define OBJECT_ATTRIBUTES_WIDGET

#include "stable.h"
#include "SelectionModel.h"
#include "graphics_objects/GraphicsItem.h"
#include "graphics_widgets/GraphicsItemAttributesWidget.h"

class ObjectAttributesWidget : public QDockWidget {

Q_OBJECT

public:
    ObjectAttributesWidget(SelectionModel *selectionModel, QWidget *parent = 0);
    ~ObjectAttributesWidget();

private slots:
    void selectItem(GraphicsItem *graphicsItem);
    void deselect();

private:
    QList<GraphicsItemAttributesWidget*> m_graphicsItemWidgets;
    QWidget *m_noContentWidget;
    SelectionModel *m_selectionModel;

};

#endif // OBJECT_ATTRIBUTES_WIDGET
