#include "run_task_thread.h"

RunTaskThread::RunTaskThread(QObject* parent)
    : QThread(parent)
{
    qDebug() << "RunTaskThread created.";
}

RunTaskThread::~RunTaskThread()
{
    stop();
    wait(); // 等待线程安全退出
    qDebug() << "RunTaskThread destroyed.";
}

void RunTaskThread::stop()
{
    m_running = false;
}

void RunTaskThread::run()
{
    qDebug() << "RunTaskThread started in thread:" << QThread::currentThread();

    while (m_running)
    {
        QThread::msleep(100);
		LOG_DEBUG(GlobalLog::logger, L"RunTaskThread is running...");
    }

    qDebug() << "RunTaskThread exiting normally.";
}
