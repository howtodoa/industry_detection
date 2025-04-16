#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "cameralmenu.h"
#include "cameral.h"
#include "login.h"
#include "public.h"
#include "parawidget.h"
#include "syspara.h"
#include "MZ_ClientControl.h"

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

void MainWindow::SetupCameraGridLayout(int i, QGridLayout* gridLayout, QVector<CameraLabelWidget*>& cameraLabels, QWidget* window)
{
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
    QPixmap pixmap(":/images/resources/images/image.jpg");
    cameraLabels[0]->displayimg(pixmap);

    HImage hImage;
    hImage.imageHead.width = 100;
    hImage.imageHead.height = 100;
    hImage.imageHead.channels = 4; // RGBA
    int dataLength = hImage.imageHead.getdatelength();
    hImage.data = new char[dataLength];
    for (int y = 0; y < hImage.imageHead.height; ++y) {
        for (int x = 0; x < hImage.imageHead.width; ++x) {
            int index = (y * hImage.imageHead.width + x) * 4;
            bool isWhite = ((x / 10) % 2) == ((y / 10) % 2);
            hImage.data[index] = isWhite ? 255 : 0;     // R
            hImage.data[index + 1] = isWhite ? 255 : 0; // G
            hImage.data[index + 2] = isWhite ? 255 : 0; // B
            hImage.data[index + 3] = 255;               // A (opaque)
        }
    }

    cameraLabels[1]->displayimg(pixmap);
}
