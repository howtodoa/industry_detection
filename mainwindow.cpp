#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "cameralmenu.h"
#include "cameral.h"
#include "login.h"
#include "public.h"
#include "parawidget.h"
#include "syspara.h"


#include "cameralabelwidget.h"
#include "MZ_ADOConn.h"
#include "tcp_client.h"
#include "QTimer"
#include "TCP_Client.h"



void MainWindow::test()
{

    QPixmap pixmap(":/images/resources/images/image.jpg");
    cameraLabels[4]->displayimg(pixmap);

    QPixmap pixmap1(":/images/resources/images/test.jpg");
    cameraLabels[3]->displayimg(pixmap1);


    int width = 2600;
    int height = 2160;
    int channels = 3;
    int imageDataLength = width * height * channels;  // 数据长度

    // 为图片数据创建一个缓冲区，这里用随机数据来模拟图像数据
    unsigned char* imageData = new unsigned char[imageDataLength];
    // 填充图像数据（例如：将所有值设为 255 表示白色图像）
    std::srand(std::time(0));

    // 填充图像数据（生成彩色图像）
    for (int i = 0; i < imageDataLength; i += 3) {
        imageData[i] = std::rand() % 256;       // R 通道（0-255）
        imageData[i + 1] = std::rand() % 256;   // G 通道（0-255）
        imageData[i + 2] = std::rand() % 256;   // B 通道（0-255）
    }
    // 创建 HImage 对象
    HImage disimg(width, height, channels, imageDataLength, imageData);

    delete[] imageData;

    cameraLabels[1]->displayimg(disimg);
}

void MainWindow::test_tcp()
{
     qDebug()<<Cameral::Cams[0]->tcp->m_port.PortName;
     qDebug()<<Cameral::Cams[0]->tcp->m_port.remote_IP.Port;
     qDebug()<<Cameral::Cams[0]->tcp->m_isInitialized;
     if(Cameral::Cams[0]->tcp->m_clientOp==nullptr) qDebug()<<"nullptr";
//     Cameral::Cams[0]->tcp->startWork();
     while(1)
    {
     Sleep(1000);
     int retcode=0;
     HValues inputvalues;
     HValue v("hello");
     inputvalues.m_Values.push_back(v);
     string retmsg="";
     HValues outparas;
     HImages outimgs;
    Cameral::Cams[0]->doAction("heartbeat",inputvalues,HImages(),outparas,outimgs,retcode,retmsg);
     qDebug()<<"outparas.m_Values.size()   "<<outparas.m_Values.size() ;
     //qDebug()<<"outparas.m_Values[0].S():   "<<outparas.m_Values[0].S();
     }

}

void MainWindow::InitTcp()
{
   TCP_Client::tcp1 = new TCP_Client();
    TCP_Client::tcp2 = new TCP_Client();
    TCP_Client::tcp3 = new TCP_Client();
   TCP_Client::tcp4 = new TCP_Client();
   TCP_Client::tcp5 = new TCP_Client();
   TCP_Client::tcp6 = new TCP_Client();

    CommPorts port1;
    port1.isActAsServer = 0;
    port1.PortName = "PIC_PORT0";
    port1.remote_IP.IP = "127.0.0.1";
    port1.remote_IP.Port = 8000;

   TCP_Client::tcp1->initialize(port1);

   Callbackfunc cb;


   cb.funcname = "add_ui";
   cb.func = &CameraLabelWidget::add_ui;
   bool ret;
   cb.inputImagesnums = 0;
   cb.inputPramsnums = 0;
   cb.outputImagesnums = 0;
   cb.outputPramsnums = 1;
   ret=TCP_Client::tcp1->RegsiterFunitFun(cb);

   Callbackfunc ca;
   ca.funcname = "pic_handle";
   ca.func = &CameraLabelWidget::pic_handle;

   ca.inputImagesnums = 0;
   ca.inputPramsnums = 1;
   ca.outputImagesnums = 0;
   ca.outputPramsnums = 0;
   ret=TCP_Client::tcp1->RegsiterFunitFun(ca);
   qDebug()<<"ret=:   "<<ret;
   TCP_Client::tcp1->startWork();

}

