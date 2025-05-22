#include "CameraLabelWidget.h"
#include <QDebug>
#include <QLibrary>
#include <QElapsedTimer>
#include <qthread.h>
#include <QPixmap>
#include "MZ_ADOConn.h"
#include "ZoomableLabel.h"

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

CameraLabelWidget::CameraLabelWidget(int index, const QString &fixedTextName, QWidget *parent)
//CameraLabelWidget::CameraLabelWidget(Cameral *cam,int index, const QString &fixedTextName, QWidget *parent)
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



#if 0
    // 添加菜单选项（这里只给出一个示例，其他选项类似）
    cameraMenu->addMenuOption("运行", [index, this]() {
        qDebug() << "摄像头" << index + 1 << "的选项 运行 被点击";
          //TCP_Client::tcp1->startWork();

        HValues val1;
        HValues val2;
        HImages img;  // 用于接收返回的图像数据
        int errcode = 0;
        string errmsg = "";

        val1.m_Values.push_back(HValue(1));
        val1.m_Values.push_back(HValue(2));

        QElapsedTimer timer;
        timer.start();
        HImages primeimges;

       // QPixmap pixmap(":/images/resources/images/test.jpg");

        HImage hImage;
        hImage.imageHead.width = 640;
        hImage.imageHead.height = 480;
        hImage.imageHead.channels = 3;
        hImage.imageHead.length = 640 * 480 * 3;
        hImage.data = new unsigned char[hImage.imageHead.length];

        srand(QTime::currentTime().msec()); // 设置种子，只调用一次即可

        for (int i = 0; i < hImage.imageHead.length; ++i) {
            hImage.data[i] = static_cast<unsigned char>(rand() % 256); // 用 rand() 生成随机数
        }
        HImage& himage = hImage;
        qDebug() << "himage.imageHead.width: " << himage.imageHead.width;
        qDebug() << "himage.imageHead.height: " << himage.imageHead.height;
        qDebug() << "himage.imageHead.channels: " << himage.imageHead.channels;
        qDebug() << "himage.imageHead.length: " << himage.imageHead.length;
        qDebug() << "himage.data is nullptr? " << (himage.data == nullptr);
        primeimges.m_Images.push_back(hImage);

       //TCP_Client::tcp1->doAction(
            "pic_display_handle",   // funcName   val1, primeimges,       // 输入参数   val2, img,              // 输出参数      errcode, errmsg         // 错误码和错误信息    );

        qDebug()<<"imag.size:        "<<img.getImageNums();

        qint64 elapsedTime = timer.elapsed(); // 单位为毫秒
        qDebug() << "DoActionFun 耗时：" << elapsedTime << "ms";


        if (!img.m_Images.empty()) {
             HImage& himage = img.m_Images.front();
            qDebug() << "himage.imageHead.width: " << himage.imageHead.width;
            qDebug() << "himage.imageHead.height: " << himage.imageHead.height;
            qDebug() << "himage.imageHead.channels: " << himage.imageHead.channels;
            qDebug() << "himage.imageHead.length: " << himage.imageHead.length;
            qDebug() << "himage.data is nullptr? " << (himage.data == nullptr);
            QPixmap pixmap = convertHImageToPixmap(himage);
            qDebug()<<"label        ";
            qDebug() << "pixmap.isNull():" << pixmap.isNull();
            qDebug() << "pixmap.size():" << pixmap.size();
            displayimg(pixmap);
        } else {
            qDebug() << "接收到的 img.m_Images 是空的，无法获取 front 图像";
        }
      //TCP_Client::tcp1->stopWork();
    });
    cameraMenu->addMenuOption("停止", [index,this]() {
        qDebug() << "摄像头" << index + 1 << "的选项 停止 被点击";
      // //TCP_Client::tcp1->startWork();
        HValues val1;
        HValues val2;
        HImages img;  // 用于接收返回的图像数据
        int errcode = 0;
        string errmsg = "";

        // 创建 QElapsedTimer 对象
        QElapsedTimer timer;
        timer.start(); // 开始计时

        val1.m_Values.push_back(HValue(1));
        val1.m_Values.push_back(HValue(2));

        qDebug()<<"doaction before";
        //TCP_Client::tcp1->doAction(
            "pic_display_produce_ex",   // funcName   val1, HImages(),       // 输入参数    val2, img,              // 输出参数   errcode, errmsg         // 错误码和错误信息  );
        qDebug()<<"imag.size:"<<img.getImageNums();
        qDebug()<<"val2.m_Values[0].S()"<<val2.getValueNums();
        if(img.m_Images.size()>0)
        {
            HImage himage=img.m_Images.front();

            // 计算并输出运行时间
            qint64 elapsed = timer.elapsed(); // 获取经过的毫秒数
            qDebug() << "Execution time:" << elapsed << "ms";
            displayimg(himage);
        }
        else
        {
            qDebug()<<"empty";

        }
      // //TCP_Client::tcp1->stopWork();
    });
    cameraMenu->addMenuOption("OK", [index,this]() {
        qDebug() << "摄像头" << index + 1 << "的选项 OK 被点击";
        Mz_ClientControl::ClientOperation client;

        CommPorts port;
        port.isActAsServer = 0;  // 客户端
        port.PortName = "PIC_PORT";
        port.remote_IP.IP = "127.0.0.1";
        port.remote_IP.Port = 8000;

        client.InitSDK(port);
        client.StartWork();

        // 调用服务端的 pic_display_produce_ex 函数
        HValues val1;
        HValues val2;
        HImages img;  // 用于接收返回的图像数据
        int errcode = 0;
        string errmsg = "";

        // 创建 QElapsedTimer 对象
        QElapsedTimer timer;
        timer.start(); // 开始计时

        val1.m_Values.push_back(HValue(1));
        val1.m_Values.push_back(HValue(2));

        client.DoActionFun(port, "pic_display_produce_ex", val1, HImages(), &val2, &img, &errcode, &errmsg,10000);
        qDebug()<<"imag.size:"<<img.getImageNums();
        if(img.m_Images.size()>0)
            {
              HImage himage=img.m_Images.front();

            // 计算并输出运行时间
            qint64 elapsed = timer.elapsed(); // 获取经过的毫秒数
            qDebug() << "Execution time:" << elapsed << "ms";
            displayimg(himage);
             client.StopWork();
          //  client.FreeSDK();
        }
        else
            {
            qDebug()<<"empty";
             client.StopWork();
      //        client.FreeSDK();
            return;
        }

    });

    cameraMenu->addMenuOption("NG", [index,this]() {
        qDebug() << "摄像头" << index + 1 << "的选项 NG 被点击";

        HValues val1;
        HValues val2;
        HImages img;  // 用于接收返回的图像数据
        int errcode = 0;
        string errmsg = "";

        val1.m_Values.push_back(HValue(1));
        val1.m_Values.push_back(HValue(2));

        QElapsedTimer timer;
        timer.start();
        HImages primeimges;

        // QPixmap pixmap(":/images/resources/images/test.jpg");

        HImage hImage;
        hImage.imageHead.width = 640;
        hImage.imageHead.height = 480;
        hImage.imageHead.channels = 3;
        hImage.imageHead.length = 640 * 480 * 3;
        hImage.data = new unsigned char[hImage.imageHead.length];

        srand(QTime::currentTime().msec()); // 设置种子，只调用一次即可

        for (int i = 0; i < hImage.imageHead.length; ++i) {
            hImage.data[i] = static_cast<unsigned char>(rand() % 256); // 用 rand() 生成随机数
        }
        HImage& himage = hImage;
        qDebug() << "himage.imageHead.width: " << himage.imageHead.width;
        qDebug() << "himage.imageHead.height: " << himage.imageHead.height;
        qDebug() << "himage.imageHead.channels: " << himage.imageHead.channels;
        qDebug() << "himage.imageHead.length: " << himage.imageHead.length;
        qDebug() << "himage.data is nullptr? " << (himage.data == nullptr);
        primeimges.m_Images.push_back(hImage);

        //TCP_Client::tcp1->doAction(
            "pic_display_handle",   // funcName   val1, primeimges,       // 输入参数 val2, img,              // 输出参数errcode, errmsg         // 错误码和错误信息);

        qDebug()<<"imag.size:        "<<img.getImageNums();

        qint64 elapsedTime = timer.elapsed(); // 单位为毫秒
        qDebug() << "DoActionFun 耗时：" << elapsedTime << "ms";


        if (!img.m_Images.empty()) {
            HImage& himage = img.m_Images.front();
            qDebug() << "himage.imageHead.width: " << himage.imageHead.width;
            qDebug() << "himage.imageHead.height: " << himage.imageHead.height;
            qDebug() << "himage.imageHead.channels: " << himage.imageHead.channels;
            qDebug() << "himage.imageHead.length: " << himage.imageHead.length;
            qDebug() << "himage.data is nullptr? " << (himage.data == nullptr);
            QPixmap pixmap = convertHImageToPixmap(himage);
            qDebug()<<"label        ";
            qDebug() << "pixmap.isNull():" << pixmap.isNull();
            qDebug() << "pixmap.size():" << pixmap.size();
            displayimg(pixmap);
        } else {
            qDebug() << "接收到的 img.m_Images 是空的，无法获取 front 图像";
        }

    });

    cameraMenu->addMenuOption("错误", [index,this]() {
        qDebug() << "摄像头" << index + 1 << "的选项 错误 被点击";
        Mz_ClientControl::ClientOperation client;

        CommPorts port;
        port.isActAsServer = 0;  // 客户端
        port.PortName = "PIC_PORT";

        port.remote_IP.IP = "127.0.0.1";
        port.remote_IP.Port = 8001;
        client.InitSDK(port);
        client.StartWork();

        HValues val1;
        HValues val2;
        HImages img;  // 用于接收返回的图像数据
        int errcode = 0;
        string errmsg = "";

        val1.m_Values.push_back(HValue(1));
        val1.m_Values.push_back(HValue(2));

        QElapsedTimer timer;
        timer.start();
        HImages primeimges;

        // QPixmap pixmap(":/images/resources/images/test.jpg");

        HImage hImage;
        hImage.imageHead.width = 640;
        hImage.imageHead.height = 480;
        hImage.imageHead.channels = 3;
        hImage.imageHead.length = 640 * 480 * 3;
        hImage.data = new unsigned char[hImage.imageHead.length];

        srand(QTime::currentTime().msec()); // 设置种子，只调用一次即可

        for (int i = 0; i < hImage.imageHead.length; ++i) {
            hImage.data[i] = static_cast<unsigned char>(rand() % 256); // 用 rand() 生成随机数
        }
        HImage& himage = hImage;
        qDebug() << "himage.imageHead.width: " << himage.imageHead.width;
        qDebug() << "himage.imageHead.height: " << himage.imageHead.height;
        qDebug() << "himage.imageHead.channels: " << himage.imageHead.channels;
        qDebug() << "himage.imageHead.length: " << himage.imageHead.length;
        qDebug() << "himage.data is nullptr? " << (himage.data == nullptr);
        primeimges.m_Images.push_back(hImage);

        client.DoActionFun(port, "pic_display_handle", val1, primeimges, &val2, &img, &errcode, &errmsg,10000);


        qDebug()<<"imag.size:        "<<img.getImageNums();

        qint64 elapsedTime = timer.elapsed(); // 单位为毫秒
        qDebug() << "DoActionFun 耗时：" << elapsedTime << "ms";


        if (!img.m_Images.empty()) {
            HImage& himage = img.m_Images.front();
            qDebug() << "himage.imageHead.width: " << himage.imageHead.width;
            qDebug() << "himage.imageHead.height: " << himage.imageHead.height;
            qDebug() << "himage.imageHead.channels: " << himage.imageHead.channels;
            qDebug() << "himage.imageHead.length: " << himage.imageHead.length;
            qDebug() << "himage.data is nullptr? " << (himage.data == nullptr);
            QPixmap pixmap = convertHImageToPixmap(himage);
            qDebug()<<"label        ";
            qDebug() << "pixmap.isNull():" << pixmap.isNull();
            qDebug() << "pixmap.size():" << pixmap.size();
            displayimg(pixmap);
        } else {
            qDebug() << "接收到的 img.m_Images 是空的，无法获取 front 图像";
        }
        client.StopWork();
        client.FreeSDK();
    });

    cameraMenu->addMenuOption("参数", [this]() {
        // 将 Cameral 的参数传入 ParaWidget 并显示
        ParaWidget* parawidget = new ParaWidget(cameral->rangeParams,
                                                cameral->cameralParams,
                                                cameral->algoParams);
        parawidget->show();
    });

    cameraMenu->addMenuOption("相机", [index,this]() {
        qDebug() << "摄像头" << index + 1 << "的选项 相机 被点击";

#if 1
        Mz_ClientControl::ClientOperation server;

        CommPorts port;
        port.isActAsServer = 1;
        port.PortName = "PIC_PORT";
        port.localhost_IP.IP = "127.0.0.1";
        port.localhost_IP.Port = 8001;
qDebug()<<"get success11111!";
        server.InitSDK(port);

        Callbackfunc cb;
        cb.funcname = "pic_display";
       cb.func = &CameraLabelWidget::pic_display;
qDebug()<<"get success22222!";
        cb.inputImagesnums = 0;
        cb.inputPramsnums = 0;
        cb.outputImagesnums = 1;
        cb.outputPramsnums = 0;
        qDebug()<<"get success333333";
        server.RegsiterFunitFun(cb);
          qDebug()<<"get success333333";
        server.StartWork();
 qDebug()<<"get success!";
       // QThread::msleep(10000);
// server.StopWork();
      //  server.FreeSDK();


#else

        Mz_ClientControl::ClientOperation client;

        CommPorts port;
        port.isActAsServer = 0;  // 客户端
        port.PortName = "PIC_PORT";
        port.localhost_IP.IP = "127.0.0.1";
        port.localhost_IP.Port = 8000;

        client.InitSDK(port);
        client.StartWork();


        // 调用服务端的 get_frame 函数
        HValues val1;
        HValues val2;
        HImages img;  // 用于接收返回的图像数据
        int errcode = 0;
        string errmsg = "";

        val1.m_Values.push_back(HValue(1));
        val1.m_Values.push_back(HValue(2));

        QElapsedTimer timer;
        timer.start();

        client.DoActionFun(port, "get_frame", val1, HImages(), &val2, &img, &errcode, &errmsg,20000);
        qDebug()<<"imag.size:        "<<img.getImageNums();

        qint64 elapsedTime = timer.elapsed(); // 单位为毫秒
        qDebug() << "DoActionFun 耗时：" << elapsedTime << "ms";

        qDebug() << "imag.size:        " << img.getImageNums();

        if (!img.m_Images.empty()) {
            qDebug()<<"label1        ";
            HImage himage = img.m_Images.front();
             qDebug()<<"label2        ";
            QPixmap pixmap = convertHImageToPixmap(himage);
               qDebug()<<"label        ";
            qDebug() << "pixmap.isNull():" << pixmap.isNull();
            qDebug() << "pixmap.size():" << pixmap.size(); // 应该是 QSize(2448, 2048)
            displayimg(pixmap);
        } else {
            qDebug() << "接收到的 img.m_Images 是空的，无法获取 front 图像";
        }

      //  client.StopWork();
     //   client.FreeSDK();

#endif

    });

    cameraMenu->addMenuOption("全屏", [this]() {
        if (!currentPixmap.isNull()) {
            FullScreenWindow *fullscreenWindow = new FullScreenWindow(currentPixmap);
            fullscreenWindow->showFullScreen();
        } else {
            qDebug() << "当前没有图片可供显示";
        }
    });
