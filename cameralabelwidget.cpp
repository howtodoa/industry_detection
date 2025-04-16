#include "CameraLabelWidget.h"
#include <QDebug>
#include "ZoomableLabel.h"  // 包含自定义的 ZoomableLabel 头文件

CameraLabelWidget::CameraLabelWidget(int index, const QString &fixedTextName, QWidget *parent)
    : QWidget(parent)
{
    // 设置主容器的样式
    setStyleSheet("background-color: lightgray; border: 1px solid gray;");

    // 创建显示图像的标签，使用 ZoomableLabel 替换原有的 QLabel
    imageLabel = new ZoomableLabel(this);
    imageLabel->setObjectName("cameraImageLabel");
    imageLabel->setStyleSheet("background-color: lightgray;");
    imageLabel->setAlignment(Qt::AlignCenter);
    // 不再调用 setScaledContents，缩放逻辑由 ZoomableLabel 内部处理

    // 创建固定文本标签
    fixedText = new QLabel(fixedTextName, this);
    fixedText->setStyleSheet("background-color: transparent; color: black; font-size: 20px;");
    fixedText->setAttribute(Qt::WA_TransparentForMouseEvents);
    fixedText->setAlignment(Qt::AlignCenter);

    // 创建 CameraMenu 并设置菜单选项
    cameraMenu = new CameraMenu(this);

    // 初始化 Cameral 对象，并设置参数
    cameral = new Cameral(this);
    cameral->rangeParams = {10.0f, 5.0f, 12.0f, 20.0f, 5.0f, 3.0f, 15.0f, 8.0f, 18.0f, 25.0f, 10.0f, 8.0f};
    cameral->cameralParams = { "Camera" + QString::number(index + 1),
                              "SN" + QString::number(index + 1),
                              "192.168.1." + QString::number(index + 1),
                              1, index + 1, 50, 100, 200 };
    cameral->algoParams = { "Algorithm" + QString::number(index + 1),
                           {"参数一", "参数二"},
                           {"100", "200"} };

    // 添加菜单选项（这里只给出一个示例，其他选项类似）
    cameraMenu->addMenuOption("运行", [index]() {
        qDebug() << "摄像头" << index + 1 << "的选项 运行 被点击";
    });
    cameraMenu->addMenuOption("停止", [index]() {
        qDebug() << "摄像头" << index + 1 << "的选项 停止 被点击";
    });
    cameraMenu->addMenuOption("OK", [index]() {
        qDebug() << "摄像头" << index + 1 << "的选项 OK 被点击";
    });
    cameraMenu->addMenuOption("NG", [index]() {
        qDebug() << "摄像头" << index + 1 << "的选项 NG 被点击";
    });
    cameraMenu->addMenuOption("错误", [index]() {
        qDebug() << "摄像头" << index + 1 << "的选项 错误 被点击";
    });
    cameraMenu->addMenuOption("参数", [this]() {
        // 将 Cameral 的参数传入 ParaWidget 并显示
        ParaWidget* parawidget = new ParaWidget(cameral->rangeParams,
                                                cameral->cameralParams,
                                                cameral->algoParams);
        parawidget->show();
    });

    cameraMenu->addMenuOption("相机", [index]() {
        qDebug() << "摄像头" << index + 1 << "的选项 相机 被点击";
    });

    cameraMenu->addMenuOption("全屏", [this]() {
        if (!currentPixmap.isNull()) {
            FullScreenWindow *fullscreenWindow = new FullScreenWindow(currentPixmap);
            fullscreenWindow->showFullScreen();
        } else {
            qDebug() << "当前没有图片可供显示";
        }
    });
    cameraMenu->addMenuOption("录像", [index]() {
        qDebug() << "摄像头" << index + 1 << "的选项 录像 被点击";
    });

    // 设置菜单按钮（箭头）样式：无边框，与固定文本高度一致
    cameraMenu->getMenuButton()->setStyleSheet(
        "background-color: transparent; "
        "font-size: 20px; "
        "padding: 0px; "
        "border: none;"
        );
    cameraMenu->getMenuButton()->setFixedHeight(fixedText->sizeHint().height());
    cameraMenu->getMenuButton()->setMinimumWidth(40);
    cameraMenu->setStyleSheet(
        "QMenu { background-color: white; border: 1px solid black; }"
        "QMenu::item { background-color: lightgray; color: black; }"
        "QMenu::item:hover { background-color: lightblue; }"
        );

    // 创建水平布局，将固定文本和菜单按钮并排
    QHBoxLayout *textButtonLayout = new QHBoxLayout();
    textButtonLayout->addWidget(fixedText);
    textButtonLayout->addWidget(cameraMenu->getMenuButton());
    textButtonLayout->setSpacing(2);

    // 创建覆盖层布局，将水平布局整体上移
    QVBoxLayout *overlayLayout = new QVBoxLayout();
    overlayLayout->addLayout(textButtonLayout);
    overlayLayout->addStretch();
    overlayLayout->setContentsMargins(0, 0, 2, 2);

    QWidget *overlayWidget = new QWidget(this);
    overlayWidget->setStyleSheet("background-color: transparent; border: none;");
    overlayWidget->setLayout(overlayLayout);

    // 创建容器布局，将 imageLabel 和 overlayWidget 叠加在一起
    QGridLayout *containerLayout = new QGridLayout(this);
    containerLayout->setContentsMargins(0, 0, 0, 0);
    containerLayout->addWidget(imageLabel, 0, 0);
    containerLayout->addWidget(overlayWidget, 0, 0, Qt::AlignRight | Qt::AlignTop);
    // imageLabel->setPixmap(QPixmap(":/images/resources/images/image.jpg"));

    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    setLayout(containerLayout);
}

