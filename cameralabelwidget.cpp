#include "CameraLabelWidget.h"
#include "CapacitanceProgram.h"
#include "typdef.h"
#include <QLibrary>
#include <QElapsedTimer>
#include <qthread.h>
#include <QPixmap>
#include <QMimeData>
#include <QImageReader>
#include <QUrl>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QtConcurrent/QtConcurrentRun>
#include"public.h"
#include "MZ_VC3000H.h"
#include "imagepaint.h"
#include "MZ_ADOConn.h"
#include "ZoomableLabel.h"
#include "imageviewerwindow.h"
#include "MsvDeviceLib.h"
#include "role.h"
#include "Imageprocess_plate.h"
#include "Imageprocess_flower.h"
#include "Imageprocess_red.h"
#include "imageprocess_Fourbrader.h"
#include "imageprocess_image.h"
#include "common.h"

HImage CameraLabelWidget::convertQPixmapToHImage(const QPixmap& pixmap) {
	HImage hImage;

	if (pixmap.isNull()) {
		qDebug() << "convertQPixmapToHImage: pixmap is null";
		return hImage;
	}

	QImage qImage = pixmap.toImage();

	//  RGB888 格式（适合 JPG 图像）
	if (qImage.format() != QImage::Format_RGB888) {
		qImage = qImage.convertToFormat(QImage::Format_RGB888);
	}

	int width = qImage.width();
	int height = qImage.height();
	int channels = 3;  // RGB888
	int bytesPerLine = qImage.bytesPerLine();
	int dataSize = bytesPerLine * height;

	// 设置图像头
	hImage.imageHead.width = width;
	hImage.imageHead.height = height;
	hImage.imageHead.channels = channels;
	hImage.imageHead.length = dataSize;

	// 分配并拷贝数据
	hImage.data = new unsigned char[dataSize];
	if (hImage.data != nullptr) {
		memcpy(hImage.data, qImage.constBits(), dataSize);
	}
	else {
		qDebug() << "convertQPixmapToHImage: memory allocation failed!";
	}

	return hImage;
}

void CameraLabelWidget::onImageLoaded(std::shared_ptr<cv::Mat> image)
{
	qDebug() << "this is onImageLoaded";
	if (!image || image->empty()) {
		qDebug() << "onImageLoaded 图像无效：指针是 null 或者图像是 empty";
	}
	else {
		qDebug() << "onImageLoaded 图像有效";
	}
	this->learnimage = image;
	this->checkimage = image;
	this->currentPixmap = convertMatToPixmap(*image);
	displayimg(currentPixmap);
}

void CameraLabelWidget::onLearn()
{
	if (!this->learnimage || this->learnimage->empty()) {
		qDebug() << "图像无效：指针是 null 或者图像是 empty";
		return;
	}
	else {
		qDebug() << "图像有效";
	}
	// std::shared_ptr<cv::Mat> afterImagePtr=std::shared_ptr<cv::Mat>();
	cv::Mat afterImagePtr;
	if (m_cam->indentify == "Carrier_NaYin")
	{
		//LearnPara::inParam1.scoresNegLimit = m_cam->RI->score_neg;
		//LearnPara::inParam1.scoresPosLimit = m_cam->RI->score_pos;

		int ret;
		qDebug() << "onLearn start";

		ret = ExportSpace::RunStamp(*this->learnimage, 0, 1, LearnPara::inParam1);

		qDebug() << ret;
		OutStampResParam para;
		ExportSpace::ResultOutStamp(afterImagePtr, para, 1);
		LearnPara::inNum = para.textNum;
#ifdef  ADAPTATEION

#else
		for (int j = 0; j < LearnPara::inNum; j++)
		{
			m_cam->RI->m_PaintData[7 + j].check = true;
		}
		for (int j = para.textNum; j < 15; j++)
		{
			m_cam->RI->m_PaintData[7 + j].check = false;
		}
#endif
		qDebug() << "onLearn end";

		std::string path = "../../../resources/images/Carrier_NaYin.jpg";
		cv::imwrite(path, *this->learnimage);

		this->currentPixmap = convertMatToPixmap(afterImagePtr);
		this->onViewImageClicked();

	}
	else if (m_cam->indentify == "NaYin")
	{


		qDebug() << "onLearn start";
		ExportSpace::RunStamp(*this->learnimage, 0, 0, LearnPara::inParam2);

		OutStampResParam para;
		ExportSpace::ResultOutStamp(afterImagePtr, para, 0);
		qDebug() << "Ny 原图尺寸:" << this->learnimage->cols << "x" << this->learnimage->rows;
		qDebug() << "Ny 结果图(afterImagePtr)尺寸:" << afterImagePtr.cols << "x" << afterImagePtr.rows;
		LearnPara::inNum = para.textNum;
#ifdef  ADAPTATEION

#else
		for (int j = 0; j < LearnPara::inNum; j++)
		{
			m_cam->RI->m_PaintData[7 + j].check = true;
		}
		for (int j = para.textNum; j < 15; j++)
		{
			m_cam->RI->m_PaintData[7 + j].check = false;
		}

#endif
		qDebug() << "onLearn end";

		std::string path = "../../../resources/images/NaYin.jpg";
		cv::imwrite(path, *this->learnimage);

		this->currentPixmap = convertMatToPixmap(afterImagePtr);
		this->onViewImageClicked();
	}
	else if (m_cam->indentify == "Top")
	{
		qDebug() << "onLearn start";
		InTopParam inpara = LearnPara::inParam3;
		inpara.al_core = true;
		BraidedTapeSpace::RunTop(*this->learnimage,inpara);

		OutTopParam outpara;
		BraidedTapeSpace::ResultOutTop(afterImagePtr, outpara);
		std::string path = "../../../resources/images/Top.jpg";
		cv::imwrite(path, *this->learnimage);

		this->currentPixmap = convertMatToPixmap(afterImagePtr);
		this->onViewImageClicked();
	}
	else if (m_cam->indentify == "Side")
	{
		qDebug() << "onLearn start";
		InSideParam inpara=LearnPara::inParam4;
		inpara.al_core = true;
		BraidedTapeSpace::RunSide(*this->learnimage, inpara);

		OutSideParam outpara;
		BraidedTapeSpace::ResultOutSide(afterImagePtr, outpara);
		std::string path = "../../../resources/images/Side.jpg";
		cv::imwrite(path, *this->learnimage);

		this->currentPixmap = convertMatToPixmap(afterImagePtr);
		this->onViewImageClicked();
	}
	else if (m_cam->indentify == "Pin")
	{
		InPinParam inpara = LearnPara::inParam5;
		inpara.al_core = true;
		BraidedTapeSpace::RunPin(*this->learnimage, inpara);
		OutPinParam outpara;
		BraidedTapeSpace::ResultOutPin(afterImagePtr, outpara);
		std::string path = "../../../resources/images/Pin.jpg";
		cv::imwrite(path, *this->learnimage);

		this->currentPixmap = convertMatToPixmap(afterImagePtr);
		this->onViewImageClicked();
	}
	else if (m_cam->indentify == "Ny")
	{
		g_detector->m_params.LearnFlag = true;
		ALLResult result;
		g_detector->Process(1, *this->learnimage, result);
		afterImagePtr = result.nyResult.dstImg;
		qDebug() << "Ny 原图尺寸:" << this->learnimage->cols << "x" << this->learnimage->rows;
		qDebug() << "Ny 结果图(afterImagePtr)尺寸:" << afterImagePtr.cols << "x" << afterImagePtr.rows;
		this->currentPixmap = convertMatToPixmap(afterImagePtr);
		this->onViewImageClicked();
	}
}

void CameraLabelWidget::onCheck()
{
	check_flag.store(true);
	{
		std::unique_lock<std::mutex> lock(m_cam->imageQueue.mutex);
		m_cam->imageQueue.stop_flag.store(false);
	}
	m_cam->imageQueue.cond.notify_all();
		

	if (this->checkimage) {
	
			std::unique_lock<std::mutex> lock(m_cam->imageQueue.mutex);
			m_cam->imageQueue.queue.push_back(this->checkimage);
			qDebug() << "Pushed image to queue. Current queue size:" << m_cam->imageQueue.queue.size();
			lock.unlock();
			m_cam->imageQueue.cond.notify_all();
	

	}
	else {
		
	}

}

void CameraLabelWidget::pic_display(HImages inputImages, HValues inputParams, HImages& outputImages, HValues& outputParams, int& errcode, string& errmsg)
{
	qDebug() << "start";
	// 使用 QPixmap 从路径加载图像
	QPixmap pixmap(":/images/resources/images/OIP-C.jpg");  

	// 调用转换函数将 QPixmap 转换为 HImage
	HImage img = convertQPixmapToHImage(pixmap);
	qDebug() << "start1";
	// 如果转换失败，则返回错误
	if (img.data == nullptr)
	{
		errcode = 1;
		errmsg = "图片加载或转换失败";
		return;
	}

	// 将图像添加到输出图像列表
	outputImages.m_Images.push_back(img);

	// 设置返回值
	errcode = 0;  // 没有错误
	errmsg = "图像生成成功";  
	qDebug() << "start2";
}

void CameraLabelWidget::onCameraConnect(const QString& color)
{
	
	QString newButtonStyle = QString(
		"QToolButton { "
		"   background-color: %1; "
		"   color: black; "
		"   font-size: 20px; "
		"   padding: 0px; "
		"   border: none; "
		"} "
		"QToolButton:hover { "
		"   background-color: #1782AB; "
		"} "
		"QToolButton:pressed { "
		"   background-color: %1; "
		"} "
		"QToolButton:focus { "
		"   outline: none; "
		"} "
		"QToolButton::menu-indicator { image: none; }"
	).arg(color);

	// 应用新的样式表
	textMenuButton->setStyleSheet(newButtonStyle);
}

void CameraLabelWidget::add_ui(HImages inputImages, HValues inputParams,
	HImages& outputImages, HValues& outputParams,
	int& errcode, std::string& errmsg)
{
	qDebug() << "start";
	HValue v = inputParams.m_Values[0].I() + inputParams.m_Values[1].I();
	outputParams.m_Values.push_back(v);

	qDebug() << "start2";
}