TCP_Client* MainWindow::InitTcp(CommPorts port)
{
    TCP_Client* tcp=new TCP_Client();
      tcp->initialize(port);
    Callbackfunc cb;


    cb.funcname = "add_ui";
    cb.func = &CameraLabelWidget::add_ui;
    bool ret;
    cb.inputImagesnums = 0;
    cb.inputPramsnums = 0;
    cb.outputImagesnums = 0;
    cb.outputPramsnums = 1;
    ret=tcp->RegsiterFunitFun(cb);

    Callbackfunc ca;
    ca.funcname = "pic_handle";
    ca.func = &CameraLabelWidget::pic_handle;

    ca.inputImagesnums = 0;
    ca.inputPramsnums = 1;
    ca.outputImagesnums = 1;
    ca.outputPramsnums = 0;
    ret=tcp->RegsiterFunitFun(ca);
    qDebug()<<"ret=:dddddddddddddddddddddddddddddddddd   "<<ret;
    //tcp->startWork();
    return tcp;
}

TCP_Client * MainWindow::InitTcp(int index)
{
    TCP_Client* tcp=new TCP_Client();

     CommPorts port1;
    port1.isActAsServer = 0;
    port1.PortName = "PORT" + std::to_string(index);
    port1.remote_IP.IP = "127.0.0.1";
    port1.remote_IP.Port = 8000+index;

    tcp->initialize(port1);

    Callbackfunc cb;


    cb.funcname = "add_ui";
    cb.func = &CameraLabelWidget::add_ui;
    bool ret;
    cb.inputImagesnums = 0;
    cb.inputPramsnums = 0;
    cb.outputImagesnums = 0;
    cb.outputPramsnums = 1;
    ret=tcp->RegsiterFunitFun(cb);

    Callbackfunc ca;
    ca.funcname = "pic_handle";
    ca.func = &CameraLabelWidget::pic_handle;

    ca.inputImagesnums = 0;
    ca.inputPramsnums = 1;
    ca.outputImagesnums = 0;
    ca.outputPramsnums = 0;
    ret=tcp->RegsiterFunitFun(ca);
    qDebug()<<"ret=:dddddddddddddddddddddddddddddddddd   "<<ret;
    //tcp->startWork();
    return tcp;
}

