#ifndef OBJECT_SCHEDULE_WIDGET
#define OBJECT_SCHEDULE_WIDGET

#include "stable.h"
#include "ObjectScheduleTreeView.h"

class ObjectScheduleWidget : public QDockWidget {

Q_OBJECT

public:
    ObjectScheduleWidget(ObjectScheduleTreeView *objectSchedule, QWidget *parent = 0);
    ~ObjectScheduleWidget();

private slots:
    void addSpline();
    void addGear();

private:
    ObjectScheduleTreeView *m_objectSchedule;

    QPushButton *m_addItemButton,
                *m_removeItemButton;

    QMenu *m_addMenu;

};

#endif // OBJECT_SCHEDULE_WIDGET
