#include "outputhread.h"
#include "MZ_VC3000H.h"

OutPutThread::OutPutThread(QObject* parent)
    : QThread(parent)
{
    qDebug() << "OutPutThread created.";
}

OutPutThread::~OutPutThread()
{
    stop();
    wait();  // 等待线程安全退出
    qDebug() << "OutPutThread destroyed.";
}

void OutPutThread::stop()
{
    m_running = false;
    g_cv.notify_all();
}

void OutPutThread::run()
{
    qDebug() << "OutPutThread started in thread:" << QThread::currentThread();

    while (m_running)
    {
        std::unique_lock<std::mutex> lk(g_mutex);

        g_cv.wait(lk, [this]() {
            if (!m_running) {
                return true;
            }
            if (MergePointVec.isEmpty()) {
                return false;
            }

            // 检查所有 Deque 是否都非空 (即队头都有数据)
            for (const auto& key : MergePointVec.keys()) {
                const auto& deque = MergePointVec.value(key);

                if (deque.empty()) {
                    return false; // 只要有一个为空，就继续等待
                }
            }

            return true; // 所有 Deque 都非空
            });

        if (!m_running) {
            // 如果是因为停止信号而退出 wait，则跳出 while 循环
            lk.unlock();
            break;
        }

        qDebug() << "out consumer wait";

        // 业务判断：读取当前队头进行判断
        bool allOne = true;
        for (const auto& key : MergePointVec.keys()) {
            const auto& deque = MergePointVec.value(key);
            if (deque.front() != 1) {
                allOne = false;
                LOG_DEBUG(GlobalLog::logger,
                    QString("Deque at key %1 front is %2, not 1")
                    .arg(key)
                    .arg(deque.front())
					.toStdWString().c_str());
                break;
            }
        }

        PCI::pci().setOutputMode(GlobalPara::MergePoint, allOne, 100);

        LOG_DEBUG(GlobalLog::logger,
            QString("setOutputMode: MergePoint=%1, allOne=%2")
            .arg(GlobalPara::MergePoint)
            .arg(allOne)
            .toStdWString().c_str());

        // 移除所有 Deque 的队头元素
        for (auto it = MergePointVec.begin(); it != MergePointVec.end(); ++it) {
            it.value().pop_front();
        }

        lk.unlock();
        g_cv.notify_all();

         //std::this_thread::sleep_for(std::chrono::milliseconds(10)); 
    }

    qDebug() << "OutPutThread exiting normally.";
}
