#ifndef OBJECT_ATTRIBUTES_WIDGET
#define OBJECT_ATTRIBUTES_WIDGET

#include "stable.h"
#include "SceneTreeModel.h"
#include "ConnectionModel.h"
#include "GeometryModel.h"
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
    QList<GeometryModel*> m_modelList;
    QHash<GeometryModel*, QWidget*> m_modelWidgetsHash;
    QHash<GeometryModel*, QDataWidgetMapper*> m_modelMapperHash;
    QWidget *m_noContentWidget;

};

#endif // OBJECT_ATTRIBUTES_WIDGET