void CameraLabelWidget::pic_handle(HImages inputImages, HValues inputParams, HImages& outputImages, HValues& outputParams, int& errcode, string& errmsg)
{
	qDebug() << "start";
	qDebug() << "inputImages.m_Images.size()  " << inputImages.m_Images.size();
	qDebug() << "inputImages.m_Images[0].getdatelength()" << inputImages.m_Images[0].getdatelength();

	// displayimg(inputImages.m_Images[0]);
	 // 设置返回值
	outputImages.m_Images.push_back(inputImages.m_Images[0]);
	errcode = 0;  // 没有错误
	errmsg = "图像生成成功";  
	qDebug() << "start2";
}




CameraLabelWidget::CameraLabelWidget(Cameral* cam, int index, const QString& fixedTextName, SaveQueue* m_saveQueue, QWidget* parent)
	: QWidget(parent)
{
	
	setStyleSheet("background-color: transparent;");
#ifdef QIMAGE
	imageLabel = new ZoomableQImage(this);
#else
	imageLabel = new ZoomableLabel(this);
	imageLabel->setAlignment(Qt::AlignCenter);
#endif

	imageLabel->setObjectName("cameraImageLabel");
	// 设置 imageLabel 的样式：背景色为 rgb边框为 1px solid black 
	imageLabel->setStyleSheet("background-color: rgb(24, 26, 32); border: 1px solid #D8D8D8;");



	// 定义统一的顶部背景色 (#ADD8E6)
	const QString topBarBackgroundColor = "#D8D8D8"; //灰色
	const QString topBarHoverColor = "#1782AB"; // 原始的悬停深蓝色

	// 创建文字菜单按钮 (恢复并保持其功能)
	textMenuButton = new QToolButton(this);
	textMenuButton->setText(fixedTextName); // 设置按钮文本
	textMenuButton->setToolButtonStyle(Qt::ToolButtonTextOnly); // 只显示文本
	textMenuButton->setToolTip(fixedTextName); // 为文字菜单按钮添加悬停提示
	// 样式调整：背景色为新的蓝色，文字颜色为黑色，移除边框和菜单箭头 
	textMenuButton->setStyleSheet(
		"QToolButton { "
		"   background-color: " + topBarBackgroundColor + "; "
		"   color: black; "
		"   font-size: 20px; "
		"   padding: 0px; "
		"   border: none; "
		"} "
		"QToolButton:hover { "
		"   background-color: " + topBarHoverColor + "; "
		"} "
		"QToolButton:pressed { "
		"   background-color: " + topBarBackgroundColor + "; "
		"} "
		"QToolButton:focus { " //移除焦点矩形
		"    outline: none; "
		"} "
		"QToolButton::menu-indicator { image: none; }"
	);
	textMenuButton->setCheckable(false);
	textMenuButton->setContentsMargins(0, 0, 0, 0); // 移除按钮内部的边距

	// 创建一个 QMenu 实例，并设置给 textMenuButton
	QMenu* textMenu = new QMenu(this);
	textMenuButton->setMenu(textMenu);
	textMenuButton->setPopupMode(QToolButton::InstantPopup); // 即时弹出菜单

	textMenu->setStyleSheet(
		"QMenu {"
		"    background-color: " + topBarBackgroundColor + ";" // 菜单整体背景色
		"    border: 1px solid #555;"                         // 添加一个边框，使其更清晰
		"}"
		"QMenu::item {"
		"    color: black;"                                   // 菜单项文字颜色
		"    background-color: transparent;"                  // 菜单项默认背景透明
		"    padding: 4px 20px;"                              // 增加内边距，方便点击
		"}"
		"QMenu::item:selected {"                              // 鼠标悬停在菜单项上时
		"    background-color: " + topBarHoverColor + ";"     // 背景色变为深蓝色
		"}"
	);


	if (cam->indentify == "Carrier_NaYin" || cam->indentify == "NaYin")
	{
	
		QAction* onlineLearnAction = new QAction("在线学习", this);

	
		connect(onlineLearnAction, &QAction::triggered, [this, cam]() {

			qDebug() << "通过菜单触发在线学习...";
			cam->photo.store(true);
			cam->learn.store(true);
			triggerCameraPhoto(cam);
			});

		textMenu->addAction(onlineLearnAction);
	}


	//设置路径
	dataToSave.savePath_OK = cam->ok_path.toStdString();
	dataToSave.savePath_NG = cam->ng_path.toStdString();
	dataToSave.savePath_Pre = cam->localpath.toStdString();

	//QString& savePath_OK = dataToSave.savePath_OK;
	//savePath_OK = &cam->ok_path.toStdString;

	this->sumcount = &cam->sum_count;
	this->ngcount = &cam->error_count;



	this->saveToQueue = m_saveQueue;

	//初始化运行线程
#ifdef USE_MAIN_WINDOW_CAPACITY


#ifdef FOURBRADER
	this->m_imageProcessor = new Imageprocess_FourBrader(cam, m_saveQueue, this);
	connect(m_imageProcessor, &ImageProcess::imageProcessed,
		this, &CameraLabelWidget::onImageProcessed_plate);
#elif defined(ADAPTATEION)
	this->m_imageProcessor = new Imageprocess_Plate(cam, m_saveQueue, this);
	connect(m_imageProcessor, &ImageProcess::imageProcessed,
		this, &CameraLabelWidget::onImageProcessed_plate);
#else
	this->m_imageProcessor = new Imageprocess_Image(cam, m_saveQueue, this);
	connect(m_imageProcessor, &ImageProcess::imageProcessed_QImage,
		this, &CameraLabelWidget::onImageProcessed_QImage);
#endif

#elif USE_MAIN_WINDOW_BRADER
	this->m_imageProcessor = new Imageprocess_Brader(cam, m_saveQueue, this);

	connect(m_imageProcessor, &ImageProcess::imageProcessed_Brader,
		this, &CameraLabelWidget::onImageProcessed_Brader);

	connect(m_imageProcessor, &ImageProcess::StartGetIn,
		this, &CameraLabelWidget::onStartGetIn);

#else USE_MAIN_WINDOW_FLOWER
	this->m_imageProcessor = new Imageprocess_Flower(cam, m_saveQueue, this);
	connect(m_imageProcessor, &ImageProcess::imageProcessed,
		this, &CameraLabelWidget::onImageProcessed_flower);

	if(cam->indentify!="FlowerLook")this->m_imageProcessor_Red = new Imageprocess_Red(cam, m_saveQueue, this);
#endif // USE_MAIN_WINDOW_CAPACITY


	

	//先初始化再绑定槽函数

	connect(m_imageProcessor, &ImageProcess::ImageLoaded,
		this, &CameraLabelWidget::onImageLoaded);

	connect(m_imageProcessor, &ImageProcess::Learn,
		this, &CameraLabelWidget::onLearn);

	connect(m_imageProcessor, &ImageProcess::SetButtonBackground,
		this, &CameraLabelWidget::onSetButtonBackground);

	//启动执行线程
	this->m_imageProcessor->start();

#ifdef  USE_MAIN_WINDOW_FLOWER
	if(cam->indentify!="FlowerLook") this->m_imageProcessor_Red->start();
#endif //  USE_MAIN_WINDOW_FLOWER


	// 创建水平布局用于放置顶部图标和文字菜单按钮
	QHBoxLayout* topLayout = new QHBoxLayout();
	topLayout->setContentsMargins(0, 0, 0, 0); // 移除与窗口顶部的空隙
	topLayout->setSpacing(0); // 移除功能按钮和文字菜单按钮之间的空隙

	// 创建一个 QWidget 来包裹 topLayout，并设置其背景色为原始的浅蓝色
	QWidget* topBarWidget = new QWidget(this);
	topBarWidget->setStyleSheet("background-color: " + topBarBackgroundColor + ";"); // 顶部区域统一原始浅蓝色背景
	topBarWidget->setLayout(topLayout);


	// 将文字菜单按钮添加到顶部布局的最左边 (恢复)
	topLayout->addWidget(textMenuButton);

	// 定义图标路径
	const QString iconPath = ":/images/resources/images/oo.ico";
	const QString iconPath_start = ":/images/resources/images/start.ico";
	const QString iconPath_camera = ":/images/resources/images/camera.ico";
	const QString iconPath_video = ":/images/resources/images/video.ico";
	const QString iconPath_localrun = ":/images/resources/images/localrun.ico";
	const QString iconPath_ok = ":/images/resources/images/OK.ico";
	const QString iconPath_ng = ":/images/resources/images/NG.ico";
	const QString iconPath_data = ":/images/resources/images/NG.ico";
	const QString iconPath_pause = ":/images/resources/images/pause.ico";
	const QString iconPath_settings = ":/images/resources/images/settings.ico";
	const QString iconPath_expand = ":/images/resources/images/expand.ico";
	const QString iconPath_onlinelearn = ":/images/resources/images/onlinelearn.ico";
	const QString iconPath_ten = ":/images/resources/images/ten.ico";


	// --- 创建功能按钮并添加到顶部布局 ---
	// 统一按钮样式，背景色为原始的浅蓝色，文字颜色为黑色，移除边框和内边距，实现无竖线效果
	QString buttonStyle =
		"QPushButton {"
		"   background-color: " + topBarBackgroundColor + "; " // 按钮背景为原始浅蓝色
		"   color: black; "                                 // 文字颜色黑色
		"   border: none; "                                 // 移除边框
		"   padding: 0px; "                                 // 移除内边距
		"    icon-size: 32px; "
		"}"
		"QPushButton:hover { background-color: " + topBarHoverColor + "; }"; // 悬停效果
	;

	// 运行按钮
	runButton = new QPushButton(QIcon(iconPath_start), "", this);
//	runButton->setFixedSize(40, 40); // 设置固定大小
	runButton->setToolTip("运行"); // 鼠标悬停提示
	runButton->setStyleSheet(buttonStyle); // 应用样式
	topLayout->addWidget(runButton);
	
#ifdef USE_MAIN_WINDOW_FLOWER
	connect(runButton, &QPushButton::clicked, [this, index, cam]() mutable {
		if (!runButton->isEnabled()) {
			qDebug() << "运行按钮被禁用，忽略本次点击。";
			return;
		}
		this->runButton->setEnabled(false); // 防止连点
		qDebug() << "摄像头" << index + 1 << "的选项 运行 被点击。";
		if (cam->running.load() == false)
		{
			int ret;
			cam->running.store(true);
			ret = cam->camOp->MsvOpenDevice();
			if (ret != 0)
			{
				QString logMsg = QString("open cameral ret value: %1").arg(ret);

				// 转为 wchar_t* 并调用日志宏
				LOG_DEBUG(GlobalLog::logger, logMsg.toStdWString().c_str());
				QString warningMessage = cam->cameral_name + "相机未连接";
				QMessageBox::warning(nullptr, "连接警告", warningMessage);


			}
			ret = cam->camOp->RegisterImageCallBack(MyImageCallback_Flower, &cam->imageQueue);

			ret = cam->camOp->SetTrrigerModel(0);

			ret = cam->camOp->MsvStartImageCapture();

		}
		else {
			int ret;
			//triggerCameraStop(cam);
			onStreamCapture();
		}

		this->runButton->setEnabled(true);//恢复
		});
#else
	connect(runButton, &QPushButton::clicked, [this, index, cam]() mutable {
		if (!runButton->isEnabled()) {
			qDebug() << "运行按钮被禁用，忽略本次点击。";
			return;
		}
		this->runButton->setEnabled(false); // 防止连点
		qDebug() << "摄像头" << index + 1 << "的选项 运行 被点击。";
		if (cam->running.load() == false)
		{
			cam->running.store(true);
			this->triggerCameraStart(cam);
		}
		else {
			int ret;
			//triggerCameraStop(cam);
			ret = cam->camOp->MsvStopImageCapture();

			Sleep(50);
			// 设置触发模式
			ret = cam->camOp->SetTrrigerModel(1);
			if (ret == 0) {
				LOG_DEBUG(GlobalLog::logger, QString("SetTrrigerModel() successful").toStdWString().c_str());
			}
			else {
				LOG_DEBUG(GlobalLog::logger, QString("SetTrrigerModel() failed, ret = %1").arg(ret).toStdWString().c_str());
			}

			Sleep(50);
			ret = cam->camOp->SetTrrigerSource(cam->rounte);
			if (ret == 0) {
				LOG_DEBUG(GlobalLog::logger, QString("SetTrrigerSource() successful").toStdWString().c_str());
			}
			else {
				LOG_DEBUG(GlobalLog::logger, QString("SetTrrigerSource() failed, ret = %1").arg(ret).toStdWString().c_str());
				if (cam->rounte == 0)ret = cam->camOp->SetTrrigerSource(1);
				else  ret = cam->camOp->SetTrrigerSource(0);
				LOG_DEBUG(GlobalLog::logger, QString("SetTrrigerSource() twice, ret = %1").arg(ret).toStdWString().c_str());
			}
			ret = cam->camOp->MsvStartImageCapture();
			LOG_DEBUG(GlobalLog::logger, QString("MsvStartImageCapture , ret = %1").arg(ret).toStdWString().c_str());


		}

		this->runButton->setEnabled(true);//恢复
		});
#endif
	// 拍照按钮
	this->captureButton = new QPushButton(QIcon(iconPath_camera), "", this);
	//captureButton->setFixedSize(32, 32);
	this->captureButton->setToolTip("拍照");
	this->captureButton->setStyleSheet(buttonStyle);
	topLayout->addWidget(captureButton);
	connect(captureButton, &QPushButton::clicked, [this, index, cam]() mutable {
		//// 静态变量：只初始化一次，所有 captureButton 共用
		//static qint64 lastClickTime = 0;

		//qint64 now = QDateTime::currentMSecsSinceEpoch();

		//
		//if (now - lastClickTime < 1000) {
		//	
		//	GlobalPara::cheatFlag.store(true);
		//
		//	return;
		//}
		//lastClickTime = now;

		if (!this->captureButton->isEnabled()) {
			qDebug() << "拍照按钮被禁用，忽略本次点击。";
			return;
		}

		this->captureButton->setEnabled(false); // 禁用，防止连点
		qDebug() << "摄像头" << index + 1 << "的选项 拍照 被点击";

		GlobalPara::changed.store(false);
		m_cam->video.store(false);
#ifndef USE_MAIN_WINDOW_CAPACITY
		this->onSetButtonBackground("#1782AB");
		//this->thread_exit = true;
#endif

		if (cam->running == false) {
			this->onCameraStart();
		}
		qDebug() << "photo ing1 ";
		cam->photo.store(true);

		qDebug() << "photo ing2 ";
	//	triggerCameraPhoto_Stream(cam);

		triggerCameraPhoto(cam);


		this->captureButton->setEnabled(true);
		qDebug() << "拍照按钮已恢复可用";
		});

	// 推流按钮
	QPushButton* pushStreamButton = new QPushButton(QIcon(iconPath_video), "", this);
	//pushStreamButton->setFixedSize(40, 40);
	pushStreamButton->setToolTip("录像");
	pushStreamButton->setStyleSheet(buttonStyle);

	connect(pushStreamButton, &QPushButton::clicked, [this, index, cam, pushStreamButton]() mutable {
		//if (Role::GetCurrentRole() == "操作员") return;
		if (!pushStreamButton->isEnabled()) {
			//qDebug() << "推流按钮被禁用，忽略本次点击。";
		//	LOG_DEBUG(GlobalLog::logger, L"推流按钮被禁用，忽略本次点击。");
			//this->stopButton->click();	
			return;
		}
		pushStreamButton->setEnabled(false); // 禁用，防止连点
		qDebug() << "摄像头" << index + 1 << "的选项 推流 被点击";
		if (cam->running.load() == false)
		{
			this->onCameraStart();
		}
		cam->local.store(true);
		{
			std::unique_lock<std::mutex> lock(cam->imageQueue.mutex);
			cam->imageQueue.stop_flag = false;
		}
		cam->imageQueue.cond.notify_all();
		cam->video = true;
		handleCameraPush(cam);
		pushStreamButton->setEnabled(true); // 恢复按钮点击
		});

	// 本地运行按钮
	QPushButton* localRunButton = new QPushButton(QIcon(iconPath_localrun), "", this);
	//localRunButton->setFixedSize(40, 40);
	localRunButton->setToolTip("本地运行");
	localRunButton->setStyleSheet(buttonStyle);
	topLayout->addWidget(localRunButton);
	
	connect(localRunButton, &QPushButton::clicked, [this, index, cam, localRunButton]() mutable {
		if (!localRunButton->isEnabled()) {
			qDebug() << "本地运行按钮被禁用，忽略本次点击。";
			return;
		}
		localRunButton->setEnabled(false); // 禁用，防止连点
		qDebug() << "摄像头" << index + 1 << "的选项 本地运行 被点击";

		handleCameraLocalRun(cam);
		localRunButton->setEnabled(true); // 恢复按钮点击
		});

	// 在线学习按钮
	if (cam->indentify == "Carrier_NaYin" || cam->indentify == "NaYin"||cam->indentify=="Top"  ||cam->indentify=="Pin"||cam->indentify=="Ny")
	{
		onlineLearnButton = new QPushButton(QIcon(iconPath_onlinelearn), "", this);
		//onlineLearnButton->setFixedSize(40, 40);
		onlineLearnButton->setToolTip("在线学习");
		onlineLearnButton->setStyleSheet(buttonStyle);
		topLayout->addWidget(onlineLearnButton);
		connect(onlineLearnButton, &QPushButton::clicked, [this, index, cam]() mutable {
			cam->photo.store(true);
			cam->learn.store(true);
			triggerCameraPhoto(cam);	
			});
#ifdef USE_MAIN_WINDOW_CAPACITY
		if (cam->indentify != "Ny") {
			onlineLearnButton->hide();
		}
#endif
}

	// 连续学习按钮
	tenButton = new QPushButton(QIcon(iconPath_ten), "", this);
	tenButton->setToolTip("连续学习");
	tenButton->setStyleSheet(buttonStyle);
	topLayout->addWidget(tenButton);
	connect(tenButton, &QPushButton::clicked, [this, cam]() mutable {
		if (!this->tenButton->isEnabled()) {			
			return;
		}
		this->tenButton->setEnabled(false); // 禁用自己

		cam->ten.store(10);
		
		this->tenButton->setEnabled(true);

		});
	if (cam->indentify!= "Side") tenButton->hide();

	// 停止按钮
	stopButton = new QPushButton(QIcon(iconPath_pause), "", this);
	//stopButton->setFixedSize(40, 40);
	stopButton->setToolTip("停止");
	stopButton->setStyleSheet(buttonStyle);
	topLayout->addWidget(stopButton);
	connect(stopButton, &QPushButton::clicked, [this, cam, index, pushStreamButton, localRunButton]() mutable {
		if (!this->stopButton->isEnabled()) {
			qDebug() << "停止按钮被禁用，忽略本次点击。";
			return;
		}
		this->stopButton->setEnabled(false); // 禁用自己
		qDebug() << "摄像头" << index + 1 << "的选项 停止 被点击。";
		this->triggerCameraStop(cam);
		// 恢复所有按钮的可用状态
		this->stopButton->setEnabled(true);
		this->runButton->setEnabled(true);
		this->captureButton->setEnabled(true);
		pushStreamButton->setEnabled(true);
		localRunButton->setEnabled(true);
		qDebug() << "所有按钮已恢复可用状态。";
		cam->running = false;
		cam->learn = false;
		cam->video = false;
		cam->photo = false;
		});
	if (GlobalPara::envirment == GlobalPara::IPCEn) stopButton->hide();
	// Data 路径按钮
	QPushButton* dataButton = new QPushButton(QIcon(iconPath_data), "", this);
	
	dataButton->setToolTip("打开图像路径");
	dataButton->setStyleSheet(buttonStyle);
	topLayout->addWidget(dataButton);
	connect(dataButton, &QPushButton::clicked, [index, this, cam]() {
		qDebug() << "摄像头" << index + 1 << "的选项 Data 被点击";
		if (!QDesktopServices::openUrl(QUrl::fromLocalFile(SystemPara::DATA_DIR+"/"+QString::fromStdString(cam->indentify)))) {
			qWarning() << "无法打开路径:" << SystemPara::DATA_DIR;
		}
		});
	
	QPushButton* okButton = new QPushButton(QIcon(iconPath_ok), "", this);
	//okButton->setFixedSize(40, 40);
	okButton->setToolTip("打开OK图像路径");
	okButton->setStyleSheet(buttonStyle);
	topLayout->addWidget(okButton);
	connect(okButton, &QPushButton::clicked, [index, this, cam]() {
		qDebug() << "摄像头" << index + 1 << "的选项 OK 被点击";
		if (!QDesktopServices::openUrl(QUrl::fromLocalFile(cam->ok_path))) {
			qWarning() << "无法打开路径:" << cam->ok_path;
		}
		});

	// NG 路径按钮
	QPushButton* ngButton = new QPushButton(QIcon(iconPath_ng), "", this);
	//ngButton->setFixedSize(40, 40);
	ngButton->setToolTip("打开NG图像路径");
	ngButton->setStyleSheet(buttonStyle);
	topLayout->addWidget(ngButton);
	connect(ngButton, &QPushButton::clicked, [index, this, cam]() {
		qDebug() << "摄像头" << index + 1 << "的选项 NG 被点击";
		if (!QDesktopServices::openUrl(QUrl::fromLocalFile(cam->ng_path))) {
			qWarning() << "无法打开路径:" << cam->ng_path;
		}
		});

	// 参数按钮
	paramButton = new QPushButton(QIcon(iconPath_settings), "", this);
	paramButton->setFixedSize(40, 40);
	paramButton->setToolTip("参数设置");
	paramButton->setStyleSheet(buttonStyle);
	topLayout->addWidget(paramButton);
	connect(paramButton, &QPushButton::clicked, [this, cam]() mutable {
		if (!this->paramButton->isEnabled()) {
			qDebug() << "参数按钮被禁用，忽略本次点击。";
			return;
		}
		this->paramButton->setEnabled(false); // 防连点
		qDebug() << "打开参数设置窗口";
		this->parawidget = new ParaWidget(cam->RC, cam->CC, cam->AC, cam, &cam->DI);
		connect(parawidget, &ParaWidget::SaveDebugInfo, this, &CameraLabelWidget::onSaveDebugInfo);
		connect(parawidget, &ParaWidget::ImageLoaded, this, &CameraLabelWidget::onImageLoaded);
		connect(parawidget, &ParaWidget::Learn, this, &CameraLabelWidget::onLearn);
		connect(parawidget, &ParaWidget::Check, this, &CameraLabelWidget::onCheck);
		connect(parawidget, &QObject::destroyed, this, [this]() {
			this->paramButton->setEnabled(true);
			qDebug() << "参数窗口关闭，按钮恢复可用。";
			});
		parawidget->show(); // 非模态显示
		});

	// 查看图像按钮
	QPushButton* viewImageButton = new QPushButton(QIcon(iconPath_expand), "", this);
	viewImageButton->setFixedSize(40, 40);
	viewImageButton->setToolTip("查看图像");
	viewImageButton->setStyleSheet(buttonStyle);
	topLayout->addWidget(pushStreamButton);//防止误点
	topLayout->addWidget(viewImageButton);
	
	connect(viewImageButton, &QPushButton::clicked, this, [this]() {
		this->imageView = new ImageViewerWindow(this->currentPixmap, nullptr);
		this->imageView->show();
		connect(this, &CameraLabelWidget::FullShow, imageView, &ImageViewerWindow::setPixmap);
		this->onSwitchFullScreen(true);

		connect(imageView, &ImageViewerWindow::windowClosed, this, [this]() {
		this->onSwitchFullScreen(false);
			});
		});

	// 添加一个伸缩器，让所有按钮靠左排列
	topLayout->addStretch();
	okButton->hide();
	ngButton->hide();
	if (GlobalPara::envirment == GlobalPara::IPCEn)
	{
		localRunButton->hide();
		runButton->hide();
	}
#ifndef USE_MAIN_WINDOW_CAPACITY
	paramButton->hide();
#endif // USE_MAIN_WINDOW_CAPACITY



	// 创建一个主垂直布局
	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	mainLayout->setContentsMargins(0, 0, 0, 0); // 移除主布局的空隙
	mainLayout->setSpacing(0); // 移除主布局中子布局之间的空隙

	// 将带有原始浅蓝色背景的 topBarWidget（包含文字菜单按钮和图标按钮）添加到主布局
	mainLayout->addWidget(topBarWidget);

	// 添加图像显示标签
	mainLayout->addWidget(imageLabel);

	imageLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding); // 图像标签扩展以填充可用空间

	setLayout(mainLayout); // 设置主布局

	this->m_cam = cam;
}

