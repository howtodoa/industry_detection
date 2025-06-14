#include "CameraLabelWidget.h"
#include <QDebug>
#include <QLibrary>
#include <QElapsedTimer>
#include <qthread.h>
#include <QPixmap>
#include <QMimeData>
#include <QImageReader>
#include <QUrl>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include "MZ_ADOConn.h"
#include "ZoomableLabel.h"
#include "imageviewerwindow.h"

HImage CameraLabelWidget::convertQPixmapToHImage(const QPixmap &pixmap) {
    HImage hImage;

    if (pixmap.isNull()) {
        qDebug() << "convertQPixmapToHImage: pixmap is null";
        return hImage;
    }

    QImage qImage = pixmap.toImage();

    //  RGB888 格式（适合 JPG 图像）
    if (qImage.format() != QImage::Format_RGB888) {
        qImage = qImage.convertToFormat(QImage::Format_RGB888);
    }

    int width = qImage.width();
    int height = qImage.height();
    int channels = 3;  // RGB888
    int bytesPerLine = qImage.bytesPerLine();
    int dataSize = bytesPerLine * height;

    // 设置图像头
    hImage.imageHead.width = width;
    hImage.imageHead.height = height;
    hImage.imageHead.channels = channels;
    hImage.imageHead.length = dataSize;

    // 分配并拷贝数据
    hImage.data = new unsigned char[dataSize];
    if (hImage.data != nullptr) {
        memcpy(hImage.data, qImage.constBits(), dataSize);
    } else {
        qDebug() << "convertQPixmapToHImage: memory allocation failed!";
    }

    return hImage;
}


void CameraLabelWidget::pic_display(HImages inputImages, HValues inputParams, HImages& outputImages, HValues& outputParams,int& errcode, string& errmsg)
{
    qDebug()<<"start";
    // 使用 QPixmap 从路径加载图像
    QPixmap pixmap(":/images/resources/images/OIP-C.jpg");  // 使用 Qt 的资源系统加载图片

    // 调用转换函数将 QPixmap 转换为 HImage
    HImage img = convertQPixmapToHImage(pixmap);
qDebug()<<"start1";
    // 如果转换失败，则返回错误
    if (img.data == nullptr)
    {
        errcode = 1;
        errmsg = "图片加载或转换失败";
        return;
    }

    // 将图像添加到输出图像列表
    outputImages.m_Images.push_back(img);

    // 设置返回值
    errcode = 0;  // 没有错误
    errmsg = "图像生成成功";  // 成功的消息
    qDebug()<<"start2";
}



void CameraLabelWidget::add_ui(HImages inputImages, HValues inputParams,
         HImages& outputImages, HValues& outputParams,
         int& errcode, std::string& errmsg)
{
qDebug()<<"start";
    HValue v = inputParams.m_Values[0].I() + inputParams.m_Values[1].I();
    outputParams.m_Values.push_back(v);

   qDebug()<<"start2";
}


void CameraLabelWidget::pic_handle(HImages inputImages, HValues inputParams, HImages& outputImages, HValues& outputParams,int& errcode, string& errmsg)
{
    qDebug()<<"start";
    qDebug()<<"inputImages.m_Images.size()  "<<inputImages.m_Images.size();
    qDebug()<<"inputImages.m_Images[0].getdatelength()"<<inputImages.m_Images[0].getdatelength();

   // displayimg(inputImages.m_Images[0]);
    // 设置返回值
    outputImages.m_Images.push_back(inputImages.m_Images[0]);
    errcode = 0;  // 没有错误
    errmsg = "图像生成成功";  // 成功的消息
    qDebug()<<"start2";
}

