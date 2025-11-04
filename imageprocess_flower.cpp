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


		if (GlobalPara::changed.load() == true && cam_instance->learn.load() == false) continue;

		std::shared_ptr<cv::Mat> afterImagePtr = std::make_shared<cv::Mat>();
		std::shared_ptr<cv::Mat> backupImagePtr = std::make_shared<cv::Mat>(currentImagePtr->clone());
		cv::Mat image;
		int ret = -1;

		if (cam_instance->video == false) // 非推流的情况
		{

			QElapsedTimer timer;
			timer.start();  // 开始计时
			for (int i = 0; i < cam_instance->RI->m_PaintData.size(); ++i) {
				cam_instance->RI->m_PaintData[i].value = ""; // 清空实际值
				cam_instance->RI->m_PaintData[i].result = 0; // 设置结果为 NG
			}

             if (cam_instance->indentify == "FlowerPin")
			{
				ret = ExportFlowerSpace::RunPosFlowerPin(*currentImagePtr, LearnPara::inParam7);
				OutFlowerPinResParam para;
				ExportFlowerSpace::ResultOutPosFlowerPin(*afterImagePtr, para);
				qint64 elapsed = timer.elapsed();
				qDebug() << cam_instance->cameral_name << "算法耗时：" << elapsed << "毫秒";
				if (ret == 0) {
					m_inputQueue->process_flag.store(true);
					//调用算法第二个接口
					//
					m_inputQueue->process_flag.store(false);
					cam_instance->RI->updateActualValues(para);
					cam_instance->RI->applyScaleFactors(cam_instance->DI.scaleFactor.load());
					ret = cam_instance->RI->judge_flower_pin(para);
					if (ret == 1) ret = -1;
				}
				else if (ret == 2)
				{
					continue;
				}
				else 
				{
					// --- 赋单值数据 ---
					para.flowerNum = 5;            // 花瓣数量
					para.areaFoil = 2.345f;        // 箔裂面积
					para.disFlw2L = 1.05f;         // 花到L2距离
					para.disFlw2Pin2 = 8.123f;     // 最后一朵花到针距离 (NG/OK检测项)
					para.disFlw2Pin = 3.98f;       // 第一朵花到针距离
					para.disFlowerAngle = 90.5f;   // 花的角度
					para.disPinAngle = -15.0f;     // 针的角度
					para.disL2Heigh = 0.55f;       // L2的高度
					
					para.flowerArea = { 10.1f, 9.8f, 10.3f, 12.01f, 9.9f }; // 12.01f 可能是 NG 值

					para.flowetLength = { 5.2f, 5.1f, 5.0f, 4.9f, 5.3f ,2.3f,2.3f,2.5f};

					para.allFlowerLength = { 0.1f, 0.1f, 0.1f, 0.1f, 0.1f };

					cam_instance->RI->updateActualValues(para);
					cam_instance->RI->applyScaleFactors(cam_instance->DI.scaleFactor.load());
					ret = cam_instance->RI->judge_flower_pin(para);

				}
			}
			else if (cam_instance->indentify == "FlowerPinNeg")
			{
				ret = ExportFlowerSpace::RunNegFlowerPin(*currentImagePtr, LearnPara::inParam8);
				OutFlowerPinResParam para;
				ExportFlowerSpace::ResultOutNegFlowerPin(*afterImagePtr, para);
				qint64 elapsed = timer.elapsed();
				qDebug() << cam_instance->cameral_name << "算法耗时：" << elapsed << "毫秒";
				if (ret == 0) {
					cam_instance->RI->updateActualValues(para);
					cam_instance->RI->applyScaleFactors(cam_instance->DI.scaleFactor.load());
					ret = cam_instance->RI->judge_flower_pin(para);
				}
				else ret = -1;
			}
			else if (cam_instance->indentify == "FlowerLook")
			{
				ret = ExportFlowerSpace::RunLookFlowerPin(*currentImagePtr, LearnPara::inParam9);
				OutLookPinResParam para; 
				ExportFlowerSpace::ResultOutLookFlowerPin(*afterImagePtr, para);
				qint64 elapsed = timer.elapsed();
				qDebug() << cam_instance->cameral_name << "算法耗时：" << elapsed << "毫秒";
				if (ret == 0) {
					cam_instance->RI->updateActualValues(para);
					cam_instance->RI->applyScaleFactors(cam_instance->DI.scaleFactor.load());
					ret = cam_instance->RI->judge_look(para);
				}
				else ret = -1;
			}
			info.timeStr = QString::number(timer.elapsed()).toStdString();
		}
		else // 推流的情况
		{
			afterImagePtr = currentImagePtr;
			if (afterImagePtr) qDebug() << "afterImagePtrptr is not null";
			else qDebug() << "afterImagePtrptr is null";
			emit imageProcessed_Brader(afterImagePtr, info);
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

		}
		else if (GlobalPara::envirment == GlobalPara::IPCEn && ret == -1 || ret == 1 || ret == 3)//非本地运行的情况
		{

		}

		//存图
		if (cam_instance->DI.saveflag.load() > 1 && cam_instance->video == false)
		{
			std::unique_lock<std::mutex> lock(saveToQueue->mutex);
			if (saveToQueue->queue.size() > 100)
			{
				GlobalLog::logger.Mz_AddLog(L"deque size more than 100");
			}
			else if (cam_instance->DI.saveflag.load() == 2 && (info.ret == -1 || info.ret == 3))
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
			emit imageProcessed(backupImagePtr, info);

		}
		else {
			emit imageProcessed(afterImagePtr, info);

		}

	} // while 循环结束

	qDebug() << "ImageProcess::run() (thread finished).";
}