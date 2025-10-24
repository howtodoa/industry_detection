#include "Imageprocess_plate.h"
#include "imageprocess.h"
#include "MZ_VC3000H.h"
#include <string>
#include "CapacitanceProgram.h"

Imageprocess_Plate::Imageprocess_Plate(Cameral* cam, SaveQueue* m_saveQueue, QObject* parent)
	: ImageProcess(cam, m_saveQueue, parent)
{

}

Imageprocess_Plate::~Imageprocess_Plate()
{

}

void Imageprocess_Plate::run()
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
		if (GlobalPara::changed.load() == true) continue;
		std::shared_ptr<cv::Mat> afterImagePtr = std::make_shared<cv::Mat>();
		std::shared_ptr<cv::Mat> backupImagePtr = std::make_shared<cv::Mat>(currentImagePtr->clone());
		cv::Mat image;
		int ret = -1;

		if (cam_instance->video == false) // 非推流的情况
		{
			if (GlobalPara::envirment == GlobalPara::IPCEn) // 非本地运行的情况
			{
				// 先给复位信号false
				//bool outputSignalInvert = false;
				//int durationMs = 100; // 脉冲持续时间
				//int result = PCI::pci().setOutputMode(cam_instance->pointNumber.load(), outputSignalInvert ? true : false, durationMs);

				//QString logMsg = QString("相机名称:%1,第一次setOutputMode() 返回值: %2").arg(cam_instance->cameral_name).arg(result);
				//LOG_DEBUG(GlobalLog::logger, logMsg.toStdWString().c_str());
			}

			QElapsedTimer timer;
			timer.start();  // 开始计时
			//for (int i = 0; i < cam_instance->RI->m_PaintData.size(); ++i) {
			//	cam_instance->RI->m_PaintData[i].value = ""; // 清空实际值
			//	cam_instance->RI->m_PaintData[i].result = 0; // 设置结果为 NG
			//}

			if (cam_instance->indentify == "NaYin") {
				/*		LearnPara::inParam2.scoresNegLimit = cam_instance->RI->score_neg;
						LearnPara::inParam2.scoresPosLimit = cam_instance->RI->score_pos;*/
				ret = ExportSpace::RunStamp(*currentImagePtr, 1, 0, LearnPara::inParam2);
				OutStampResParam para;
				ExportSpace::ResultOutStamp(*afterImagePtr, para, 0);
				qint64 elapsed = timer.elapsed();
				qDebug() << cam_instance->cameral_name << "算法耗时：" << elapsed << "毫秒";
				if (elapsed >= 150) GlobalLog::logger.Mz_AddLog(L"alog process more than 150");
				if (ret == 0) {
					cam_instance->RI->scaleDimensions(para, cam_instance->DI.scaleFactor.load());
					ret = cam_instance->RI->judge_stamp(para);
				}
				else if (ret == 2) {
					if (cam_instance->DI.EmptyIsOK == true) ret = 0;
					else ret = -1;
				}
				else ret = -1;
				QString logMsg = QString("NaYin ret=%1").arg(ret);
				LOG_DEBUG(GlobalLog::logger, logMsg.toStdWString().c_str());
			}
			else if (cam_instance->indentify == "Plate") {
				ret = ExportSpace::RunPlate(*currentImagePtr, 0, cam_instance->DI.Angle);
				OutPlateResParam para;
				ExportSpace::ResultOutPlate(*afterImagePtr, para, 0);
				qint64 elapsed = timer.elapsed();
				qDebug() << cam_instance->cameral_name << "算法耗时：" << elapsed << "毫秒";
				if (elapsed >= 150) GlobalLog::logger.Mz_AddLog(L"alog process more than 150");
				if (ret == 0) {
					cam_instance->RI->updateActualValues(para);
					cam_instance->RI->applyScaleFactors(cam_instance->DI.scaleFactor.load());
					ret = cam_instance->RI->judge_plate(para);
					if (ret == 1) ret = -1;
				}
				else if (ret == 2) {
					if (cam_instance->DI.EmptyIsOK == true) ret = 0;
					else ret = -1;

				}
				else ret = -1;
				QString logMsg = QString("Plate ret=%1").arg(ret);
				LOG_DEBUG(GlobalLog::logger, logMsg.toStdWString().c_str());
			}
			else if (cam_instance->indentify == "Lift") {
				ret = ExportSpace::RunLift(*currentImagePtr, 0, cam_instance->DI.Angle, true);
				OutLiftResParam para;
				ExportSpace::ResultOutLift(*afterImagePtr, para, 0);
				qint64 elapsed = timer.elapsed();
				qDebug() << cam_instance->cameral_name << "算法耗时：" << elapsed << "毫秒";
				if (elapsed >= 150) GlobalLog::logger.Mz_AddLog(L"alog process more than 200");
				if (ret == 0)
				{
					cam_instance->RI->scaleDimensions(para, cam_instance->DI.scaleFactor.load());

					ret = cam_instance->RI->judge_lift(para);

				}
				else if (ret == 2)
				{
					if (cam_instance->DI.EmptyIsOK == true) ret = 0;
					else ret = -1;

				}
				else ret = -1;
			}
			else if (cam_instance->indentify == "YYGJ") {
				ret = ExportSpace::RunYYGJ(*currentImagePtr, cam_instance->DI.Angle, true);
				OutAbutResParam para;
				ExportSpace::ResultOutYYGJ(*afterImagePtr);
				qint64 elapsed = timer.elapsed();
				qDebug() << cam_instance->cameral_name << "算法耗时：" << elapsed << "毫秒";
				if (elapsed >= 150) GlobalLog::logger.Mz_AddLog(L"alog process more than 150");
			}
			else if (cam_instance->indentify == "Abut") {
				InAbutParam inParam = LearnPara::inParam6;
				ret = ExportSpace::RunAbut(*currentImagePtr, inParam);
				OutAbutResParam OutResParam;
				ExportSpace::ResultOutAbut(*afterImagePtr, OutResParam);
				qint64 elapsed = timer.elapsed();
				cam_instance->RI->updateActualValues(OutResParam);
				cam_instance->RI->applyScaleFactors(cam_instance->DI.scaleFactor.load());
				ret = cam_instance->RI->judge_abut(OutResParam);
				if (ret == 1) ret = -1;
				qDebug() << cam_instance->cameral_name << "算法耗时：" << elapsed << "毫秒";
				if (elapsed >= 150) GlobalLog::logger.Mz_AddLog(L"alog process more than 150");
			}
			else if (cam_instance->indentify == "Rift") {
				ret = ExportSpace::RunLift(*currentImagePtr, 1, cam_instance->DI.Angle, true);
				OutLiftResParam para;
				ExportSpace::ResultOutLift(*afterImagePtr, para, 1);
				qint64 elapsed = timer.elapsed();
				qDebug() << cam_instance->cameral_name << "算法耗时：" << elapsed << "毫秒";
				if (elapsed >= 150) GlobalLog::logger.Mz_AddLog(L"alog process more than 150");
				if (ret == 0)
				{
					cam_instance->RI->scaleDimensions(para, cam_instance->DI.scaleFactor.load());
					ret = cam_instance->RI->judge_lift(para);
				}
				else if (ret == 2)
				{
					if (cam_instance->DI.EmptyIsOK == true) ret = 0;
					else ret = -1;
				}
				else ret = -1;
			}
			else if (cam_instance->indentify == "Carrier_Plate") {
				ret = ExportSpace::RunPlate(*currentImagePtr, 1, cam_instance->DI.Angle);
				OutPlateResParam para;
				ExportSpace::ResultOutPlate(*afterImagePtr, para, 1);
				qint64 elapsed = timer.elapsed();
				qDebug() << cam_instance->cameral_name << "算法耗时：" << elapsed << "毫秒";
				if (elapsed >= 150) GlobalLog::logger.Mz_AddLog(L"alog process more than 150");
				if (ret == 0) {
					cam_instance->RI->scaleDimensions(para, cam_instance->DI.scaleFactor.load());
					ret = cam_instance->RI->judge_plate(para);
				}
				else if (ret == 2) {
					if (cam_instance->DI.EmptyIsOK == true) ret = 0;
					else ret = -1;

				}
				else ret = -1;
				QString logMsg = QString("Carrier_Plate ret=%1").arg(ret);
				LOG_DEBUG(GlobalLog::logger, logMsg.toStdWString().c_str());
			}
			else if (cam_instance->indentify == "Carrier_NaYin") {
				/*			LearnPara::inParam1.scoresNegLimit = cam_instance->RI->score_neg;
							LearnPara::inParam1.scoresPosLimit = cam_instance->RI->score_pos;*/
				std::cout << " LearnPara::inParam1.textkernl: " << LearnPara::inParam1.textkernl;
				ret = ExportSpace::RunStamp(*currentImagePtr, 1, 1, LearnPara::inParam1);
				OutStampResParam para;
				ExportSpace::ResultOutStamp(*afterImagePtr, para, 1);
				qint64 elapsed = timer.elapsed();
				qDebug() << cam_instance->cameral_name << "算法耗时：" << elapsed << "毫秒";
				if (elapsed >= 150) GlobalLog::logger.Mz_AddLog(L"alog process more than 150");
				if (ret == 0) {
					cam_instance->RI->scaleDimensions(para, cam_instance->DI.scaleFactor.load());
					ret = cam_instance->RI->judge_stamp(para);
				}
				else if (ret == 2) {
					if (cam_instance->DI.EmptyIsOK == true) ret = 0;
					else ret = -1;
				}
				else ret = -1;
				QString logMsg = QString("Carrier_NaYin ret=%1").arg(ret);
				LOG_DEBUG(GlobalLog::logger, logMsg.toStdWString().c_str());
			}
			else if (cam_instance->indentify == "null") {
				Sleep(20);
				ret = 0;
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

			if (GlobalPara::MergePointNum == 0)
			{
				bool outputSignalInvert = true;
				int durationMs = 100; // 脉冲持续时间
				int result = PCI::pci().setOutputMode(cam_instance->pointNumber.load(), outputSignalInvert ? true : false, durationMs);
				QString logMsg = QString("相机名称:%1,第二次setOutputMode() 返回值: %2").arg(cam_instance->cameral_name).arg(result);
				LOG_DEBUG(GlobalLog::logger, logMsg.toStdWString().c_str());
			}
			else
			{
				std::unique_lock<std::mutex> lk(g_mutex);
				int num;
				if (cam_instance->indentify == "Plate") num = 0;
				else num = 1;
				g_cv.wait(lk, [num]() { return MergePointVec[num] == 2; });

				// 满足条件，写入自己的值
				MergePointVec[num] = 1;
				lk.unlock();
				g_cv.notify_all();
			}


		}
		else if (GlobalPara::envirment == GlobalPara::IPCEn && ret == -1)//非本地运行的情况
		{
			if (GlobalPara::MergePointNum == 0)
			{
				bool outputSignalInvert = false;
				int durationMs = 100; // 脉冲持续时间
				int result = PCI::pci().setOutputMode(cam_instance->pointNumber.load(), outputSignalInvert ? true : false, durationMs);

				QString logMsg = QString("相机名称:%1,第三次setOutputMode() 返回值: %2").arg(cam_instance->cameral_name).arg(result);
				LOG_DEBUG(GlobalLog::logger, logMsg.toStdWString().c_str());
			}
			else
			{
				std::unique_lock<std::mutex> lk(g_mutex);
				int num;
				if (cam_instance->indentify == "Plate") num = 0;
				else num = 1;
				g_cv.wait(lk, [num]() { return MergePointVec[num] == 2; });

				// 满足条件，写入自己的值
				MergePointVec[num] = 0;
				lk.unlock();
				g_cv.notify_all();
			}
		}

		//存图
		if (cam_instance->DI.saveflag.load() > 1 && cam_instance->video == false)
		{
			std::unique_lock<std::mutex> lock(saveToQueue->mutex);
			if (saveToQueue->queue.size() > 100)
			{
				GlobalLog::logger.Mz_AddLog(L"deque size more than 100");
			}
			else if (cam_instance->DI.saveflag.load() == 2 && info.ret == -1)
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

		if (!afterImagePtr || afterImagePtr->empty()) {
			LOG_DEBUG(GlobalLog::logger, L"ImageProcess::run(): 准备发出信号时 afterImagePtr 为空或数据无效，发送备用图像");
			emit imageProcessed(backupImagePtr, info);

		}
		else {
			emit imageProcessed(afterImagePtr, info);

		}

	} // while 循环结束

	qDebug() << "ImageProcess::run() (thread finished).";
} // run() 函数结束

