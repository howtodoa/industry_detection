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
#include "MZ_VC3000H.h"
//#include <openvino/openvino.hpp>


int StopAllVC3000HDevices()
{
    int result = 0;

    result = PCI::pci().InitialSystem();
    if (result != 0)
    {
        std::cout << "[ERROR] 系统初始化失败，错误码：" << result << std::endl;
        return -1;
    }
    std::cout << "[INFO] 系统初始化成功。" << std::endl;

    result = PCI::pci().openseral("Com3");
    if (result != 0)
    {
        std::cout << "[ERROR] 打开串口 Com3 失败，错误码：" << result << std::endl;
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

LONG WINAPI MyCrashHandler(_EXCEPTION_POINTERS* pExPtrs)
{
    // 获取当前工作目录
    wchar_t currentDir[MAX_PATH];
    GetCurrentDirectoryW(MAX_PATH, currentDir);

    // 构建 dump 文件名
    wchar_t dumpFilePath[MAX_PATH];
    SYSTEMTIME st;
    GetLocalTime(&st);
    wsprintfW(dumpFilePath, L"%s\\crash_%04d%02d%02d_%02d%02d%02d.dmp",
        currentDir,
        st.wYear, st.wMonth, st.wDay,
        st.wHour, st.wMinute, st.wSecond);

    // 创建 dump 文件
    HANDLE hFile = CreateFileW(dumpFilePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        MINIDUMP_EXCEPTION_INFORMATION info;
        info.ThreadId = GetCurrentThreadId();
        info.ExceptionPointers = pExPtrs;
        info.ClientPointers = FALSE;

        MiniDumpWriteDump(
            GetCurrentProcess(),
            GetCurrentProcessId(),
            hFile,
            static_cast<MINIDUMP_TYPE>(MiniDumpWithFullMemory | MiniDumpWithHandleData | MiniDumpWithThreadInfo),
            &info,
            nullptr,
            nullptr
        );

        FlushFileBuffers(hFile);
        CloseHandle(hFile);
    }

    // 打印到 stderr（可选）
    fprintf(stderr, "Application crashed! Dump written to %ws\n", dumpFilePath);

    return EXCEPTION_EXECUTE_HANDLER;
}

void InstallCrashHandler()
{
    // VEH 优先级高于 SEH
   // AddVectoredExceptionHandler(1, MyCrashHandler);

    // 防止其他 DLL 覆盖你的 SetUnhandledExceptionFilter
    SetUnhandledExceptionFilter(MyCrashHandler);
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

#ifndef USE_MAIN_WINDOW_CAPACITY

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
    InstallCrashHandler();
	//redirectToFile();
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
            DWORD result = WaitForSingleObject(pi.hProcess, INFINITE);
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
#ifdef USE_MAIN_WINDOW_CAPACITY
            StopAllVC3000HDevices();
#endif // USE_MAIN_WINDOW_CAPACITY
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
    InstallCrashHandler();
    redirectToFile();


    HANDLE hMutex = CreateMutex(NULL, TRUE, L"Industry_Detection");
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        qDebug() << "程序已在运行中。";
        return 0;  // 退出
    }

   // SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);
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