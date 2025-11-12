#include "imageprocess_flower.h"
#include "imageprocess.h"
#include "MZ_VC3000H.h"
#include <string>
#include "CapacitanceProgram.h"
#include "Api_FlowerPinDetection.h"

Imageprocess_Flower::Imageprocess_Flower(Cameral* cam, SaveQueue* m_saveQueue, QObject* parent)
	: ImageProcess(cam, m_saveQueue, parent)
{

}

Imageprocess_Flower::~Imageprocess_Flower()
{

}

void Imageprocess_Flower::run()
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
			qDebug() << "ImageProcess::run() (Queue) - 准备进入等待，队列大小:" << m_inputQueue->queue.size()
				<< " 停止标志(Input):" << m_inputQueue->stop_flag.load()
				<< " 停止标志(Thread):" << thread_stopFlag.load();

			std::unique_lock<std::mutex> lock(m_inputQueue->mutex);

			m_inputQueue->cond.wait(lock, [this] {

				bool queue_non_empty = !m_inputQueue->queue.empty();
				bool input_stop_set = m_inputQueue->stop_flag.load();
				bool thread_stop_set = thread_stopFlag.load();

				// --- 2. 每次条件检查时的状态打印 ---
				qDebug() << "ImageProcess::run() (Queue) - [Wait Check] 队列非空:" << queue_non_empty
					<< " 输入停止:" << input_stop_set
					<< " 线程停止:" << thread_stop_set
					<< " 结果(OR):" << (queue_non_empty || input_stop_set || thread_stop_set);

				return queue_non_empty || input_stop_set || thread_stop_set;
				});

			// --- 3. 被唤醒后的状态打印 ---
			qDebug() << "ImageProcess::run() (Queue) - **唤醒**，队列大小:" << m_inputQueue->queue.size()
				<< " 停止标志(Input):" << m_inputQueue->stop_flag.load()
				<< " 停止标志(Thread):" << thread_stopFlag.load();

			if (thread_stopFlag.load() || (m_inputQueue->stop_flag.load() && m_inputQueue->queue.empty())) {
				qDebug() << "ImageProcess::run() (Queue) - 满足退出/继续循环条件，执行 continue。";
				continue;
			}

			// --- 4. 准备处理前的确认打印 ---
			qDebug() << "ImageProcess::run() (Queue) - 条件满足，准备取出图像进行处理。";

			currentImagePtr = m_inputQueue->queue.front();
			qDebug() << "usually ";
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


		if (GlobalPara::changed.load() == true && cam_instance->learn.load() == false) continue;

		std::shared_ptr<cv::Mat> afterImagePtr = std::make_shared<cv::Mat>();
		std::shared_ptr<cv::Mat> backupImagePtr = std::make_shared<cv::Mat>(currentImagePtr->clone());
		cv::Mat image;
		int ret = -1;

		if (cam_instance->video == false) // 非推流的情况
		{

			QElapsedTimer timer;
			timer.start();  // 开始计时

             if (cam_instance->indentify == "FlowerPin")
			{
#if 1
				 qDebug() << "this is the FlowerPin";
				ret = ExportFlowerSpace::RunPosFlowerPin(*currentImagePtr, LearnPara::inParam7);
				qint64 elapsed = timer.elapsed();
				qDebug() << cam_instance->cameral_name << "算法耗时：" << elapsed << "毫秒";
				if (ret == 0) {
					m_inputQueue->process_flag.store(true);
					//调用算法第二个接口
					OutFlowerPinResParam para;
					
					ret=ExportFlowerSpace::RunPosFlowerPinDeal(*currentImagePtr, LearnPara::inParam7);
					ExportFlowerSpace::ResultOutPosFlowerPin(*afterImagePtr, para);
					
					LOG_DEBUG(
						GlobalLog::logger,
						QString("disFlw2Pin2 value: %1").arg(
							para.disFlw2Pin2, // 要打印的 float 变量
							0,                // 字段宽度 (0 为动态)
							'f',              // 格式：'f' 表示定点小数格式
							4                 // 精度：小数点后保留 4 位（您可以根据需要调整）
						).toStdWString().c_str()
					);

					m_inputQueue->process_flag.store(false);
					if (ret == 0)
					{
						cam_instance->RI->updateActualValues(para);
						cam_instance->RI->applyScaleFactors(cam_instance->DI.scaleFactor.load());
						ret = cam_instance->RI->judge_flower_pin(para);
					//	cam_instance->RI.unifyParams->toLogString();
						if (ret == 1) ret = -1;
                    }

				}
				else if (ret == 3)
				{
					continue;
				}
#endif				
			}
			else if (cam_instance->indentify == "FlowerPinNeg")
			{
#if 1
				 qDebug() << "this is the FlowerPinNeg";
				ret = ExportFlowerSpace::RunNegFlowerPin(*currentImagePtr, LearnPara::inParam8);
				qint64 elapsed = timer.elapsed();
				qDebug() << cam_instance->cameral_name << "算法耗时：" << elapsed << "毫秒";
				if (ret == 0) {
					m_inputQueue->process_flag.store(true);
					//调用算法第二个接口
					OutFlowerPinResParam para;
					ret = ExportFlowerSpace::RunNegFlowerPinDeal(*currentImagePtr, LearnPara::inParam8);
					ExportFlowerSpace::ResultOutNegFlowerPin(*afterImagePtr, para);
					m_inputQueue->process_flag.store(false);
					if (ret == 0)
					{
						cam_instance->RI->updateActualValues(para);
						cam_instance->RI->applyScaleFactors(cam_instance->DI.scaleFactor.load());
						ret = cam_instance->RI->judge_flower_pin(para);
						if (ret == 1) ret = -1;
					}

				}
				else if (ret == 3)
				{
					continue;
				}
#endif
			}
			else if (cam_instance->indentify == "FlowerLook")
			{
#if 1
				 qDebug() << "this is the Look";
				ret = ExportFlowerSpace::RunLookFlowerPin(*currentImagePtr, LearnPara::inParam9);
				qint64 elapsed = timer.elapsed();
				qDebug() << cam_instance->cameral_name << "算法耗时：" << elapsed << "毫秒";
				if (ret == 0) {
					m_inputQueue->process_flag.store(true);
					OutLookPinResParam para;
					ExportFlowerSpace::RunLookFlowerPinDeal(*currentImagePtr, LearnPara::inParam9);
					ExportFlowerSpace::ResultOutLookFlowerPin(*afterImagePtr, para);
					m_inputQueue->process_flag.store(false);

					cam_instance->RI->updateActualValues(para);
					cam_instance->RI->applyScaleFactors(cam_instance->DI.scaleFactor.load());
					ret = cam_instance->RI->judge_look(para);
					if (ret == 1) ret = -1;
				}
				else if (ret == 3)
				{
					continue;
				}
#endif 
			}
			info.timeStr = QString::number(timer.elapsed()).toStdString();

		}
		else // 推流的情况
		{
			afterImagePtr = currentImagePtr;
			if (afterImagePtr) qDebug() << "afterImagePtrptr is not null";
			else qDebug() << "afterImagePtrptr is null";
			emit imageProcessed(afterImagePtr, info);
			currentImagePtr.reset();
			backupImagePtr.reset();
			afterImagePtr.reset();
			continue;
		}

		if (GlobalPara::cheatFlag.load() == false)info.ret = ret;


		// 拍照的情况
		if (cam_instance->photo.load() == true)
		{
			emit ImageLoaded(currentImagePtr);
			qDebug() << "start photo learn";
			if (cam_instance->learn.load() == true) emit Learn();
			cam_instance->photo.store(false);
			cam_instance->learn.store(false);
		}

		if (cam_instance->DI.Shield == true) ret = 0;


		if (GlobalPara::envirment == GlobalPara::IPCEn && ret == 0)//非本地运行的情况
		{
			QString camId = QString::fromStdString(cam_instance->indentify);
			std::unique_lock<std::mutex> lk(g_mutex);
			if (MergePointVec.contains(camId)) {
				MergePointVec[camId].push_back(1);
			}
			else GateStatus.store(1);
			lk.unlock();
			g_cv.notify_all();
		}
		else if (GlobalPara::envirment == GlobalPara::IPCEn && ret == -1 || ret == 1 || ret == 2)//非本地运行的情况
		{
			QString camId = QString::fromStdString(cam_instance->indentify);
			std::unique_lock<std::mutex> lk(g_mutex);
			if (MergePointVec.contains(camId)) {
				MergePointVec[camId].push_back(0);
			}
			else GateStatus.store(0);
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
			else if (cam_instance->DI.saveflag.load() == 2 && (info.ret == -1 || info.ret == 2))
			{
				dataToSave.imagePtr = currentImagePtr;
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
				dataToSave.imagePtr = currentImagePtr;
				saveToQueue->queue.push_back(dataToSave);
				GlobalLog::logger.Mz_AddLog(L"all Save");
				qDebug() << "图像数据和信息已推入保存队列。当前队列大小：" << saveToQueue->queue.size();
			}
			saveToQueue->cond.notify_one();
		}

		UpdateRealtimeData(cam_instance->RI->unifyParams);

		if (!afterImagePtr || afterImagePtr->empty()) {
			LOG_DEBUG(GlobalLog::logger, L"ImageProcess::run(): 准备发出信号时 afterImagePtr 为空或数据无效，发送备用图像");
		//	emit imageProcessed(backupImagePtr, info);

		}
		else {
			emit imageProcessed(afterImagePtr, info);

		}

	} // while 循环结束

	qDebug() << "ImageProcess::run() (thread finished).";
}