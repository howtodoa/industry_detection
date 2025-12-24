#include "syspara.h"
#include <QtConcurrent/QtConcurrentRun>
#include <QFuture>
#include "fileoperator.h"

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
    updateSystemDirsToJsonAsync(SystemPara::GLOBAL_DIR);
    QMessageBox::information(this, "提示", "参数已修改");
    close(); // 关闭窗口
}

QFuture<bool> SysPara::updateSystemDirsToJsonAsync(const QString& filePath)
{
    // 在后台线程中执行操作
    QFuture<bool> future = QtConcurrent::run([filePath]() {
        qDebug() << "Background thread: Executing SystemPara directory sync...";

        const QString newDataDirValue = SystemPara::DATA_DIR;
        const QString newLogDirValue = SystemPara::LOG_DIR;

        // 读取 JSON 文件
        QVariantMap dataMap = FileOperator::readJsonMap(filePath);
        if (dataMap.isEmpty()) {
            qWarning() << "Background thread: Failed to read or parse JSON file from" << filePath;
            return false;
        }


        // 更新 DATA_DIR
        if (dataMap.contains("DATA_DIR")) {
            QVariantMap dataDirMap = dataMap.value("DATA_DIR").toMap();
            dataDirMap["值"] = newDataDirValue;
            dataMap["DATA_DIR"] = dataDirMap;
            qDebug() << "Updated DATA_DIR to:" << newDataDirValue;
        }
        else {
            qWarning() << "Background thread: JSON structure missing 'DATA_DIR' key.";
        }

        // 更新 LOG_DIR
        if (dataMap.contains("LOG_DIR")) {
            QVariantMap logDirMap = dataMap.value("LOG_DIR").toMap();
            logDirMap["值"] = newLogDirValue;
            dataMap["LOG_DIR"] = logDirMap;
            qDebug() << "Updated LOG_DIR to:" << newLogDirValue;
        }
        else {
            qWarning() << "Background thread: JSON structure missing 'LOG_DIR' key.";
        }

        // 写入更新后的 QVariantMap 到 JSON 文件
        if (FileOperator::writeJsonMap(filePath, dataMap)) {
            qDebug() << "Background thread: System parameters saved successfully to" << filePath;
            return true;
        }
        else {
            qWarning() << "Background thread: Failed to save system parameters to" << filePath;
            return false;
        }
        });

    qDebug() << "Main thread: Async System directory sync initiated.";
    return future;
}

QFuture<bool> SysPara::updateSystemParamToJsonAsync(
    const QString& filePath,
    const QString& key,
    const QString& newValue)
{
    return QtConcurrent::run([filePath, key, newValue]() {

        qDebug() << "Background thread: Updating"
            << key << "->" << newValue;

        QVariantMap dataMap = FileOperator::readJsonMap(filePath);
        if (dataMap.isEmpty()) {
            qWarning() << "Failed to read JSON:" << filePath;
            return false;
        }

        if (!dataMap.contains(key)) {
            qWarning() << "JSON missing key:" << key;
            return false;
        }

        QVariantMap paramMap = dataMap.value(key).toMap();
        paramMap["值"] = newValue;
        dataMap[key] = paramMap;

        if (!FileOperator::writeJsonMap(filePath, dataMap)) {
            qWarning() << "Failed to write JSON:" << filePath;
            return false;
        }

        return true;
        });
}


void SysPara::onCancelClicked()
{
    close();
}
