#include "fullscreenwindow.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QKeyEvent>
#include <QApplication>
#include <QScreen>


FullScreenWindow* FullScreenWindow::s_lastInstance = nullptr;

FullScreenWindow::FullScreenWindow(const QPixmap &pixmap, QWidget *parent)
    : QWidget(parent), m_pixmap(pixmap)
{

    // 创建用于显示图片的 QLabel，直接设置传入的图片
    imageLabel = new ZoomableLabel(this);
    imageLabel->setPixmap(m_pixmap);
    //imageLabel->setAlignment(Qt::AlignCenter);

    // 创建关闭按钮，固定尺寸，样式简单
    //closeButton = new QPushButton("X", this);
    //closeButton->setFixedSize(40, 40);
    //closeButton->setStyleSheet("background-color: red; color: white; font-size: 20px; border-radius: 20px;");
    //connect(closeButton, &QPushButton::clicked, this, &FullScreenWindow::close);


}

void FullScreenWindow::closeEvent(QCloseEvent* event)
{
    // 关键步骤：检查当前关闭的窗口是否是我们正在追踪的那个
    if (s_lastInstance == this) {
        s_lastInstance = nullptr;
        qDebug() << "单实例窗口指针已成功清除（复位为 nullptr）。";
    }

    // 调用基类的 closeEvent，确保窗口关闭流程继续
    QWidget::closeEvent(event);
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

void FullScreenWindow::ShowOriginalSize(const QPixmap& pixmap, bool isCloseRequest /*= false*/)
{

    if (isCloseRequest) {
        if (s_lastInstance) {
            s_lastInstance->close();
            qDebug() << "收到关闭请求，唯一窗口已关闭。";
        }
        else {
            qDebug() << "收到关闭请求，但没有打开的窗口。";
        }
        return;
    }


    // 判断指针是否为空：如果指针不为空，直接退出，不创建新窗口，也不更新现有窗口。
    if (s_lastInstance)
    {
        qDebug() << "窗口已存在，不创建/不更新内容，直接退出。";
        return;
    }

    // --- 窗口不存在，创建新实例 ---

    if (pixmap.isNull()) {
        qWarning() << "收到空图像，无法创建。";
        return;
    }

    FullScreenWindow* window = new FullScreenWindow(pixmap);

    s_lastInstance = window;

    window->setAttribute(Qt::WA_DeleteOnClose, true);

    window->resize(pixmap.size());

    window->show();
    window->raise();

    qDebug() << "新的唯一 NG 窗口已创建并显示 (拒绝后续更新)。";
}

void FullScreenWindow::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    // 确保窗口的大小更新时，图像也被适配
   imageLabel->resize(size());  // 使 ZoomableLabel 自适应窗口大小
    imageLabel->updateScaledPixmap();  // 调用自定义的缩放更新函数
}
