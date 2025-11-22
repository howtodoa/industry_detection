#include "Imageprocess_Image.h" 
#include "MZ_VC3000H.h"
#include <string>
#include "CapacitanceProgram.h"

Imageprocess_Image::Imageprocess_Image(Cameral* cam, SaveQueue* m_saveQueue, QObject* parent)
	: ImageProcess(cam, m_saveQueue, parent)
{

}

Imageprocess_Image::~Imageprocess_Image()
{

}

void Imageprocess_Image::run()
{
	qDebug() << "Imageprocess_Image::run() (thread started) in QThread ID:" << QThread::currentThreadId();

	if (!m_inputQueue) {
		qCritical() << "Imageprocess_Image::run(): Input queue not available! Image processing cannot start.";
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
				qWarning() << "Imageprocess_Image::run(): 准备发出信号时 currentImagePtr 为空或数据无效，跳过发出信号。";
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

			QElapsedTimer timer;
			timer.start();  // 开始计时
			for (int i = 0; i < cam_instance->RI->m_PaintData.size(); ++i) {
				cam_instance->RI->m_PaintData[i].value = ""; // 清空实际值
				cam_instance->RI->m_PaintData[i].result = 0; // 设置结果为 NG
			}

			if (cam_instance->indentify == "NaYin") {
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
					cam_instance->RI->scaleDimensions(para, cam_instance->DI.scaleFactor.load());
					ret = cam_instance->RI->judge_plate(para);
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

			bool outputSignalInvert = true;
			int durationMs = 100; // 脉冲持续时间
			// 第二次调用,如果OK给true信号
			int result = PCI::pci().setOutputMode(cam_instance->pointNumber.load(), outputSignalInvert ? true : false, durationMs);
			QString logMsg = QString("相机：%1,第二次setOutputMode() 返回值: %2").arg(cam_instance->cameral_name).arg(result);
			LOG_DEBUG(GlobalLog::logger, logMsg.toStdWString().c_str());


		}
		else if (GlobalPara::envirment == GlobalPara::IPCEn && ret == -1)//非本地运行的情况
		{
			bool outputSignalInvert = false;
			int durationMs = 100; // 脉冲持续时间
			int result = PCI::pci().setOutputMode(cam_instance->pointNumber.load(), outputSignalInvert ? true : false, durationMs);

			QString logMsg = QString("相机名称:%1,第三次setOutputMode() 返回值: %2").arg(cam_instance->cameral_name).arg(result);
			LOG_DEBUG(GlobalLog::logger, logMsg.toStdWString().c_str());
		}

		QImage imgToSave=convertMatToQImage(*backupImagePtr);
		QImage imgToPlay=convertMatToQImage(*afterImagePtr);

		//存图
		if (cam_instance->DI.saveflag.load() > 1 && cam_instance->video == false)
		{
			std::unique_lock<std::mutex> lock(saveToQueue->mutex);

			SaveData currentSaveData;
			currentSaveData.savePath_Pre = dataToSave.savePath_Pre;
			currentSaveData.work_path = dataToSave.work_path;
			currentSaveData.savePath_OK = dataToSave.savePath_OK;
			currentSaveData.savePath_NG = dataToSave.savePath_NG;

			if (saveToQueue->queue.size() > 100)
			{
				GlobalLog::logger.Mz_AddLog(L"deque size more than 100");
			}
			else if (cam_instance->DI.saveflag.load() == 2 && info.ret == -1)
			{
				currentSaveData.image = imgToSave;
				saveToQueue->queue.push_back(currentSaveData);
				GlobalLog::logger.Mz_AddLog(L"pre Save");
				qDebug() << "图像数据和信息已推入保存队列。当前队列大小：" << saveToQueue->queue.size();
			}
			else if (cam_instance->DI.saveflag.load() == 2 && info.ret == 0)
			{
				qDebug() << "图像数据和信息已推入保存队列。当前队列大小：" << saveToQueue->queue.size();
			}
			else
			{
				currentSaveData.image = imgToSave;
				saveToQueue->queue.push_back(currentSaveData);
				GlobalLog::logger.Mz_AddLog(L"all Save");
				qDebug() << "图像数据和信息已推入保存队列。当前队列大小：" << saveToQueue->queue.size();
			}
			saveToQueue->cond.notify_one();
		}
		int num = 5;
		while (cam_instance->ui_signal.load() != false)
		{
			Sleep(5);
			num += 5;
			if (num > 60) {
				cam_instance->ui_signal.store(false);
				break;
			}
		}

		if (!afterImagePtr || afterImagePtr->empty()) {
			LOG_DEBUG(GlobalLog::logger, L"Imageprocess_Image::run(): 准备发出信号时 afterImagePtr 为空或数据无效，发送备用图像");
			emit imageProcessed_QImage(imgToSave, info);

		}
		else {
			emit imageProcessed_QImage(imgToPlay, info);

		}

	} // while 循环结束

	qDebug() << "Imageprocess_Image::run() (thread finished).";
} // run() 函数结束
