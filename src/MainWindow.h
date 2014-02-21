#ifndef MAIN_WINDOW
#define MAIN_WINDOW

#include "stable.h"
#include "GraphicsView.h"
#include "GraphicsScene.h"

class MainWindow : public QMainWindow {

public:
    MainWindow(QWidget *parent = 0, Qt::WindowFlags flags = 0);
    ~MainWindow();

    QIcon icon() const;


private:
    GraphicsView *view;
    GraphicsScene *scene;
};


#endif // MAIN_WINDOW