#endif
    cameraMenu->addMenuOption("录像", [index,this]() {
 qDebug() << "录像" << index + 1 << "的选项 录像 被点击";

       // qDebug()<< cam->tcp->m_port.PortName;
       // qDebug()<<cam->tcp->m_port.remote_IP.Port;
       // qDebug()<<cam->tcp->m_isInitialized;
      //  cam->tcp->startWork();
     //  Mz_ClientControl::ClientOperation *client=new  Mz_ClientControl::ClientOperation();

     //   CommPorts port;
     //   port.isActAsServer = 0;
      //   port.PortName = "PORT0";
      //   port.remote_IP.IP = "127.0.0.1";
      //   port.remote_IP.Port = 8000;

      //  client->InitSDK(port);
      // client->StartWork();

       //cam->tcp->initialize(port);
      //cam->tcp->startWork();
       //cam->start();
       Sleep(100);
       int retcode=0;
       HValues inputvalues;
       HValue v("hello");
       inputvalues.m_Values.push_back(v);
       string retmsg="";
       HValues outparas;
       HImages outimgs;
     TCP_Client::tcp1->doAction("heartbeat",inputvalues,HImages(),outparas,outimgs,retcode,retmsg);
       qDebug()<<outparas.m_Values[0].S();
     //  qDebug()<<"cam->tcp->m_port.PortName  "<<cam->tcp->m_port.PortName;
//qDebug()<<"cam->tcp->m_port.remote_IP.Port  "<<cam->tcp->m_port.remote_IP.Port;
    //   cam->start();
    //   Sleep(1000);
     //  cam->doAction("heartbeat",inputvalues,HImages(),outparas,outimgs,retcode,retmsg);
//qDebug()<<"outparas.m_Values.size()   "<<outparas.m_Values.size() ;
//qDebug()<<"outparas.m_Values[0].S():   "<<outparas.m_Values[0].S();
      // qDebug()<<retmsg;
     //  TCP_Client *tcp=new  TCP_Client();
    //   tcp->initialize(port);
    //   tcp->startWork();

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
