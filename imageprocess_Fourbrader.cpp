#include "imageprocess_Fourbrader.h"
#include "imageprocess.h"
#include "MZ_VC3000H.h"
#include <string>
#include "CapacitanceProgram.h"
#include "public.h"
#include "algoclass_xs.h"
#include "algoclass_ny.h"
#include "algoclass_bottom.h"

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

		if (cam_instance->video == false && GlobalPara::AlogReady == true) // 非推流的情况
		{

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
					dataToSave.imagePtr = backupImagePtr;
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
					dataToSave.imagePtr = backupImagePtr;
					saveToQueue->queue.push_back(dataToSave);
					GlobalLog::logger.Mz_AddLog(L"all Save");
					qDebug() << "图像数据和信息已推入保存队列。当前队列大小：" << saveToQueue->queue.size();
				}
				saveToQueue->cond.notify_one();
			}


			if (GlobalPara::envirment == GlobalPara::IPCEn) // 非本地运行的情况
			{
				// 先给复位信号false
				bool outputSignalInvert = false;
				int durationMs = 100; // 脉冲持续时间
				int result = PCI::pci().setOutputMode(cam_instance->pointNumber.load() - 1, outputSignalInvert ? true : false, durationMs);

				QString logMsg = QString("相机名称:%1,第一次setOutputMode() 返回值: %2").arg(cam_instance->cameral_name).arg(result);
				LOG_DEBUG(GlobalLog::logger, logMsg.toStdWString().c_str());
			}

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
				AlgoClass_Xs* xsPtr = static_cast<AlgoClass_Xs*>(cam_instance->AC);
				g_detector->InParam.SJ_WidthMin = xsPtr->SJ_WidthMin;
				g_detector->InParam.SJ_WidthMax = xsPtr->SJ_WidthMax;
				g_detector->InParam.JM_Height = xsPtr->JM_Height;
				g_detector->InParam.JM_Thresholdup = xsPtr->JM_Thresholdup;
				g_detector->InParam.JM_Thresholddown = xsPtr->JM_Thresholddown;

				g_detector->InParam.AX_max = xsPtr->AX_max;
				g_detector->InParam.CMAX_max = xsPtr->CMAX_max;
				g_detector->InParam.HS_max = xsPtr->HS_max;
				g_detector->InParam.ZW_max = xsPtr->ZW_max;
				g_detector->InParam.XS_BDparam = xsPtr->XS_BDparam;

				int algo_id = 0;
				cv::Mat imgCopy = currentImagePtr->clone();

				ALLResult result;
				qDebug() << "into XS process";
				g_detector->Process(algo_id, imgCopy, result);
				qDebug() << "out XS process";
				//if (result.xsResult.NGResult != XS_NGReults::OK)
				//{
				//	switch (result.xsResult.NGResult)
				//	{
				//	case XS_NGReults::AX_Err:
				//		info.errmsg << "凹陷NG";
				//		break;

				//	case XS_NGReults::CMAX_Err:
				//		info.errmsg << "侧面凹陷NG";
				//		break;

				//	case XS_NGReults::HS_Err:
				//		info.errmsg << "划伤NG";
				//		break;

				//	case XS_NGReults::ZW_Err:
				//		info.errmsg << "脏污NG";
				//		break;

				//	case XS_NGReults::FK_Err:
				//		info.errmsg << "未封口";
				//		break;

				//	case XS_NGReults::SJ_Err:
				//		info.errmsg << "未束胶";
				//		break;

				//	case XS_NGReults::JM_Err:
				//		info.errmsg << "胶帽突出";
				//		break;

				//	case XS_NGReults::NG:
				//		info.errmsg << "NG";
				//		break;

				//	default:
				//		info.errmsg << "未知错误";
				//		break;
				//	}

				//	ret = -1;
				//}
				//else
				//{
				cam_instance->RI->updateActualValues(result.xsResult);
				cam_instance->RI->applyScaleFactors(cam_instance->DI.scaleFactor.load());
				ret = cam_instance->RI->judge_xs(result.xsResult);
				//}
				*afterImagePtr = result.xsResult.dstImg.clone();
			}

			// 捺印 Ny
			else if (cam_instance->indentify == "Ny") {
				AlgoClass_Ny* nyPtr = static_cast<AlgoClass_Ny*>(cam_instance->AC);
				g_detector->InParam.CQ_AreaMin = nyPtr->CQ_AreaMin;
				g_detector->InParam.GS_AreaMin = nyPtr->GS_AreaMin;
				g_detector->InParam.HS_AreaMin = nyPtr->HS_AreaMin;
				g_detector->InParam.QP_AreaMin = nyPtr->QP_AreaMin;
				g_detector->InParam.YH_AreaMin = nyPtr->YH_AreaMin;
				g_detector->InParam.ZW_AreaMin = nyPtr->ZW_AreaMin;
				g_detector->InParam.NY_SeCha_Ratio = nyPtr->NY_SeCha_Ratio; // 相似度阈值控制参数
				g_detector->InParam.OCR_Control = nyPtr->OCR_Control;
				g_detector->InParam.XT_ZF_ColorThreshold = nyPtr->XT_ZF_ColorThreshold;
				g_detector->InParam.NY_BDparam = nyPtr->NY_BDparam;
				g_detector->InParam.XT_Similarity_Threshold = nyPtr->XT_Similarity_Threshold;
				g_detector->InParam.NY_H = nyPtr->NY_H;
				g_detector->InParam.NY_S = nyPtr->NY_S;

				int algo_id = 1;
				cv::Mat imgCopy = currentImagePtr->clone();

				ALLResult result;
				g_detector->Process(algo_id, imgCopy, result);
				LOG_DEBUG(GlobalLog::logger, QString("Ny ret= %1").arg((int)result.nyResult.NGResult_single).toStdWString().c_str());
				if (result.nyResult.NGResult_single != NY_NGReults::OK)
				{
					switch (result.nyResult.NGResult_single)
					{
					case NY_NGReults::NULL_Err:
						info.errmsg << "空白捺印";
						break;

					case NY_NGReults::CQ_Err:
					//	info.errmsg << "捺印残缺";
						break;

					case NY_NGReults::GS_Err:
						//info.errmsg << "捺印刮伤";
						break;

					case NY_NGReults::HS_Err:
					//	info.errmsg << "捺印划伤";
						break;

					case NY_NGReults::QP_Err:
						//info.errmsg << "捺印负极气泡";
						break;

					case NY_NGReults::YH_Err:
						//info.errmsg << "捺印压痕";
						break;

					case NY_NGReults::ZW_Err:
						//info.errmsg << "捺印脏污NG";
						break;

					case NY_NGReults::XT_ZF_Err:
						info.errmsg << "型替字符";
						break;

					case NY_NGReults::XT_Color_Err:
						info.errmsg << "型替颜色";
						break;

					default:
						//info.errmsg << "未知错误";
						break;
					}

					ret = -1;
				}
				if (result.nyResult.NGResult_single == NY_NGReults::XT_Color_Err || result.nyResult.NGResult_single == NY_NGReults::XT_ZF_Err || result.nyResult.NGResult_single == NY_NGReults::NULL_Err)
				{

				}
				else
				{
					cam_instance->RI->updateActualValues(result.nyResult);
					cam_instance->RI->applyScaleFactors(cam_instance->DI.scaleFactor.load());
					ret = cam_instance->RI->judge_ny(result.nyResult);
				}
				



				*afterImagePtr = result.nyResult.dstImg.clone();
			}

			// 底面 Bottom1
			else if (cam_instance->indentify == "Bottom1") {
				AlgoClass_Bottom* bottomPtr = static_cast<AlgoClass_Bottom*>(cam_instance->AC);
				g_detector->InParam.LK_min = bottomPtr->LK_min;
				g_detector->InParam.JM_min = bottomPtr->JM_min;
				g_detector->InParam.WB_min = bottomPtr->WB_min;
				g_detector->InParam.LK_BOTTOM_BDparam = bottomPtr->LK_BOTTOM_BDparam;
				g_detector->InParam.JM_BOTTOM_BDparam = bottomPtr->JM_BOTTOM_BDparam;
				LOG_DEBUG(GlobalLog::logger, L"into Bottom1 process");
				int algo_id = 3;//胶帽
				cv::Mat imgCopy = currentImagePtr->clone();

				ALLResult result;
				//	g_detector_mutex.lock();

				g_detector->Process(algo_id, imgCopy, result);

				//ret = (result.crop_bootomResult.NGResult == Crop_Bottom_NGReults::OK) ? 0 : -1;

				//if (ret == 0)
				//{
				//	// NG 情况（匹配错误信息）
				//	switch (result.crop_bootomResult.NGResult)
				//	{
				//	case Crop_Bottom_NGReults::LKPS_Err:
				//		info.errmsg << "铝壳破损";   // 漏孔破损
				//		break;

				//	case Crop_Bottom_NGReults::JMPS_Err:
				//		info.errmsg << "胶帽破损";
				//		break;

				//	case Crop_Bottom_NGReults::WBTC_Err:
				//		info.errmsg << "丸棒突出";
				//		break;

				//	case Crop_Bottom_NGReults::NG:
				//		info.errmsg << "NG";
				//		break;

				//	default:
				//		info.errmsg << "未知错误";
				//		break;
				//	}

				//	ret = -1;
				//}
				// OK 情况
				cam_instance->RI->updateActualValues(result.crop_bootomResult);
				cam_instance->RI->applyScaleFactors(cam_instance->DI.scaleFactor.load());
				ret = cam_instance->RI->judge_bottom(result.crop_bootomResult);

				// 你可以选择 JM 或 LK 图，或者做 merge
				*afterImagePtr = result.crop_bootomResult.JM_dstImg.clone();
				//	g_detector_mutex.unlock();
			}

			// 底面 Bottom2
			else if (cam_instance->indentify == "Bottom2") {
				LOG_DEBUG(GlobalLog::logger, L"into Bottom2 process");
				int algo_id = 2;//铝壳
				cv::Mat imgCopy = currentImagePtr->clone();

				ALLResult result;

				//	g_detector_mutex.lock();


				g_detector->Process(algo_id, imgCopy, result);

				ret = (result.crop_bootomResult.NGResult == Crop_Bottom_NGReults::OK) ? 0 : -1;

				/*	if (ret == 0)
					{*/
					// OK 情况
				cam_instance->RI->updateActualValues(result.crop_bootomResult);
				cam_instance->RI->applyScaleFactors(cam_instance->DI.scaleFactor.load());
				ret = cam_instance->RI->judge_bottom(result.crop_bootomResult);
				//}
				//else
				//{
				//	// NG 情况（匹配错误信息）
				//	switch (result.crop_bootomResult.NGResult)
				//	{
				//	case Crop_Bottom_NGReults::LKPS_Err:
				//		info.errmsg << "铝壳破损";   // 漏孔破损
				//		break;

				//	case Crop_Bottom_NGReults::JMPS_Err:
				//		info.errmsg << "胶帽破损";
				//		break;

				//	case Crop_Bottom_NGReults::WBTC_Err:
				//		info.errmsg << "丸棒突出";
				//		break;

				//	case Crop_Bottom_NGReults::NG:
				//		info.errmsg << "NG";
				//		break;

				//	default:
				//		info.errmsg << "未知错误";
				//		break;
				//	}

				//	ret = -1;
				//}

				*afterImagePtr = result.crop_bootomResult.LK_dstImg.clone();

				//	g_detector_mutex.unlock();
			}
			info.timeStr = QString::number(timer.elapsed()).toStdString();

		}
		else if (cam_instance->video == true) // 推流的情况
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
		if (GlobalPara::envirment == GlobalPara::IPCEn && ret == 0 && cam_instance->photo.load() == false)//非本地运行的情况
		{
			QString camId = QString::fromStdString(cam_instance->indentify);
			if (MergePointVec.contains(camId) == false)
			{
				bool outputSignalInvert = true;
				int durationMs = 100; // 脉冲持续时间
				int result = PCI::pci().setOutputMode(cam_instance->pointNumber.load() - 1, outputSignalInvert ? true : false, durationMs);
				//int result = PCI::pci().setoutput(cam_instance->pointNumber.load() - 1, outputSignalInvert ? true : false);

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
		else if (GlobalPara::envirment == GlobalPara::IPCEn && cam_instance->photo.load() == false)//非本地运行的情况
		{
			QString camId = QString::fromStdString(cam_instance->indentify);
			if (GlobalPara::MergePointNum == 0)
			{
				bool outputSignalInvert = false;
				int durationMs = 100; // 脉冲持续时间
				int result = PCI::pci().setOutputMode(cam_instance->pointNumber.load() - 1, outputSignalInvert ? true : false, durationMs);
				//int result = PCI::pci().setoutput(cam_instance->pointNumber.load() , outputSignalInvert ? true : false);

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



		if (!afterImagePtr || afterImagePtr->empty()) {
			LOG_DEBUG(GlobalLog::logger, L"ImageProcess::run(): 准备发出信号时 afterImagePtr 为空或数据无效，发送备用图像");
			emit imageProcessed(backupImagePtr, info);

		}
		else {
			emit imageProcessed(afterImagePtr, info);

		}
		info.errmsg.clear();

	} // while 循环结束

	qDebug() << "ImageProcess::run() (thread finished).";
} // run() 函数结束

