#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "cameralmenu.h"
#include "cameral.h"
#include "login.h"
#include "public.h"
#include "parawidget.h"
#include "syspara.h"
#include "fileoperator.h"
#include "cameralabelwidget.h"
#include "addcameradialog.h"
#include "MZ_ADOConn.h"
#include "tcp_client.h"
#include "rezultinfo_nayin.h"
#include "QTimer"


void MainWindow::loadjson_layer(const QString& filePath)
{
    // 使用 FileOperator 读取 JSON 文件为 QVariantMap
    QVariantMap configMap = FileOperator::readJsonMap(filePath);


    if (configMap.isEmpty()) {
        qWarning() << "Failed to load configuration from" << filePath;
        return;
    }

    // 从 configMap 中解析每个 ParamDetail，并将其 '值' 赋给 SystemPara 的静态成员
    // 检查每个键是否存在，以避免潜在的崩溃并提供更好的错误提示
    if (configMap.contains("OK_DIR")) {
        ParamDetail detail(configMap.value("OK_DIR").toMap());
        SystemPara::OK_DIR = detail.value.toString();
        qDebug() << "Parsed OK_DIR:" << SystemPara::OK_DIR;
    } else {
        qWarning() << "JSON file missing 'OK_DIR' entry.";
    }

    if (configMap.contains("NG_DIR")) {
        ParamDetail detail(configMap.value("NG_DIR").toMap());
        SystemPara::NG_DIR = detail.value.toString();
        qDebug() << "Parsed NG_DIR:" << SystemPara::NG_DIR;
    } else {
        qWarning() << "JSON file missing 'NG_DIR' entry.";
    }

    if (configMap.contains("LOG_DIR")) {
        ParamDetail detail(configMap.value("LOG_DIR").toMap());
        SystemPara::LOG_DIR = detail.value.toString();
        qDebug() << "Parsed LOG_DIR:" << SystemPara::LOG_DIR;
    } else {
        qWarning() << "JSON file missing 'LOG_DIR' entry.";
    }

    if (configMap.contains("DATA_DIR")) {
        ParamDetail detail(configMap.value("DATA_DIR").toMap());
        SystemPara::DATA_DIR = detail.value.toString();
        qDebug() << "Parsed DATA_DIR:" << SystemPara::DATA_DIR;
    } else {
        qWarning() << "JSON file missing 'DATA_DIR' entry.";
    }

    if (configMap.contains("ROOT_DIR")) {
        ParamDetail detail(configMap.value("ROOT_DIR").toMap());
        SystemPara::ROOT_DIR = detail.value.toString();
        qDebug() << "Parsed ROOT_DIR:" << SystemPara::ROOT_DIR;
    } else {
        qWarning() << "JSON file missing 'ROOT_DIR' entry.";
    }
    qDebug() << "JSON parsing complete and SystemPara members populated.";
}

void MainWindow::loadjson_layer2(const QString& filePath)
{
    // 使用 FileOperator 读取 JSON 文件为 QVariantMap
    QVariantMap configMap = FileOperator::readJsonMap(filePath);

    // 检查是否成功读取到配置
    if (configMap.isEmpty()) {
        qWarning() << "Failed to load configuration from" << filePath;
        return;
    }

    // 遍历 configMap 中的所有顶层键（例如 "相机1"）
    for (auto it = configMap.begin(); it != configMap.end(); ++it) {
        if (it.value().type() == QVariant::Map) {
            QVariantMap cameraData = it.value().toMap(); // 获取 "相机1" 下面的所有数据

            Camerinfo currentCamera; // 创建一个 Camerinfo 对象来存储当前相机的信息

            // 相机是否可见,如果不可见直接跳过
            if (cameraData.contains("检测")) {
                ParamDetail nameDetail(cameraData.value("检测").toMap());
                currentCamera.check = nameDetail.value.toString();
                if(currentCamera.check=="false") continue;
            } else {
                qWarning() << "Missing '相机名称' for camera:" << it.key();
            }

            // 解析 "相机名称"
            if (cameraData.contains("相机名称")) {
                ParamDetail nameDetail(cameraData.value("相机名称").toMap());
                currentCamera.name = nameDetail.value.toString();
            } else {
                qWarning() << "Missing '相机名称' for camera:" << it.key();
            }

            // 解析 "相机ip"
            if (cameraData.contains("相机ip")) {
                ParamDetail ipDetail(cameraData.value("相机ip").toMap());
                currentCamera.ip = ipDetail.value.toString();
            } else {
                qWarning() << "Missing '相机ip' for camera:" << it.key();
            }

            // 解析 "相机port"
            if (cameraData.contains("相机port")) {
                ParamDetail portDetail(cameraData.value("相机port").toMap());
                currentCamera.port = portDetail.value.toInt();
            } else {
                qWarning() << "Missing '相机port' for camera:" << it.key();
            }

            // 解析 "相机SN"
            if (cameraData.contains("相机SN")) {
                ParamDetail snDetail(cameraData.value("相机SN").toMap());
                currentCamera.SN = snDetail.value.toString();
            } else {
                qWarning() << "Missing '相机SN' for camera:" << it.key();
            }

            // 解析 "相机配置文件路径"
            if (cameraData.contains("配置文件路径")) {
                ParamDetail snDetail(cameraData.value("配置文件路径").toMap());
                currentCamera.path= snDetail.value.toString();
            } else {
                qWarning() << "Missing '配置文件路径' for camera:" << it.key();
            }
            // 将解析好的 Camerinfo 对象添加到 QVector 中
            caminfo.append(currentCamera);
            qDebug() << "Parsed Camera Info for" << it.key()
                     << ": Name=" << currentCamera.name
                     << ", IP=" << currentCamera.ip
                     << ", Port=" << currentCamera.port
                     << ", SN=" << currentCamera.SN
                     <<",path="<<currentCamera.path;
        } else {
            qWarning() << "Top-level key '" << it.key() << "' is not a map. Skipping.";
        }
    }

    qDebug() << "Camera configuration parsing complete.";
}



