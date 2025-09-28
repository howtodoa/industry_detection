#include "algoclass.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>


// 默认构造函数
AlgoClass::AlgoClass(QObject *parent)
    : QObject(parent)
{
    qDebug() << "AlgoClass: 默认构造函数被调用。";
}


// 带参数文件路径的构造函数
AlgoClass::AlgoClass(QString algopath, QObject *parent)
    : QObject(parent), m_cameralPath(algopath) // 初始化 m_cameralPath
{
 
}

QWidget* AlgoClass::createLeftPanel(QWidget* parent)
{
    QWidget* panel = new QWidget(parent);
    QVBoxLayout* layout = new QVBoxLayout(panel);
    layout->addWidget(new QPushButton("启动算法 ", panel));
    layout->addStretch();
    return panel;
}

QPixmap AlgoClass::convertMatToPixmap(const cv::Mat& mat)
{
	if (mat.empty()) {
		qWarning("convertMatToPixmap: 输入图像为空");
		return QPixmap();
	}

	QImage image;

	switch (mat.type()) {
	case CV_8UC1:
		image = QImage(mat.data, mat.cols, mat.rows, static_cast<int>(mat.step), QImage::Format_Grayscale8);
		break;
	case CV_8UC3:
		image = QImage(mat.data, mat.cols, mat.rows, static_cast<int>(mat.step), QImage::Format_BGR888);
		break;
	case CV_8UC4:
		image = QImage(mat.data, mat.cols, mat.rows, static_cast<int>(mat.step), QImage::Format_ARGB32);
		break;
	default:
		qWarning("convertMatToPixmap: 不支持的图像类型: %d", mat.type());
		return QPixmap();
	}

	if (image.isNull()) {
		qWarning("convertMatToPixmap: QImage 构造失败");
		return QPixmap();
	}

	// 深拷贝为 QPixmap 返回
	return QPixmap::fromImage(image.copy());
}

void AlgoClass::saveParamAsync()
{
  
}

