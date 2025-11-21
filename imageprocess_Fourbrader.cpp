#include "imageprocess_Fourbrader.h"
#include "imageprocess.h"
#include "MZ_VC3000H.h"
#include <string>
#include "CapacitanceProgram.h"
#include "public.h"

Imageprocess_FourBrader::Imageprocess_FourBrader(Cameral* cam, SaveQueue* m_saveQueue, QObject* parent)
	: ImageProcess(cam, m_saveQueue, parent)
{

}

Imageprocess_FourBrader::~Imageprocess_FourBrader()
{

}

void Imageprocess_FourBrader::run()
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
//		QMap<QString, UnifyParam>& unifyParams = cam_instance->RI->unifyParams;
		//for (auto it = unifyParams.begin(); it != unifyParams.end(); ++it)
		//{
		//	const QString paramKey = it.key();

		//	UnifyParam& config = it.value();


		//	config.change_value();

	
		//}

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

				}
				else if (ret == 2) {
					if (cam_instance->DI.EmptyIsOK == true) ret = 0;
					else ret = -1;
				}
				else ret = -1;
				QString logMsg = QString("NaYin ret=%1").arg(ret);
				LOG_DEBUG(GlobalLog::logger, logMsg.toStdWString().c_str());
				qDebug() << cam_instance->cameral_name << "算法耗时：" << elapsed << "毫秒";
				info.timeStr = QString::number(elapsed).toStdString();
			}
			// 线扫 Xs
			else if (cam_instance->indentify == "Xs") {
				int algo_id = 0;
				cv::Mat imgCopy = currentImagePtr->clone();

				ALLResult result;
				qDebug() << "into XS process";	
				g_detector->Process(algo_id, imgCopy, result);
				qDebug() << "out XS process";
				ret = result.xsResult.NGResult == XS_NGReults::OK ? 0 : -1;
			
					cam_instance->RI->updateActualValues(result.xsResult);
					cam_instance->RI->applyScaleFactors(cam_instance->DI.scaleFactor.load());
					ret = cam_instance->RI->judge_xs(result.xsResult);


				*afterImagePtr = result.xsResult.dstImg.clone();
			}

			// 捺印 Ny
			else if (cam_instance->indentify == "Ny") {
				int algo_id = 1;
				cv::Mat imgCopy = currentImagePtr->clone();

				ALLResult result;
				g_detector->Process(algo_id, imgCopy, result);

				ret = result.nyResult.NGResult == NY_NGReults::OK ? 0 : -1;
				
				
					cam_instance->RI->updateActualValues(result.nyResult);
					cam_instance->RI->applyScaleFactors(cam_instance->DI.scaleFactor.load());
					ret = cam_instance->RI->judge_ny(result.nyResult);
			

				*afterImagePtr = result.nyResult.dstImg.clone();
			}

			// 底面 Bottom1
			else if (cam_instance->indentify == "Bottom1") {
				LOG_DEBUG(GlobalLog::logger, L"into Bottom1 process");
				int algo_id = 3;//胶帽
				cv::Mat imgCopy = currentImagePtr->clone();

				ALLResult result;
				g_detector_mutex.lock();

				g_detector->Process(algo_id, imgCopy, result);

				ret = result.crop_bootomResult.NGResult == Crop_Bottom_NGReults::OK ? 0 : -1;
			
		
					cam_instance->RI->updateActualValues(result.crop_bootomResult);
					cam_instance->RI->applyScaleFactors(cam_instance->DI.scaleFactor.load());
					// 你可以选择 JM 或 LK 图，或者做 merge
					ret = cam_instance->RI->judge_bottom(result.crop_bootomResult);
		

				// 你可以选择 JM 或 LK 图，或者做 merge
				*afterImagePtr = result.crop_bootomResult.JM_dstImg.clone();
				g_detector_mutex.unlock();
			}

			// 底面 Bottom2
			else if (cam_instance->indentify == "Bottom2") {
				LOG_DEBUG(GlobalLog::logger, L"into Bottom2 process");
				int algo_id = 2;//铝壳
				cv::Mat imgCopy = currentImagePtr->clone();

				ALLResult result;

				g_detector_mutex.lock();
		

				g_detector->Process(algo_id, imgCopy, result);

				ret = result.crop_bootomResult.NGResult == Crop_Bottom_NGReults::OK ? 0 : -1;
		
		
					cam_instance->RI->updateActualValues(result.crop_bootomResult);
					cam_instance->RI->applyScaleFactors(cam_instance->DI.scaleFactor.load());
					// 你可以选择 JM 或 LK 图，或者做 merge
					ret = cam_instance->RI->judge_bottom(result.crop_bootomResult);
				

				*afterImagePtr = result.crop_bootomResult.LK_dstImg.clone();

				g_detector_mutex.unlock();
			}
			info.timeStr = QString::number(timer.elapsed()).toStdString();

		}
		else if(cam_instance->video==true) // 推流的情况
		{
			afterImagePtr = currentImagePtr;
			if (afterImagePtr) qDebug() << "afterImagePtrptr is not null";
			else qDebug() << "afterImagePtrptr is null";
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
		if (GlobalPara::envirment == GlobalPara::IPCEn && ret == 0 &&cam_instance->photo.load()==false)//非本地运行的情况
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
			else
			{
				QElapsedTimer timer;
				timer.start();  // 开始计时

				std::unique_lock<std::mutex> lk(g_mutex);

				if (MergePointVec.contains(camId)) {
					MergePointVec[camId].push_back(1);
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

