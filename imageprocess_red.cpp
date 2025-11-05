#include "imageprocess_red.h"
#include "imageprocess.h"
#include "MZ_VC3000H.h"
#include <string>
#include "CapacitanceProgram.h"
#include "Api_FlowerPinDetection.h"

Imageprocess_Red::Imageprocess_Red(Cameral* cam, SaveQueue* m_saveQueue, QObject* parent)
	: ImageProcess(cam, m_saveQueue, parent)
{

}

Imageprocess_Red::~Imageprocess_Red()
{

}

void Imageprocess_Red::run()
{
	qDebug() << "ImageProcess::run() (thread started) in QThread ID:" << QThread::currentThreadId();

	if (!m_inputQueue) {
		qCritical() << "ImageProcess::run(): Input queue not available! Image processing cannot start.";
		return;
	}

	while (!thread_stopFlag.load())
	{
		std::shared_ptr<cv::Mat> currentImagePtr;

		{
			std::unique_lock<std::mutex> lock(m_inputQueue->mutex);

			m_inputQueue->cond.wait(lock, [this] {
				return !m_inputQueue->queue.empty() || m_inputQueue->stop_flag.load() || thread_stopFlag.load();
				});

			if (thread_stopFlag.load() || (m_inputQueue->stop_flag.load() && m_inputQueue->queue.empty())) {
				continue;
			}

			currentImagePtr = m_inputQueue->queue.front();

			if (!currentImagePtr || currentImagePtr->empty()) {
				LOG_DEBUG(GlobalLog::logger, L"ptr null");
				qWarning() << "ImageProcess::run(): 准备发出信号时 currentImagePtr 为空或数据无效，跳过发出信号。";
			}
			else {
				qDebug() << "currentImagePtr not empty()";
			}

			m_inputQueue->queue.pop_front();
			std::cout << "image has output m_inputQueue->queue.pop_front():" << m_inputQueue->queue.size() << std::endl;
		}


		std::shared_ptr<cv::Mat> afterImagePtr = std::make_shared<cv::Mat>();
		cv::Mat image;
		int ret = -1;

		if (cam_instance->video == false) // 非推流的情况
		{

			QElapsedTimer timer;
			timer.start();  // 开始计时

           if (cam_instance->indentify == "FlowerPin")
			{
			   m_inputQueue->red_process_flag.store(true);
			   ret = ExportFlowerSpace::RunPosTape(*currentImagePtr);
			   m_inputQueue->red_process_flag.store(false);
				qint64 elapsed = timer.elapsed();
				qDebug() << cam_instance->cameral_name << "红胶带算法耗时：" << elapsed << "毫秒";
				if (ret == 0) {
					continue;
				}
				else
				{
					info.ret = ret;
					if (cam_instance->DI.Shield == true) ret = 0;
					ret = -1;
				}
			}
			else if (cam_instance->indentify == "FlowerPinNeg")
			{
			   m_inputQueue->red_process_flag.store(true);
			   ret = ExportFlowerSpace::RunPosTape(*currentImagePtr);
			   m_inputQueue->red_process_flag.store(false);
			   qint64 elapsed = timer.elapsed();
			   qDebug() << cam_instance->cameral_name << "红胶带算法耗时：" << elapsed << "毫秒";
			   if (ret == 0) {
				   continue;
			   }
			   else
			   {
				   info.ret = ret;
				   if (cam_instance->DI.Shield == true) ret = 0;
				    
			   }
			}
			info.timeStr = QString::number(timer.elapsed()).toStdString();
		}
		else // 推流的情况
		{
			continue;
		}

		//没有屏蔽,正常处理
		if (ret != 0)
		{
			QString camId = QString::fromStdString(cam_instance->indentify);
			std::unique_lock<std::mutex> lk(g_mutex);

			MergePointVec[camId].polution();

			lk.unlock();
			g_cv.notify_all();
		}

		//存图
		if (cam_instance->DI.saveflag.load() > 1 && cam_instance->video == false)
		{
			std::unique_lock<std::mutex> lock(saveToQueue->mutex);
			if (saveToQueue->queue.size() > 100)
			{
				GlobalLog::logger.Mz_AddLog(L"deque size more than 100");
			}
			else if (cam_instance->DI.saveflag.load() == 2 && (info.ret == -1 || info.ret == 3|| info.ret==1))
			{
				dataToSave.imagePtr = afterImagePtr;
				saveToQueue->queue.push_back(dataToSave);
				//GlobalLog::logger.Mz_AddLog(L"pre Save");
				qDebug() << "图像数据和信息已推入保存队列。当前队列大小：" << saveToQueue->queue.size();
			}
			else if (cam_instance->DI.saveflag.load() == 2 && info.ret == 0)
			{
				qDebug() << "图像数据和信息已推入保存队列。当前队列大小：" << saveToQueue->queue.size();
			}
			else
			{
				dataToSave.imagePtr = afterImagePtr;
				saveToQueue->queue.push_back(dataToSave);
				GlobalLog::logger.Mz_AddLog(L"all Save");
				qDebug() << "图像数据和信息已推入保存队列。当前队列大小：" << saveToQueue->queue.size();
			}
			saveToQueue->cond.notify_one();
		}


	} // while 循环结束

	qDebug() << "ImageProcess::run() (thread finished).";
}