void CameraLabelWidget::onCameraStart()
{
	this->runButton->click();
}
void CameraLabelWidget::onCameraStop()
{
	this->stopButton->click();
}
void CameraLabelWidget::onCameraPhoto()
{
	this->captureButton->click();
}

void CameraLabelWidget::onSwitchFullScreen(bool status)
{
    this->FullScreen.store(status);
}

void CameraLabelWidget::onSetButtonBackground(const QString& color)
{
	if (!this->captureButton) return;

	QString style = QString(
		"QPushButton { "
		"   background-color: %1; "
		"   color: black; "
		"   border: none; "
		"   padding: 0px; "
		"   icon-size: 32px; "
		"} "
		"QPushButton:hover { "
		"   background-color: %1; "
		"} "
	).arg(color);

	this->captureButton->setStyleSheet(style);
}

void CameraLabelWidget::handleCameraLocalRun(Cameral* cam)
{
	QDir monitorDir(cam->localpath);
	if (!monitorDir.exists()) {
		qWarning() << "本地运行：监控路径不存在：" << cam->localpath;
		return;
	}

	QStringList nameFilters;
	nameFilters << "*.jpg" << "*.jpeg";
	QStringList fileNames = monitorDir.entryList(nameFilters, QDir::Files | QDir::NoDotAndDotDot);

	if (fileNames.size() < 1) {
		qWarning() << "本地运行：监控路径中没有 JPG/JPEG 文件, 当前找到：" << fileNames.size() << "张。";
		return;
	}

	if (this->m_localfilethread != nullptr) {
		qDebug() << "本地文件检测线程已经存在，无需重复启动。";
		return;
	}

	{
		std::unique_lock<std::mutex> lock(cam->imageQueue.mutex);
		cam->imageQueue.stop_flag.store(false);
	}
	cam->imageQueue.cond.notify_all();

	// 创建新的本地检测线程
	this->m_localfilethread = new LocalFileDetector(&cam->imageQueue, cam->localpath, this);
	qDebug() << "创建新的本地文件检测线程实例。";

	connect(this->m_localfilethread, &LocalFileDetector::started, []() {
		qDebug() << "LocalFileDetector 线程已启动。";
		});
	connect(this->m_localfilethread, &LocalFileDetector::finished, []() {
		qDebug() << "LocalFileDetector 线程已完成。";
		});

	this->m_localfilethread->start();
	qDebug() << "本地文件检测线程启动成功。";
}

