#ifndef OBJECT_ATTRIBUTES_WIDGET
#define OBJECT_ATTRIBUTES_WIDGET

#include "stable.h"
#include "SceneTreeModel.h"
#include "ConnectionModel.h"
#include "GraphicsItemModel.h"
#include "GraphicsItemAttributesWidget.h"
#include "SplineModel.h"

class ObjectAttributesWidget : public QDockWidget {

Q_OBJECT

public:
    ObjectAttributesWidget(SceneTreeModel *sceneTreeModel, ConnectionModel *connectionModel, QWidget *parent = 0);
    ~ObjectAttributesWidget();

private slots:
    void selectItem(QGraphicsItem *graphicsItem);
    void deselect();

private:
    QList<GraphicsItemModel*> m_modelList;
    QHash<GraphicsItemModel*, GraphicsItemAttributesWidget*> m_modelWidgetsHash;
    QWidget *m_noContentWidget;
    ConnectionModel *m_connectionModel;

    QGraphicsItem *m_currentItem;

};

#endif // OBJECT_ATTRIBUTES_WIDGET
