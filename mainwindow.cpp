#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "cameralmenu.h"
#include "cameral.h"
#include "login.h"
#include "public.h"

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

}

MainWindow::~MainWindow()
{
    delete ui;
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

    QMenu *menuLog = menuRecord->addMenu("日志");
    QMenu *menuData = menuRecord->addMenu("数据");

    QMenu *menuBlock = menuTools->addMenu("屏蔽输出");
    QMenu *menuLight = menuTools->addMenu("光源与IO");
    QMenu *menuSelfStart = menuTools->addMenu("开机自启动");
    QMenu *menuSystemPara = menuTools->addMenu("系统参数");
    QMenu *menuSModeCancel = menuTools->addMenu("SModeCancel");
    QMenu *menuSModeAutostart = menuTools->addMenu("SMode_Autostart");
    QMenu *menuForceMode = menuTools->addMenu("ForceMode");

    loginAction = menuUser->addAction("登录"); // 直接赋值给成员变量

    QMenu *menuLogout = menuUser->addMenu("注销");

    QMenu *menuBlockCameral1 = menuBlock->addMenu("屏蔽正引脚输出");
    QMenu *menuBlockCameral2 = menuBlock->addMenu("屏蔽座板输出");
    QMenu *menuBlockCameral3 = menuBlock->addMenu("屏蔽负引脚输出");
    QMenu *menuBlockCameral4 = menuBlock->addMenu("屏蔽捺印输出");
    QMenu *menuBlockCameral5 = menuBlock->addMenu("屏蔽载带输出");
    QMenu *menuBlockCameral6 = menuBlock->addMenu("屏蔽H部输出");
    QMenu *menuBlockCameral7 = menuBlock->addMenu("屏蔽载带座板输出");
    QMenu *menuBlockPLC = menuBlock->addMenu("启动PLC链接");

    // 连接信号并检查连接状态
    bool connected = connect(loginAction, &QAction::triggered, this, [this]() {
        qDebug() << "登录动作触发";
        Login loginDialog(nullptr);
        if (loginDialog.exec() == QDialog::Accepted) {
            QString password = loginDialog.GetPassword();
            qDebug() << "输入的密码:" << password;
            if (!password.isEmpty()) {
                loginAction->setEnabled(false); // 禁用登录项
                show(); // 显示主窗口
            } else {
                qDebug() << "密码为空，登录失败";
            }
        } else {
            qDebug() << "登录取消";
        }
    });
    qDebug() << "信号连接状态:" << connected; // 调试：确认连接是否成功
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

    // 创建 8 个框
    for (int i = 0; i < 8; ++i) {
        if (i < 7) {
            // 前 7 个框：调用创建函数，传入索引和固定文字
            QWidget *cameraLabel = CreateCameraLabel(i, CameralName(i));
            cameraLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding); // 确保可扩展
            cameraLabels.append(cameraLabel);
            gridLayout->addWidget(cameraLabel, i / 4, i % 4); // 2 行 4 列布局
        } else {
            // 第 8 个框：创建并初始化空框
            QWidget *emptyWidget = CreateEighthFrame();
            emptyWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding); // 确保可扩展
            gridLayout->addWidget(emptyWidget, i / 4, i % 4);
        }
    }

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
    cameraMenu->addMenuOption("运行", [i]() { qDebug() << "摄像头" << i + 1 << "的选项 运行 被点击"; });
    cameraMenu->addMenuOption("停止", [i]() { qDebug() << "摄像头" << i + 1 << "的选项 停止 被点击"; });
    cameraMenu->addMenuOption("OK", [i]() { qDebug() << "摄像头" << i + 1 << "的选项 OK 被点击"; });
    cameraMenu->addMenuOption("NG", [i]() { qDebug() << "摄像头" << i + 1 << "的选项 NG 被点击"; });
    cameraMenu->addMenuOption("错误", [i]() { qDebug() << "摄像头" << i + 1 << "的选项 错误 被点击"; });
    cameraMenu->addMenuOption("参数", [i]() { qDebug() << "摄像头" << i + 1 << "的选项 参数 被点击"; });
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
