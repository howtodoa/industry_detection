#include "ZoomableQImage.h"
#include <QPainter>
#include <QWheelEvent>
#include <QMouseEvent>
#include "typdef.h"
#include <QDebug>

ZoomableQImage::ZoomableQImage(QWidget* parent)
    : QWidget(parent),
    m_scaleFactor(1.0f),
    m_offset(0, 0),
    m_dragging(false)
{
    setMouseTracking(true);
    setAttribute(Qt::WA_OpaquePaintEvent); // 提升绘制性能
    setAttribute(Qt::WA_NoSystemBackground);
}

void ZoomableQImage::setImage(const QImage& image)
{
    if (image.isNull()) {
        qWarning() << "setImage: 输入图像为 null。";
        return;
    }

    // 增加尺寸检查
    if (image.width() <= 0 || image.height() <= 0) {
        qWarning() << "setImage: 输入图像尺寸无效 ("
            << image.width() << "x" << image.height() << ")，已拒绝。";
        return;
    }
    try
    {
        m_image = image;
        updateToFit(); // 此时 updateToFit 已经通过了所有安全检查
        update();
    }
    catch (...)
    {
		LOG_DEBUG(GlobalLog::logger, L"ZoomableQImage::setImage: 设置图像时发生异常，catch成功");
        return;
    }
}

void ZoomableQImage::resetView()
{
    updateToFit();
    m_offset = QPointF(0, 0);
    update();
}

void ZoomableQImage::updateToFit()
{
    if (m_image.isNull() || width() == 0 || height() == 0)
        return;

    float wFactor = (float)width() / m_image.width();
    float hFactor = (float)height() / m_image.height();

    m_scaleFactor = qMin(wFactor, hFactor);
}

void ZoomableQImage::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter p(this);
    p.setRenderHint(QPainter::SmoothPixmapTransform, true);

    // 清理背景
    p.fillRect(rect(), Qt::black); // 或者 Qt::white，或自定义颜色

    if (m_image.isNull()) return;

    QSizeF scaledSize = m_image.size() * m_scaleFactor;
    QPointF centerPoint((width() - scaledSize.width()) / 2.0f,
        (height() - scaledSize.height()) / 2.0f);
    QPointF drawPos = centerPoint + m_offset;

    p.drawImage(QRectF(drawPos, scaledSize), m_image);
}
void ZoomableQImage::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    updateToFit();
    update();
}

void ZoomableQImage::wheelEvent(QWheelEvent* event)
{
    if (m_image.isNull()) return;

    float oldScale = m_scaleFactor;

    // 滚轮缩放
    if (event->angleDelta().y() > 0)
        m_scaleFactor *= 1.1f;
    else
        m_scaleFactor /= 1.1f;

    // 限制缩放范围（避免无限缩小导致漂移）
    m_scaleFactor = qBound(0.05f, m_scaleFactor, 50.0f);

    // 缩放中心 = 鼠标位置
    QPointF mousePos = event->position();

    // 偏移计算：保持鼠标放大点不动
    m_offset = (m_offset + mousePos) * (m_scaleFactor / oldScale) - mousePos;

    update();
}

void ZoomableQImage::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragging = true;
        m_lastMousePos = event->pos();
    }
}

void ZoomableQImage::mouseMoveEvent(QMouseEvent* event)
{
    if (m_dragging) {
        QPointF delta = event->pos() - m_lastMousePos;
        m_offset += delta;
        m_lastMousePos = event->pos();
        update();
    }
}

void ZoomableQImage::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
        m_dragging = false;
}

void ZoomableQImage::mouseDoubleClickEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
    resetView();
}
