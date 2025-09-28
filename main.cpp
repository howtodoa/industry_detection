#include <QApplication>
#include <QDebug>
#include <QString>
#include <QPixmap>
#include <QSplashScreen>
#include "mainwindow.h"
#include <windows.h>
#include <dbghelp.h>
#include "initsystem.h"
//#include <openvino/openvino.hpp>


LONG WINAPI MyUnhandledExceptionFilter(_EXCEPTION_POINTERS *pExceptionPointers)
{
    // 获取当前时间作为文件名一部分
    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
    QString exePath = QCoreApplication::applicationDirPath();
    QString dumpFilePath = exePath + QString("/crash_%1.dmp").arg(timestamp);

    // 创建转储文件
    HANDLE hDumpFile = CreateFileW(
        (LPCWSTR)dumpFilePath.utf16(),
        GENERIC_WRITE,
        0,
        nullptr,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        nullptr
        );

    if (hDumpFile != INVALID_HANDLE_VALUE)
    {
        MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
        dumpInfo.ThreadId = GetCurrentThreadId();
        dumpInfo.ExceptionPointers = pExceptionPointers;
        dumpInfo.ClientPointers = FALSE;

        // 写入 dump 文件
        MiniDumpWriteDump(
            GetCurrentProcess(),
            GetCurrentProcessId(),
            hDumpFile,
            (MINIDUMP_TYPE)(
                MiniDumpWithFullMemory |
                MiniDumpWithFullMemoryInfo |
                MiniDumpWithHandleData |
                MiniDumpWithThreadInfo
                ),
            &dumpInfo,
            nullptr,
            nullptr
            );
        CloseHandle(hDumpFile);
    }

    // 返回 EXCEPTION_EXECUTE_HANDLER 终止程序
    return EXCEPTION_EXECUTE_HANDLER;
}


void redirectToFile()
{
    // 获取 exe 所在目录
    QString exeDir = QCoreApplication::applicationDirPath();

    // 构建完整的文件路径
    QString stdoutPath = QDir(exeDir).filePath("stdout.txt");
    QString stderrPath = QDir(exeDir).filePath("stderr.txt");

    // 删除已存在的旧文件
    QFile::remove(stdoutPath);
    QFile::remove(stderrPath);

    // 转换为本地编码的文件路径（Windows需要）
    QByteArray stdoutPathLocal = stdoutPath.toLocal8Bit();
    QByteArray stderrPathLocal = stderrPath.toLocal8Bit();

    // 重定向标准输出
    if (freopen(stdoutPathLocal.constData(), "w", stdout) == nullptr) {
        qWarning() << "Failed to redirect stdout to" << stdoutPath;
    }

    // 重定向标准错误
    if (freopen(stderrPathLocal.constData(), "w", stderr) == nullptr) {
        qWarning() << "Failed to redirect stderr to" << stderrPath;
    }

    // 设置缓冲区立即刷新
    setvbuf(stdout, nullptr, _IONBF, 0);
    setvbuf(stderr, nullptr, _IONBF, 0);

    // 写入初始信息
    fprintf(stdout, "=== Application Standard Output ===\n");
    fprintf(stderr, "=== Application Error Output ===\n");

    qDebug() << "Output redirected to:" << stdoutPath;
    qDebug() << "Error output redirected to:" << stderrPath;
}

int main(int argc, char *argv[])
{
    //redirectToFile();
 

    HANDLE hMutex = CreateMutex(NULL, TRUE, L"Industry_Detection");
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        qDebug() << "程序已在运行中。";
        return 0;  // 退出
    }

    SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);
    QApplication app(argc, argv);
    QApplication::setAttribute(Qt::AA_SynthesizeTouchForUnhandledMouseEvents);

    QIcon icon(":/images/resources/images/oo.ico");
    QPixmap pixmap = icon.pixmap(512, 512);  
    QSplashScreen splash(pixmap);
    splash.show();
    app.processEvents();  // 立即显示 splash 图

    QDir::setCurrent(QCoreApplication::applicationDirPath());//设置工作路径
    InitSystem("../../../ini/globe/Global.json");
#ifdef USE_MAIN_WINDOW_CAPACITY
    MainWindow w;
#else  
    MainWindow w(1);

#endif
    w.showMaximized();  // 启动时最大化显示

    splash.finish(&w);

    return app.exec();
}