void CameraLabelWidget::onParam()
{
	this->paramButton->click();
}

void CameraLabelWidget::handleCameraPush(Cameral* cam)
{
	int ret = 0;

	ret = cam->camOp->MsvStopImageCapture();

	// 设置触发模式
	ret = cam->camOp->SetTrrigerModel(0);
	if (ret == -1)
		LOG_DEBUG(GlobalLog::logger, QString("SetTrrigerModel(0) failed, error: %1").arg(ret).toStdWString().c_str());
	else
		LOG_DEBUG(GlobalLog::logger, QString("SetTrrigerModel(0) successful").toStdWString().c_str());

	// 开始采图
	ret = cam->camOp->MsvStartImageCapture();
	if (ret == -1)
		LOG_DEBUG(GlobalLog::logger, QString("MsvStartImageCapture() failed, error: %1").arg(ret).toStdWString().c_str());
	else
		LOG_DEBUG(GlobalLog::logger, QString("MsvStartImageCapture() successful").toStdWString().c_str());

	Sleep(50);  // 启动前必须等待一段时间
}

void CameraLabelWidget::onStreamCapture()
{
	int ret = 0;

	ret = m_cam->camOp->MsvStopImageCapture();

	// 设置触发模式
	ret = m_cam->camOp->SetTrrigerModel(0);
	if (ret == -1)
		LOG_DEBUG(GlobalLog::logger, QString("SetTrrigerModel(0) failed, error: %1").arg(ret).toStdWString().c_str());
	else
		LOG_DEBUG(GlobalLog::logger, QString("SetTrrigerModel(0) successful").toStdWString().c_str());

	// 开始采图
	ret = m_cam->camOp->MsvStartImageCapture();
	if (ret == -1)
		LOG_DEBUG(GlobalLog::logger, QString("MsvStartImageCapture() failed, error: %1").arg(ret).toStdWString().c_str());
	else
		LOG_DEBUG(GlobalLog::logger, QString("MsvStartImageCapture() successful").toStdWString().c_str());

}


void CameraLabelWidget::triggerCameraPhoto_Stream(Cameral* cam)
{
	{
		std::unique_lock<std::mutex> lock(cam->imageQueue.mutex);
		cam->imageQueue.stop_flag = false;
	}
	cam->imageQueue.cond.notify_all();

	int ret = 0;

	ret = cam->camOp->MsvStopImageCapture();


	// 设置触发模式
	ret = cam->camOp->SetTrrigerModel(1);
	if (ret == 0) {
		LOG_DEBUG(GlobalLog::logger, QString("SetTrrigerModel() successful").toStdWString().c_str());
	}
	else {
		LOG_DEBUG(GlobalLog::logger, QString("SetTrrigerModel() failed, ret = %1").arg(ret).toStdWString().c_str());
	}

	// 设置触发源
	ret = cam->camOp->SetTrrigerSource(7);
	if (ret == 0) {
		LOG_DEBUG(GlobalLog::logger, QString("SetTrrigerSource() successful").toStdWString().c_str());
	}
	else {
		LOG_DEBUG(GlobalLog::logger, QString("SetTrrigerSource() failed, ret = %1").arg(ret).toStdWString().c_str());
	}

	ret = cam->camOp->MsvStartImageCapture();

	startTask(cam, 1);

	Sleep(50);

	ret = cam->camOp->SetTrrigerModel(0);

	ret = cam->camOp->MsvStartImageCapture();
}

