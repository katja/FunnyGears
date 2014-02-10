#ifndef MAIN_WINDOW
#define MAIN_WINDOW

#include "stable.h"

class MainWindow : public QMainWindow {

public:
    MainWindow(QWidget *parent = 0, Qt::WindowFlags flags = 0);
    ~MainWindow();

    QIcon icon() const;
};


#endif // MAIN_WINDOW
