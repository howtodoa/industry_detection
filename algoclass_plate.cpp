#include "algoclass_plate.h"
#include <QObject>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include "typdef.h"
#include <QFileDialog>
#include <QMessageBox>

AlgoClass_Plate::AlgoClass_Plate(QObject *parent)
    : AlgoClass{parent}
{}

AlgoClass_Plate::AlgoClass_Plate(QString algopath, int al_core, float* Angle, QObject* parent)
    : AlgoClass(algopath, parent),
    al_core(al_core), 
    Angle(Angle)     
{
   
}

QWidget* AlgoClass_Plate::createLeftPanel(QWidget* parent)
{
	QWidget* panel = new QWidget(parent);
	QVBoxLayout* layout = new QVBoxLayout(panel);

	QPushButton* btn0 = new QPushButton("启动座板算法", panel);
	QPushButton* btn1 = new QPushButton("座板 第一模型输出图", panel);
	QPushButton* btn2 = new QPushButton("座板 第二模型输出图", panel);
	QPushButton* btn3 = new QPushButton("座板 检测结果输出图", panel);

	layout->addWidget(btn0);
	layout->addWidget(btn1);
	layout->addWidget(btn2);
	layout->addWidget(btn3);

	layout->addStretch();

	connect(btn0, &QPushButton::clicked, this, &AlgoClass_Plate::onRunPlate);
	connect(btn1, &QPushButton::clicked, this, &AlgoClass_Plate::onGetSegmentPlateImg);
	connect(btn2, &QPushButton::clicked, this, &AlgoClass_Plate::onGetDetectorPlateImg);
	connect(btn3, &QPushButton::clicked, this, &AlgoClass_Plate::onResultOutPlate);

	layout->addStretch();
	return panel;
}


void AlgoClass_Plate::onGetDetectorPlateImg()
{
	if (image.empty()) {
		qWarning() << "onGetStampDetectImg: member variable 'image' is empty. Please set input image first.";
		return;
	}

	ExportSpace::GetDetectorPlateImg(image, al_core);

	emit TransMat(image);
}

void AlgoClass_Plate::onResultOutPlate()
{
	if (image.empty()) {
		qWarning() << "onResultOutPlate: member variable 'image' is empty. Please set input image first.";
		return;
	}


	OutPlateResParam outParam;

	ExportSpace::ResultOutPlate(image, outParam, al_core);

	emit TransMat(image);
}


void AlgoClass_Plate::onRunPlate()
{
	QString defaultPath = "../../../data";  
	QString filePath = QFileDialog::getOpenFileName(
		nullptr,
		"选择图像文件",
		defaultPath,
		"图像文件 (*.png *.jpg *.bmp *.jpeg *.tif *.tiff);;所有文件 (*.*)");

	if (filePath.isEmpty()) {
		return;
	}

	QString m_tempImagePath = filePath;
	qDebug() << "用户选择图片路径：" << filePath;

	cv::Mat temp = cv::imread(filePath.toStdString());
	if (temp.empty()) {
		QMessageBox::warning(nullptr, "错误", "加载失败！");
		return;
	}

	TransMat(temp);

	ExportSpace::RunPlate(temp, al_core, *Angle);

	image = temp;
}

void AlgoClass_Plate::onGetSegmentPlateImg()
{
	if (image.empty()) {
		qWarning() << "onGetStampDetectImg: member variable 'image' is empty. Please set input image first.";
		return;
	}

	ExportSpace::GetSegmentPlateImg(image, al_core);

	emit TransMat(image);
}
