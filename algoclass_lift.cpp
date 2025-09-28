#include "algoclass_lift.h"
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


AlgoClass_Lift::AlgoClass_Lift(QObject *parent)
    : AlgoClass{parent}
{}

AlgoClass_Lift::AlgoClass_Lift(QString algopath, int al_core, float* Angle, QObject* parent)
    : AlgoClass(algopath, parent),
    al_core(al_core), 
    Angle(Angle)      
{
   
}

QWidget* AlgoClass_Lift::createLeftPanel(QWidget* parent)
{
	QWidget* panel = new QWidget(parent);
	QVBoxLayout* layout = new QVBoxLayout(panel);

	QPushButton* btn0 = new QPushButton("启动翘脚算法", panel);
	QPushButton* btn1 = new QPushButton("翘脚 第一模型输出图", panel);
	QPushButton* btn2 = new QPushButton("翘脚 检测结果输出图", panel); 

	layout->addWidget(btn0);
	layout->addWidget(btn1);
	layout->addWidget(btn2); 

	layout->addStretch();

	connect(btn0, &QPushButton::clicked, this, &AlgoClass_Lift::onRunLift);
	connect(btn1, &QPushButton::clicked, this, &AlgoClass_Lift::onGetLiftDetect1Img);
	connect(btn2, &QPushButton::clicked, this, &AlgoClass_Lift::onResultOutLift); 

	layout->addStretch();
	return panel;
}

void AlgoClass_Lift::onGetLiftDetect1Img()
{
	if (image.empty()) {
		qWarning() << "onGetStampDetectImg: member variable 'image' is empty. Please set input image first.";
		return;
	}

	ExportSpace::GetLiftDetect1Img(image, al_core);

	emit TransMat(image);
}

void AlgoClass_Lift::onResultOutLift()
{
	if (image.empty()) {
		qWarning() << "onResultOutLift: member variable 'image' is empty. Please set input image first.";
		return;
	}

	OutLiftResParam outParam;    

	ExportSpace::ResultOutLift(image, outParam, al_core);


	emit TransMat(image);

}

void AlgoClass_Lift::onRunLift()
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

	ExportSpace::RunLift(temp,al_core,*Angle,true);

	image = temp;
}