void MainWindow::test()
{

   // QPixmap pixmap(":/images/resources/images/image.jpg");
    //cameraLabels[4]->displayimg(pixmap);

    QPixmap pixmap1(":/images/resources/images/test.jpg");
    cameraLabels[0]->displayimg(pixmap1);

    cams[2]->NY->printProcessedData();

    OutResParam testResult;
    testResult.phi = -15;
    testResult.posWidth = 5;
    testResult.posErr = 0.08f;
    testResult.negWidth = 7;
    testResult.negErr = 0.03f;
    testResult.textNum = 2;
    testResult.logoScores = 0.1f;
    testResult.textScores = {0.8f, 0.7f};
    //cams[2]->NY->judge(testResult);
    int judgmentResult1 = cams[2]->NY->judge(testResult);
    if (judgmentResult1 == 0) {
        qDebug() << "第一次判断结果：                   所有参数都在范围内 。";
    } else {
        qDebug() << "第一次判断结果：                    有参数不在范围内。";
    }
    // int width = 2600;
    // int height = 2160;
    // int channels = 3;
    // int imageDataLength = width * height * channels;  // 数据长度

    // // 为图片数据创建一个缓冲区，这里用随机数据来模拟图像数据
    // unsigned char* imageData = new unsigned char[imageDataLength];
    // // 填充图像数据（例如：将所有值设为 255 表示白色图像）
    // std::srand(std::time(0));

    // // 填充图像数据（生成彩色图像）
    // for (int i = 0; i < imageDataLength; i += 3) {
    //     imageData[i] = std::rand() % 256;       // R 通道（0-255）
    //     imageData[i + 1] = std::rand() % 256;   // G 通道（0-255）
    //     imageData[i + 2] = std::rand() % 256;   // B 通道（0-255）
    // }
    // // 创建 HImage 对象
    // HImage disimg(width, height, channels, imageDataLength, imageData);

    // delete[] imageData;

    // cameraLabels[0]->displayimg(disimg);
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
    init_log();
    loadjson_layer("../../../ini/globe/other.json");
    qDebug()<<"SystemPara::LOG_DIR"<<SystemPara::OK_DIR;
    loadjson_layer2("../../../ini/globe/cameral.json");
    qDebug()<<caminfo[0].name;
    initcams(caminfo.size());
    CreateMenu();
    CreateImageGrid(caminfo.size());
   test();

}

