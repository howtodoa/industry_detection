#include "outputhread_flower.h"
#include "MZ_VC3000H.h"
#include "MyStl.h"

OutPutThread_Flower::OutPutThread_Flower(QObject* parent)
    : OutPutThread(parent)
{
    qDebug() << "OutPutThread_Flower created.";
}

OutPutThread_Flower::~OutPutThread_Flower()
{
    stop();
    wait();
    qDebug() << "OutPutThread_Flower destroyed.";
}

void OutPutThread_Flower::run()
{
    qDebug() << "OutPutThread_Flower started in thread:" << QThread::currentThread();

    while (m_running)
    {

        std::unique_lock<std::mutex> lk(g_mutex);

        g_cv.wait(lk, [this]() {
            if (!m_running)
                return true;

            if (MergePointVec.isEmpty())
                return false;

            // 检查所有 Deque 是否都非空
            for (const auto& key : MergePointVec.keys()) {
                const auto& deque = MergePointVec.value(key);
                if (deque.empty() || GateStatus.load() == 2)
                    return false;
            }

            return true;
            });

        if (!m_running) {
            lk.unlock();
            break;
        }

        qDebug() << "OutPutThread_Flower consumer wakeup";



        // 业务逻辑：判断所有队头是否都为 1
        bool allOne = true;
        for (const auto& key : MergePointVec.keys()) {
            auto& deque = MergePointVec[key];
            if (deque.front() != 1) {
                allOne = false;
                break;
            }
        }

        PCI::pci().setOutputMode(
            GlobalPara::MergePoint,
            (allOne && GateStatus.load() == 1) ? true : false,
            100
        );

        LOG_DEBUG(GlobalLog::logger,
            QString("setOutputMode: MergePoint=%1, allOne=%2")
            .arg(GlobalPara::MergePoint)
            .arg(allOne)
            .toStdWString().c_str()
        );

        // 移除所有 Deque 的队头
        for (auto it = MergePointVec.begin(); it != MergePointVec.end(); ++it) {
            it.value().pop_front();
        }

        // 业务逻辑：队列不满则自动填充空料（0）
        for (auto& key : MergePointVec.keys()) {
            auto& deque = MergePointVec[key];
            deque.fill(0); 
        }

        GateStatus.store(2);
        lk.unlock();
        g_cv.notify_all();

    }

    qDebug() << "OutPutThread_Flower exiting normally.";
}
