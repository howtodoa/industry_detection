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
    wait();  // �ȴ��̰߳�ȫ�˳�
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

            // ������� Deque �Ƿ񶼷ǿ� (����ͷ��������)
            for (const auto& key : MergePointVec.keys()) {
                const std::deque<int>& deque = MergePointVec.value(key);

                if (deque.empty()) {
                    return false; // ֻҪ��һ��Ϊ�գ��ͼ����ȴ�
                }
            }

            return true; // ���� Deque ���ǿ�
            });

        if (!m_running) {
            // �������Ϊֹͣ�źŶ��˳� wait�������� while ѭ��
            lk.unlock();
            break;
        }

        qDebug() << "out consumer wait";

        // ҵ���жϣ���ȡ��ǰ��ͷ�����ж�
        bool allOne = true;
        for (const auto& key : MergePointVec.keys()) {
            const std::deque<int>& deque = MergePointVec.value(key);
            if (deque.front() != 1) {
                allOne = false;
                break;
            }
        }

        PCI::pci().setOutputMode(GlobalPara::MergePoint, allOne, 100);

        LOG_DEBUG(GlobalLog::logger,
            QString("setOutputMode: MergePoint=%1, allOne=%2")
            .arg(GlobalPara::MergePoint)
            .arg(allOne)
            .toStdWString().c_str());

        // �Ƴ����� Deque �Ķ�ͷԪ��
        for (auto it = MergePointVec.begin(); it != MergePointVec.end(); ++it) {
            it.value().pop_front();
        }

        lk.unlock();
        g_cv.notify_all();

         std::this_thread::sleep_for(std::chrono::milliseconds(10)); 
    }

    qDebug() << "OutPutThread exiting normally.";
}