void MainWindow::init_log()
{
    GlobalLog::logger.Mz_GetInstance(L"app.log");

    // 写入日志
    LOG_DEBUG(GlobalLog::logger, L"Application started.");
      GlobalLog::logger.Mz_AddLog(L"hello");

    // 模拟日期检查（通常由定时器或每次写入时触发）
    GlobalLog::logger.Mz_CheckDateTime();

    // 释放资源（删除所有日志文件！）
    //    logger.Mz_Realse();

}
void MainWindow::initcams(int camnumber)
{
     QString rootpath = SystemPara::ROOT_DIR;
   for(int i=1;i<=camnumber;i++)
    {
         Cameral * cam=new Cameral(this);
       // cam->algopath = rootpath +"" QString::number(i) + "/algo.json";
       // cam->cameralpath = rootpath + QString::number(i) + "/cameral.json";
       // cam->rangepath = rootpath + QString::number(i) + "/range.json";
         cam->rangepath=caminfo[i-1].path+ "/range.json";
         cam->cameralpath=caminfo[i-1].path+"/cameral.json";
         cam->algopath=caminfo[i-1].path+ "/algo.json";

         QString dateStr = QDate::currentDate().toString("yyyyMMdd");
         cam->ok_path=SystemPara::ROOT_DIR+"/data/"+caminfo[i-1].name+"/OK/"+dateStr;

         qDebug()<<"  cam->ok_path:     "<<  cam->ok_path;

         if (!QDir(cam->ok_path).exists()) {
             QDir dir;
             if (dir.mkpath(cam->ok_path)) {
                 qDebug() << "路径创建成功:" << cam->ok_path;
             } else {
                 qDebug() << "路径创建失败:" << cam->ok_path;
             }
         } else {
             qDebug() << "路径已存在:" << cam->ok_path;
         }


         cam->ng_path=SystemPara::ROOT_DIR+"/data/"+caminfo[i-1].name+"/NG/"+dateStr;

         qDebug()<<"  cam->ng_path:     "<<  cam->ng_path;

         if (!QDir(cam->ng_path).exists()) {
             QDir dir;
             if (dir.mkpath(cam->ng_path)) {
                 qDebug() << "路径创建成功:" << cam->ng_path;
             } else {
                 qDebug() << "路径创建失败:" << cam->ng_path;
             }
         } else {
             qDebug() << "路径已存在:" << cam->ng_path;
         }



       cam->RC=new RangeClass(cam->rangepath,nullptr);
       cam->CC=new CameralClass(cam->cameralpath);
       cam->AC=new AlgoClass(cam->algopath);
       cam->NY=new RezultInfo_NaYin(&cam->RC->m_parameters,nullptr);


        cams.push_back(cam);

   }
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

    QMenu *menuStart = menuExecute->addMenu("运行全部检测");
    QMenu *menuStop = menuExecute->addMenu("停止全部检测");



  //  QMenu *menuBlock = menuTools->addMenu("屏蔽输出");
   // QMenu *menuLight = menuTools->addMenu("光源与IO");
    QAction*ActionSelfStart = menuTools->addAction("开机自启动");


    QAction* actionLogout = menuUser->addAction("注销");
    connect(actionLogout, &QAction::triggered, this, [roleLabel](){
        Role::ChangeRole("操作员");
        roleLabel->setText(Role::GetCurrentRole());
    });

    // QMenu *menuBlockPLC = menuBlock->addMenu("启动PLC链接");

    QAction *loginAction = menuUser->addAction("登录"); // 直接赋值给成员变量
    QAction *LogAction = menuRecord->addAction("日志");
    QAction *DataAction = menuRecord->addAction("数据");
    QAction *SystemParaAction = menuTools->addAction("系统参数");
    QAction *CameralSetAction = menuTools->addAction("相机配置");


    connect(CameralSetAction, &QAction::triggered, this, [=]() {
        AddCameraDialog dlg("../../../ini/globe/cameral.json", this);
        dlg.exec();  // 使用 exec() 以对话框方式打开
    });


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
                QString logFolderPath = SystemPara::LOG_DIR;

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
                QString logFolderPath = SystemPara::DATA_DIR;

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
    qDebug()<<"Cams.size():  "<<cams.size();
    if (cams.size() < i) {
        qWarning() << "Cameral::Cams 中数据不足，无法初始化 " << i << " 个相机框！";
        return;
    }

    for (int idx = 0; idx < i && idx < cams.size(); ++idx) {
        CameraLabelWidget* cameraLabel = new CameraLabelWidget(
            cams[idx],       // 使用 idx 而非 i 访问 cams
            idx + 1,         // 编号从1开始
            caminfo[idx].name,
            window
            );
        cameraLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        cameraLabels.append(cameraLabel);
        gridLayout->addWidget(cameraLabel, idx / cols, idx % cols);
    }
    // 添加第八个固定框
    QWidget* emptyWidget = CreateEighthFrame(i);
    emptyWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    gridLayout->addWidget(emptyWidget, i / cols, i % cols);

    // 根据 i 动态调整窗口大小
    int frameWidth = 200;  // 每个框的基准宽度
    int frameHeight = 150; // 每个框的基准高度（
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

void MainWindow::CreateImageGrid(int camnumber)
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
    SetupCameraGridLayout(camnumber, gridLayout, cameraLabels, centralWidget);

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
    if(i==1) return QString("正引脚");
    if(i==2) return QString("座板");
    if(i==3) return QString("负引脚");
    if(i==4) return QString("捺印");
    if(i==5) return QString("载带");
    if(i==6) return QString("H部");
    if(i==7) return QString("载带座板");
    return QString("");
}