void CameraLabelWidget::triggerCameraPhoto(Cameral* cam)
{
	qDebug() << "photo ing 3";
	{
		std::unique_lock<std::mutex> lock(cam->imageQueue.mutex);
		cam->imageQueue.stop_flag = false;
	}
	qDebug() << "photo ing 5";
	cam->imageQueue.cond.notify_all();

	int ret = 0;

	ret = cam->camOp->MsvStopImageCapture();


	// 设置触发模式
	ret = cam->camOp->SetTrrigerModel(1);
	if (ret == 0) {
		LOG_DEBUG(GlobalLog::logger, QString("SetTrrigerModel() successful").toStdWString().c_str());
	}
	else {
		LOG_DEBUG(GlobalLog::logger, QString("SetTrrigerModel() failed, ret = %1").arg(ret).toStdWString().c_str());
	}

	// 设置触发源
	ret = cam->camOp->SetTrrigerSource(7);
	if (ret == 0) {
		LOG_DEBUG(GlobalLog::logger, QString("SetTrrigerSource() successful").toStdWString().c_str());
	}
	else {
		LOG_DEBUG(GlobalLog::logger, QString("SetTrrigerSource() failed, ret = %1").arg(ret).toStdWString().c_str());
	}
	qDebug() << "photo ing 4";
	ret = cam->camOp->MsvStartImageCapture();

	startTask(cam, 1);

	Sleep(50);

	ret = cam->camOp->SetTrrigerSource(cam->rounte);
	if (ret == 0) {
		LOG_DEBUG(GlobalLog::logger, QString("SetTrrigerSource() successful").toStdWString().c_str());
	}
	else {
		LOG_DEBUG(GlobalLog::logger, QString("SetTrrigerSource() failed, ret = %1").arg(ret).toStdWString().c_str());
		if(cam->rounte==0)ret = cam->camOp->SetTrrigerSource(1);
		else  ret = cam->camOp->SetTrrigerSource(0);
		LOG_DEBUG(GlobalLog::logger, QString("SetTrrigerSource() twice, ret = %1").arg(ret).toStdWString().c_str());
	}


	ret = cam->camOp->MsvStartImageCapture();
}

CameraLabelWidget::~CameraLabelWidget()
{
	qDebug() << "~CameraLabelWidget()析构函数调用";
	thread_exit = true;
	Sleep(100);
	// if (m_imageProcessor)//线程父对象是当前对象时不需要手动调用子类的析构函数
	// {

	//      delete m_imageProcessor;
	//     m_imageProcessor = nullptr;

	//     qDebug()<<" m_imageProcessor->deleteLater()";
	// }

	// 释放图像显示标签
	if (imageLabel)
	{
		delete imageLabel;
		imageLabel = nullptr;
	}

	// 释放固定文本标签
	//if (fixedText)
	//{
	//	delete fixedText;
	//	fixedText = nullptr;
	//}

	//// 释放菜单
	//if (cameraMenu)
	//{
	//	delete cameraMenu;
	//	cameraMenu = nullptr;
	//}
}

void CameraLabelWidget::ChangeDateDir(Cameral &cam)
{
	dataToSave.savePath_OK = cam.ok_path.toStdString();
	dataToSave.savePath_NG = cam.ng_path.toStdString();
	dataToSave.savePath_Pre = cam.localpath.toStdString();

}

void CameraLabelWidget::onViewImageClicked()
{
	if (!currentPixmap.isNull()) {
		auto* viewer = new ImageViewerWindow(currentPixmap, nullptr);
		viewer->setAttribute(Qt::WA_DeleteOnClose);
		viewer->show();
	}
	else {
		auto* viewer = new ImageViewerWindow(nullptr);
		viewer->setAttribute(Qt::WA_DeleteOnClose);
		viewer->show();
	}
}

void CameraLabelWidget::displayimg(QPixmap& pixmap)
{
	currentPixmap = pixmap;
	imageLabel->setPixmap(pixmap);
	qDebug() << "cccccccc";
}

void CameraLabelWidget::displayimg(HImage& himage)
{
	QPixmap pixmap = convertHImageToPixmap(himage);
	currentPixmap = pixmap;
	imageLabel->setPixmap(pixmap);
	qDebug() << "bbbbbbb";
}

QPixmap CameraLabelWidget::convertHImageToPixmap(const HImage& hImage) {
	const ImageHeader& header = hImage.imageHead;

	if (!hImage.data || header.width <= 0 || header.height <= 0 || header.channels <= 0 || header.length <= 0) {
		return QPixmap(); // 无效图像
	}

	QImage::Format format;
	switch (header.channels) {
	case 1:
		format = QImage::Format_Grayscale8;
		break;
	case 3:
		format = QImage::Format_RGB888;
		break;
	case 4:
		format = QImage::Format_RGBA8888;
		break;
	default:
		return QPixmap(); // 不支持的通道数
	}


	QImage image(hImage.data, header.width, header.height, header.width * header.channels, format);


	return QPixmap::fromImage(image.copy());
}

void CameraLabelWidget::stopLocalFileDetectorThread()
{
	if (m_localfilethread) {
		qDebug() << "尝试停止本地文件检测线程...";
		m_localfilethread->stop();
		m_localfilethread->quit();
		m_localfilethread->wait();

		delete m_localfilethread; // 显式删除线程对象
		m_localfilethread = nullptr;

		qDebug() << "本地文件检测线程已成功停止并销毁。";
	}
	else {
		qDebug() << "本地文件检测线程未运行或已停止。";
	}
}

void CameraLabelWidget::updateDebugValuesAsync(const QString& cameraKey, const DebugInfo* DI, const QString& filePath)
{
	if (!DI) {
		qWarning() << "CameraLabelWidget: DebugInfo pointer is null, cannot update/save.";
		return;
	}

	qDebug() << "CameraLabelWidget: Initiating async update and save for camera" << cameraKey;

	// 捕获必要值（线程安全）
	QString capturedCameraKey = cameraKey;
	QString capturedFilePath = filePath;
	double diScaleFactor = DI->scaleFactor;
	float diAngle = DI->Angle;
	int saveFlag = DI->saveflag;
	bool Sheild = DI->Shield;
	bool emptyIsOK = DI->EmptyIsOK;
	QFuture<bool> future = QtConcurrent::run([capturedCameraKey, capturedFilePath, diScaleFactor, diAngle, saveFlag, Sheild, emptyIsOK]() {
		qDebug() << "Background thread: Starting update and save for camera" << capturedCameraKey;

		QVariantMap rootMap = FileOperator::readJsonMap(capturedFilePath);
		if (rootMap.isEmpty()) {
			qWarning() << "Background thread: Failed to read JSON file:" << capturedFilePath;
			return false;
		}

		QString matchedKey;
		for (auto it = rootMap.begin(); it != rootMap.end(); ++it) {
			if (it.value().type() != QVariant::Map) continue;
			QVariantMap cameraMap = it.value().toMap();
			if (!cameraMap.contains("相机名称")) continue;

			QVariantMap nameField = cameraMap["相机名称"].toMap();
			if (nameField.contains("值") && nameField["值"].toString() == capturedCameraKey) {
				matchedKey = it.key();
				break;
			}
		}

		if (matchedKey.isEmpty()) {
			qWarning() << "Background thread: Camera name not found:" << capturedCameraKey;
			return false;
		}

		QVariantMap cameraMap = rootMap[matchedKey].toMap();

		// === 更新 "标定值" ===
		if (!cameraMap.contains("标定值")) {
			qWarning() << "Background thread: Missing '标定值' field for camera:" << matchedKey;
			return false;
		}
		{
			ParamDetail calib(cameraMap["标定值"].toMap());
			calib.value = QVariant::fromValue(diScaleFactor);
			cameraMap["标定值"] = calib.toVariantMap();
		}

		// === 更新 "角度" ===
		if (cameraMap.contains("角度")) {
			ParamDetail angle(cameraMap["角度"].toMap());
			angle.value = QVariant::fromValue(diAngle);
			cameraMap["角度"] = angle.toVariantMap();
		}
		else {
			qDebug() << "Background thread: '角度' field not found, creating it.";
			QVariantMap newAngle;
			newAngle["值"] = QVariant::fromValue(diAngle);
			newAngle["单位"] = "";
			newAngle["可见"] = "true";
			newAngle["检测"] = "";
			newAngle["类型"] = "float"; 
			newAngle["范围"] = "";

			cameraMap["角度"] = newAngle;
		}

		// === 更新 "存图开关" ===
		if (cameraMap.contains("存图开关")) {
			ParamDetail flag(cameraMap["存图开关"].toMap());
			flag.value = QVariant::fromValue(saveFlag);
			cameraMap["存图开关"] = flag.toVariantMap();
		}
		else {
			qWarning() << "Background thread: Missing '存图开关' field for camera:" << matchedKey << ", skipping flag update.";
			return false;
		}

		// === 更新 "屏蔽开关" ===
		if (cameraMap.contains("屏蔽开关")) {
			ParamDetail flag(cameraMap["屏蔽开关"].toMap());
			flag.value = QVariant::fromValue(Sheild);
			cameraMap["屏蔽开关"] = flag.toVariantMap();
		}
		else {
			qWarning() << "Background thread: Missing '屏蔽开关' field for camera:" << matchedKey << ", skipping flag update.";
			return false;
		}

		if (cameraMap.contains("空料OK")) {
			ParamDetail emptyOK(cameraMap["空料OK"].toMap());
			emptyOK.value = QVariant::fromValue(emptyIsOK);
			cameraMap["空料OK"] = emptyOK.toVariantMap();
		}
		else {
			qWarning() << "Background thread: Missing '空料OK' field for camera:" << matchedKey << ", skipping flag update.";
			return false;
		}
		rootMap[matchedKey] = cameraMap;

		if (!FileOperator::writeJsonMap(capturedFilePath, rootMap)) {
			qWarning() << "Background thread: Failed to write file:" << capturedFilePath;
			return false;
		}

		qDebug() << "Background thread: Successfully updated 标定值, 角度, 存图开关,  for camera" << capturedCameraKey;
		return true;
		});

	QFutureWatcher<bool>* watcher = new QFutureWatcher<bool>(this);
	watcher->setFuture(future);

	qDebug() << "CameraLabelWidget: updateDebugValuesAsync initiated, main thread continues.";
}

