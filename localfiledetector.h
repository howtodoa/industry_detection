#ifndef LOCALFILEDETECTOR_H
#define LOCALFILEDETECTOR_H

#include <QObject>
#include <QThread>
#include "public.h"

class LocalFileDetector : public QThread
{
    Q_OBJECT 

public:
    // 构造函数：接收图像队列的指针和要检测的目录路径
    explicit LocalFileDetector(ImageQueuePack* imageQueuePack, const QString& monitorPath, QObject *parent = nullptr);

    // 析构函数：确保线程在对象销毁前停止
    ~LocalFileDetector() override;

    // 控制线程停止的方法
    void stop();

protected:
    // 线程的实际执行函数：重写 QThread 的 run 方法
    void run() override;

private:
    // 私有成员变量，存储线程运行所需的数据
    ImageQueuePack* m_imageQueuePack;
    QString m_monitorPath;
    std::atomic<bool> m_stopRequested{false};
    QFileInfoList m_initialFileList;


};

#endif // LOCALFILEDETECTOR_H
