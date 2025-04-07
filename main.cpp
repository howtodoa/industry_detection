#include <QApplication>
#include <QDebug>
#include <QString>
#include "login.h"
#include "mainwindow.h"

// #include "MZ_ADOConn.h"

// void test()
// {
//     //wchar_t类型无法直接在qt中使用，必须转换
//     wchar_t connStr[] = L"Driver={MySQL ODBC 8.0 Unicode Driver};Server=localhost;Database=mz_test;Uid=root;Pwd=123456;";
//     QString qtStr = QString::fromWCharArray(connStr);
//     Mz_DB::DBOperation db;
//     int conret;
//     conret=db.Mz_ConnectDB(connStr);
//     if(conret==0) qDebug()<<"connect success";
//     db.Mz_CloseDB();
// }

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setAttribute(Qt::AA_SynthesizeTouchForUnhandledMouseEvents);

    MainWindow w;
    w.show();
    return app.exec();
}
