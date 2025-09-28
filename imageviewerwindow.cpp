#include "imageviewerwindow.h"
#include <QResizeEvent>
#include <QWindowStateChangeEvent>
#include <QTimer>
#include <QDebug>
#include <QScreen> 

ImageViewerWindow::ImageViewerWindow(QWidget *parent)
    : ImageViewerWindow(QPixmap(), parent) // 调用带 QPixmap 参数的构造函数，传入一个空图片
{
}

ImageViewerWindow::ImageViewerWindow(const QPixmap& pixmap, QWidget* parent)
    : QWidget(parent), originalPixmap(pixmap)
{
    this->setWindowTitle("图像查看");

    // 直接根据传入的图片尺寸来设置窗口大小
    if (!originalPixmap.isNull()) {
        this->resize(originalPixmap.size());
    }
    else {
        // 如果传入的图片为空，则设置一个默认尺寸
        this->resize(480, 480);
    }

    imageLabel = new QLabel(this);
    imageLabel->setAlignment(Qt::AlignCenter);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(imageLabel);
    this->setLayout(layout);

    updateImageDisplay();
}
ImageViewerWindow::~ImageViewerWindow()
{

}

void ImageViewerWindow::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    updateImageDisplay();
}

void ImageViewerWindow::updateImageDisplay()
{
    if (!originalPixmap.isNull()) {
        
        QPixmap scaled = originalPixmap.scaled(
            imageLabel->size(),
            Qt::KeepAspectRatio, // 
            Qt::SmoothTransformation
            );
        imageLabel->setPixmap(scaled);
       
    } else {
        imageLabel->clear(); 
    }
}

void ImageViewerWindow::setPixmap(const QPixmap& pixmap)
{
    originalPixmap = pixmap; // 更新原始 QPixmap
    updateImageDisplay();    // 更新显示
}

void ImageViewerWindow::closeEvent(QCloseEvent* event)
{
    emit windowClosed();
    // 调用基类的 closeEvent，确保窗口正常关闭
    QWidget::closeEvent(event);
}

void ImageViewerWindow::changeEvent(QEvent* event)
{
    QWidget::changeEvent(event);

    if (event->type() == QEvent::WindowStateChange)
    {
        auto* stateChangeEvent = static_cast<QWindowStateChangeEvent*>(event);
        bool wasMaximized = (stateChangeEvent->oldState() & Qt::WindowMaximized);
        bool isNormal = (this->windowState() == Qt::WindowNoState);

        if (wasMaximized && isNormal)
        {
            if (!originalPixmap.isNull())
            {
                //强制恢复原图窗口大小
                this->setFixedSize(originalPixmap.size());

              //解除强制，防止第三次点击无法放大
                this->setMinimumSize(0, 0);
                this->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
            }
        }
    }
}

void ImageViewerWindow::setPixmapPtr(QSharedPointer<QPixmap> pixmapPtr)
{
    if (pixmapPtr.isNull()) {
        qWarning() << "ImageViewerWindow::setPixmap(QSharedPointer): Received null pixmap pointer.";
        originalPixmap = QPixmap(); // 清空当前显示的图像
    }
    else {
        originalPixmap = *pixmapPtr;
      
    }
    updateImageDisplay(); // 更新显示
}