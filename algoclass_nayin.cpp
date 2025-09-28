#include "algoclass_nayin.h"
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
#include <QJsonDocument>
#include <QJsonObject>
#include <QComboBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QtConcurrent/QtConcurrentRun>
#include <QMap>
#include <QVariant>

AlgoClass_NaYin::AlgoClass_NaYin(QObject* parent)
	: AlgoClass{ parent }
{}

AlgoClass_NaYin::AlgoClass_NaYin(QString algopath, int al_core, float* Angle, QObject* parent)
	: AlgoClass(algopath, parent),
	al_core(al_core),
	Angle(Angle)
{
	QVariantMap map = FileOperator::readJsonMap(algopath);

	if (al_core == 1) {
		if (map.contains("textkernl")) {
			ParamDetail detail(map.value("textkernl").toMap());
			LearnPara::inParam1.textkernl = detail.value.toInt();
		}
		if (map.contains("logoid")) {
			ParamDetail detail(map.value("logoid").toMap());
			LearnPara::inParam1.logoid = detail.value.toInt();
		}
		if (map.contains("textextralenabel")) {
			ParamDetail detail(map.value("textextralenabel").toMap());
			LearnPara::inParam1.textextralenabel = detail.value.toBool();
		}
		if (map.contains("textMissPixEnable")) {
			ParamDetail detail(map.value("textMissPixEnable").toMap());
			LearnPara::inParam1.textMissPixEnable = detail.value.toBool();
		}
		if (map.contains("isRect")) {
			ParamDetail detail(map.value("isRect").toMap());
			LearnPara::inParam1.isRect = detail.value.toBool();
		}
		if (map.contains("isOCR")) {
			ParamDetail detail(map.value("isOCR").toMap());
			LearnPara::inParam1.isOCR = detail.value.toBool();
		}
	}
	else if (al_core == 0) {
		if (map.contains("textkernl")) {
			ParamDetail detail(map.value("textkernl").toMap());
			LearnPara::inParam2.textkernl = detail.value.toInt();
		}
		if (map.contains("logoid")) {
			ParamDetail detail(map.value("logoid").toMap());
			LearnPara::inParam2.logoid = detail.value.toInt();
		}
		if (map.contains("textextralenabel")) {
			ParamDetail detail(map.value("textextralenabel").toMap());
			LearnPara::inParam2.textextralenabel = detail.value.toBool();
		}
		if (map.contains("textMissPixEnable")) {
			ParamDetail detail(map.value("textMissPixEnable").toMap());
			LearnPara::inParam2.textMissPixEnable = detail.value.toBool();
		}
		if (map.contains("isRect")) {
			ParamDetail detail(map.value("isRect").toMap());
			LearnPara::inParam2.isRect = detail.value.toBool();
		}
		if (map.contains("isOCR")) {
			ParamDetail detail(map.value("isOCR").toMap());
			LearnPara::inParam2.isOCR = detail.value.toBool();
		}
	}

}

