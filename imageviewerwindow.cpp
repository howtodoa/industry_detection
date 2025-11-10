#include "imageviewerwindow.h"
#include <QResizeEvent>
#include <QWindowStateChangeEvent>
#include <QTimer>
#include <QDebug>
#include <QScreen> 
#include <QGuiApplication> // 用于获取 QScreen 实例
#include <QRect>           // 用于处理屏幕几何尺寸

// 默认构造函数：调用带 QPixmap 参数的构造函数，传入一个空图片
ImageViewerWindow::ImageViewerWindow(QWidget* parent)
    : ImageViewerWindow(QPixmap(), parent)
{
}

// 带 QPixmap 参数的构造函数：实现方法一的屏幕尺寸限制逻辑
ImageViewerWindow::ImageViewerWindow(const QPixmap& pixmap, QWidget* parent)
    : QWidget(parent), originalPixmap(pixmap)
{
    this->setWindowTitle("图像查看");

    // --- 【关键修改点：处理图片尺寸大于屏幕可用区域的情况】 ---
    if (!originalPixmap.isNull()) {
        QScreen* screen = QGuiApplication::primaryScreen();
        // 获取当前屏幕的可用工作区大小 (排除了任务栏、Dock栏等)
        QRect availableGeometry = screen->availableGeometry();

        QSize originalSize = originalPixmap.size();
        QSize targetSize;

        // 检查原始图片是否大于屏幕可用区域
        if (originalSize.width() > availableGeometry.width() ||
            originalSize.height() > availableGeometry.height())
        {
            // 如果大于，按比例缩放至屏幕可用尺寸
            // 窗口的最大尺寸设定为屏幕可用尺寸的 95%，留出一点边距更美观
            QSize maxSize(availableGeometry.width() * 0.95, availableGeometry.height() * 0.95);

            targetSize = originalSize.scaled(
                maxSize,
                Qt::KeepAspectRatio // 保持图片的原始宽高比
            );

        }
        else {
            // 如果图片尺寸小于屏幕，则使用原尺寸
            targetSize = originalSize;
        }

        // 设置窗口尺寸为计算出的目标尺寸
        this->resize(targetSize);

    }
    else {
        // 如果传入的图片为空，则设置一个默认尺寸
        this->resize(480, 480);
    }
    // --- 【关键修改点结束】 ---

    imageLabel = new QLabel(this);
    imageLabel->setAlignment(Qt::AlignCenter);

    QVBoxLayout* layout = new QVBoxLayout(this);
    // 设置边距为 0，使图片填充整个窗口
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(imageLabel);
    this->setLayout(layout);

    updateImageDisplay();
}

ImageViewerWindow::~ImageViewerWindow()
{
}

// 窗口大小改变事件：更新图片显示
void ImageViewerWindow::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    updateImageDisplay();
}

// 更新图片显示：将图片等比例缩放到 imageLabel 的尺寸
void ImageViewerWindow::updateImageDisplay()
{
    if (!originalPixmap.isNull()) {
        QPixmap scaled = originalPixmap.scaled(
            imageLabel->size(),
            Qt::KeepAspectRatio, // 保持宽高比
            Qt::SmoothTransformation
        );
        imageLabel->setPixmap(scaled);
    }
    else {
        imageLabel->clear(); // 没有图片时清空标签
    }
}

// 设置新图片
void ImageViewerWindow::setPixmap(const QPixmap& pixmap)
{
    originalPixmap = pixmap; // 更新原始 QPixmap
    updateImageDisplay();    // 更新显示
}

// 关闭事件：发出信号
void ImageViewerWindow::closeEvent(QCloseEvent* event)
{
    emit windowClosed();
    // 调用基类的 closeEvent，确保窗口正常关闭
    QWidget::closeEvent(event);
}

// 状态改变事件：处理最大化/还原的特殊逻辑
void ImageViewerWindow::changeEvent(QEvent* event)
{
    QWidget::changeEvent(event);

    if (event->type() == QEvent::WindowStateChange)
    {
        auto* stateChangeEvent = static_cast<QWindowStateChangeEvent*>(event);
        bool wasMaximized = (stateChangeEvent->oldState() & Qt::WindowMaximized);
        bool isNormal = (this->windowState() == Qt::WindowNoState);

        // 如果是从“最大化”状态恢复到“普通”状态
        if (wasMaximized && isNormal)
        {
            if (!originalPixmap.isNull())
            {
                // 强制恢复原图窗口大小
                // 注意：这里恢复的是原图尺寸，如果原图尺寸超出屏幕，可能会再次被系统限制
                this->setFixedSize(originalPixmap.size());

                // 解除强制，防止第三次点击无法放大
                this->setMinimumSize(0, 0);
                this->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
            }
        }
    }
}

// 使用 QSharedPointer 设置新图片
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