void CameraLabelWidget::onImageProcessed(std::shared_ptr<cv::Mat> processedImagePtr, DetectInfo info)
{
	m_cam->ui_signal.store(true);
	qDebug() << "this isonImageProcessed info.ret=:" << info.ret;
	if (!processedImagePtr || processedImagePtr->empty()) {
		LOG_DEBUG(GlobalLog::logger, _T("m_pParaDock ptr null"));
		return;
	}
	if (m_cam->video == false)
	{
		if (info.ret != 0)
		{
			dataToSave.work_path = dataToSave.savePath_NG;
			this->ngcount->fetch_add(1);
		}
		else dataToSave.work_path = dataToSave.savePath_OK;

		this->sumcount->fetch_add(1);
	}

	LOG_DEBUG(GlobalLog::logger, QString("camra:%1 sumcount: %2")
		.arg(m_cam->indentify.c_str()) 
		.arg(this->sumcount->load())
		.toStdWString().c_str());

	LOG_DEBUG(GlobalLog::logger, QString("camra:%1 ngcount: %2")
		.arg(m_cam->indentify.c_str()) 
		.arg(this->ngcount->load())
		.toStdWString().c_str());

	//dataToSave.imagePtr = processedImagePtr; // shared_ptr 的浅拷贝，引用计数增加



	//cv::Mat& mat_to_check = *processedImagePtr;// 用一个引用方便查看
	//qDebug() << "准备转换的 Mat 不是空的。";
	//qDebug() << "它的类型ID是: " << mat_to_check.type(); // 打印类型ID
	//qDebug() << "它的通道数是: " << mat_to_check.channels(); // 打印通道数
	//qDebug() << "它的位深度是: " << mat_to_check.depth(); // 打印位深度 (例如 CV_8U 是 0)




	QElapsedTimer timer;
	timer.start();  // 开始计时

	QImage displayImage = convertMatToQImage(processedImagePtr->clone());//备份

	//QImage displayImage = convertMatToQImage(*processedImagePtr);
	//      processedImagePtr.reset();
			// 2. 检查转换是否成功并显示
	if (!displayImage.isNull()) {
		// 3. 将 QImage 深拷贝到成员变量 currentPixmap8
		//this->currentPixmap = QPixmap::fromImage(displayImage).copy();
		this->currentPixmap = QPixmap::fromImage(displayImage);
		if (CheckPixmap(this->currentPixmap) == -1) return;

		try
		{
			if (m_cam->video == false)
			{
				if (m_cam->noneDisplay.load() == false && info.ret == -1)
				{
					ImagePaint::drawPaintDataEx(currentPixmap, m_cam->RI->m_PaintData, imageLabel->size());

				}
				else if (info.ret == 0) ImagePaint::drawPaintDataEx_VI(currentPixmap, m_cam->RI->m_PaintData, imageLabel->size());
				m_cam->noneDisplay.store(false);
				ImagePaint::drawDetectionResultExQt(currentPixmap, info);

			}
		}
		catch (...)
		{
			GlobalLog::logger.Mz_AddLog(L"!!! 捕获到致命错误 ...");
			return; // 失败了，直接从函数返回
		}

		std::shared_ptr<cv::Mat> afterImagePtr;
		try
		{
			cv::Mat mat = QPixmapToMat(currentPixmap).clone();
			afterImagePtr = std::make_shared<cv::Mat>(mat);
		}
		catch (...)
		{
			GlobalLog::logger.Mz_AddLog(L"!!! 捕获到致命错误 ...");
			return; // 失败了，直接从函数返回
		}

		if (!afterImagePtr || afterImagePtr->empty()) {
			LOG_DEBUG(GlobalLog::logger, _T("afterImagePtr ptr null"));
			return;
		}

		dataToSave.imagePtr = afterImagePtr; // shared_ptr 的浅拷贝，引用计数增加


		// 存图
		if (!m_cam->video && (m_cam->DI.saveflag == 3 || (m_cam->DI.saveflag <= 2 && info.ret == -1)))
		{
			std::unique_lock<std::mutex> lock(saveToQueue->mutex); // 获取互斥锁，保护保存队列

			if (saveToQueue->queue.size() > 100)
			{
				GlobalLog::logger.Mz_AddLog(L"deque size more than 100");
			}
			else
			{
				saveToQueue->queue.push_back(dataToSave); // 将 SaveData 对象推入队列
				LOG_DEBUG(GlobalLog::logger, QString("图像推入保存队列。当前队列大小：%1").arg(saveToQueue->queue.size()).toStdWString().c_str());

			}
			//      saveToQueue->queue.push_back(dataToSave); // 将 SaveData 对象推入队列
			//          qDebug() << "图像数据和信息已推入保存队列。当前队列大小：" << saveToQueue->queue.size();
		}
		saveToQueue->cond.notify_one(); // 通知保存线程
		// 4. 将深拷贝后的 QPixmap 显示在 ZoomableLabel 中
 //   this->currentPixmap = QPixmap();
		if (check_flag.load() == true)
		{
			parawidget->imageViewer->setPixmap(this->currentPixmap);
			check_flag.store(false);
		}
		else if (FullScreen.load() == false)imageLabel->setPixmap(this->currentPixmap);
		else emit FullShow(this->currentPixmap);
		qDebug() << "图像已成功深拷贝并显示在 ZoomableLabel 中。";

	}
	else {
		qWarning() << "CameraLabelWidget: 转换图像用于显示失败，无法显示。";
		LOG_DEBUG(GlobalLog::logger, _T("displayImage ptr null"));
	}
	qint64 elapsed = timer.elapsed();  // 获取经过的毫秒数
	qDebug() << "显示耗时：" << elapsed << "毫秒";
	m_cam->ui_signal.store(false);
}


void CameraLabelWidget::onSaveDebugInfo(DebugInfo* DI)
{

	updateDebugValuesAsync(m_cam->cameral_name, DI, SystemPara::CAMERAL_DIR);

}


void CameraLabelWidget::onImageProcessed_plate(std::shared_ptr<cv::Mat> processedImagePtr, DetectInfo info)
{
	
	qDebug() << "this isonImageProcessed info.ret=:"<<info.ret;
	if (!processedImagePtr || processedImagePtr->empty()) {	
		LOG_DEBUG(GlobalLog::logger, _T("m_pParaDock ptr null"));
		return;
	}
	if (info.ret != 0)
	{
		if(info.errmsg.isEmpty())
		{
			QString errstr = FirstFailedParamLabel(m_cam->RI->unifyParams);
			if (errstr == "") dataToSave.work_path = dataToSave.savePath_NG;
			dataToSave.work_path = dataToSave.savePath_NG + "/" + errstr.toStdString() + "/";
		}
		else
		{
			dataToSave.work_path = dataToSave.savePath_NG + "/" + info.errmsg.value(0).toStdString() + "/";
			//dataToSave.work_path = dataToSave.savePath_NG;
		}
		this->ngcount->fetch_add(1);
	}
	else
	{
		QTime currentTime = QTime::currentTime();
		int hour = currentTime.hour();  // 0~23
		std::string hourStr = std::to_string(hour);
		dataToSave.work_path = dataToSave.savePath_OK + "/" + hourStr + "/";
		//dataToSave.work_path = dataToSave.savePath_OK;

	}
	this->sumcount->fetch_add(1);
	LOG_DEBUG(GlobalLog::logger,QString("sumcount: %1").arg(this->sumcount->load()).toStdWString().c_str());


	QElapsedTimer timer;
	timer.start();  // 开始计时


	//转换显示
	QImage displayImage = convertMatToQImage(*processedImagePtr);
	//      processedImagePtr.reset();
			// 2. 检查转换是否成功并显示
	if (!displayImage.isNull()) {
		// 3. 将 QImage 深拷贝到成员变量 currentPixmap8
		//this->currentPixmap = QPixmap::fromImage(displayImage).copy();
		this->currentPixmap = QPixmap::fromImage(displayImage);
		if (CheckPixmap(this->currentPixmap) == -1) return;

		if (m_cam->video == false)
		{
			if (m_cam->noneDisplay.load() == false && (info.ret == -1 || info.ret == 1))
			{
				//ImagePaint::drawPaintDataEx(currentPixmap, m_cam->RI->m_PaintData, imageLabel->size());
				if (info.errmsg.isEmpty()) ImagePaint::drawPaintDataEx(currentPixmap, m_cam->RI->unifyParams, imageLabel->size());
				else ImagePaint::drawPaintDataEx_Ultra(currentPixmap, imageLabel->size(), info.errmsg);
			}
			//else if(info.ret==0) ImagePaint::drawPaintDataEx_VI(currentPixmap, m_cam->RI->m_PaintData, imageLabel->size());
			else if(info.ret==0)  ImagePaint::drawPaintDataEx(currentPixmap, m_cam->RI->unifyParams, imageLabel->size());
			m_cam->noneDisplay.store(false);
			ImagePaint::drawDetectionResultExQt(currentPixmap, info);
			
		}
		std::shared_ptr<cv::Mat> afterImagePtr;
		try
		{
			cv::Mat mat = QPixmapToMat(currentPixmap).clone();
			afterImagePtr = std::make_shared<cv::Mat>(mat);
		}
		catch (...)
		{
			GlobalLog::logger.Mz_AddLog(L"!!! 捕获到致命错误 ...");
			return; // 失败了，直接从函数返回
		}

		if (!afterImagePtr || afterImagePtr->empty()) {			
			LOG_DEBUG(GlobalLog::logger, _T("afterImagePtr ptr null"));
			return;
		}

		dataToSave.imagePtr = afterImagePtr; // shared_ptr 的浅拷贝，引用计数增加


		// 存图
		if (!m_cam->video && (m_cam->DI.saveflag == 3 || (m_cam->DI.saveflag <= 2 && info.ret == -1)))
		{
			std::unique_lock<std::mutex> lock(saveToQueue->mutex); // 获取互斥锁，保护保存队列

			if (saveToQueue->queue.size() > 100)
			{
				GlobalLog::logger.Mz_AddLog(L"deque size more than 100");
			}
			else
			{
				saveToQueue->queue.push_back(dataToSave); // 将 SaveData 对象推入队列
				qDebug() << "图像数据和信息已推入保存队列。当前队列大小：" << saveToQueue->queue.size();
				
			}
			//      saveToQueue->queue.push_back(dataToSave); // 将 SaveData 对象推入队列
			//          qDebug() << "图像数据和信息已推入保存队列。当前队列大小：" << saveToQueue->queue.size();
		}
		saveToQueue->cond.notify_one(); // 通知保存线程
		// 4. 将深拷贝后的 QPixmap 显示在 ZoomableLabel 中
 //   this->currentPixmap = QPixmap();
		if (check_flag.load() == true)
		{
			parawidget->imageViewer->setPixmap(this->currentPixmap);
			check_flag.store(false);
		}
		else if (FullScreen.load() == false)imageLabel->setPixmap(this->currentPixmap);
		else emit FullShow(this->currentPixmap);
		qDebug() << "图像已成功深拷贝并显示在 ZoomableLabel 中。";

	}
	else {
		qWarning() << "CameraLabelWidget: 转换图像用于显示失败，无法显示。";
		LOG_DEBUG(GlobalLog::logger, _T("displayImage ptr null"));
	}
	qint64 elapsed = timer.elapsed();  // 获取经过的毫秒数
	qDebug() << "显示耗时：" << elapsed << "毫秒";

}