QWidget* AlgoClass_NaYin::createLeftPanel(QWidget* parent)
{
	QWidget* panel = new QWidget(parent);
	QVBoxLayout* layout = new QVBoxLayout(panel);

	// === 按钮区域 ===
	// 按钮代码与之前相同，此处省略以保持简洁
	QPushButton* btn0 = new QPushButton("启动捺印算法", panel);
	QPushButton* btn1 = new QPushButton("捺印 第一模型输出图", panel);
	QPushButton* btn2 = new QPushButton("捺印 第二模型输出图", panel);
	QPushButton* btn3 = new QPushButton("捺印 第三模型输出图", panel);
	QPushButton* btn4 = new QPushButton("捺印 第三模型输入图", panel);
	QPushButton* btn5 = new QPushButton("捺印 第三模型分割图", panel);
	QPushButton* btn6 = new QPushButton("捺印 检测结果输出图", panel);

	layout->addWidget(btn0);
	layout->addWidget(btn1);
	layout->addWidget(btn2);
	layout->addWidget(btn4);
	layout->addWidget(btn3);
	layout->addWidget(btn5);
	layout->addWidget(btn6);

	connect(btn0, &QPushButton::clicked, this, &AlgoClass_NaYin::onRunStamp);
	connect(btn1, &QPushButton::clicked, this, &AlgoClass_NaYin::onGetStampMaskExpect);
	connect(btn2, &QPushButton::clicked, this, &AlgoClass_NaYin::onGetStampSegmentImg);
	connect(btn3, &QPushButton::clicked, this, &AlgoClass_NaYin::onGetStampDetectImg);
	connect(btn4, &QPushButton::clicked, this, &AlgoClass_NaYin::onGetStampCropimage);
	connect(btn5, &QPushButton::clicked, this, &AlgoClass_NaYin::onGetStampCropSegmentImg);
	connect(btn6, &QPushButton::clicked, this, &AlgoClass_NaYin::onResultOutStamp);

	// === 参数配置区 ===
	layout->addSpacing(20);
	QLabel* lblParam = new QLabel("捺印算法参数配置", panel);
	layout->addWidget(lblParam);

	// 根据 al_core 选择正确的参数集
	const auto& currentParams = (al_core == 1) ? LearnPara::inParam1 : LearnPara::inParam2;

	// 字符核大小
	QHBoxLayout* kernLayout = new QHBoxLayout;
	QLabel* lblKern = new QLabel("字符核大小:", panel);
	QLineEdit* editKern = new QLineEdit(QString::number(currentParams.textkernl), panel);
	kernLayout->addWidget(lblKern);
	kernLayout->addWidget(editKern);
	layout->addLayout(kernLayout);

	// log标签
	QHBoxLayout* logoidLayout = new QHBoxLayout;
	QLabel* lblLogo = new QLabel("负极LOG标签:", panel);
	QLineEdit* editLogo = new QLineEdit(QString::number(currentParams.logoid), panel);
	logoidLayout->addWidget(lblLogo);
	logoidLayout->addWidget(editLogo);
	layout->addLayout(logoidLayout);

	// 勾选项
	QCheckBox* chkExtra = new QCheckBox("字符超出检测", panel);
	chkExtra->setChecked(currentParams.textextralenabel);
	layout->addWidget(chkExtra);

	QCheckBox* chkMiss = new QCheckBox("字符缺失检测", panel);
	chkMiss->setChecked(currentParams.textMissPixEnable);
	layout->addWidget(chkMiss);

	QCheckBox* chkRect = new QCheckBox("负极矩形区域", panel);
	chkRect->setChecked(currentParams.isRect);
	layout->addWidget(chkRect);

	QCheckBox* chkOCR = new QCheckBox("OCR检测", panel);
	chkOCR->setChecked(currentParams.isOCR);
	layout->addWidget(chkOCR);

	// 保存按钮
	QPushButton* btnSave = new QPushButton("保存参数", panel);
	layout->addWidget(btnSave);

	// 保存时根据 al_core 将参数写回正确的结构体
	connect(btnSave, &QPushButton::clicked, this, [=]() mutable {
		if (al_core == 1) {
			LearnPara::inParam1.textkernl = editKern->text().toInt();
			LearnPara::inParam1.logoid = editLogo->text().toInt();
			LearnPara::inParam1.textextralenabel = chkExtra->isChecked();
			LearnPara::inParam1.textMissPixEnable = chkMiss->isChecked();
			LearnPara::inParam1.isRect = chkRect->isChecked();
			LearnPara::inParam1.isOCR = chkOCR->isChecked();
		}
		else {
			LearnPara::inParam2.textkernl = editKern->text().toInt();
			LearnPara::inParam2.logoid = editLogo->text().toInt();
			LearnPara::inParam2.textextralenabel = chkExtra->isChecked();
			LearnPara::inParam2.textMissPixEnable = chkMiss->isChecked();
			LearnPara::inParam2.isRect = chkRect->isChecked();
			LearnPara::inParam2.isOCR = chkOCR->isChecked();
		}
		saveParamAsync();
		});

	layout->addStretch();
	return panel;
}

