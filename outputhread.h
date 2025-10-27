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

    void stop();  // �ⲿ�����԰�ȫ�����߳�

protected:
    void run() override;  // �߳�ִ����,��run���Դ�������ṹ�ĸ��ӳ̶�

private:
    std::atomic<bool> m_running{ true };
};

#endif // OUTPUTTHREAD_H
