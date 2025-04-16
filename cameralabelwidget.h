#ifndef CAMERALABELWIDGET_H
#define CAMERALABELWIDGET_H
#include "MZ_ClientControl.h"
#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "cameralmenu.h"
#include "Cameral.h"
#include "parawidget.h"

class CameraLabelWidget : public QWidget
{
    Q_OBJECT
public:
    // 构造函数：index 用于生成唯一的名称、显示文字，parent 为父控件
    explicit CameraLabelWidget(int index, const QString &fixedTextName, QWidget *parent = nullptr);

    void displayimg(QPixmap &pixmap);
    void displayimg(HImage &himage);
    HImage convertQPixmapToHImage(const QPixmap &pixmap);
    QPixmap convertHImageToPixmap(const HImage& hImage);
    // 获取内部显示图像的 QLabel，便于外部更新图片
    QLabel* getImageLabel() const { return imageLabel; }

private:
    QLabel    *imageLabel; // 显示图像
    QLabel    *fixedText;  // 固定文本标签
    CameraMenu *cameraMenu; // 操作菜单
    Cameral   *cameral;    // 摄像头参数对象
};

#endif // CAMERALABELWIDGET_H
