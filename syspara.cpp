#include "syspara.h"

SysPara::SysPara(QWidget *parent) : QWidget(parent)
{
    qDebug() << "SysPara constructor called";

    // 设置窗口标题
    setWindowTitle("系统参数设置");

    // 设置窗口标志，确保有边框和标题栏
    setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);

    // 设置背景为白色，避免透明
    setStyleSheet("SysPara { background-color: white; }");

    // 创建控件
    dataDirEdit = new QLineEdit(SystemPara::DATA_DIR, this);
    logDirEdit = new QLineEdit(SystemPara::LOG_DIR, this);
    QPushButton *modifyButton = new QPushButton("修改", this);
    QPushButton *cancelButton = new QPushButton("取消", this);

    // 创建布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QHBoxLayout *dataLayout = new QHBoxLayout();
    QHBoxLayout *logLayout = new QHBoxLayout();
    QHBoxLayout *buttonLayout = new QHBoxLayout();

    // 数据目录行
    dataLayout->addWidget(new QLabel("Data Directory:", this));
    dataLayout->addWidget(dataDirEdit);

    // 日志目录行
    logLayout->addWidget(new QLabel("Log Directory:", this));
    logLayout->addWidget(logDirEdit);

    // 按钮行
    buttonLayout->addStretch(); // 左边填充空白
    buttonLayout->addWidget(modifyButton);
    buttonLayout->addWidget(cancelButton);

    // 主布局
    mainLayout->addLayout(dataLayout);
    mainLayout->addLayout(logLayout);
    mainLayout->addLayout(buttonLayout);

    // 设置窗口大小
    setFixedSize(500, 200); // 确保大小合适

    // 连接信号槽
    connect(modifyButton, &QPushButton::clicked, this, &SysPara::onModifyClicked);
    connect(cancelButton, &QPushButton::clicked, this, &SysPara::onCancelClicked);

    qDebug() << "SysPara constructor finished";
}


void SysPara::onModifyClicked()
{
    // 读取文本框的值并更新 SystemPara
    SystemPara::DATA_DIR = dataDirEdit->text();
    SystemPara::LOG_DIR = logDirEdit->text();

    QMessageBox::information(this, "提示", "参数已修改");
    close(); // 关闭窗口
}

void SysPara::onCancelClicked()
{
    close();
}
