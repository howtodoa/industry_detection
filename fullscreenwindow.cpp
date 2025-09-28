#include "fullscreenwindow.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QKeyEvent>
#include <QApplication>
#include <QScreen>

FullScreenWindow::FullScreenWindow(const QPixmap &pixmap, QWidget *parent)
    : QWidget(parent), m_pixmap(pixmap)
{

    // 创建用于显示图片的 QLabel，直接设置传入的图片
    imageLabel = new ZoomableLabel(this);
    imageLabel->setPixmap(m_pixmap);
    //imageLabel->setAlignment(Qt::AlignCenter);

    // 创建关闭按钮，固定尺寸，样式简单
    closeButton = new QPushButton("X", this);
    closeButton->setFixedSize(40, 40);
    closeButton->setStyleSheet("background-color: red; color: white; font-size: 20px; border-radius: 20px;");
    connect(closeButton, &QPushButton::clicked, this, &FullScreenWindow::close);


}



FullScreenWindow::~FullScreenWindow()
{
    // 析构函数，释放资源
}

void FullScreenWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        close();  // 按下 ESC 键时关闭窗口
    }
}

void FullScreenWindow::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    // 确保窗口的大小更新时，图像也被适配
   imageLabel->resize(size());  // 使 ZoomableLabel 自适应窗口大小
    imageLabel->updateScaledPixmap();  // 调用自定义的缩放更新函数
}
