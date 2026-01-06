#include "imageprocess_brader.h"
#include "imageprocess.h"
#include "MZ_VC3000H.h"
#include <string>
#include "CapacitanceProgram.h"

Imageprocess_Brader::Imageprocess_Brader(Cameral* cam, SaveQueue* m_saveQueue, QObject* parent)
	: ImageProcess(cam, m_saveQueue, parent)
{

}

Imageprocess_Brader::~Imageprocess_Brader()
{

}

void Imageprocess_Brader::run()
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
		if(GlobalPara::AlogReady !=true) continue;

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
			else if (cam_instance->indentify == "Top") {
				InTopParam inpara;
				ret = BraidedTapeSpace::RunTop(*currentImagePtr, LearnPara::inParam3);
				OutTopParam para;
				BraidedTapeSpace::ResultOutTop(*afterImagePtr, para);
				qint64 elapsed = timer.elapsed();
				qDebug() << cam_instance->cameral_name << "算法耗时：" << elapsed << "毫秒";
				if (elapsed >= 150) GlobalLog::logger.Mz_AddLog(L"alog process more than 150");
				if (ret == 0) {
					cam_instance->RI->scaleDimensions(para, cam_instance->DI.scaleFactor.load());
					ret = cam_instance->RI->judge_top(para);
				}
				else if (ret == 1 || ret == 4 || ret == 7)
				{
					//	cam_instance->RI->m_PaintData[0].result = -1;
					if (GlobalPara::NG_Count_Able.load() == true)	cam_instance->RI->m_PaintData[0].count++;
					ret = -1;
				}
				else if (ret == 2) {
					cam_instance->noneDisplay.store(true);
					if (cam_instance->DI.EmptyIsOK == true) ret = 0;
					else
					{
						//		cam_instance->RI->m_PaintData[0].result = -1;
						if (GlobalPara::NG_Count_Able.load() == true)	cam_instance->RI->m_PaintData[0].count++;
						ret = -1;
					}
					QString logMsg = QString("Top ret=%1").arg(ret);
					LOG_DEBUG(GlobalLog::logger, logMsg.toStdWString().c_str());
				}
				else ret = -1;
			}
			else if (cam_instance->indentify == "Side") {
				InSideParam inpara = LearnPara::inParam4;
				if (cam_instance->ten != 0)
				{
					inpara.al_core = true;
					cam_instance->ten -= 1;
				}
				ret = BraidedTapeSpace::RunSide(*currentImagePtr, inpara);
				if (inpara.al_core == true && ret != 0) cam_instance->ten += 1;
				OutSideParam para;
				BraidedTapeSpace::ResultOutSide(*afterImagePtr, para);
				qint64 elapsed = timer.elapsed();
				qDebug() << cam_instance->cameral_name << "算法耗时：" << elapsed << "毫秒";
				if (elapsed >= 150) GlobalLog::logger.Mz_AddLog(L"alog process more than 150");
				if (ret == 0) {
					QElapsedTimer timer;
					timer.start();  // 开始计时

					cam_instance->RI->scaleDimensions(para, cam_instance->DI.scaleFactor.load());
					ret = cam_instance->RI->judge_side(para);

					qint64 elapsed = timer.elapsed();  // 返回毫秒数
					qDebug() << "执行 scaleDimensions + judge_pin 耗时:" << elapsed << "ms";
				}
				else if (ret == 2) {
					cam_instance->noneDisplay.store(true);
					if (cam_instance->DI.EmptyIsOK == true) ret = 0;
					else
					{
						//	cam_instance->RI->m_PaintData[0].result = -1;
						if (GlobalPara::NG_Count_Able.load() == true)	cam_instance->RI->m_PaintData[0].count++;
						ret = -1;
					}
					QString logMsg = QString("Side ret=%1").arg(ret);
					LOG_DEBUG(GlobalLog::logger, logMsg.toStdWString().c_str());
				}
				else if (ret == 1)
				{
					//	cam_instance->RI->m_PaintData[0].result = -1;
					if (GlobalPara::NG_Count_Able.load() == true)	cam_instance->RI->m_PaintData[0].count++;
					//ret = -1;
				}
				else if (ret == 5 || ret == 6)
				{
					//		cam_instance->RI->m_PaintData[0].result = -1;
					if (GlobalPara::NG_Count_Able.load() == true)	cam_instance->RI->m_PaintData[0].count++;
				}
				else ret = -1;
			}
			else if (cam_instance->indentify == "Pin") {
				ret = BraidedTapeSpace::RunPin(*currentImagePtr, LearnPara::inParam5);
				OutPinParam para;
				BraidedTapeSpace::ResultOutPin(*afterImagePtr, para);
				qint64 elapsed = timer.elapsed();
				qDebug() << cam_instance->cameral_name << "算法耗时：" << elapsed << "毫秒";
				if (elapsed >= 150) GlobalLog::logger.Mz_AddLog(L"alog process more than 150");
				if (ret == 0) {
					QElapsedTimer timer;
					timer.start();  // 开始计时

					cam_instance->RI->scaleDimensions(para, cam_instance->DI.scaleFactor.load());
					ret = cam_instance->RI->judge_pin(para);

					qint64 elapsed = timer.elapsed();  // 返回毫秒数
					qDebug() << "执行 scaleDimensions + judge_pin 耗时:" << elapsed << "ms";
				}
				else if (ret == 1)
				{
					//	cam_instance->RI->m_PaintData[0].result = -1;
					if (GlobalPara::NG_Count_Able.load() == true)	cam_instance->RI->m_PaintData[0].count++;
					//ret = -1;
				}
				else if (ret == 2) {
					cam_instance->noneDisplay.store(true);
					if (cam_instance->DI.EmptyIsOK == true) ret = 0;
					else
					{
						//	cam_instance->RI->m_PaintData[0].result = -1;
						if (GlobalPara::NG_Count_Able.load() == true)	cam_instance->RI->m_PaintData[0].count++;
						ret = -1;
					}
					QString logMsg = QString("Pin ret=%1").arg(ret);
					LOG_DEBUG(GlobalLog::logger, logMsg.toStdWString().c_str());
				}
				else if (ret == 3 || ret == 8)
				{
					//	cam_instance->RI->m_PaintData[0].result = -1;
					if (GlobalPara::NG_Count_Able.load() == true)	cam_instance->RI->m_PaintData[0].count++;
					QString logMsg = QString("Pin ret=%1").arg(ret);
					LOG_DEBUG(GlobalLog::logger, logMsg.toStdWString().c_str());
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
			bool outputSignalInvert;
			if (GlobalPara::changed.load() == false)	outputSignalInvert = false;
			else outputSignalInvert = true;

			int durationMs = 10; // 脉冲持续时间
			// 第二次调用,如果OK给true信号
			int result = PCI::pci().setOutputMode(cam_instance->pointNumber.load(), outputSignalInvert ? true : false, durationMs);
			QString logMsg = QString("相机：%1,第二次bradersetOutputMode() 返回值: %2").arg(cam_instance->cameral_name).arg(result);
			LOG_DEBUG(GlobalLog::logger, logMsg.toStdWString().c_str());
		}
		else if (GlobalPara::envirment == GlobalPara::IPCEn && ret == -1 || ret == 1 || ret >= 3)//非本地运行的情况
		{

			if (0)
			{
				bool outputSignalInvert = true;
				int durationMs = 10; // 脉冲持续时间
				int result;
				GlobalPara::changed.store(true);
				// 第二次调用,如果OK给false信号
				result = PCI::pci().setOutputMode(cam_instance->pointNumber.load(), outputSignalInvert ? false : true, durationMs);
				Sleep(10);
				result = PCI::pci().setOutputMode(cam_instance->pointNumber.load(), outputSignalInvert ? true : false, durationMs);
				Sleep(300);
				result = PCI::pci().setOutputMode(cam_instance->pointNumber.load(), outputSignalInvert ? false : true, durationMs);

				//	PCI::pci().setoutput(cam_instance->pointNumber.load(), false);

				std::cout << "change:" << GlobalPara::changed.load();
				emit SetButtonBackground("red");
				QString logMsg = QString("相机：%1,第三次setOutputMode() 返回值: %2").arg(cam_instance->cameral_name).arg(result);
				LOG_DEBUG(GlobalLog::logger, logMsg.toStdWString().c_str());
			}
			else
			{
				bool outputSignalInvert = true;

				int durationMs = 10; // 脉冲持续时间
				int result;
				// 第二次调用,如果OK给false信号
				result = PCI::pci().setOutputMode(cam_instance->pointNumber.load(), outputSignalInvert ? true : false, durationMs);
				GlobalPara::changed.store(true);
				Sleep(200);
				GlobalPara::changed.store(false);
				emit StartGetIn();
				//	emit SetButtonBackground("red");
				QString logMsg = QString("相机：%1,第三次setOutputMode() 返回值: %2").arg(cam_instance->cameral_name).arg(result);
				LOG_DEBUG(GlobalLog::logger, logMsg.toStdWString().c_str());
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
			else if (cam_instance->DI.saveflag.load() == 2 && (info.ret == -1 || info.ret >= 3))
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

		PaintSend(cam_instance->RI->m_PaintData);

		if (!afterImagePtr || afterImagePtr->empty()) {
			LOG_DEBUG(GlobalLog::logger, L"ImageProcess::run(): 准备发出信号时 afterImagePtr 为空或数据无效，发送备用图像");
			emit imageProcessed_Brader(backupImagePtr, info);

		}
		else {
			emit imageProcessed_Brader(afterImagePtr, info);

		}

	} // while 循环结束

	qDebug() << "ImageProcess::run() (thread finished).";
}