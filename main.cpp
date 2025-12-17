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

int StopAllVC3000HDevices()
{
    int result = 0;
    PCI::pci().Destroy();
    Sleep(30);
    result = PCI::pci().InitialSystem();
    if (result != 0)
    {
		LOG_DEBUG(GlobalLog::logger, L"[ERROR] 重新初始化 VC3000H 设备失败");
		std::cout << "[ERROR] 重新初始化 VC3000H 设备失败，错误码：" << result << std::endl;
        return -1;
    }
    std::cout << "[INFO] 系统初始化成功。" << std::endl;

    result = PCI::pci().openseral("Com3");
    if (result != 0)
    {
        LOG_DEBUG(GlobalLog::logger, L"[ERROR] 重新打开串口设备失败");
        return -1;
    }
    std::cout << "[INFO] 串口 Com3 打开成功。PLC连接状态：已连接" << std::endl;

    for (int cameraID = 1; cameraID <= 8; ++cameraID) {
        int result = 0; // 错误码变量

        if ((result = PCI::pci().setOutputMode(cameraID, false, 200)) != 0) {

            //QString errorMsg = QString("[ERROR] 设置 camera%1Output 模式失败，错误码：%2")
            //    .arg(cameraID)
            //    .arg(result);

            //// 打印错误日志
            //LOG_DEBUG(GlobalLog::logger, errorMsg.toStdWString().c_str());
        }
    }

    // PCI::pci().SaveParam();
    std::cout << "[INFO] 所有初始化和设置流程完成。" << std::endl;
    PCI::pci().Destroy();
    return 0;
}

LONG WINAPI MyUnhandledExceptionFilter(_EXCEPTION_POINTERS* pExceptionPointers)
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

    // ===== 1. 删除旧文件（保持你之前逻辑）=====
    QFile::remove(stdoutPath);
    QFile::remove(stderrPath);

    // ===== 2. 转换为本地路径 =====
    QByteArray stdoutPathLocal = stdoutPath.toLocal8Bit();
    QByteArray stderrPathLocal = stderrPath.toLocal8Bit();

    // ===== 3. 重定向 stdout =====
    FILE* fOut = freopen(stdoutPathLocal.constData(), "w", stdout);
    if (!fOut) {
        qWarning() << "Failed to redirect stdout to" << stdoutPath;
        return;
    }

    // ===== 4. 重定向 stderr =====
    FILE* fErr = freopen(stderrPathLocal.constData(), "w", stderr);
    if (!fErr) {
        qWarning() << "Failed to redirect stderr to" << stderrPath;
        return;
    }

    // ===== 5. 设置为无缓冲（立即写入）=====
    setvbuf(stdout, nullptr, _IONBF, 0);
    setvbuf(stderr, nullptr, _IONBF, 0);

    // ===== 6. 写入初始头 =====
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
   // redirectToFile();
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
    int ReStartCount = 0;

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
			ReStartCount++;
            if (ReStartCount > 5) {
                //大于五次，父进程也退出不重启
                break;
            }
            // fprintf(stderr, "子进程异常退出，重启\n");
//#ifdef USE_MAIN_WINDOW_CAPACITY
           // StopAllVC3000HDevices();
//#endif // USE_MAIN_WINDOW_CAPACITY
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
   // redirectToFile();


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