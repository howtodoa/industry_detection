#ifndef RUN_TASK_THREAD_H
#define RUN_TASK_THREAD_H

#include "typdef.h"
#include <QThread>
#include <atomic>
#include <QDebug>

class RunTaskThread : public QThread
{
    Q_OBJECT

public:
    explicit RunTaskThread(QObject* parent = nullptr);
    ~RunTaskThread() override;

    void stop();   // 外部调用用于安全停止线程

protected:
    void run() override;  // 线程执行体，用 run 简化结构

private:
    std::atomic<bool> m_running{ true };
};

#endif // RUN_TASK_THREAD_H
