#include "zoomablelabel.h"
#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>
#include "public.h"


ZoomableLabel::ZoomableLabel(QWidget *parent)
    : QLabel(parent), scaleFactor(1.0), offset(0, 0), dragging(false)
{
    setAlignment(Qt::AlignCenter);  // 图像居中显示
    setScaledContents(false);  // 禁用自动缩放
}

void ZoomableLabel::setPixmap(const QPixmap& pixmap)
{
#ifdef USE_MAIN_WINDOW_CAPACITY
    if (CheckRAM() == -1) return;
#endif
    if (CheckPixmap(pixmap) == -1) return;
    // 浅拷贝
    originalPixmap = pixmap;
    scaleFactor = 1.0;
    updateScaledPixmap();
}
void ZoomableLabel::setOriginalPixmap(const QPixmap &pixmap)
{
    originalPixmap = pixmap;
    scaleFactor = 1.0;
    updateScaledPixmap();
    qDebug() << "ZoomableLabel size:" << size();  // 打印控件当前大小
    qDebug() << "Original Image size:" << originalPixmap.size();
}

void ZoomableLabel::updateScaledPixmap()
{
    if (!originalPixmap.isNull()) {
        QSize labelSize = size();  // 获取控件的大小
        QSize imageSize = originalPixmap.size();  // 获取图片的原始大小

        qDebug() << "ZoomableLabel size:" << labelSize;  // 输出控件大小
        qDebug() << "Original Image size:" << imageSize;  // 输出图片的原始大小

        // 计算适合的缩放因子，确保图片完整显示在控件中
        float widthScaleFactor = static_cast<float>(labelSize.width()) / imageSize.width();
        float heightScaleFactor = static_cast<float>(labelSize.height()) / imageSize.height();

        // 选择较小的缩放因子，以保持图片比例，避免图片被拉伸
        scaleFactor = qMin(widthScaleFactor, heightScaleFactor);

        // 更新图像显示
        update();  // 重新绘制图像
    }
}

void ZoomableLabel::resetZoom()
{
    scaleFactor = 1.0;  // 重置缩放因子
    offset = QPoint(0, 0);  // 重置平移偏移

    // 确保图片大小被重新计算并更新显示
    updateScaledPixmap();  // 强制更新图像
    update();  // 强制重绘控件
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

    QLabel::resizeEvent(event);  // 调用基类的 resizeEvent

    if (!originalPixmap.isNull()) {
        QSize labelSize = size();  // 获取控件的大小
        QSize imageSize = originalPixmap.size();  // 获取图片的原始大小

        // 计算缩放比例
        float widthScaleFactor = static_cast<float>(labelSize.width()) / imageSize.width();
        float heightScaleFactor = static_cast<float>(labelSize.height()) / imageSize.height();

        // 选择较小的缩放因子以保证图片不被拉伸
        scaleFactor = qMin(widthScaleFactor, heightScaleFactor);

        // 更新图像显示
        update();  // 重新绘制图像
    }
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
    if (originalPixmap.isNull())
        return;  // 如果没有有效的图像，则不绘制

    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);  // 启用平滑转换

    // 根据 scaleFactor 计算缩放后的图像尺寸
    QSize scaledSize = originalPixmap.size() * scaleFactor;
    QPixmap scaled = originalPixmap.scaled(scaledSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // 计算图像在标签中的绘制位置，确保居中显示
    QPoint drawPoint = rect().center() - QPoint(scaled.width() / 2, scaled.height() / 2) + offset;
    painter.drawPixmap(drawPoint, scaled);  // 绘制图像
}

/*
 * 强行拉伸填充的版本
void ZoomableLabel::updateScaledPixmap()
{
    if (!originalPixmap.isNull()) {
        QSize labelSize = size();  // 获取控件的大小
        QSize imageSize = originalPixmap.size();  // 获取图片的原始大小

        qDebug() << "ZoomableLabel size:" << labelSize;
        qDebug() << "Original Image size:" << imageSize;

        // !!! 直接无脑拉伸，不保持比例
        QPixmap scaled = originalPixmap.scaled(labelSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

        QLabel::setPixmap(scaled);

        qDebug() << "Scaled Image size:" << scaled.size();
    }
}

void ZoomableLabel::paintEvent(QPaintEvent *event)
{
    if (originalPixmap.isNull())
        return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    QSize scaledSize = originalPixmap.size() * scaleFactor;
    QPixmap scaled = originalPixmap.scaled(scaledSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    QPoint drawPoint = rect().center() - QPoint(scaled.width() / 2, scaled.height() / 2) + offset;
    painter.drawPixmap(drawPoint, scaled);
}
*/
