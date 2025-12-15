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

    void stop();  

protected:
    void run() override; 

private:
    std::atomic<bool> m_running{ true };
};

#endif // RUN_TASK_THREAD_H