void MainWindow::InitCam(int index)
{
    for(int idx = 0; idx <index ; idx++)
    {
        Sleep(50);
        // 初始化 Cameral 对象，并设置参数
       Cameral* cam = new Cameral();
    //   std::shared_ptr cam=std::make_shared<Cameral>();
        cam->rangeParams = {10.0f, 5.0f, 12.0f, 20.0f, 5.0f, 3.0f, 15.0f, 8.0f, 18.0f, 25.0f, 10.0f, 8.0f};
        cam->cameralParams = { "Camera" + QString::number(idx),
                                        "SN" + QString::number(idx),
                                        "192.168.1." + QString::number(idx),
                                        1, idx, 50, 100, 200 };
        cam->algoParams = { "Algorithm" + QString::number(idx),
                                     {"参数一", "参数二"},
                                     {"100", "200"} };
   //     cam->tcp= InitTcp(idx);
        CommPorts port;
        port.isActAsServer = 0;
        port.PortName = "PORT"+std::to_string(idx);
        port.remote_IP.IP = "127.0.0.1";
        port.remote_IP.Port = 8000+idx;
     //   cam->tcp = InitTcp(idx);
        cam->tcp->initialize(port);

        Callbackfunc cb;
        cb.funcname = "add_ui";
        cb.func = &CameraLabelWidget::add_ui;

        cb.inputImagesnums = 0;
        cb.inputPramsnums = 0;
        cb.outputImagesnums = 0;
        cb.outputPramsnums = 1;
        cam->RegsiterFunitFun(cb);
        Callbackfunc ca;
        ca.funcname = "pic_handle";
        ca.func = &CameraLabelWidget::pic_handle;

        ca.inputImagesnums = 0;
        ca.inputPramsnums = 1;
        ca.outputImagesnums = 0;
        ca.outputPramsnums = 0;
        cam->RegsiterFunitFun(ca);

        Callbackfunc cc;
        cc.funcname="heartbeat_ui";
        cc.inputImagesnums = 0;
        cc.inputPramsnums = 1;
        cc.outputImagesnums = 0;
        cc.outputPramsnums = 1;
        cc.func=[&](HImages inputImages, HValues inputPrams,
                      HImages& outputImages, HValues& outputParams,
                      int& errcode, std::string& errmsg)
        {
            cam->heartbeat_ui(inputImages, inputPrams, outputImages, outputParams, errcode, errmsg);
        };
        cam->RegsiterFunitFun(cc);
       // if(idx==0)
       //  {
       //      cam->start();
       //     int retcode=0;
       //     HValues inputvalues;
       //     HValue v("hello");
       //     inputvalues.m_Values.push_back(v);
       //     string retmsg="";
       //     HValues outparas;
       //     HImages outimgs;
       //     cam->doAction("heartbeat",inputvalues,HImages(),outparas,outimgs,retcode,retmsg);
       // }
       // cam->start();
        qDebug()<<"idx:   "<<idx;
        //Cameral::Cams.push_back(std::move(cam));
        Cameral::Cams.push_back(cam);
         if(idx==0)
          {
               Cameral::Cams[0]->start();
         }

    }
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("电容视觉检测系统");
    // 获取主屏幕的分辨率
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry(); // 获取屏幕的几何信息
    int screenWidth = screenGeometry.width();  // 屏幕宽度
    int screenHeight = screenGeometry.height(); // 屏幕高度

    int windowWidth = screenWidth * 1;
    int windowHeight = screenHeight * 1;

    resize(windowWidth, windowHeight);

    // 可选：将窗口居中
    int x = (screenWidth - windowWidth) / 2;
    int y = (screenHeight - windowHeight) / 2;
    move(x, y);

  //  InitTcp();
  // InitCam(7);
   // test_tcp();
   CreateMenu();
    CreateImageGrid();
    test();

}

MainWindow::~MainWindow()
{
    delete ui;
    delete menuExecute; // "执行"菜单
    delete menuRecord;  // "记录"菜单
    delete menuTools;   // "工具"菜单
    delete menuUser;    // "用户"菜单
    for (QWidget* widget : cameraLabels) {
        delete widget;
    }

}


