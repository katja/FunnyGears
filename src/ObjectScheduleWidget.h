#ifndef OBJECT_SCHEDULE_WIDGET
#define OBJECT_SCHEDULE_WIDGET

#include "stable.h"
#include "ObjectScheduleView.h"

class ObjectScheduleWidget : public QDockWidget {

public:
    ObjectScheduleWidget(ObjectScheduleView *objectSchedule, QWidget *parent = 0);
    ~ObjectScheduleWidget();

private:
    ObjectScheduleView *m_objectSchedule;

    QPushButton *m_addItemButton,
                *m_removeItemButton;

};

#endif // OBJECT_SCHEDULE_WIDGET
