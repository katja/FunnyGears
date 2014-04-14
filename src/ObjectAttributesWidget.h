#ifndef OBJECT_ATTRIBUTES_WIDGET
#define OBJECT_ATTRIBUTES_WIDGET

#include "stable.h"
#include "SceneTreeModel.h"
#include "ConnectionModel.h"
#include "GraphicsItemAttributesWidget.h"

class ObjectAttributesWidget : public QDockWidget {

Q_OBJECT

public:
    ObjectAttributesWidget(ConnectionModel *connectionModel, QWidget *parent = 0);
    ~ObjectAttributesWidget();

private slots:
    void selectItem(QGraphicsItem *graphicsItem);
    void deselect();

private:
    QList<GraphicsItemAttributesWidget*> m_graphicsItemWidgets;
    QWidget *m_noContentWidget;
    ConnectionModel *m_connectionModel;

};

#endif // OBJECT_ATTRIBUTES_WIDGET