void MainWindow::CreateMenu()
{
    QMenuBar *menuBar = this->menuBar();

    menuExecute = menuBar->addMenu("执行");
    menuRecord = menuBar->addMenu("记录");
    menuTools = menuBar->addMenu("工具");
    menuUser = menuBar->addMenu("用户");



    QMenu *menuStart = menuExecute->addMenu("运行全部检测");
    QMenu *menuStop = menuExecute->addMenu("停止全部检测");



    QMenu *menuBlock = menuTools->addMenu("屏蔽输出");
    QMenu *menuLight = menuTools->addMenu("光源与IO");
    QMenu *menuSelfStart = menuTools->addMenu("开机自启动");
    QMenu *menuSModeCancel = menuTools->addMenu("SModeCancel");
    QMenu *menuSModeAutostart = menuTools->addMenu("SMode_Autostart");
    QMenu *menuForceMode = menuTools->addMenu("ForceMode");


    QMenu *menuLogout = menuUser->addMenu("注销");

    QMenu *menuBlockCameral1 = menuBlock->addMenu("屏蔽正引脚输出");
    QMenu *menuBlockCameral2 = menuBlock->addMenu("屏蔽座板输出");
    QMenu *menuBlockCameral3 = menuBlock->addMenu("屏蔽负引脚输出");
    QMenu *menuBlockCameral4 = menuBlock->addMenu("屏蔽捺印输出");
    QMenu *menuBlockCameral5 = menuBlock->addMenu("屏蔽载带输出");
    QMenu *menuBlockCameral6 = menuBlock->addMenu("屏蔽H部输出");
    QMenu *menuBlockCameral7 = menuBlock->addMenu("屏蔽载带座板输出");
    QMenu *menuBlockPLC = menuBlock->addMenu("启动PLC链接");

    QAction *loginAction = menuUser->addAction("登录"); // 直接赋值给成员变量
    QAction *LogAction = menuRecord->addAction("日志");
    QAction *DataAction = menuRecord->addAction("数据");
    QAction *SystemParaAction = menuTools->addAction("系统参数");

    // 获取或创建状态栏
    QStatusBar *statusBar = this->statusBar();
    if (!statusBar) {
        statusBar = new QStatusBar(this);
        setStatusBar(statusBar);
    }
    statusBar->setMinimumHeight(25); // 设置最小高度，确保可见
    statusBar->show(); // 确保显示

    // 创建运行时间和角色的 QLabel
    QLabel *runtimeLabel = new QLabel("运行时间: 00:00:00", this);
    QLabel *roleLabel = new QLabel(role.GetCurrentRole(), this);
    runtimeLabel->setStyleSheet("QLabel { color: black; }");
    roleLabel->setStyleSheet("QLabel { color: black; }");

    // 将 QLabel 添加到状态栏左侧
    statusBar->addWidget(runtimeLabel); // 靠左显示
    statusBar->addWidget(roleLabel);

    // 设置定时器更新运行时间
    QTimer *timer = new QTimer(this);
    QDateTime startTime = QDateTime::currentDateTime();
    connect(timer, &QTimer::timeout, this, [runtimeLabel, startTime]() {
        QDateTime currentTime = QDateTime::currentDateTime();
        int seconds = startTime.secsTo(currentTime);
        int hours = seconds / 3600;
        int minutes = (seconds % 3600) / 60;
        int secs = seconds % 60;
        runtimeLabel->setText(QString("运行时间: %1:%2:%3")
                                  .arg(hours, 2, 10, QChar('0'))
                                  .arg(minutes, 2, 10, QChar('0'))
                                  .arg(secs, 2, 10, QChar('0')));
    });
    timer->start(100);

    // 连接登录动作以更新角色信息
    connect(loginAction, &QAction::triggered, this, [this, loginAction, roleLabel]() {
        qDebug() << "登录动作触发";
        Login *loginDialog = new Login(this);
        loginDialog->setAttribute(Qt::WA_DeleteOnClose);
        connect(loginDialog, &Login::loginSuccess, this, &Role::ChangeRole);
        connect(loginDialog, &QDialog::accepted, this, [this, loginDialog, loginAction, roleLabel]() {
            QString password = loginDialog->GetPassword();

            qDebug() << "输入的密码:" << password;
            if (!password.isEmpty()) {

                roleLabel->setText(role.GetCurrentRole());
                show();
            } else {
                qDebug() << "密码为空，登录失败";
            }
        });
        loginDialog->show();
    });

    connect(LogAction, &QAction::triggered, this, [this, LogAction]() {
                QString logFolderPath = SystemPara::log_dir;

                QDir dir(logFolderPath);
                if (!dir.exists()) {
                    qDebug() << "错误: log 文件夹不存在于路径:" << logFolderPath;
                    return;
                }

                QString selectedFile = QFileDialog::getOpenFileName(
                    this,
                    "选择日志文件",
                    logFolderPath,
                    "Text Files (*.txt)"
                );

                if (!selectedFile.isEmpty()) {
                    qDebug() << "用户选择的文件:" << selectedFile;
                    bool opened = QDesktopServices::openUrl(QUrl::fromLocalFile(selectedFile));
                    if (!opened) {
                        qDebug() << "无法打开文件:" << selectedFile;
                    }
                } else {
                    qDebug() << "用户取消了文件选择";
                }
    });

    connect(DataAction, &QAction::triggered, this, [this, DataAction]() {
                QString logFolderPath = SystemPara::data_dir;

                QDir dir(logFolderPath);
                if (!dir.exists()) {
                    qDebug() << "错误: data 文件夹不存在于路径:" << logFolderPath;
                    return;
                }

                QString selectedFile = QFileDialog::getOpenFileName(
                    this,
                    "选择数据文件",
                    logFolderPath,
                    "Text Files (*.txt)"
                );

                if (!selectedFile.isEmpty()) {
                    qDebug() << "用户选择的文件:" << selectedFile;
                    bool opened = QDesktopServices::openUrl(QUrl::fromLocalFile(selectedFile));
                    if (!opened) {
                        qDebug() << "无法打开文件:" << selectedFile;
                    }
                } else {
                    qDebug() << "用户取消了文件选择";
                }
    });

    connect(SystemParaAction, &QAction::triggered, this, [this]() {
        SysPara *syspara = new SysPara(this);
        QScreen *screen = QApplication::primaryScreen();
        QRect screenGeometry = screen->geometry();
        int x = (screenGeometry.width() - syspara->width()) / 2;
        int y = (screenGeometry.height() - syspara->height()) / 2;
        syspara->move(x, y);
        syspara->show();

    });

}

