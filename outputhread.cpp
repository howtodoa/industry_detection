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
}

void OutPutThread::run()
{
    qDebug() << "OutPutThread started in thread:" << QThread::currentThread();

    while (m_running)
    {
        std::unique_lock<std::mutex> lk(g_mutex);

        g_cv.wait(lk, []() {
            for (int v : MergePointVec.values()) {
                if (v == 2) return false;
            }
            return true;
            });

        qDebug() << "out consumer wait";

        bool allOne = true;
        for (int v : MergePointVec.values()) {
            if (v != 1) {
                allOne = false;
                break;
            }
        }

        PCI::pci().setOutputMode(GlobalPara::MergePoint, allOne, 100);

        for (auto it = MergePointVec.begin(); it != MergePointVec.end(); ++it) {
            it.value() = 2;
        }

        lk.unlock();
        g_cv.notify_all();

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    qDebug() << "OutPutThread exiting normally.";
}
