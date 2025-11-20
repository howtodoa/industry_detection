#ifndef FULLSCREENWINDOW_H
#define FULLSCREENWINDOW_H

#include <QWidget>
#include <QPixmap>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include "zoomablelabel.h"  // 引入 ZoomableLabel
#include <QKeyEvent>        // 需要引入 QKeyEvent
#include <QResizeEvent>     // 需要引入 QResizeEvent

class FullScreenWindow : public QWidget
{
    Q_OBJECT

public:
    explicit FullScreenWindow(const QPixmap &pixmap = QPixmap(), QWidget *parent = nullptr);
    ~FullScreenWindow();
    ZoomableLabel *imageLabel;
    static void ShowOriginalSize(const QPixmap& pixmap, bool deleteOnClose = true);
    static FullScreenWindow* s_lastInstance;
    void keyPressEvent(QKeyEvent *event) override;  // 用于关闭窗口
    void resizeEvent(QResizeEvent *event) override; // 窗口大小调整时
    void closeEvent(QCloseEvent* event) override;
    QPixmap m_pixmap;              // 存储图像的 QPixmap
       // 用 ZoomableLabel 显示图像
    QPushButton *closeButton;      // 关闭按钮
};

#endif // FULLSCREENWINDOW_H