void CameraLabelWidget::onImageProcessed_flower(std::shared_ptr<cv::Mat> processedImagePtr, DetectInfo info)
{
	qDebug() << "this isonImageProcessed info.ret=:" << info.ret;
	if (!processedImagePtr || processedImagePtr->empty()) {
		LOG_DEBUG(GlobalLog::logger, _T("m_pParaDock ptr null"));
		return;
	}
	if (info.ret == -1)
	{
		dataToSave.work_path = dataToSave.savePath_NG;
		this->ngcount->fetch_add(1);
	}
	else dataToSave.work_path = dataToSave.savePath_OK;
	this->sumcount->fetch_add(1);
	LOG_DEBUG(GlobalLog::logger, QString("sumcount: %1").arg(this->sumcount->load()).toStdWString().c_str());


	QElapsedTimer timer;
	timer.start();  // 开始计时


	//转换显示
	QImage displayImage = convertMatToQImage(*processedImagePtr);
	//      processedImagePtr.reset();
			// 2. 检查转换是否成功并显示
	if (!displayImage.isNull()) {
		// 3. 将 QImage 深拷贝到成员变量 currentPixmap8
		//this->currentPixmap = QPixmap::fromImage(displayImage).copy();
		this->currentPixmap = QPixmap::fromImage(displayImage);
		if (CheckPixmap(this->currentPixmap) == -1) return;

		if (m_cam->video == false)
		{
			if (m_cam->noneDisplay.load() == false)
			{				
				ImagePaint::drawPaintDataEx_flower(currentPixmap, m_cam->RI->unifyParams, imageLabel->size());
			}			
			m_cam->noneDisplay.store(false);
			ImagePaint::drawDetectionResultExQt(currentPixmap, info);

		}
		std::shared_ptr<cv::Mat> afterImagePtr;
		try
		{
			cv::Mat mat = QPixmapToMat(currentPixmap).clone();
			afterImagePtr = std::make_shared<cv::Mat>(mat);
		}
		catch (...)
		{
			GlobalLog::logger.Mz_AddLog(L"!!! 捕获到致命错误 ...");
			return; // 失败了，直接从函数返回
		}

		if (!afterImagePtr || afterImagePtr->empty()) {
			LOG_DEBUG(GlobalLog::logger, _T("afterImagePtr ptr null"));
			return;
		}

		dataToSave.imagePtr = afterImagePtr; // shared_ptr 的浅拷贝，引用计数增加


		// 存图
		if (!m_cam->video && (m_cam->DI.saveflag == 3 || (m_cam->DI.saveflag <= 2 && info.ret == -1)))
		{
			std::unique_lock<std::mutex> lock(saveToQueue->mutex); // 获取互斥锁，保护保存队列

			if (saveToQueue->queue.size() > 100)
			{
				GlobalLog::logger.Mz_AddLog(L"deque size more than 100");
			}
			else
			{
				saveToQueue->queue.push_back(dataToSave); // 将 SaveData 对象推入队列
				qDebug() << "图像数据和信息已推入保存队列。当前队列大小：" << saveToQueue->queue.size();

			}
			//      saveToQueue->queue.push_back(dataToSave); // 将 SaveData 对象推入队列
			//          qDebug() << "图像数据和信息已推入保存队列。当前队列大小：" << saveToQueue->queue.size();
		}
		saveToQueue->cond.notify_one(); // 通知保存线程
		// 4. 将深拷贝后的 QPixmap 显示在 ZoomableLabel 中
 //   this->currentPixmap = QPixmap();
		if (check_flag.load() == true)
		{
			parawidget->imageViewer->setPixmap(this->currentPixmap);
			check_flag.store(false);
		}
		else if (FullScreen.load() == false)imageLabel->setPixmap(this->currentPixmap);
		else emit FullShow(this->currentPixmap);
		qDebug() << "图像已成功深拷贝并显示在 ZoomableLabel 中。";

	}
	else {
		qWarning() << "CameraLabelWidget: 转换图像用于显示失败，无法显示。";
		LOG_DEBUG(GlobalLog::logger, _T("displayImage ptr null"));
	}
	qint64 elapsed = timer.elapsed();  // 获取经过的毫秒数
	qDebug() << "显示耗时：" << elapsed << "毫秒";
}


void CameraLabelWidget::onImageProcessed_Brader(std::shared_ptr<cv::Mat> processedImagePtr, DetectInfo info)
{

	qDebug() << "this isonImageProcessed info.ret=:" << info.ret;
	if (!processedImagePtr || processedImagePtr->empty()) {
		qWarning() << "CameraLabelWidget: 收到空或无效的处理后图像指针，跳过处理。";
		LOG_DEBUG(GlobalLog::logger, _T("m_pParaDock ptr null"));
		//return;
	}
	if (info.ret == -1||info.ret==1 || info.ret>=3)
	{
		dataToSave.work_path = dataToSave.savePath_NG;
		if (this->ngDisplay.load() == false) this->ngcount->fetch_add(1);
	}
	else  dataToSave.work_path = dataToSave.savePath_OK;
	this->sumcount->fetch_add(1);
	LOG_DEBUG(GlobalLog::logger, QString("sumcount: %1").arg(this->sumcount->load()).toStdWString().c_str());



	//转换显示
	QImage displayImage = convertMatToQImage(*processedImagePtr);
	//      processedImagePtr.reset();
			// 2. 检查转换是否成功并显示
	if (!displayImage.isNull()) {
		// 3. 将 QImage 深拷贝到成员变量 currentPixmap8
		//this->currentPixmap = QPixmap::fromImage(displayImage).copy();
		this->currentPixmap = QPixmap::fromImage(displayImage);


		if (m_cam->video == false)
		{
			if (m_cam->noneDisplay.load() == false && info.ret == -1)
			{
				ImagePaint::drawPaintDataEx(currentPixmap, m_cam->RI->m_PaintData, imageLabel->size());
				this->ngDisplay.store(true);
			}
			else if (info.ret == 0)
			{
				ImagePaint::drawPaintDataEx_V(currentPixmap, m_cam->RI->m_PaintData, imageLabel->size());
				this->ngDisplay.store(false);
			}
			else if (info.ret == 1)
			{
				info.ret = -1;
				ImagePaint::drawPaintDataEx_I(currentPixmap, m_cam->RI->m_PaintData, imageLabel->size());
				this->ngDisplay.store(true);
			}
			else if (info.ret == 3)
			{
				ImagePaint::drawPaintDataEx_II(currentPixmap, m_cam->RI->m_PaintData, imageLabel->size());
				this->ngDisplay.store(true);
			}
			else if (info.ret == 4)
			{
				ImagePaint::drawPaintDataEx_III(currentPixmap, m_cam->RI->m_PaintData, imageLabel->size());
				this->ngDisplay.store(true);
			}
			else if (info.ret == 5)
			{
				ImagePaint::drawPaintDataEx_PixmapUltra(currentPixmap, "套管不良", imageLabel->size());
				this->ngDisplay.store(true);
			}
			else if(info.ret==6)
			{
				ImagePaint::drawPaintDataEx_PixmapUltra(currentPixmap, "针脚不良", imageLabel->size());
				this->ngDisplay.store(true);
			}
			else if(info.ret==7)
			{
				ImagePaint::drawPaintDataEx_PixmapUltra(currentPixmap, "顶面异常", imageLabel->size());
				this->ngDisplay.store(true);
			}
			else if(info.ret==8)
			{
				ImagePaint::drawPaintDataEx_PixmapUltra(currentPixmap, "针数不对", imageLabel->size());
				this->ngDisplay.store(true);
			}
			m_cam->noneDisplay.store(false);
			ImagePaint::drawDetectionResultExQt(currentPixmap, info);

		}
		cv::Mat mat = QPixmapToMat(currentPixmap).clone();
		std::shared_ptr<cv::Mat> afterImagePtr = std::make_shared<cv::Mat>(mat);

		if (!afterImagePtr || afterImagePtr->empty()) {
			qWarning() << "CameraLabelWidget: 收到空或无效的处理后图像指针，跳过处理。";
			LOG_DEBUG(GlobalLog::logger, _T("afterImagePtr ptr null"));
			return;
		}

		dataToSave.imagePtr = afterImagePtr; // shared_ptr 的浅拷贝，引用计数增加


		// 存图
		if (!m_cam->video && (m_cam->DI.saveflag == 3 || (m_cam->DI.saveflag <= 2 && (info.ret == -1 || info.ret==3 || info.ret==4))))
		{
			std::unique_lock<std::mutex> lock(saveToQueue->mutex); // 获取互斥锁，保护保存队列

			if (saveToQueue->queue.size() > 100)
			{
				GlobalLog::logger.Mz_AddLog(L"deque size more than 100");
			}
			else
			{
				saveToQueue->queue.push_back(dataToSave); // 将 SaveData 对象推入队列
				qDebug() << "图像数据和信息已推入保存队列。当前队列大小：" << saveToQueue->queue.size();

			}
			
		}
		saveToQueue->cond.notify_one(); // 通知保存线程


		if(this->ngDisplay.load()==false || m_cam->video.load()==true || m_cam->DI.Shield==true) FullScreenWindow::ShowOriginalSize(currentPixmap,true);
		else
		{
			FullScreenWindow::ShowOriginalSize(currentPixmap, false);
			return;
		}

		if (check_flag.load() == true)
		{
			parawidget->imageViewer->setPixmap(this->currentPixmap);
			check_flag.store(false);
		}
		else if (FullScreen.load() == false)imageLabel->setPixmap(this->currentPixmap);
		else emit FullShow(this->currentPixmap);
		qDebug() << "图像已成功深拷贝并显示在 ZoomableLabel 中。";

	}
	else {
		qWarning() << "CameraLabelWidget: 转换图像用于显示失败，无法显示。";
		LOG_DEBUG(GlobalLog::logger, _T("displayImage ptr null"));
	}


}


