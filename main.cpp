#include <QApplication>
#include <QDebug>
#include <QString>
#include "login.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setAttribute(Qt::AA_SynthesizeTouchForUnhandledMouseEvents);

    MainWindow w;
    w.showMaximized();  // 启动时最大化显示
    return app.exec();
}