void AlgoClass_NaYin::onGetStampMaskExpect()
{
	if (image.empty()) {
	    qWarning() << "onGetStampMaskExpect: member variable 'image' is empty. Please set input image first.";
	    return;
	}

	ExportSpace::GetStampMaskExpect(image, al_core); 

	emit TransMat(image);
}

void AlgoClass_NaYin::onResultOutStamp()
{
	if (image.empty()) {
		qWarning() << "onResultOutStamp: member variable 'image' is empty. Please set input image first.";
		return;
	}

	OutStampResParam outParam;      
	ExportSpace::ResultOutStamp(image, outParam, al_core);

	emit TransMat(image);


}


void AlgoClass_NaYin::onGetStampDetectImg()
{
	   if (image.empty()) {
		   qWarning() << "onGetStampDetectImg: member variable 'image' is empty. Please set input image first.";
		   return;
	   }

	   ExportSpace::GetStampDetectImg(image, al_core);

	   emit TransMat(image);
}

void AlgoClass_NaYin::onGetStampCropimage()
{
	if (image.empty()) {
	    qWarning() << "onGetStampCropimage: member variable 'image' is empty. Please set input image first.";
	    return;
	}

	ExportSpace::GetStampCropimage(image, al_core); 

	emit TransMat(image);
}

void AlgoClass_NaYin::onGetStampCropSegmentImg()
{
	   if (image.empty()) {
		   qWarning() << "onGetStampCropSegmentImg: member variable 'image' is empty. Please set input image first.";
		   return;
	   }

	   ExportSpace::GetStampCropSegmentImg(image, al_core);

	   emit TransMat(image);
}

void AlgoClass_NaYin::onGetStampSegmentImg()
{
	if (image.empty()) {
	    qWarning() << "onGetStampSegmentImg: member variable 'image' is empty. Please set input image first.";
	    return;
	}

	ExportSpace::GetStampSegmentImg(image, al_core); 

	emit TransMat(image);
}

void AlgoClass_NaYin::onRunStamp()
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

	if(al_core==1)ExportSpace::RunStamp(temp, 1, al_core, LearnPara::inParam1);
	else ExportSpace::RunStamp(temp, 1, al_core, LearnPara::inParam2);
	image = temp;
}

void AlgoClass_NaYin::saveParamAsync()
{
	// 创建并填充 QVariantMap
	QVariantMap mapToSave;

	if (al_core == 1) {
		mapToSave["textkernl"] = QVariantMap{ {"值", LearnPara::inParam1.textkernl} };
		mapToSave["logoid"] = QVariantMap{ {"值", LearnPara::inParam1.logoid} };
		mapToSave["textextralenabel"] = QVariantMap{ {"值", LearnPara::inParam1.textextralenabel} };
		mapToSave["textMissPixEnable"] = QVariantMap{ {"值", LearnPara::inParam1.textMissPixEnable} };
		mapToSave["isRect"] = QVariantMap{ {"值", LearnPara::inParam1.isRect} };
		mapToSave["isOCR"] = QVariantMap{ {"值", LearnPara::inParam1.isOCR} };
	}
	else if (al_core == 0) {
		mapToSave["textkernl"] = QVariantMap{ {"值", LearnPara::inParam2.textkernl} };
		mapToSave["logoid"] = QVariantMap{ {"值", LearnPara::inParam2.logoid} };
		mapToSave["textextralenabel"] = QVariantMap{ {"值", LearnPara::inParam2.textextralenabel} };
		mapToSave["textMissPixEnable"] = QVariantMap{ {"值", LearnPara::inParam2.textMissPixEnable} };
		mapToSave["isRect"] = QVariantMap{ {"值", LearnPara::inParam2.isRect} };
		mapToSave["isOCR"] = QVariantMap{ {"值", LearnPara::inParam2.isOCR} };
	}

	// 捕获文件路径和数据
	QString filePath = m_cameralPath;
	QVariantMap dataToSave = mapToSave;

	// 运行异步任务
	QtConcurrent::run([filePath, dataToSave]() {
		FileOperator::writeJsonMap(filePath, dataToSave);
		});
}