void CameraLabelWidget::displayimg(QPixmap &pixmap)
{
    currentPixmap = pixmap;
    imageLabel->setPixmap(pixmap);
}

void CameraLabelWidget::displayimg(HImage &himage)
{
    QPixmap pixmap = convertHImageToPixmap(himage);
    currentPixmap = pixmap;
    imageLabel->setPixmap(pixmap);
}

QPixmap CameraLabelWidget::convertHImageToPixmap(const HImage& hImage) {
    const ImageHeader& header = hImage.imageHead;
    if (!hImage.data || header.width <= 0 || header.height <= 0 || header.channels <= 0) {
        return QPixmap();
    }
    QImage::Format format;
    switch (header.channels) {
    case 1: format = QImage::Format_Grayscale8; break;
    case 3: format = QImage::Format_RGB888; break;
    case 4: format = QImage::Format_RGBA8888; break;
    default: return QPixmap();
    }
    QImage qImage(
        reinterpret_cast<const uchar*>(hImage.data),
        header.width,
        header.height,
        header.width * header.channels,
        format
        );
    return QPixmap::fromImage(qImage);
}

HImage CameraLabelWidget::convertQPixmapToHImage(const QPixmap &pixmap) {
    HImage hImage;
    if (pixmap.isNull()) {
        return hImage;
    }
    QImage qImage = pixmap.toImage();
    QImage::Format format = qImage.format();
    int channels = 0;
    if (format == QImage::Format_Grayscale8) {
        channels = 1;
    } else if (format == QImage::Format_RGB888) {
        channels = 3;
    } else if (format == QImage::Format_RGBA8888 || format == QImage::Format_ARGB32) {
        channels = 4;
    } else {
        qImage = qImage.convertToFormat(QImage::Format_RGBA8888);
        channels = 4;
    }
    hImage.imageHead.width = qImage.width();
    hImage.imageHead.height = qImage.height();
    hImage.imageHead.channels = channels;
    int dataLength = hImage.imageHead.getdatelength();
    if (dataLength > 0) {
        hImage.data = new char[dataLength];
        memcpy(hImage.data, qImage.constBits(), dataLength);
    }
    return hImage;
}
