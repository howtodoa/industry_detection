#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "cameralmenu.h"
#include "cameral.h"
#include "login.h"
#include "public.h"
#include "parawidget.h"
#include "syspara.h"
#include "imagedisplay.h"

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

void MainWindow::SetupCameraGridLayout(int i, QGridLayout* gridLayout, QVector<QWidget*>& cameraLabels, QWidget* window) {
    // 输入验证：i 范围 [0, 7]，gridLayout 和 window 非空
    if (!gridLayout || !window || i < 0 || i > 7) return;

    // 清空旧布局
    QLayoutItem* item;
    while ((item = gridLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
    cameraLabels.clear();

    // 固定 2 行 4 列布局
    int cols = 4;

    // 添加前 i 个相机框
    for (int idx = 0; idx < i; ++idx) {
        QWidget* cameraLabel = CreateCameraLabel(idx, CameralName(idx));
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
    int rows = (usedSlots + 3) / 4; // 向上取整，最大 2 行
    int usedCols = (usedSlots <= 4) ? usedSlots : 4; // 第一行最多 4 列

    // 计算窗口大小
    int windowWidth = usedCols * frameWidth + (usedCols - 1) * spacing + 2 * margin;
    int windowHeight = rows * frameHeight + (rows - 1) * spacing + 2 * margin;

    // 调整窗口大小
    window->resize(windowWidth, windowHeight);

}

void MainWindow::CreateImageGrid()
{
    // 创建中央控件
    QWidget *centralWidget = new QWidget(this);
    centralWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding); // 确保中央控件可扩展
    setCentralWidget(centralWidget);

    // 创建网格布局
    QGridLayout *gridLayout = new QGridLayout(centralWidget);
    gridLayout->setContentsMargins(0, 0, 0, 0); // 移除边距
    gridLayout->setSpacing(0); // 移除控件间距

    //根据i创建相机
   SetupCameraGridLayout(7, gridLayout, cameraLabels, this);

    // 设置行列拉伸因子，确保填满空间
    for (int row = 0; row < 2; ++row) {
        gridLayout->setRowStretch(row, 1); // 每行均分高度
    }
    for (int col = 0; col < 4; ++col) {
        gridLayout->setColumnStretch(col, 1); // 每列均分宽度
    }
}



QWidget* MainWindow::CreateCameraLabel(int i, const QString& fixedTextName)
{
    // 创建主容器并设置样式
    QWidget *container = new QWidget(this);
    container->setStyleSheet("background-color: lightgray; border: 1px solid gray;");

    // 创建显示图像的标签
    QLabel *imageLabel = new QLabel(container);
    imageLabel->setStyleSheet("background-color: lightgray;");
    imageLabel->setAlignment(Qt::AlignCenter);

    // 创建固定文本标签
    QLabel *fixedText = new QLabel(fixedTextName, container);
    fixedText->setStyleSheet("background-color: transparent; color: black; font-size: 20px;");
    fixedText->setAttribute(Qt::WA_TransparentForMouseEvents);
    fixedText->setAlignment(Qt::AlignCenter);

    // 创建 CameraMenu 并设置菜单选项
    CameraMenu *cameraMenu = new CameraMenu(container);

    // 初始化 Cameral 对象
    Cameral *cameral = new Cameral(container); // 以 container 作为父对象
    // 设置默认值（已在 Cameral 构造函数中初始化为 0，这里可根据需要调整）
    cameral->rangeParams = {10.0f, 5.0f, 12.0f, 20.0f, 5.0f, 3.0f, 15.0f, 8.0f, 18.0f, 25.0f, 10.0f, 8.0f};
    cameral->cameralParams = {"Camera" + QString::number(i + 1), "SN" + QString::number(i + 1),
                              "192.168.1." + QString::number(i + 1), 1, i + 1, 50, 100, 200};
    cameral->algoParams = {"Algorithm" + QString::number(i + 1), {"参数一", "参数二"}, {"100", "200"}};

    // 添加菜单选项
    cameraMenu->addMenuOption("运行", [i]() { qDebug() << "摄像头" << i + 1 << "的选项 运行 被点击"; });
    cameraMenu->addMenuOption("停止", [i]() { qDebug() << "摄像头" << i + 1 << "的选项 停止 被点击"; });
    cameraMenu->addMenuOption("OK", [i]() { qDebug() << "摄像头" << i + 1 << "的选项 OK 被点击"; });
    cameraMenu->addMenuOption("NG", [i]() { qDebug() << "摄像头" << i + 1 << "的选项 NG 被点击"; });
    cameraMenu->addMenuOption("错误", [i]() { qDebug() << "摄像头" << i + 1 << "的选项 错误 被点击"; });
    cameraMenu->addMenuOption("参数", [cameral]() {
        // 将 Cameral 的三个参数传入 ParaWidget
        ParaWidget* parawidget = new ParaWidget(cameral->rangeParams,
                                                cameral->cameralParams,
                                                cameral->algoParams);
        parawidget->show(); // 显示窗口
    });
    cameraMenu->addMenuOption("相机", [i]() { qDebug() << "摄像头" << i + 1 << "的选项 相机 被点击"; });
    cameraMenu->addMenuOption("全屏", [i]() { qDebug() << "摄像头" << i + 1 << "的选项 全屏 被点击"; });
    cameraMenu->addMenuOption("录像", [i]() { qDebug() << "摄像头" << i + 1 << "的选项 录像 被点击"; });

    // 设置菜单按钮（箭头）样式：无边框，与文本高度一致
    cameraMenu->getMenuButton()->setStyleSheet(
        "background-color: transparent; "
        "font-size: 20px; "  // 与 fixedText 字体大小一致
        "padding: 0px; "     // 移除内边距以对齐高度
        "border: none;"
        );
    cameraMenu->getMenuButton()->setFixedHeight(fixedText->sizeHint().height()); // 固定高度与文本一致
    cameraMenu->getMenuButton()->setMinimumWidth(40); // 设置最小宽度，避免太窄

    // 修改菜单项的背景颜色
    cameraMenu->setStyleSheet(
        "QMenu { background-color: white; border: 1px solid black; }"
        "QMenu::item { background-color: lightgray; color: black; }"
        "QMenu::item:hover { background-color: lightblue; }"
        );

    // 创建水平布局，保持箭头和固定文本并排
    QHBoxLayout *textButtonLayout = new QHBoxLayout();
    textButtonLayout->addWidget(fixedText);
    textButtonLayout->addWidget(cameraMenu->getMenuButton());
    textButtonLayout->setSpacing(2);

    // 创建覆盖层布局，将水平布局整体上移
    QVBoxLayout *overlayLayout = new QVBoxLayout();
    overlayLayout->addLayout(textButtonLayout);
    overlayLayout->addStretch();
    overlayLayout->setContentsMargins(0, 0, 2, 2);

    // 设置 overlayWidget 无边框
    QWidget *overlayWidget = new QWidget(container);
    overlayWidget->setStyleSheet("background-color: transparent; border: none;");
    overlayWidget->setLayout(overlayLayout);

    // 创建容器布局
    QGridLayout *containerLayout = new QGridLayout(container);
    containerLayout->setContentsMargins(0, 0, 0, 0);
    containerLayout->addWidget(imageLabel, 0, 0);
    containerLayout->addWidget(overlayWidget, 0, 0, Qt::AlignRight | Qt::AlignTop);


    return container;
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

void MainWindow::test()
{
    static ImageDisplay* display = nullptr;
    if (!display) {
        display = new ImageDisplay(cameraLabels[0]); // QWidget*
    }
    display->setImage(":/images/resources/images/image.jpg");  // 使用绝对路径或者资源路径
}
