#include "Imageprocess_Image.h" 
#include "MZ_VC3000H.h"
#include <string>
#include "CapacitanceProgram.h"
#include "MZ_VC5000.h"
#include "MZ_PCI1230.h"

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
	
		cv::Mat currentImagePtr;
		{
			std::unique_lock<std::mutex> lock(m_inputQueue->mutex);

			m_inputQueue->cond.wait(lock, [this] {
				return !m_inputQueue->queue.empty() || m_inputQueue->stop_flag.load() || thread_stopFlag.load();
				});

			if (thread_stopFlag.load() || (m_inputQueue->stop_flag.load() && m_inputQueue->queue.empty())) {
				continue;
			}

			std::shared_ptr<cv::Mat> TempImagePtr = m_inputQueue->queue.front();

	
			if (!TempImagePtr || !isMatSafe(*TempImagePtr)) {
				LOG_DEBUG(GlobalLog::logger, L"ptr null or Mat unsafe");
				qWarning() << "Imageprocess_Image::run(): 准备发出信号时 TempImagePtr为空或数据无效，跳过发出信号。";
				m_inputQueue->queue.pop_front();
				continue;
			}
			else {
				qDebug() << "TempImagePtr not empty() and Mat is safe";
			}
			currentImagePtr = TempImagePtr->clone();
			m_inputQueue->queue.pop_front();
			std::cout << "image has output m_inputQueue->queue.pop_front():" << m_inputQueue->queue.size() << std::endl;
		}

	/*	if (!currentImagePtr || !isMatSafe(*currentImagePtr)) {
			continue;
		}*/

		if (GlobalPara::changed.load() == true) continue;
		cv::Mat afterImagePtr;

		cv::Mat backupImagePtr = currentImagePtr.clone();


		int ret = -1;

		if (cam_instance->video == false&&GlobalPara::AlogReady==true) // 非推流的情况
		{

			QElapsedTimer timer;
			timer.start();
			for (int i = 0; i < cam_instance->RI->m_PaintData.size(); ++i) {
				cam_instance->RI->m_PaintData[i].value = ""; // 清空实际值
				cam_instance->RI->m_PaintData[i].result = 0; // 设置结果为 NG
			}

			if (cam_instance->indentify == "NaYin") {
				ret = ExportSpace::RunStamp(currentImagePtr, 1, 0, LearnPara::inParam2);
				OutStampResParam para;
				ExportSpace::ResultOutStamp(afterImagePtr, para, 0);
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
				ret = ExportSpace::RunPlate(currentImagePtr, 0, cam_instance->DI.Angle);
				OutPlateResParam para;
				ExportSpace::ResultOutPlate(afterImagePtr, para, 0);
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
				ret = ExportSpace::RunLift(currentImagePtr, 0, cam_instance->DI.Angle, true);
				OutLiftResParam para;
				ExportSpace::ResultOutLift(afterImagePtr, para, 0);
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
				ret = ExportSpace::RunYYGJ(currentImagePtr, cam_instance->DI.Angle, true);
				OutAbutResParam para;
				ExportSpace::ResultOutYYGJ(afterImagePtr);
				qint64 elapsed = timer.elapsed();
				qDebug() << cam_instance->cameral_name << "算法耗时：" << elapsed << "毫秒";
				if (elapsed >= 150) GlobalLog::logger.Mz_AddLog(L"alog process more than 150");
				QString logMsg = QString("YYGJ ret=%1").arg(ret);
				LOG_DEBUG(GlobalLog::logger, logMsg.toStdWString().c_str());
			}
			else if (cam_instance->indentify == "Abut") {
				InAbutParam inParam = LearnPara::inParam6;
				ret = ExportSpace::RunAbut(currentImagePtr, inParam);
				OutAbutResParam OutResParam;
				ExportSpace::ResultOutAbut(afterImagePtr, OutResParam);
				qint64 elapsed = timer.elapsed();
				cam_instance->RI->updateActualValues(OutResParam);
				cam_instance->RI->applyScaleFactors(cam_instance->DI.scaleFactor.load());
				ret = cam_instance->RI->judge_abut(OutResParam);
				if (ret == 1) ret = -1;
				qDebug() << cam_instance->cameral_name << "算法耗时：" << elapsed << "毫秒";
				if (elapsed >= 150) GlobalLog::logger.Mz_AddLog(L"alog process more than 150");
			}
			else if (cam_instance->indentify == "Rift") {
				ret = ExportSpace::RunLift(currentImagePtr, 1, cam_instance->DI.Angle, true);
				OutLiftResParam para;
				ExportSpace::ResultOutLift(afterImagePtr, para, 1);
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
				ret = ExportSpace::RunPlate(currentImagePtr, 1, cam_instance->DI.Angle);
				OutPlateResParam para;
				ExportSpace::ResultOutPlate(afterImagePtr, para, 1);
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
				ret = ExportSpace::RunStamp(currentImagePtr, 1, 1, LearnPara::inParam1);
				OutStampResParam para;
				ExportSpace::ResultOutStamp(afterImagePtr, para, 1);
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
			else if (cam_instance->indentify == "wg1")
			{
				ret=ExportSpace::RunVC1(currentImagePtr, LearnPara::inParam10);
				OutStampResParam para;
				ExportSpace::ResultOutVC1(afterImagePtr);
				qint64 elapsed = timer.elapsed();
				qDebug() << cam_instance->cameral_name << "算法耗时：" << elapsed << "毫秒";
	
				QString logMsg = QString("wg1 ret=%1").arg(ret);
				LOG_DEBUG(GlobalLog::logger, logMsg.toStdWString().c_str());
				}
			else if (cam_instance->indentify == "wg2")
			{
				ret=ExportSpace::RunVC2(currentImagePtr,LearnPara::inParam11);
				OutStampResParam para;
				ExportSpace::ResultOutVC2(afterImagePtr);
				qint64 elapsed = timer.elapsed();
				qDebug() << cam_instance->cameral_name << "算法耗时：" << elapsed << "毫秒";

				QString logMsg = QString("wg2 ret=%1").arg(ret);
				LOG_DEBUG(GlobalLog::logger, logMsg.toStdWString().c_str());
				}
			else if (cam_instance->indentify == "wg3")
			{
				ret=ExportSpace::RunVC3(currentImagePtr, LearnPara::inParam12);
				OutStampResParam para;
				ExportSpace::ResultOutVC3(afterImagePtr);
				qint64 elapsed = timer.elapsed();
				qDebug() << cam_instance->cameral_name << "算法耗时：" << elapsed << "毫秒";

				QString logMsg = QString("wg3 ret=%1").arg(ret);
				LOG_DEBUG(GlobalLog::logger, logMsg.toStdWString().c_str());
				}
			else if (cam_instance->indentify == "wg4")
			{
				ret=ExportSpace::RunVC4(currentImagePtr, LearnPara::inParam13);
				OutStampResParam para;
				ExportSpace::ResultOutVC4(afterImagePtr);
				qint64 elapsed = timer.elapsed();
				qDebug() << cam_instance->cameral_name << "算法耗时：" << elapsed << "毫秒";

				QString logMsg = QString("wg4 ret=%1").arg(ret);
				LOG_DEBUG(GlobalLog::logger, logMsg.toStdWString().c_str());
				}
			else {
				Sleep(20);
				afterImagePtr = backupImagePtr;
				ret = 0;
			}
			info.timeStr = QString::number(timer.elapsed()).toStdString();
		}
		else // 推流的情况
		{
			afterImagePtr = currentImagePtr;
			// 推流情况下无需额外的 afterImagePtr 检查
			if (afterImagePtr.empty()) qDebug() << "afterImagePtrptr is not null";
			else qDebug() << "afterImagePtrptr is null";

		}

		if (GlobalPara::cheatFlag.load() == false)info.ret = ret;


		// 拍照的情况
		if (cam_instance->photo.load() == true)
		{
			std::shared_ptr<cv::Mat> ImagePtr = std::make_shared<cv::Mat>(backupImagePtr.clone());
			emit ImageLoaded(ImagePtr);
			qDebug() << "start photo learn";
			if (cam_instance->learn.load() == true) emit Learn();
			cam_instance->photo.store(false);
			cam_instance->learn.store(false);
		}

		if (cam_instance->DI.Shield == true) ret = 0;


		if (GlobalPara::envirment == GlobalPara::IPCEn && ret == 0)//非本地运行的情况
		{

			bool outputSignalInvert = true; 
			if (cam_instance->indentify == "wg4")  outputSignalInvert = false;
			int durationMs = 100; // 脉冲持续时间
			// 第二次调用,如果OK给true信号
			int result;
			if (GlobalPara::ioType == GlobalPara::VC3000H)result = PCI::pci().setOutputMode(cam_instance->pointNumber.load(), outputSignalInvert ? true : false, durationMs);
			else if (GlobalPara::ioType == GlobalPara::VC5000)result = VC5000DLL::vc5000_inst().setoutput(cam_instance->pointNumber.load(), outputSignalInvert ? true : false);
			else if(GlobalPara::ioType==GlobalPara::PCI1230) result= PCI1230DLL::pci1230_inst().WriteOutput(cam_instance->pointNumber.load(), outputSignalInvert ? true : false);
			QString logMsg = QString("相机：%1,第二次setOutputMode() 返回值: %2").arg(cam_instance->cameral_name).arg(result);
			LOG_DEBUG(GlobalLog::logger, logMsg.toStdWString().c_str());


		}
		else if (GlobalPara::envirment == GlobalPara::IPCEn && ret == -1)//非本地运行的情况
		{
			bool outputSignalInvert = false;
			if (cam_instance->indentify == "wg4")  outputSignalInvert = true;
			int durationMs = 100; // 脉冲持续时间
			int result;
			if(GlobalPara::ioType==GlobalPara::VC3000H) result = PCI::pci().setOutputMode(cam_instance->pointNumber.load(), outputSignalInvert ? true : false, durationMs);
			else if (GlobalPara::ioType == GlobalPara::VC5000)result = VC5000DLL::vc5000_inst().setoutput(cam_instance->pointNumber.load(), outputSignalInvert ? true : false);
			else if (GlobalPara::ioType == GlobalPara::PCI1230) result = PCI1230DLL::pci1230_inst().WriteOutput(cam_instance->pointNumber.load(), outputSignalInvert ? true : false);
			QString logMsg = QString("相机名称:%1,第三次setOutputMode() 返回值: %2").arg(cam_instance->cameral_name).arg(result);
			LOG_DEBUG(GlobalLog::logger, logMsg.toStdWString().c_str());
		}

		// 在使用 backupImagePtr 和 afterImagePtr 之前，再次检查它们的安全性
		QImage imgToSave;
		if (isMatSafe(backupImagePtr)) {
			imgToSave = convertMatToQImage(backupImagePtr);
		}
		else {
			qWarning() << "Backup image is not safe, cannot convert to QImage for saving/display.";
			continue;
		}

		QImage imgToPlay;
		bool afterImageSafe = false;
		if (isMatSafe(afterImagePtr)) {
			imgToPlay = convertMatToQImage(afterImagePtr);
			afterImageSafe = true;
		}
		else {
			qWarning() << "After process image is not safe, cannot convert to QImage for display.";
			continue;
		}


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
				if (!imgToSave.isNull()) { // 检查 imgToSave 是否成功转换
					currentSaveData.image = imgToSave;
					saveToQueue->queue.push_back(currentSaveData);
					GlobalLog::logger.Mz_AddLog(L"pre Save");
					qDebug() << "图像数据和信息已推入保存队列。当前队列大小：" << saveToQueue->queue.size();
				}
			}
			else if (cam_instance->DI.saveflag.load() == 2 && info.ret == 0)
			{
				qDebug() << "OK 结果，按设置不保存。当前队列大小：" << saveToQueue->queue.size();
			}
			else
			{
				if (!imgToSave.isNull()) { // 检查 imgToSave 是否成功转换
					currentSaveData.image = imgToSave;
					saveToQueue->queue.push_back(currentSaveData);
					GlobalLog::logger.Mz_AddLog(L"all Save");
					qDebug() << "图像数据和信息已推入保存队列。当前队列大小：" << saveToQueue->queue.size();
				}
			}
			saveToQueue->cond.notify_one();
		}

		info.paintDataSnapshot = cam_instance->RI->m_PaintData;

		// 替换函数末尾对 afterImagePtr 的检查
		if (!afterImageSafe) {
			LOG_DEBUG(GlobalLog::logger, L"Imageprocess_Image::run(): 准备发出信号时 afterImagePtr 为空或数据无效，发送备用图像");
			emit imageProcessed_QImage(imgToSave, info);

		}
		else {
			emit imageProcessed_QImage(imgToPlay, info);

		}

	} // while 循环结束

	qDebug() << "Imageprocess_Image::run() (thread finished).";
} // run() 函数结束
