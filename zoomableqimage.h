#ifndef ZOOMABLEQIMAGE_H
#define ZOOMABLEQIMAGE_H

#include <QWidget>
#include <QImage>
#include <QPointF>

class ZoomableQImage : public QWidget
{
    Q_OBJECT

public:
    explicit ZoomableQImage(QWidget* parent = nullptr);
    ~ZoomableQImage() override = default;

    // 设置原始图像（线程安全：QImage 自动引用计数）
    void setImage(const QImage& image);
    void setPixmap(const QPixmap&) {}  // 空实现，不会做任何操作
    // 重置缩放和偏移
    void resetView();

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

    void wheelEvent(QWheelEvent* event) override;

    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;

private:
    QImage m_image;         // 原始图像（QImage 会自动 COW）
    float m_scaleFactor;    // 缩放比例
    QPointF m_offset;       // 平移偏移
    bool m_dragging;        // 是否正在拖拽
    QPointF m_lastMousePos; // 拖拽时上次鼠标位置

    void updateToFit();     // 第一次显示时自动适配控件大小
};

#endif // ZOOMABLEQIMAGE_H
