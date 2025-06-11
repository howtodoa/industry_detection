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

protected:
    void resizeEvent(QResizeEvent *event) override;
        void updateImageDisplay();

private:
    QLabel *imageLabel;
    QPixmap originalPixmap;
};

#endif // IMAGEVIEWERWINDOW_H
