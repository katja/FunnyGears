#include <QApplication>
#include "MainWindow.h"

int main(int arg, char** argc) {
    QApplication app(arg, argc);
    MainWindow mainWindow;
    mainWindow.show();
    return app.exec();
}
