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

                    if (deque.empty()) {
                        LOG_DEBUG(GlobalLog::logger, (L"OutPutThread 阻塞原因：相机 [" + key.toStdWString() + L"] 队列为空。").c_str());
                        return false;
                    }

                    if (GateStatus.load() == 2) {
                      //  LOG_DEBUG(GlobalLog::logger, L"OutPutThread 阻塞原因：GateStatus 状态为 2。");
                        return false;
                    }
                }
            LOG_DEBUG(GlobalLog::logger, L"OutPutThread: 线程被唤醒");
            return true;
            });

        if (!m_running) {
            lk.unlock();
            break;
        }

        qDebug() << "OutPutThread_Flower consumer wakeup";
        LOG_DEBUG(GlobalLog::logger, L"OutPutThread: 线程被唤醒，开始处理合并结果。");


        // 业务逻辑：判断所有队头是否都为 1
        bool allOne = true;
        for (const auto& key : MergePointVec.keys()) {
            auto& deque = MergePointVec.find(key).value();
            if (deque.front() != 1) {
                allOne = false;

                std::wstring fail_msg = QString("MergeCheck: 队列 [%1] 导致 allOne=false (值=%2)")
                    .arg(key)
                    .arg(allOne)
                    .toStdWString();
                LOG_DEBUG(GlobalLog::logger, fail_msg.c_str());

                break;
            }
        }

        PCI::pci().setOutputMode(
            GlobalPara::MergePoint,
            (allOne==1 && GateStatus.load() == 1) ? false : true,
            100
        );

            // PCI::pci().setoutput( GlobalPara::MergePoint,(allOne==1 && GateStatus.load() == 1) ? false : true);

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
            auto& deque = MergePointVec.find(key).value();
            deque.fill(0); 
    
        }

        GateStatus.store(2);
        lk.unlock();
        g_cv.notify_all();

    }

    qDebug() << "OutPutThread_Flower exiting normally.";
}