void CameraLabelWidget::startTask(Cameral* cam, [[maybe_unused]] int seconds)
{
	std::thread([cam]() {
		// std::this_thread::sleep_for(std::chrono::milliseconds(10));
		qDebug() << "Dsdgfgffdfgds";
		int ret = cam->camOp->SoftTrigger();
		if (ret == 0) qDebug() << ">SoftTrigger() successful";
		else qDebug() << "soft trigger fail:  " << ret;


		}).detach();
}


void CameraLabelWidget::onStartGetIn()
{
	QtConcurrent::run([this]() {
		while (PCI::pci().getinput(5) == 0 && !thread_exit)
		{
			QThread::msleep(200);
			qDebug() << "this is in the thread";
		}
		QMetaObject::invokeMethod(this, "onCameraPhoto", Qt::QueuedConnection);
		LOG_DEBUG(GlobalLog::logger, L"onCameraStart");
		});

}

void CameraLabelWidget::onImageProcessed_QImage(QImage image, DetectInfo info)
{
	//m_cam->ui_signal.store(true);
	qDebug() << "onImageProcessed info.ret=:" << info.ret;

	// 检查输入
	if (image.isNull()) {
		LOG_DEBUG(GlobalLog::logger, L"onImageProcessed: input QImage is null");
		m_cam->ui_signal.store(false);
		return;
	}

	// 计数与保存路径
	if (!m_cam->video)
	{
		if (info.ret != 0) {
			dataToSave.work_path = dataToSave.savePath_NG;
			this->ngcount->fetch_add(1);
		}
		else {
			dataToSave.work_path = dataToSave.savePath_OK;
		}
		this->sumcount->fetch_add(1);
	}

	LOG_DEBUG(GlobalLog::logger, QString("camra:%1 sumcount: %2")
		.arg(m_cam->indentify.c_str())
		.arg(this->sumcount->load())
		.toStdWString().c_str());

	LOG_DEBUG(GlobalLog::logger, QString("camra:%1 ngcount: %2")
		.arg(m_cam->indentify.c_str())
		.arg(this->ngcount->load())
		.toStdWString().c_str());

	QElapsedTimer timer;
	timer.start();

	// 深拷贝 QImage 用于显示和存储
	QImage displayImage = image.copy();

	// 绘制 PaintData
	if (!info.paintDataSnapshot.isEmpty())
	{
		if (info.ret == 0) // VI 情况
			ImagePaint::drawPaintDataEx_VI_QImage(displayImage, info.paintDataSnapshot, imageLabel->size());
		else
			ImagePaint::drawPaintDataEx_QImage(displayImage, info.paintDataSnapshot, imageLabel->size());
	}

	// 绘制检测结果
	ImagePaint::drawDetectionResultExQImage(displayImage, info);

	this->currentPixmap = QPixmap::fromImage(displayImage);

	// 存储到队列
	if (!m_cam->video && (m_cam->DI.saveflag == 3 || (m_cam->DI.saveflag <= 2 && info.ret == -1)))
	{
		SaveData saveItem;
		saveItem.savePath_Pre = dataToSave.savePath_Pre;
		saveItem.work_path = dataToSave.work_path;
		saveItem.savePath_OK = dataToSave.savePath_OK;
		saveItem.savePath_NG = dataToSave.savePath_NG;
		saveItem.info = dataToSave.info;

		saveItem.image = displayImage.copy(); // 深拷贝，队列使用

		{
			std::unique_lock<std::mutex> lock(saveToQueue->mutex);
			if (saveToQueue->queue.size() > 100) {
				GlobalLog::logger.Mz_AddLog(L"deque size more than 100");
			}
			else {
				saveToQueue->queue.push_back(saveItem);
				LOG_DEBUG(GlobalLog::logger, QString("图像推入保存队列。当前队列大小：%1")
					.arg(saveToQueue->queue.size()).toStdWString().c_str());
			}
		}
		saveToQueue->cond.notify_one();
	}

	// 显示
	if (check_flag.load()) {
		QPixmap displayPixmap = QPixmap::fromImage(image);
		parawidget->imageViewer->setPixmap(displayPixmap);
		check_flag.store(false);
	}
	else if (!FullScreen.load()) {
		imageLabel->setImage(displayImage);
	}
	else {
		QPixmap displayPixmap = QPixmap::fromImage(image);
		emit FullShow(displayPixmap);
	}

	qDebug() << "显示耗时：" << timer.elapsed() << "毫秒";
	//m_cam->ui_signal.store(false);
}


void CameraLabelWidget::triggerCameraStart(Cameral* cam)
{

	int ret;

	{
		std::unique_lock<std::mutex> lock(cam->imageQueue.mutex);
		cam->imageQueue.stop_flag = false;
	}
	cam->imageQueue.cond.notify_all();


	// 打开设备
	
		ret = cam->camOp->MsvOpenDevice();
		if (ret != 0)
		{
			QString logMsg = QString("open cameral ret value: %1").arg(ret);

			// 转为 wchar_t* 并调用日志宏
			LOG_DEBUG(GlobalLog::logger, logMsg.toStdWString().c_str());
			QString warningMessage = cam->cameral_name + "相机未连接";
			QMessageBox::warning(nullptr, "连接警告", warningMessage);


		}
		Sleep(50);

		ret = cam->camOp->RegisterImageCallBack(MyImageCallback, &cam->imageQueue);

		if (ret != 0)
		{
			QString logMsg = QString("Register cameral ret value: %1").arg(ret);

			// 转为 wchar_t* 并调用日志宏
			LOG_DEBUG(GlobalLog::logger, logMsg.toStdWString().c_str());

		}
		Sleep(50);
		ret = cam->camOp->SetTrrigerModel(1);
		if (ret != 0)
		{
			QString logMsg = QString("settrigger cameral ret value: %1").arg(ret);

			// 转为 wchar_t* 并调用日志宏
			LOG_DEBUG(GlobalLog::logger, logMsg.toStdWString().c_str());

		}
		Sleep(50);
		ret = cam->camOp->SetTrrigerSource(cam->rounte);
		if (ret == 0) qDebug() << "set cam->route: cam->rounte successful";
		else if (ret != 0)
		{
			qDebug() << "cam->rounte:       " << cam->rounte<<"ret=" <<ret;
			QString logMsg = QString("set triggsoureceret value: %1").arg(ret);

			LOG_DEBUG(GlobalLog::logger, logMsg.toStdWString().c_str());

			if (cam->rounte == 0)ret = cam->camOp->SetTrrigerSource(1);
			else  ret = cam->camOp->SetTrrigerSource(0);
			LOG_DEBUG(GlobalLog::logger, QString("SetTrrigerSource() twice, ret = %1").arg(ret).toStdWString().c_str());

		}
		Sleep(50);

	//	ret = cam->camOp->MsvSetAcquisitionMode(0);

	//    开始采图
		ret = cam->camOp->MsvStartImageCapture();
		if (ret != 0)
		{
			QString logMsg = QString("MsvStartImageCapture value: %1").arg(ret);

			// 转为 wchar_t* 并调用日志宏
			LOG_DEBUG(GlobalLog::logger, logMsg.toStdWString().c_str());

		}


	Sleep(50);//必须休息，不然启动不了
	//  cam->imageQueue.queue.clear();


}

void CameraLabelWidget::triggerCameraStop(Cameral* cam)
{
	if (GlobalPara::envirment == GlobalPara::IPCEn)
	{
		unsigned short durationMs = 100;
		bool outputSignalInvert = false;
		PCI::pci().setOutputMode(GlobalPara::RunPoint, outputSignalInvert ? true : false, durationMs);

	}
	cam->running.store(false);
	cam->video = false;
	cam->local.store(false);

	// 停止采图
	cam->camOp->MsvStopImageCapture();

	cam->camOp->SetTrrigerModel(1);

	int ret = cam->camOp->SetTrrigerSource(cam->rounte);
	if (ret == 0) {
		LOG_DEBUG(GlobalLog::logger, QString("SetTrrigerSource() successful").toStdWString().c_str());
	}
	else {
		LOG_DEBUG(GlobalLog::logger, QString("SetTrrigerSource() failed, ret = %1").arg(ret).toStdWString().c_str());
		if (cam->rounte == 0)ret = cam->camOp->SetTrrigerSource(1);
		else  ret = cam->camOp->SetTrrigerSource(0);
		LOG_DEBUG(GlobalLog::logger, QString("SetTrrigerSource() twice, ret = %1").arg(ret).toStdWString().c_str());
	}


	//cam->camOp->MsvCloseDevice();
	//  cam->imageQueue.queue.clear();


	{
		std::unique_lock<std::mutex> lock(cam->imageQueue.mutex);
		cam->imageQueue.stop_flag = true;
	}
	cam->imageQueue.cond.notify_all();

	stopLocalFileDetectorThread();

}

