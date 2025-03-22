#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QScreen"
#include "QString"
#include "QMenu"
#include "QMenuBar"
#include "QAction"
#include "QLabel"
#include "cameralmenu.h"
#include "QGridLayout"
#include "QPushButton"
#include "cameral.h"
#include "QList"
#include "QDebug"
#include "QLabel"
#include "QFont"

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
    // 获取菜单栏
    QMenuBar *menuBar = this->menuBar();


    // 创建 "执行" 菜单
    QMenu *menuExecute = menuBar->addMenu("执行");
    QMenu *menuRecord = menuBar->addMenu("记录");
    QMenu *menuTool = menuBar->addMenu("工具");
    QMenu *menuUser = menuBar->addMenu("用户");

    // 添加子菜单 "开始"
    QMenu *menuStart = menuExecute->addMenu("运行全部检测");
    QMenu *menuStop = menuExecute->addMenu("停止全部检测");

    QMenu *menuLog = menuRecord->addMenu("日志");
    QMenu *menuData = menuRecord->addMenu("数据");

    QMenu *menuBlock = menuTool->addMenu("屏蔽输出");
    QMenu *menuLight = menuTool->addMenu("光源与IO");
    QMenu *menuSelfStart = menuTool->addMenu("开机自启动");
    QMenu *menuSystemPara = menuTool->addMenu("系统参数");
    QMenu *menuSModeCancel = menuTool->addMenu("SModeCancel");
    QMenu *menuSModeAutostart = menuTool->addMenu("SMode_Autostart");
    QMenu *menuForceMode = menuTool->addMenu("ForceMode");

    QMenu *menuLogin = menuUser->addMenu("登录");
    QMenu *Logout= menuUser->addMenu("注销");

    QMenu *menuBlockCameral1 = menuBlock->addMenu("屏蔽正引脚输出");
    QMenu *menuBlockCameral2 = menuBlock->addMenu("屏蔽座板输出");
    QMenu *menuBlockCameral3 = menuBlock->addMenu("屏蔽负引脚输出");
    QMenu *menuBlockCameral4 = menuBlock->addMenu("屏蔽捺印输出");
    QMenu *menuBlockCameral5 = menuBlock->addMenu("屏蔽载带输出");
    QMenu *menuBlockCameral6 = menuBlock->addMenu("屏蔽H部输出");
    QMenu *menuBlockCameral7 = menuBlock->addMenu("屏蔽载带座板输出");
    QMenu *menuBlockPLC = menuBlock->addMenu("启动PLC链接");

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
    fixedText->setStyleSheet("background-color: transparent; color: black; font-size: 20px; border: none;");
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

    // 保证箭头按钮背景不变
    cameraMenu->getMenuButton()->setStyleSheet(
        "background-color: transparent; "
        "font-size: 15px; "
        "padding: 2px 5px;"  // 调整内边距，使按钮更紧凑
        "border: 1px solid black;"  // 设置边框，避免外框过大
    );
    // 设置按钮最小尺寸，避免按钮变成点
    cameraMenu->getMenuButton()->setMinimumSize(50, 40);  // 设置最小尺寸

    // 修改菜单项的背景颜色，修复悬停时透明的问题
    cameraMenu->setStyleSheet(
        "QMenu { background-color: white; border: 1px solid black; }"
        "QMenu::item { background-color: lightgray; color: black; }"
        "QMenu::item:hover { background-color: lightblue; }"
    );

    // 创建覆盖层布局，包含固定文本和菜单按钮
    QHBoxLayout *overlayLayout = new QHBoxLayout();
    overlayLayout->addStretch();
    overlayLayout->addWidget(fixedText);
    overlayLayout->addWidget(cameraMenu->getMenuButton());
    overlayLayout->setSpacing(2);  // 增加间隔
    overlayLayout->setContentsMargins(0, 0, 2, 2);  // 增加右边和底部的边距

    QWidget *overlayWidget = new QWidget(container);
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
