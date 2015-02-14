#ifndef OBJECT_SCHEDULE_VIEW_WIDGET
#define OBJECT_SCHEDULE_VIEW_WIDGET

#include "stable.h"
#include "Model.h"
#include "ModelTreeView.h"
#include "SelectionModel.h"
#include "basic_objects/Spline.h"
#include "graphics_objects/GraphicsSplineGear.h"

class ObjectScheduleViewWidget : public QDockWidget {

Q_OBJECT

public:
    ObjectScheduleViewWidget(Model *model, QWidget *parent = 0);
    virtual ~ObjectScheduleViewWidget();

    SelectionModel* setSelectionModel(SelectionModel *model);

private slots:
    void addActionsFor(GraphicsItem*);
    void removeAdditionalActions();
    void addSpline();
    void addGearTooth();
    void addGearPair();
    void addInvoluteGear();
    void removeItems();

private:
    ModelTreeView *m_treeView;

    QMenu *m_addMenu;

    QAction *m_curveAsGearAction;
    const Spline *m_spline;

    QAction *m_pairGearAction;
    const GraphicsSplineGear *m_graphicsSplineGear;

    QPushButton *m_addItemButton,
                *m_removeItemButton;

    Model *m_model;
    SelectionModel* m_selectionModel;

    void setWidgetLayout();
};

#endif // OBJECT_SCHEDULE_VIEW_WIDGET