CameraLabelWidget::CameraLabelWidget(Cameral *cam,int index, const QString &fixedTextName, QWidget *parent)
    : QWidget(parent)
{

    // 设置主容器的样式
    setStyleSheet("background-color: lightgray; border: 1px solid gray;");

    // 创建显示图像的标签，使用 ZoomableLabel 替换原有的 QLabel
    imageLabel = new ZoomableLabel(this);
    imageLabel->setObjectName("cameraImageLabel");
    imageLabel->setStyleSheet("background-color: lightgray;");
    imageLabel->setAlignment(Qt::AlignCenter);

    // 创建固定文本标签
    fixedText = new QLabel(fixedTextName, this);
    fixedText->setStyleSheet("background-color: transparent; color: black; font-size: 20px;");
    fixedText->setAttribute(Qt::WA_TransparentForMouseEvents);
    fixedText->setAlignment(Qt::AlignCenter);

    // 创建 CameraMenu 并设置菜单选项
    cameraMenu = new CameraMenu(this);

    // 添加菜单选项（这里只给出一个示例，其他选项类似）
    cameraMenu->addMenuOption("运行", [index, this]() {
        qDebug() << "摄像头" << index + 1 << "的选项 运行 被点击";
    });
    cameraMenu->addMenuOption("停止", [index,this]() {
        qDebug() << "摄像头" << index + 1 << "的选项 停止 被点击";
    });
    cameraMenu->addMenuOption("OK", [index,this,cam]() {
        qDebug() << "摄像头" << index + 1 << "的选项 OK 被点击";

        if (!QDesktopServices::openUrl(QUrl::fromLocalFile(cam->ok_path))) {
            qWarning() << "无法打开路径:" << cam->ok_path;
        }
    });

    cameraMenu->addMenuOption("NG", [index,this,cam]() {
        qDebug() << "摄像头" << index + 1 << "的选项 NG 被点击";
        if (!QDesktopServices::openUrl(QUrl::fromLocalFile(cam->ng_path))) {
            qWarning() << "无法打开路径:" << cam->ng_path;
        }
    });

    cameraMenu->addMenuOption("参数", [this,cam]() {
        // 将 Cameral 的参数传入 ParaWidget 并显示
        ParaWidget* parawidget = new ParaWidget(cam->RC,
                                                cam->CC,
                                               cam->AC);
       parawidget->show();
    });

    cameraMenu->addMenuOption("拍照", [index,this]() {
        qDebug() << "摄像头" << index + 1 << "的选项 相机 被点击";

    });

    // cameraMenu->addMenuOption("全屏", [this]() {
    //     if (!currentPixmap.isNull()) {
    //         FullScreenWindow *fullscreenWindow = new FullScreenWindow(currentPixmap);
    //         fullscreenWindow->showFullScreen();
    //     } else {
    //         qDebug() << "当前没有图片可供显示";
    //     }
    // });
    cameraMenu->addMenuOption("查看图像", [this]() {
        if (!currentPixmap.isNull()) {
            auto *viewer = new ImageViewerWindow(currentPixmap, nullptr);
            viewer->setAttribute(Qt::WA_DeleteOnClose);
            viewer->show();
        } else {
             auto *viewer = new ImageViewerWindow(nullptr);
            viewer->setAttribute(Qt::WA_DeleteOnClose);
            viewer->show();
        }
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
    qDebug()<<"cccccccc";
}

void CameraLabelWidget::displayimg(HImage &himage)
{
    QPixmap pixmap = convertHImageToPixmap(himage);
    currentPixmap = pixmap;
    imageLabel->setPixmap(pixmap);
    qDebug()<<"bbbbbbb";
}

QPixmap CameraLabelWidget::convertHImageToPixmap(const HImage& hImage) {
    const ImageHeader& header = hImage.imageHead;

    if (!hImage.data || header.width <= 0 || header.height <= 0 || header.channels <= 0 || header.length <= 0) {
        return QPixmap(); // 无效图像
    }

    QImage::Format format;
    switch (header.channels) {
    case 1:
        format = QImage::Format_Grayscale8;
        break;
    case 3:
        format = QImage::Format_RGB888;
        break;
    case 4:
        format = QImage::Format_RGBA8888;
        break;
    default:
        return QPixmap(); // 不支持的通道数
    }


    QImage image(hImage.data, header.width, header.height, header.width * header.channels, format);


    return QPixmap::fromImage(image.copy());
}