void MainWindow::SetupCameraGridLayout(int i, QGridLayout* gridLayout, QVector<CameraLabelWidget*>& cameraLabels, QWidget* window)
{
   // auto& Cams = Cameral::Cams;
    // 输入验证：i 范围 [0, 7]，gridLayout 和 window 非空
    if (!gridLayout || !window || i < 0 || i > 7)
        return;

    // 清空旧布局
    QLayoutItem* item;
    while ((item = gridLayout->takeAt(0)) != nullptr) {
        if (item->widget())
            delete item->widget();
        delete item;
    }
    cameraLabels.clear();

    // 固定 2 行 4 列布局
    int cols = 4;
    // qDebug()<<"Cams.size():  "<<Cameral::Cams.size();
    // if (Cameral::Cams.size() < i) {
    //     qWarning() << "Cameral::Cams 中数据不足，无法初始化 " << i << " 个相机框！";
    //     return;
    // }
    // 添加前 i 个相机框，使用新设计的 CameraLabelWidget
    for (int idx = 0; idx < i; ++idx) {
        // CameralName(idx) 为生成固定文本的函数
        CameraLabelWidget* cameraLabel = new CameraLabelWidget(idx, CameralName(idx), window);
        cameraLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        cameraLabels.append(cameraLabel);
        gridLayout->addWidget(cameraLabel, idx / cols, idx % cols);
    }

    // 添加第八个固定框
    QWidget* emptyWidget = CreateEighthFrame();
    emptyWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    gridLayout->addWidget(emptyWidget, i / cols, i % cols);

    // 根据 i 动态调整窗口大小
    int frameWidth = 200;  // 每个框的基准宽度（可调整）
    int frameHeight = 150; // 每个框的基准高度（可调整）
    int spacing = 10;      // 网格间距
    int margin = 20;       // 窗口边距

    // 计算使用的行列数
    int usedSlots = i + 1; // i 个相机框 + 第八个固定框
    int rows = (usedSlots + 3) / 4; // 向上取整
    int usedCols = (usedSlots <= 4) ? usedSlots : 4; // 第一行最多 4 列

    // 计算窗口大小
    int windowWidth = usedCols * frameWidth + (usedCols - 1) * spacing + 2 * margin;
    int windowHeight = rows * frameHeight + (rows - 1) * spacing + 2 * margin;

    // 调整窗口大小
    window->resize(windowWidth, windowHeight);
}

