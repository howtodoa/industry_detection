#include "zoomablelabel.h"
#include <QPainter>
#include <QMouseEvent>

ZoomableLabel::ZoomableLabel(QWidget *parent)
    : QLabel(parent), scaleFactor(1.0), offset(0, 0), dragging(false)
{
    setAlignment(Qt::AlignCenter);  // 图像居中显示
    setScaledContents(false);  // 禁用自动缩放
}

void ZoomableLabel::setPixmap(const QPixmap &pixmap)
{
    originalPixmap = pixmap;
    scaleFactor = 1.0;  // 重置缩放因子
    updateScaledPixmap();  // 更新显示
}

void ZoomableLabel::updateScaledPixmap()
{
    if (!originalPixmap.isNull()) {
        QSize scaledSize = size() * scaleFactor;  // 按当前缩放比例调整图像大小
        QPixmap scaled = originalPixmap.scaled(scaledSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        QLabel::setPixmap(scaled);  // 更新当前显示的图像
        setAlignment(Qt::AlignCenter);  // 图像居中显示
    }
}

void ZoomableLabel::resetZoom()
{
    scaleFactor = 1.0;  // 重置缩放因子
    offset = QPoint(0, 0);  // 重置平移偏移
    update();  // 重绘图像
}

void ZoomableLabel::wheelEvent(QWheelEvent *event)
{
    QPoint numDegrees = event->angleDelta();  // 获取滚轮变化值
    if (!numDegrees.isNull()) {
        float oldScale = scaleFactor;
        if (numDegrees.y() > 0)
            scaleFactor *= 1.1f;  // 放大
        else
            scaleFactor /= 1.1f;  // 缩小

        // 保持鼠标指针下位置不变（提供更好的缩放体验）
        QPointF cursorPos = event->position();
        QPointF offsetF = QPointF(offset);  // 将 offset 转换为 QPointF 类型
        QPointF cursorPosF = cursorPos;     // 保持 cursorPos 为 QPointF 类型

        // 计算新的偏移量，保持缩放时鼠标指针不移动
        offsetF = (offsetF + cursorPosF) * (scaleFactor / oldScale) - cursorPosF;

        // 将结果转换回 QPoint 类型（如果 offset 是 QPoint 类型）
        offset = offsetF.toPoint();
        update();  // 重绘图像
    }
}

void ZoomableLabel::resizeEvent(QResizeEvent *event)
{
    QLabel::resizeEvent(event);
    update();  // 窗口大小改变时更新图像
}

void ZoomableLabel::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    resetZoom();  // 双击时重置缩放和平移
}

void ZoomableLabel::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        dragging = true;
        lastMousePos = event->pos();  // 记录鼠标按下的位置
    }
}

void ZoomableLabel::mouseMoveEvent(QMouseEvent *event)
{
    if (dragging) {
        QPoint delta = event->pos() - lastMousePos;  // 计算鼠标移动的偏移量
        offset += delta;  // 更新偏移量
        lastMousePos = event->pos();  // 更新鼠标当前位置
        update();  // 重绘图像
    }
}

void ZoomableLabel::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        dragging = false;  // 停止拖动
    }
}

void ZoomableLabel::paintEvent(QPaintEvent *event)
{
    // 不调用基类的 paintEvent，防止默认绘制,出现两张图
    // QLabel::paintEvent(event);

    if (originalPixmap.isNull())
        return;  // 如果没有有效的图像，则不绘制

    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);  // 启用平滑转换

    // 计算缩放后的图像尺寸
    QSize scaledSize = originalPixmap.size() * scaleFactor;
    QPixmap scaled = originalPixmap.scaled(scaledSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // 计算图像在标签中的绘制位置
    QPoint drawPoint = rect().center() - QPoint(scaled.width() / 2, scaled.height() / 2) + offset;
    painter.drawPixmap(drawPoint, scaled);  // 绘制图像
}

