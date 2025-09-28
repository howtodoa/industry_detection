#ifndef IMAGEVIEWERWINDOW_H
#define IMAGEVIEWERWINDOW_H

#include <QWidget>
#include <QLabel>
#include <QPixmap>
#include <QVBoxLayout>

class ImageViewerWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ImageViewerWindow(QWidget *parent = nullptr);
    explicit ImageViewerWindow(const QPixmap &pixmap, QWidget *parent = nullptr);

    ~ImageViewerWindow();

public slots:
    void setPixmap(const QPixmap& pixmap);
    void setPixmapPtr(QSharedPointer<QPixmap> pixmapPtr);
    void changeEvent(QEvent* event) override;

protected:
    void resizeEvent(QResizeEvent *event) override;
    void closeEvent(QCloseEvent* event) override;
    void updateImageDisplay();

signals:
    void windowClosed();
private:
    QLabel *imageLabel;
    QPixmap originalPixmap;
};

#endif // IMAGEVIEWERWINDOW_H
