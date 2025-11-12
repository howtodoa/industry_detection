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
			// --- 1. 尝试进入等待前的状态打印 ---
			qDebug() << "ImageProcess::run() (Red) - 准备进入等待，队列大小:" << m_inputQueue->queue_red.size()
				<< " 停止标志(Input):" << m_inputQueue->stop_flag.load()
				<< " 停止标志(Thread):" << thread_stopFlag.load();

			std::unique_lock<std::mutex> lock(m_inputQueue->mutex_red);

			m_inputQueue->cond_red.wait(lock, [this] {

				bool queue_non_empty = !m_inputQueue->queue_red.empty();
				bool input_stop_set = m_inputQueue->stop_flag.load();
				bool thread_stop_set = thread_stopFlag.load();

				// --- 2. 每次条件检查时的状态打印 ---
				qDebug() << "ImageProcess::run() (Red) - [Wait Check] 队列非空:" << queue_non_empty
					<< " 输入停止:" << input_stop_set
					<< " 线程停止:" << thread_stop_set
					<< " 结果(OR):" << (queue_non_empty || input_stop_set || thread_stop_set);

				return queue_non_empty || input_stop_set || thread_stop_set;
				});

			// --- 3. 被唤醒后的状态打印 ---
			qDebug() << "ImageProcess::run() (Red) - **唤醒**，队列大小:" << m_inputQueue->queue_red.size()
				<< " 停止标志(Input):" << m_inputQueue->stop_flag.load()
				<< " 停止标志(Thread):" << thread_stopFlag.load();

			if (thread_stopFlag.load() || (m_inputQueue->stop_flag.load() && m_inputQueue->queue_red.empty())) {
				qDebug() << "ImageProcess::run() (Red) - 满足退出/继续循环条件，执行 continue。";
				continue;
			}

			// --- 4. 准备处理前的确认打印 ---
			qDebug() << "ImageProcess::run() (Red) - 条件满足，准备取出图像进行处理。";

			currentImagePtr = m_inputQueue->queue_red.front();
			qDebug() << "red";
			if (!currentImagePtr || currentImagePtr->empty()) {
				LOG_DEBUG(GlobalLog::logger, L"ptr null");
				qWarning() << "ImageProcess::run(): 准备发出信号时 currentImagePtr 为空或数据无效，跳过发出信号。";
			}
			else {
				qDebug() << "currentImagePtr not empty()";
			}

			m_inputQueue->queue_red.pop_front();
			//  修正：使用 queue_red.size() 且打印队列名称
			std::cout << "image has output m_inputQueue->queue_red.pop_front():" << m_inputQueue->queue_red.size() << std::endl;

			qDebug() << "ImageProcess::run() (Red) - 图像已弹出，锁释放。";
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
#if 1
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
#endif
			}
		   else if (cam_instance->indentify == "FlowerPinNeg")
			{
#if 1
			   m_inputQueue->red_process_flag.store(true);
			   ret = ExportFlowerSpace::RunNegTape(*currentImagePtr);
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
#endif
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

			SaveData currentSaveData;

			if (saveToQueue->queue.size() > 100)
			{
				GlobalLog::logger.Mz_AddLog(L"deque size more than 100");
			}
			else if (cam_instance->DI.saveflag.load() == 2 && (info.ret == -1 || info.ret == 3|| info.ret==1))
			{
				currentSaveData.imagePtr = currentImagePtr;
				currentSaveData.work_path = dataToSave.savePath_NG;
				saveToQueue->queue.push_back(dataToSave);

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
				currentSaveData.imagePtr = currentImagePtr;
				currentSaveData.work_path = dataToSave.savePath_OK;
				saveToQueue->queue.push_back(dataToSave);

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