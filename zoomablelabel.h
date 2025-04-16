#ifndef ZOOMABLELABEL_H
#define ZOOMABLELABEL_H

#include <QLabel>
#include <QPixmap>
#include <QPoint>

class ZoomableLabel : public QLabel
{
    Q_OBJECT
public:
    explicit ZoomableLabel(QWidget *parent = nullptr);

    void setPixmap(const QPixmap &pixmap);
    void resetZoom();
    void updateScaledPixmap();

public slots:


protected:
    void paintEvent(QPaintEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;


private:
    QPixmap originalPixmap;
    float scaleFactor;
    QPoint offset;
    QPoint lastMousePos;
    bool dragging;
};

#endif // ZOOMABLELABEL_H