QWidget* MainWindow::CreateEighthFrame(int camnumber)
{
    // 创建容器控件
    QWidget *container = new QWidget(this);
    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    container->setStyleSheet("background-color: white; border: 1px solid black;");

    // 主布局：垂直布局
    QVBoxLayout *mainLayout = new QVBoxLayout(container);
    mainLayout->setContentsMargins(5, 5, 5, 5);
    mainLayout->setSpacing(5);

    // 清空之前可能遗留的 Label 指针，防止内存泄漏和错误引用
    dataLabels.clear();

    // 计算总行数：1行标题 + camnumber行相机数据 + 1行总和 + 1行按钮
    // 原代码是10行，其中 8行是数据行，1行总和，1行按钮
    // 现在是 1行标题 + camnumber 行 + 1行总和 + 1行按钮
    int totalRows = 1 + camnumber + 1 + 1; // 标题行 + 相机数据行 + "总和"行 + 按钮行

    for (int row = 0; row < totalRows; ++row) {
        QHBoxLayout *rowLayout = new QHBoxLayout();
        rowLayout->setSpacing(3);

        // 每行创建的 QLabel 存储在这个临时向量中，用于后续添加到 dataLabels
        QVector<QLabel*> currentRowLabels;

        for (int col = 0; col < 4; ++col) {
            QString text;
            QLabel *label;
            QFont font;

            // 设置通用样式和字体
            font.setPointSize(18); // 稍微调小字体，以便在表格中容纳更多内容
            label = new QLabel("", container); // 先创建空标签，后续设置文本

            label->setAlignment(Qt::AlignCenter);
            label->setFont(font);
            label->setStyleSheet("background-color: white; border: 1px solid gray;");
            label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

            // 第一行 (row == 0) - 标题行
            if (row == 0) {
                if (col == 0) text = "相机";
                else if (col == 1) text = "总数";
                else if (col == 2) text = "NG";
                else if (col == 3) text = "良率";
                label->setFont(QFont("微软雅黑", 20, QFont::Bold)); // 标题加粗，字体大一点
                label->setStyleSheet("background-color: lightgray; border: 1px solid gray;"); // 标题背景色
            }
            // 动态创建相机数据行
            else if (row > 0 && row <= camnumber) {
                int cameraIndex = row - 1; // 对应 caminfo 数组的索引
                if (cameraIndex < caminfo.size()) { // 确保索引不越界
                    if (col == 0) {
                        text = caminfo[cameraIndex].name; // 相机名称
                        label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter); // 名称左对齐
                        label->setContentsMargins(10, 0, 0, 0); // 左侧留白
                        label->setFont(QFont("微软雅黑", 16, QFont::DemiBold)); // 相机名称字体
                    } else {
                        text = "0"; // 总数, NG, 良率 初始为 0
                    }
                } else {
                    // 如果 caminfo 数量不足 camnumber，则显示空或默认值
                    if (col == 0) text = QString("未知相机%1").arg(cameraIndex + 1);
                    else text = "0";
                    qWarning() << "caminfo does not have enough entries for camnumber:" << camnumber;
                }
                // 存储数据标签的指针，以便后续更新（仅限数据列，不包含相机名称列）
                if (col > 0) {
                    currentRowLabels.append(label);
                }
            }
            // "总和" 行 (在所有相机数据行之后)
            else if (row == camnumber + 1) { // 标题行 (0) + camnumber行数据 + 1行
                if (col == 0) { // 只有最后一列是“总和”
                    text = "总和";
                    label->setFont(QFont("微软雅黑", 18, QFont::Bold));
                    label->setStyleSheet("background-color: #E0E0E0; border: 1px solid gray;"); // 略深背景
                } else {
                    text = ""; // 其他列为空
                    label->setStyleSheet("background-color: #E0E0E0; border: 1px solid gray;");
                }
            }
            // 按钮行 (最后一行)
            else if (row == camnumber + 2) { // 标题行 (0) + camnumber行数据 + "总和"行 (1) + 1行
                // 在按钮行，你可能想用 QPushButton 而不是 QLabel
                // 这里为了保持 QLabel 的统一性，暂时用 QLabel，但建议后续替换为 QPushButton
                if (col == 0) { text = "触发"; label->setStyleSheet("background-color: lightblue; border: 1px solid gray;"); }
                else if (col == 1) { text = "启动"; label->setStyleSheet("background-color: lightgreen; border: 1px solid gray;"); }
                else if (col == 2) { text = "停止"; label->setStyleSheet("background-color: lightcoral; border: 1px solid gray;"); }
                else if (col == 3) { text = "一键清理"; label->setStyleSheet("background-color: lightyellow; border: 1px solid gray;"); }
                label->setFont(QFont("微软雅黑", 18, QFont::Bold));
            }

            label->setText(text); // 设置最终文本
            rowLayout->addWidget(label);
        }
        mainLayout->addLayout(rowLayout);

        // 如果是数据行，将当前行的 QLabel 指针保存到 dataLabels
        if (row > 0 && row <= camnumber) {
            dataLabels.append(currentRowLabels);
        }
    }

    // 如果它用于动态更新标签，你需要根据新的结构调整它。
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

