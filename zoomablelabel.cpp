#include "zoomablelabel.h"
#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QTimer>
#include <QDebug>

ZoomableLabel::ZoomableLabel(QWidget *parent)
    : QLabel(parent), scaleFactor(1.0), offset(0, 0), dragging(false)
{
    setAlignment(Qt::AlignCenter);
    setScaledContents(false);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void ZoomableLabel::setPixmap(const QPixmap& pixmap)
{
    if (pixmap.isNull()) return;

    originalPixmap = pixmap;
    scaleFactor = 1.0;

    // 延迟更新，保证布局完成
    QTimer::singleShot(0, this, [this]() {
        updateScaledPixmap();

        // ------------------------------------------------
        // 【核心修复代码】: 在图像加载并缩放完成后，强制请求父控件更新布局
        // updateGeometry() 通知父布局：我的尺寸要求可能变了，请重新计算！
        if (parentWidget()) {
            parentWidget()->updateGeometry();
        }
        // ------------------------------------------------
        });
}
void ZoomableLabel::setOriginalPixmap(const QPixmap &pixmap)
{
    originalPixmap = pixmap;
    scaleFactor = 1.0;
    QTimer::singleShot(0, this, [this]() { updateScaledPixmap(); });
}

void ZoomableLabel::updateScaledPixmap()
{
    if (originalPixmap.isNull()) return;

    QSize labelSize = rect().size(); // 使用当前控件尺寸
    QSize imageSize = originalPixmap.size();
    float wFactor = static_cast<float>(labelSize.width()) / imageSize.width();
    float hFactor = static_cast<float>(labelSize.height()) / imageSize.height();
    scaleFactor = qMin(wFactor, hFactor);

    update();
}

void ZoomableLabel::resetZoom()
{
    scaleFactor = 1.0;
    offset = QPoint(0,0);
    updateScaledPixmap();
}

void ZoomableLabel::paintEvent(QPaintEvent* event)
{
    QLabel::paintEvent(event);

    if (originalPixmap.isNull()) return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    QSize scaledSize = originalPixmap.size() * scaleFactor;
    QPixmap scaled = originalPixmap.scaled(scaledSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    QPoint center((width() - scaled.width()) / 2, (height() - scaled.height()) / 2);
    painter.drawPixmap(center + offset, scaled);
}

void ZoomableLabel::resizeEvent(QResizeEvent *event)
{
    QLabel::resizeEvent(event);
    updateScaledPixmap();
}

void ZoomableLabel::wheelEvent(QWheelEvent* event)
{
    if (originalPixmap.isNull()) return;

    QPoint numDegrees = event->angleDelta();
    if (numDegrees.isNull()) return;

    float oldScale = scaleFactor;
    if (numDegrees.y() > 0)
        scaleFactor *= 1.1f;
    else
        scaleFactor /= 1.1f;

    // 保持鼠标指针位置
    QPointF cursorPos = event->position();
    QPointF offsetF = QPointF(offset);
    offsetF = (offsetF + cursorPos) * (scaleFactor / oldScale) - cursorPos;
    offset = offsetF.toPoint();

    update();
}

void ZoomableLabel::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        dragging = true;
        lastMousePos = event->pos();
    }
}

void ZoomableLabel::mouseMoveEvent(QMouseEvent *event)
{
    if (dragging) {
        QPoint delta = event->pos() - lastMousePos;
        offset += delta;
        lastMousePos = event->pos();
        update();
    }
}

void ZoomableLabel::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        dragging = false;
}

void ZoomableLabel::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    resetZoom();
}

QSize ZoomableLabel::sizeHint() const
{
    return QSize(200, 150); // 默认推荐大小
}

void ZoomableLabel::showEvent(QShowEvent *event)
{
    QLabel::showEvent(event);
    updateScaledPixmap(); // 确保显示时尺寸正确
}