void MainWindow::CreateImageGrid()
{
    // 创建中央控件，并设置可扩展属性
    QWidget *centralWidget = new QWidget(this);
    centralWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setCentralWidget(centralWidget);

    // 创建网格布局，设置无边距和无间距
    QGridLayout *gridLayout = new QGridLayout(centralWidget);
    gridLayout->setContentsMargins(0, 0, 0, 0);
    gridLayout->setSpacing(0);

    // 根据相机数量创建相机标签（注意：cameraLabels 类型为 QVector<CameraLabelWidget*>）
    // 使用 centralWidget 作为窗口参数，便于在中央控件中查找子控件
    SetupCameraGridLayout(7, gridLayout, cameraLabels, centralWidget);

    // 设置行列拉伸因子，确保网格中的控件均分空间
    for (int row = 0; row < 2; ++row) {
        gridLayout->setRowStretch(row, 1);
    }
    for (int col = 0; col < 4; ++col) {
        gridLayout->setColumnStretch(col, 1);
    }
}


QString MainWindow::CameralName(int &i)
{
    if(i==0) return QString("正引脚");
    if(i==1) return QString("座板");
    if(i==2) return QString("负引脚");
    if(i==3) return QString("捺印");
    if(i==4) return QString("载带");
    if(i==5) return QString("H部");
    if(i==6) return QString("载带座板");
    return QString("");
}


QWidget* MainWindow::CreateEighthFrame()
{
    // 创建容器控件
    QWidget *container = new QWidget(this);
    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    container->setStyleSheet("background-color: white; border: 1px solid black;");

    // 主布局：垂直布局
    QVBoxLayout *mainLayout = new QVBoxLayout(container);
    mainLayout->setContentsMargins(5, 5, 5, 5);
    mainLayout->setSpacing(5);

    // 创建 8 行，每行 4 个标签
    for (int row = 0; row < 10; ++row) {
        QHBoxLayout *rowLayout = new QHBoxLayout();
        rowLayout->setSpacing(3);

        for (int col = 0; col < 4; ++col) {
            QString text;
            // 第一行 (row == 0)
            if (row == 0 && col == 0) {
                text = "相机";
            } else if (row == 0 && col == 1) {
                text = "总数";
            } else if (row == 0 && col == 2) {
                text = "NG";
            } else if (row == 0 && col == 3) {
                text = "良率";
            }
            // 第二行 (row == 1)
            else if (row == 1 && col == 0) {
                text = "正引脚";
            }
            // 第三行 (row == 2)
            else if (row == 2 && col == 0) {
                text = "座板";
            }
            // 第四行 (row == 3)
            else if (row == 3 && col == 0) {
                text = "负引脚";
            }
            // 第五行 (row == 4)
            else if (row == 4 && col == 0) {
                text = "捺印";
            }
            // 第六行 (row == 5)
            else if (row == 5 && col == 0) {
                text = "载带";
            }
            // 第七行 (row == 6)
            else if (row == 6 && col == 0) {
                text = "H部";
            }
            // 第八行 (row == 7)
            else if (row == 7 && col == 0) {
                text = "载带座板";
            }
            else if (row == 8 && col == 3) {
                text = "总和";
            }
            else if (row == 9 && col == 0) {
                text = "触发";
            }
            else if (row == 9 && col == 1) {
                text = "启动";
            }
            else if (row == 9 && col == 2) {
                text = "停止";
            }
            else if (row == 9 && col == 3) {
                text = "一键清理";
            }
            // 其他位置为空
            else {
                text = "";
            }

            QLabel *label = new QLabel(text, container);
            label->setAlignment(Qt::AlignCenter);
            QFont font = label->font();
            font.setPointSize(20);
            label->setFont(font);
            label->setStyleSheet("background-color: white; border: 1px solid gray;");
            label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            rowLayout->addWidget(label);
        }
        mainLayout->addLayout(rowLayout);
    }
    setLabel(mainLayout, 1, 1);
    return container;
}



void MainWindow::setLabel(QVBoxLayout *layout, int row, int col)
{
    int counter=0;
    if (row >= 0 && row < 9 && col >= 0 && col < 4) { // 边界检查
        QHBoxLayout *rowLayout = qobject_cast<QHBoxLayout*>(layout->itemAt(row)->layout());
        if (rowLayout) {
            QLabel *label = qobject_cast<QLabel*>(rowLayout->itemAt(col)->widget());
            if (label) {
                label->setText(QString::number(counter++)); // 设置内容为自增的 counter
            }
        }
    }
}

