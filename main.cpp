#include <QApplication>
#include <QDebug>
#include "login.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setAttribute(Qt::AA_SynthesizeTouchForUnhandledMouseEvents);

#if 0
    Login login;
     //设置为模态窗口
    login.setModal(true);

   // 如果登录成功，则显示主窗口
    if (login.exec() == QDialog::Accepted) {
        MainWindow w;
        w.show();
        return app.exec();
    } else {
        qDebug() << "Login canceled or failed.";
        return 0; // 登录失败，退出程序
    }
#else
         MainWindow w;
        w.show();
         return app.exec();
#endif
}
