#ifndef OUTPUTTHREAD_H
#define OUTPUTTHREAD_H

#include "typdef.h"
#include <QThread>
#include <atomic>
#include <QDebug>


class OutPutThread : public QThread
{
    Q_OBJECT

public:
    explicit OutPutThread(QObject* parent = nullptr);
    ~OutPutThread() override;

    void stop();  // 外部调用以安全结束线程

protected:
    void run() override;  // 线程执行体,用run可以大大减少类结构的复杂程度

private:
    std::atomic<bool> m_running{ true };
};

#endif // OUTPUTTHREAD_H
