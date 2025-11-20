#ifndef ZOOMABLELABEL_H
#define ZOOMABLELABEL_H

#include <QLabel>
#include <QPoint>

class ZoomableLabel : public QLabel
{
    Q_OBJECT
public:
    explicit ZoomableLabel(QWidget* parent = nullptr);

    void setPixmap(const QPixmap& pixmap);          // 设置图片
    void setOriginalPixmap(const QPixmap& pixmap);  // 设置原始图片

    void resetZoom();  // 重置缩放和平移

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void showEvent(QShowEvent* event) override;

    QSize sizeHint() const override;

private:
    void updateScaledPixmap();  // 更新缩放后的图片

private:
    QPixmap originalPixmap;
    float scaleFactor;      // 当前缩放因子
    QPoint offset;          // 平移偏移
    bool dragging;
    QPoint lastMousePos;
};

#endif // ZOOMABLELABEL_H
