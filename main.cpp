#include <QApplication>
#include <QDebug>
#include <QString>
#include <QPixmap>
#include <QSplashScreen>
#include "mainwindow.h"
#include <windows.h>
#include <dbghelp.h>
#include <QProcess>
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

#ifdef USE_MAIN_WINDOW_CAPACITY

int runBusiness(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QApplication::setAttribute(Qt::AA_SynthesizeTouchForUnhandledMouseEvents);

    QIcon icon(":/images/resources/images/oo.ico");
    QPixmap pixmap = icon.pixmap(512, 512);
    QSplashScreen splash(pixmap);
    splash.show();
    app.processEvents();

    QDir::setCurrent(QCoreApplication::applicationDirPath());
    InitSystem("../../../ini/globe/Global.json");

#ifdef USE_MAIN_WINDOW_CAPACITY
    MainWindow w;
#elif  USE_MAIN_WINDOW_BRADER
    MainWindow w(1);
#else USE_MAIN_WINDOW_FLOWER
    MainWindow w("flower");
#endif
    w.showMaximized();
    splash.finish(&w);

    return app.exec();
}

int main(int argc, char* argv[])
{

    if (argc > 1 && strcmp(argv[1], "--child") == 0) {
        // 子进程需要 QApplication 来运行 GUI
        return runBusiness(argc, argv);
    }
  
    // 父进程不使用 QCoreApplication,可能会造成界面ui使用不了的情况，因为资源文件被父进程初始化了
   
    HANDLE hMutex = CreateMutex(NULL, TRUE, L"Industry_Detection");
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        return 0;
    }

    //  获取 EXE 完整路径 
    wchar_t currentExePath[MAX_PATH];
    GetModuleFileNameW(NULL, currentExePath, MAX_PATH);
    QString currentExePathQ = QString::fromWCharArray(currentExePath);


    while (true) {

        QString program = currentExePathQ; 
        QString argumentsStr = " --child";

        STARTUPINFO si = { sizeof(si) };
        PROCESS_INFORMATION pi;

        // 必须为可写缓冲区
        std::wstring cmdW = (program + argumentsStr).toStdWString();
        wchar_t cmdBuffer[MAX_PATH * 2];
        wcscpy_s(cmdBuffer, cmdW.c_str());

        if (!CreateProcessW(
            NULL,
            cmdBuffer,
            NULL, NULL, FALSE,
            0, NULL, NULL,
            &si, &pi))
        {
            return 1;
        }

        DWORD exitCode = 0;
        while (true) {
            DWORD result = WaitForSingleObject(pi.hProcess, 3000);
            if (result == WAIT_TIMEOUT) {
                continue;
            }

            GetExitCodeProcess(pi.hProcess, &exitCode);
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
            break;
        }

        if (exitCode == 0) {
            // fprintf(stderr, "子进程正常退出，父进程也退出\n");
            break;
        }
        else {
            // fprintf(stderr, "子进程异常退出，重启\n");
            Sleep(1000);
        }
    }

    ReleaseMutex(hMutex);
    CloseHandle(hMutex);

    return 0;
} 
#else // USE_MAIN_WINDOW_CAPACITY
int main(int argc, char* argv[])
{
    redirectToFile();


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
#elif  USE_MAIN_WINDOW_BRADER
    MainWindow w(1);
#else 
    MainWindow w("flower");
#endif
    w.showMaximized();  // 启动时最大化显示

    splash.finish(&w);

    return app.exec();
}

#endif