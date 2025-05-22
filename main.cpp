#include <QApplication>
#include <QDebug>
#include <QString>
#include "mainwindow.h"

#include <windows.h>
#include <dbghelp.h>

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


int main(int argc, char *argv[])
{

    QApplication app(argc, argv);
    QApplication::setAttribute(Qt::AA_SynthesizeTouchForUnhandledMouseEvents);
    MainWindow w;
    w.showMaximized();  // 启动时最大化显示
    SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);
    return app.exec();
}
