#include "ImageDisplay.h"
#include <QVBoxLayout>
#include <QFile>
#include <QDebug>

ImageDisplay::ImageDisplay(QWidget* targetWidget)
    : QObject(targetWidget)
{
    if (!targetWidget) {
        qDebug() << "[ImageDisplay] 无效的父控件";
        return;
    }

    // 输出父控件的尺寸
    qDebug() << "[ImageDisplay] 父控件尺寸: " << targetWidget->size();

    imageLabel = new QLabel(targetWidget);
    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->setScaledContents(true);  // 自动拉伸填满

    // 设置 QSizePolicy 来确保 label 填满父控件
    imageLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // 输出 imageLabel 尺寸
    qDebug() << "[ImageDisplay] 创建的 imageLabel 尺寸 (初始化): " << imageLabel->size();

    // 如果没有布局，创建一个
    QLayout* existingLayout = targetWidget->layout();
    if (!existingLayout) {
        QVBoxLayout* layout = new QVBoxLayout(targetWidget);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(imageLabel);
        targetWidget->setLayout(layout);
    } else {
        existingLayout->addWidget(imageLabel);
    }

    // 输出布局后的 imageLabel 尺寸
    qDebug() << "[ImageDisplay] imageLabel 尺寸 (布局后): " << imageLabel->size();

    // 强制设置父控件的大小 (可选)
    targetWidget->setMinimumSize(targetWidget->sizeHint());  // 根据布局和控件调整大小

    qDebug() << "[ImageDisplay] 初始化完成，imageLabel 添加成功";
}


void ImageDisplay::setImage(const QString& imagePath)
{
    QPixmap pixmap(imagePath);
    if (pixmap.isNull()) {
        qDebug() << "[ImageDisplay] 无法加载图片: " << imagePath;
        return;
    }
    imageLabel->setPixmap(pixmap);
    qDebug() << "[ImageDisplay] 图片加载成功: " << imagePath;
}
