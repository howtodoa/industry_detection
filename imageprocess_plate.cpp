#include "Imageprocess_plate.h"
#include "imageprocess.h"
#include "MZ_VC3000H.h"
#include <string>
#include "CapacitanceProgram.h"
#include "public.h"

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

			cam_instance->triggerNum.fetch_add(1);

			int curTrigger = cam_instance->triggerNum.fetch_add(1);

			QString logMsg = QString("Camera=%1 triggerNum=%2")
				.arg(cam_instance->cameral_name)
				.arg(curTrigger);

			LOG_DEBUG(GlobalLog::logger, logMsg.toStdWString().c_str());
 
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

		//复位
		if (GlobalPara::MergePointNum == 0)
		{
			bool outputSignalInvert = false;
			int durationMs = 100; // 脉冲持续时间
			int result = PCI::pci().setOutputMode(cam_instance->pointNumber.load(), outputSignalInvert ? true : false, durationMs);

			QString logMsg = QString("相机名称:%1,第三次setOutputMode() 返回值: %2").arg(cam_instance->cameral_name).arg(result);
			LOG_DEBUG(GlobalLog::logger, logMsg.toStdWString().c_str());
		}

		long long birthtime = QDateTime::currentMSecsSinceEpoch();
		if (cam_instance->video == false) // 非推流的情况
		{
	

			QElapsedTimer timer;
			timer.start();  // 开始计时
	

			if (cam_instance->indentify == "NaYin") {
				ret = ExportSpace::RunStamp(*currentImagePtr, 1, 0, LearnPara::inParam2);
				OutStampResParam para;
				ExportSpace::ResultOutStamp(*afterImagePtr, para, 0);
				qint64 elapsed = timer.elapsed();
				qDebug() << cam_instance->cameral_name << "算法耗时：" << elapsed << "毫秒";
				if (elapsed >= 150) GlobalLog::logger.Mz_AddLog(L"alog process more than 150");
				if (ret == 0) {
					cam_instance->RI->updateActualValues(para);
					cam_instance->RI->applyScaleFactors(cam_instance->DI.scaleFactor.load());
					ret = cam_instance->RI->judge_stamp(para);
					if (ret == 1) ret = -1;
				}
				else if (ret == 2) {
					QMap<QString, UnifyParam>& unifyParams = cam_instance->RI->unifyParams;
					for (auto it = unifyParams.begin(); it != unifyParams.end(); ++it)
					{
						const QString paramKey = it.key();
						UnifyParam& config = it.value();
						config.change_value();
					}
					if (cam_instance->DI.EmptyIsOK == true) ret = 0;
					else ret = -1;
				}
				else
				{
					QMap<QString, UnifyParam>& unifyParams = cam_instance->RI->unifyParams;
					for (auto it = unifyParams.begin(); it != unifyParams.end(); ++it)
					{
						const QString paramKey = it.key();
						UnifyParam& config = it.value();
						config.change_value();
					}
					ret = -1;
				}
				QString logMsg = QString("NaYin ret=%1").arg(ret);
				LOG_DEBUG(GlobalLog::logger, logMsg.toStdWString().c_str());
				qDebug() << cam_instance->cameral_name << "算法耗时：" << elapsed << "毫秒";
				info.timeStr = QString::number(elapsed).toStdString();
			}
			else if (cam_instance->indentify == "Plate") {
				int flagCopy = 0; // 这里的 0 是固定值
				int angleCopy = cam_instance->DI.Angle; // 拷贝 angle 的值

				// 深拷贝 cv::Mat 数据。这是线程安全的关键！
				cv::Mat imageCopy = currentImagePtr->clone();

				QElapsedTimer totalTimer; // 用于测量整个流程的总耗时
				totalTimer.start();

				// 2. 调用 C++11 超时函数
				int ret_from_call = callWithTimeout_cpp11([imageCopy, flagCopy, angleCopy]() mutable -> int {

					QElapsedTimer innerTimer;
					innerTimer.start();

					// ！！在子线程中执行 RunPlate，使用副本 ！！
					int innerResult = ExportSpace::RunPlate(imageCopy, flagCopy, angleCopy);

					qint64 elapsedMs = innerTimer.elapsed();
					LOG_DEBUG(GlobalLog::logger,
						QString("RunPlate Internal Timing (C++11): Execution Time: %1 ms, Returned: %2")
						.arg(elapsedMs)
						.arg(innerResult)
						.toStdWString()
						.c_str());

					return innerResult;
					}, 200, -1); 

				ret = ret_from_call;
				OutPlateResParam para;
				ExportSpace::ResultOutPlate(*afterImagePtr, para, 0);
				qint64 elapsed = timer.elapsed();
				qDebug() << cam_instance->cameral_name << "算法耗时：" << elapsed << "毫秒";
				info.timeStr = QString::number(elapsed).toStdString();
				if (elapsed >= 150) GlobalLog::logger.Mz_AddLog(L"alog process more than 150");
				if (ret == 0) {
					QElapsedTimer timer1;
					timer1.start();  // 开始计时
					//算法复杂度待优化
					cam_instance->RI->updateActualValues(para);
					cam_instance->RI->applyScaleFactors(cam_instance->DI.scaleFactor.load());
					ret = cam_instance->RI->judge_plate(para);
					qint64 elapsed = timer1.elapsed();  // 返回毫秒数
					qDebug() << "执行 updateActualValues + applyScaleFactors 耗时:" << elapsed << "ms";
					if (ret == 1) ret = -1;
				}
				else if (ret == 3) {
					
		QMap<QString, UnifyParam>& unifyParams = cam_instance->RI->unifyParams;
		for (auto it = unifyParams.begin(); it != unifyParams.end(); ++it)
		{
			const QString paramKey = it.key();
			UnifyParam& config = it.value();
			config.change_value();
		}
					if (cam_instance->DI.EmptyIsOK == true) ret = 0;
					else ret = -1;

				}
				else
				{
					QMap<QString, UnifyParam>& unifyParams = cam_instance->RI->unifyParams;
					for (auto it = unifyParams.begin(); it != unifyParams.end(); ++it)
					{
						const QString paramKey = it.key();
						UnifyParam& config = it.value();
						config.change_value();
					}
					ret = -1;
				}
				QString logMsg = QString("Plate ret=%1").arg(ret);
				LOG_DEBUG(GlobalLog::logger, logMsg.toStdWString().c_str());
			}
			else if (cam_instance->indentify == "Lift") {
				ret = ExportSpace::RunLift(*currentImagePtr, 0, cam_instance->DI.Angle, true);
				OutLiftResParam para;
				ExportSpace::ResultOutLift(*afterImagePtr, para, 0);
				qint64 elapsed = timer.elapsed();
				qDebug() << cam_instance->cameral_name << "算法耗时：" << elapsed << "毫秒";
				if (elapsed >= 150) GlobalLog::logger.Mz_AddLog(L"alog process more than 150");
				if (ret == 0)
				{
					cam_instance->RI->updateActualValues(para);
					cam_instance->RI->applyScaleFactors(cam_instance->DI.scaleFactor.load());
					cam_instance->RI->judge_lift(para);
				}
				else if (ret == 2)
				{
					QMap<QString, UnifyParam>& unifyParams = cam_instance->RI->unifyParams;
					for (auto it = unifyParams.begin(); it != unifyParams.end(); ++it)
					{
						const QString paramKey = it.key();
						UnifyParam& config = it.value();
						config.change_value();
					}
					if (cam_instance->DI.EmptyIsOK == true) ret = 0;
					else ret = -1;

				}
				else
				{
					QMap<QString, UnifyParam>& unifyParams = cam_instance->RI->unifyParams;
					for (auto it = unifyParams.begin(); it != unifyParams.end(); ++it)
					{
						const QString paramKey = it.key();
						UnifyParam& config = it.value();
						config.change_value();
					}
					ret = -1;
				}
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
				//ret = ExportSpace::RunAbut(*currentImagePtr, inParam);
				QElapsedTimer innerTimer;
				innerTimer.start();

				InAbutParam paramCopy = LearnPara::inParam6;

				// 深拷贝 cv::Mat 数据。这是线程安全的关键！
				cv::Mat imageCopy = currentImagePtr->clone();

				// 2. 调用 C++11 超时函数
				int ret_from_call = callWithTimeout_cpp11([imageCopy, paramCopy]() mutable -> int {

					QElapsedTimer innerTimer;
					innerTimer.start();

					// ！！在子线程中执行 RunAbut，使用副本 (mutable 允许非 const 引用绑定)
					int innerResult = ExportSpace::RunAbut(imageCopy, paramCopy);

					qint64 elapsedMs = innerTimer.elapsed();
					LOG_DEBUG(GlobalLog::logger,
						QString("RunAbut Internal Timing (C++11): Execution Time: %1 ms, Returned: %2")
						.arg(elapsedMs)
						.arg(innerResult)
						.toStdWString()
						.c_str());

					return innerResult;
					}, 200, -1);

				// 3. 将结果赋给外部 ret
				ret = ret_from_call;

				qint64 elapsedMs = innerTimer.elapsed();

				LOG_DEBUG(GlobalLog::logger,
					QString("RunAbut Internal Timing: Execution Time: %1 ms, Returned: %2")
					.arg(elapsedMs)
					.arg(ret)
					.toStdWString()
					.c_str());
				OutAbutResParam OutResParam;
				ExportSpace::ResultOutAbut(*afterImagePtr, OutResParam);
				qint64 elapsed = timer.elapsed();
				info.timeStr = QString::number(elapsed).toStdString();
				if (ret == 0)
				{
					cam_instance->RI->updateActualValues(OutResParam);
					cam_instance->RI->applyScaleFactors(cam_instance->DI.scaleFactor.load());
					ret = cam_instance->RI->judge_abut(OutResParam);
				}
				else if (ret == 3)
				{
					QMap<QString, UnifyParam>& unifyParams = cam_instance->RI->unifyParams;
					for (auto it = unifyParams.begin(); it != unifyParams.end(); ++it)
					{
						const QString paramKey = it.key();
						UnifyParam& config = it.value();
						config.change_value();
					}
					if (cam_instance->DI.EmptyIsOK == true) ret = 0;
					else ret = -1;
				}
				else ret = -1;
				qDebug() << cam_instance->cameral_name << "算法耗时：" << elapsed << "毫秒";
				if (elapsed >= 150) GlobalLog::logger.Mz_AddLog(L"alog process more than 150");
				QString logMsg = QString("Abut ret=%1").arg(ret);
				LOG_DEBUG(GlobalLog::logger, logMsg.toStdWString().c_str());
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
					cam_instance->RI->updateActualValues(para);
					cam_instance->RI->applyScaleFactors(cam_instance->DI.scaleFactor.load());
					cam_instance->RI->judge_lift(para);
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
					cam_instance->RI->updateActualValues(para);
					cam_instance->RI->applyScaleFactors(cam_instance->DI.scaleFactor.load());
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
					cam_instance->RI->updateActualValues(para);
					cam_instance->RI->applyScaleFactors(cam_instance->DI.scaleFactor.load());
					ret = cam_instance->RI->judge_stamp(para);
					if (ret == 1) ret = -1;

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
			emit imageProcessed(afterImagePtr, info);
			qDebug() << "this is in push";
			currentImagePtr.reset();
			backupImagePtr.reset();
			afterImagePtr.reset();
			continue;
		}

		if (GlobalPara::cheatFlag.load() == false)info.ret = ret;




		if (cam_instance->DI.Shield == true) ret = 0;

		qDebug() << "cam_instance->photo.load() :" << cam_instance->photo.load();
		qDebug() << "ret :" << ret;
		if (GlobalPara::envirment == GlobalPara::IPCEn && ret == 0)//非本地运行的情况
		{
			QString camId =QString::fromStdString(cam_instance->indentify);
			if (MergePointVec.contains(camId)==false)
			{
				bool outputSignalInvert = true;
				int durationMs = 100; // 脉冲持续时间
				int result = PCI::pci().setOutputMode(cam_instance->pointNumber.load(), outputSignalInvert ? true : false, durationMs);
				QString logMsg = QString("相机名称:%1,第二次setOutputMode() 返回值: %2").arg(cam_instance->cameral_name).arg(result);
				LOG_DEBUG(GlobalLog::logger, logMsg.toStdWString().c_str());
			}
			else if(cam_instance->photo.load() == false)
			{
				QElapsedTimer timer;
				timer.start();  // 开始计时

				std::unique_lock<std::mutex> lk(g_mutex);

				if (MergePointVec.contains(camId)) {
					MergePointVec[camId].push_back(1);
					MergePointVec[camId].time_id = birthtime;
				}

				lk.unlock();
				g_cv.notify_all();

				qint64 elapsed = timer.elapsed();

				QString logMsg = QString("cam:=%1,push=%2").arg(camId).arg(1);
				LOG_DEBUG(GlobalLog::logger, logMsg.toStdWString().c_str());
			}


		}
		else if (GlobalPara::envirment == GlobalPara::IPCEn &&  cam_instance->photo.load() == false)//非本地运行的情况
		{
			QString camId = QString::fromStdString(cam_instance->indentify);
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

				if (MergePointVec.contains(camId)) {
					MergePointVec[camId].push_back(0);
					MergePointVec[camId].time_id = birthtime;
				}

				lk.unlock();
				g_cv.notify_all();
				QString logMsg = QString("cam:=%1,push=%2").arg(camId).arg(0);
				LOG_DEBUG(GlobalLog::logger, logMsg.toStdWString().c_str());
			}
		}

		// 拍照的情况
		if (cam_instance->photo.load() == true)
		{
			emit ImageLoaded(currentImagePtr);
			qDebug() << "start photo learn";
			if (cam_instance->learn.load() == true) emit Learn();
			cam_instance->photo.store(false);
			cam_instance->learn.store(false);
		}

		//存图
		if (cam_instance->DI.saveflag.load() > 1 && cam_instance->video == false)
		{
			std::unique_lock<std::mutex> lock(saveToQueue->mutex);
			SaveData FinalToSave;
			if (saveToQueue->queue.size() > 100)
			{
				GlobalLog::logger.Mz_AddLog(L"deque size more than 100");
			}
			else if (cam_instance->DI.saveflag.load() == 2 && (info.ret == -1 || info.ret == 2 || info.ret == 3 || ret == 1))
			{
				QTime currentTime = QTime::currentTime();
				int hour = currentTime.hour();  // 0~23
				std::string hourStr = std::to_string(hour);
				FinalToSave.work_path = dataToSave.savePath_Pre + "/"+ hourStr + "/";
				FinalToSave.imagePtr = currentImagePtr;
				saveToQueue->queue.push_back(FinalToSave);
				qDebug() << "图像数据和信息已推入保存队列。当前队列大小：" << saveToQueue->queue.size();
			}
			else if (cam_instance->DI.saveflag.load() == 2 && info.ret == 0)
			{
				qDebug() << "图像数据和信息已推入保存队列。当前队列大小：" << saveToQueue->queue.size();
			}
			else
			{
				QTime currentTime = QTime::currentTime();
				int hour = currentTime.hour();  // 0~23
				std::string hourStr = std::to_string(hour);
				FinalToSave.work_path = dataToSave.savePath_Pre + "/" + hourStr + "/";
				FinalToSave.imagePtr = currentImagePtr;
				saveToQueue->queue.push_back(FinalToSave);
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

