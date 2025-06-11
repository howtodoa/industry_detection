#include "imageviewerwindow.h"
#include <QResizeEvent>
#include <QDebug>
#include <QScreen> // Include QScreen for availableGeometry

ImageViewerWindow::ImageViewerWindow(QWidget *parent)
    : ImageViewerWindow(QPixmap(), parent) // 调用带 QPixmap 参数的构造函数，传入一个空图片
{
}

ImageViewerWindow::ImageViewerWindow(const QPixmap &pixmap, QWidget *parent)
    : QWidget(parent), originalPixmap(pixmap)
{
    this->setWindowTitle("图像查看");

    // Get the available geometry of the screen to set a reasonable initial size
    // and ensure it doesn't open too large.
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect availableGeometry = screen->availableGeometry();

    // Calculate an initial size that's a percentage of the screen, or fit the image
    // if it's smaller than that percentage.
    int initialWidth = qMin(1000, availableGeometry.width() * 80 / 100);
    int initialHeight = qMin(700, availableGeometry.height() * 80 / 100);

    if (!originalPixmap.isNull()) {
        if (originalPixmap.width() < initialWidth && originalPixmap.height() < initialHeight) {
            this->resize(originalPixmap.size()); // If image is small, size to image
        } else {
            this->resize(initialWidth, initialHeight); // Otherwise, use calculated initial size
        }
    } else {
        this->resize(initialWidth, initialHeight);
    }


    imageLabel = new QLabel(this);
    imageLabel->setAlignment(Qt::AlignCenter);
    // imageLabel->setMinimumSize(0, 0); // This is good, but the scaling strategy is key

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(imageLabel);
    this->setLayout(layout);

    // Initial display
    updateImageDisplay();
}

ImageViewerWindow::~ImageViewerWindow()
{
    // No need to explicitly delete imageLabel or layout here
    // as they are children of this QWidget and will be deleted
    // automatically when ImageViewerWindow is destroyed.
}

void ImageViewerWindow::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    updateImageDisplay();
}

void ImageViewerWindow::updateImageDisplay()
{
    if (!originalPixmap.isNull()) {
        // Crucial change: Use Qt::KeepAspectRatio
        // This will scale the pixmap down (or up) to fit within imageLabel->size()
        // while preserving its aspect ratio.
        QPixmap scaled = originalPixmap.scaled(
            imageLabel->size(),
            Qt::KeepAspectRatio, // <-- Change this
            Qt::SmoothTransformation
            );
        imageLabel->setPixmap(scaled);
        // imageLabel->update(); // Not strictly necessary as setPixmap often triggers a repaint
    } else {
        imageLabel->clear(); // Clear the pixmap if originalPixmap is null
    }
}
