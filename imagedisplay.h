#ifndef IMAGEDISPLAY_H
#define IMAGEDISPLAY_H

#include <QObject>
#include <QLabel>
#include <QPixmap>
#include <QString>

class ImageDisplay : public QObject
{
    Q_OBJECT

public:
    explicit ImageDisplay(QWidget* targetWidget);
    void setImage(const QString& imagePath);

private:
    QLabel* imageLabel;
};

#endif // IMAGEDISPLAY_H
