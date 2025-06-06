#include "parawidget.h"
#include "role.h"
#include <QVBoxLayout>
#include <QTabWidget>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>


ParaWidget:: ParaWidget( const QString&  rangepath,  const QString& cameralpath,  const QString& algopath, QWidget *parent)
    : QWidget{parent}
{
    // 设置窗口属性
    setWindowFlags(Qt::Window);
    setWindowTitle("参数设置");
    setAttribute(Qt::WA_DeleteOnClose);
    resize(800, 600);

    // 设置全局字体（放大一倍）
    QFont font = this->font();
    font.setPointSize(font.pointSize() * 2);
    setFont(font);

    // 创建主布局
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // 创建选项卡控件
    QTabWidget* tabWidget = new QTabWidget;
    mainLayout->addWidget(tabWidget);

    this->rangePath=rangepath;
    this->algoPath=algopath;
    this->algoPath=algopath;

    QVariantMap rangeParams=FileOperator::readJsonMap(rangepath);
    QVariantMap cameralParams=FileOperator::readJsonMap(cameralpath);
    QVariantMap algoParams=FileOperator::readJsonMap(algopath);

    // 设置各个选项卡
    setupRangeTab(tabWidget, rangeParams);    // 第一个选项卡，保持可见
    setupCameralTab(tabWidget, cameralParams); // 第二个选项卡
    setupAlgorithmTab(tabWidget, algoParams);  // 第三个选项卡
    if (Role::GetCurrentRole() != "厂商" && Role::GetCurrentRole() != "机修")
    {
        // 隐藏第二个和第三个选项卡的标题
        tabWidget->setTabVisible(1, false); // 隐藏 CameralTab 的标题
        tabWidget->setTabVisible(2, false); // 隐藏 AlgorithmTab 的标题
    }

    // 设置主布局
    setLayout(mainLayout);
}


ParaWidget::~ParaWidget()
{

}


void ParaWidget::setupRangeTab(QTabWidget* tabWidget, const QVariantMap& rangeParams)
{
    QWidget* rangeTab = new QWidget;
    tabWidget->addTab(rangeTab, "范围参数");

    QVBoxLayout* tabLayout = new QVBoxLayout(rangeTab);
    QGridLayout* gridLayout = new QGridLayout;

    int row = 0;

    if (rangeParams.contains("范围参数") && rangeParams["范围参数"].type() == QVariant::Map) {
        const QVariantMap actualRangeParams = rangeParams["范围参数"].toMap();

        for (auto it = actualRangeParams.begin(); it != actualRangeParams.end(); ++it, ++row) {
            const QString& paramKey = it.key();
            const QVariant& paramValue = it.value();

            if (paramValue.type() == QVariant::Map) {
                const QVariantMap paramDetails = paramValue.toMap();
                QString valueStr = paramDetails.value("值").toString();
                QString unitStr = paramDetails.value("单位").toString();

                QLabel* label = new QLabel(paramKey, this);
                QLineEdit* edit = new QLineEdit(valueStr, this);
                QLabel* unitLabel = new QLabel(unitStr, this);

                // 控制 QLineEdit 宽度，使整体布局紧凑
                edit->setFixedWidth(80);
                unitLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

                // 初始化并监听修改
                rangeModifiedMap_[paramKey] = edit->text();
                connect(edit, &QLineEdit::textChanged, this, [=]() {
                    rangeModifiedMap_[paramKey] = edit->text();
                });

                // 添加控件到布局
                gridLayout->addWidget(label, row, 0, Qt::AlignRight);
                gridLayout->addWidget(edit, row, 1);
                gridLayout->addWidget(unitLabel, row, 2);
            }
        }
    } else {
        QLabel* errorLabel = new QLabel("配置文件格式错误或为空。", this);
        gridLayout->addWidget(errorLabel, 0, 0, 1, 3);
    }

    tabLayout->addLayout(gridLayout);

    // 底部按钮
    QHBoxLayout* buttonLayout = new QHBoxLayout;
    QPushButton* saveButton = new QPushButton("保存修改", this);
    QPushButton* exitButton = new QPushButton("退出", this);
    buttonLayout->addStretch();
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(exitButton);
    buttonLayout->addStretch();
    tabLayout->addLayout(buttonLayout);

    // 保存逻辑
    connect(saveButton, &QPushButton::clicked, this, [this, rangeParams]() {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "保存确认", "确定要保存当前修改吗？",
                                      QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            QVariantMap dataToSaveActual;

            for (auto it = rangeModifiedMap_.begin(); it != rangeModifiedMap_.end(); ++it) {
                QString paramKey = it.key();
                QString modifiedValue = it.value().toString();

                QString originalUnit = "";
                if (rangeParams.contains("范围参数") && rangeParams["范围参数"].type() == QVariant::Map) {
                    const QVariantMap actualOriginalParams = rangeParams["范围参数"].toMap();
                    if (actualOriginalParams.contains(paramKey) && actualOriginalParams[paramKey].type() == QVariant::Map) {
                        originalUnit = actualOriginalParams[paramKey].toMap().value("单位").toString();
                    }
                }

                QVariantMap paramEntry;
                paramEntry["值"] = modifiedValue;
                paramEntry["单位"] = originalUnit;

                dataToSaveActual[paramKey] = paramEntry;
            }

            QVariantMap finalDataToSave;
            finalDataToSave["范围参数"] = dataToSaveActual;

            FileOperator::writeJsonMap(rangePath, finalDataToSave);
            QMessageBox::information(this, "保存成功", "范围参数已成功保存！");
        }
    });

    // 退出
    connect(exitButton, &QPushButton::clicked, this, &ParaWidget::closeWindow);
}




void ParaWidget::setupCameralTab(QTabWidget* tabWidget, const QVariantMap& cameralParams)
{
    QWidget* cameralTab = new QWidget;
    tabWidget->addTab(cameralTab, "相机参数");

    QVBoxLayout* tabLayout = new QVBoxLayout(cameralTab);
    QGridLayout* gridLayout = new QGridLayout;

    int row = 0;

    for (auto it = cameralParams.begin(); it != cameralParams.end(); ++it) {
        const QString& sectionKey = it.key(); // 例如 "分辨率", "帧率"
        const QVariant& sectionValue = it.value();

        QLabel* sectionLabel = new QLabel(QString("<b>%1</b>").arg(sectionKey), this);
        sectionLabel->setTextFormat(Qt::RichText);
        gridLayout->addWidget(sectionLabel, row, 0, 1, 3);
        row++;

        const QVariantMap subParams = sectionValue.toMap();
        QString currentUnit = subParams.value("单位").toString(); // 提取单位

        for (auto subIt = subParams.begin(); subIt != subParams.end(); ++subIt, ++row) {
            const QString& paramKey = subIt.key(); // 例如 "宽度", "高度", "值"
            const QVariant& paramValue = subIt.value();

            if (paramKey == "单位") {
                row--; // 调整行数
                continue;
            }

            QString fullParamKey = QString("%1.%2").arg(sectionKey, paramKey);

            QLabel* label = new QLabel(paramKey, this);
            QLineEdit* edit = new QLineEdit(paramValue.toString(), this);
            QLabel* unitLabel = new QLabel(currentUnit, this);

            connect(edit, &QLineEdit::textChanged, this, [=]() {
                cameralModifiedMap_[fullParamKey] = edit->text();
                // emit parametersChanged(cameralModifiedMap_); // 暂时注释此功能
            });

            gridLayout->addWidget(label, row, 0);
            gridLayout->addWidget(edit, row, 1);
            gridLayout->addWidget(unitLabel, row, 2);
        }
    }

    tabLayout->addLayout(gridLayout);

    QHBoxLayout* buttonLayout = new QHBoxLayout;
    QPushButton* saveButton = new QPushButton("保存修改", this);
    QPushButton* exitButton = new QPushButton("退出", this);
    buttonLayout->addStretch();
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(exitButton);
    buttonLayout->addStretch();
    tabLayout->addLayout(buttonLayout);

    connect(saveButton, &QPushButton::clicked, this, [this, cameralParams]() {
        // ** 添加确认对话框 **
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "保存确认", "确定要保存当前修改吗？",
                                      QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::Yes) {
            QVariantMap dataToSave;
            for (const QString& fullKey : cameralModifiedMap_.keys()) {
                QStringList parts = fullKey.split('.');
                if (parts.size() == 2) {
                    QString section = parts[0];
                    QString param = parts[1];
                    if (!dataToSave.contains(section)) {
                        dataToSave[section] = QVariantMap();
                    }
                    QVariantMap currentSectionMap = dataToSave[section].toMap();
                    currentSectionMap[param] = cameralModifiedMap_[fullKey];
                    dataToSave[section] = currentSectionMap;
                }
            }
            // 从原始参数中复制单位
            for (auto it = cameralParams.begin(); it != cameralParams.end(); ++it) {
                const QString& sectionKey = it.key();
                const QVariantMap originalSectionMap = it.value().toMap();
                if (originalSectionMap.contains("单位")) {
                    QVariantMap currentDataToSaveSection = dataToSave[sectionKey].toMap();
                    currentDataToSaveSection["单位"] = originalSectionMap["单位"];
                    dataToSave[sectionKey] = currentDataToSaveSection;
                }
            }

            FileOperator::writeJsonMap(cameralPath, dataToSave);
            QMessageBox::information(this, "保存成功", "相机参数已成功保存！");
        }
    });

    connect(exitButton, &QPushButton::clicked, this, &ParaWidget::closeWindow);
}



void ParaWidget::setupAlgorithmTab(QTabWidget* tabWidget, const QVariantMap& algoParams)
{
    QWidget* algorithmTab = new QWidget;
    tabWidget->addTab(algorithmTab, "算法参数");

    QVBoxLayout* tabLayout = new QVBoxLayout(algorithmTab);
    QGridLayout* gridLayout = new QGridLayout;

    int row = 0;

    for (auto it = algoParams.begin(); it != algoParams.end(); ++it) {
        const QString& sectionKey = it.key(); // 例如 "阈值", "检测区域"
        const QVariant& sectionValue = it.value();

        QLabel* sectionLabel = new QLabel(QString("<b>%1</b>").arg(sectionKey), this);
        sectionLabel->setTextFormat(Qt::RichText);
        gridLayout->addWidget(sectionLabel, row, 0, 1, 3);
        row++;

        const QVariantMap subParams = sectionValue.toMap();
        QString currentUnit = subParams.value("单位").toString(); // 提取单位

        for (auto subIt = subParams.begin(); subIt != subParams.end(); ++subIt, ++row) {
            const QString& paramKey = subIt.key(); // 例如 "最小", "最大", "值", "X", "Y"
            const QVariant& paramValue = subIt.value();

            if (paramKey == "单位") {
                row--; // 调整行数
                continue;
            }

            QString fullParamKey = QString("%1.%2").arg(sectionKey, paramKey);

            QLabel* label = new QLabel(paramKey, this);
            QLineEdit* edit = new QLineEdit(paramValue.toString(), this);
            QLabel* unitLabel = new QLabel(currentUnit, this);

            connect(edit, &QLineEdit::textChanged, this, [=]() {
                algoModifiedMap_[fullParamKey] = edit->text();
                // emit parametersChanged(algoModifiedMap_); // 暂时注释此功能
            });

            gridLayout->addWidget(label, row, 0);
            gridLayout->addWidget(edit, row, 1);
            gridLayout->addWidget(unitLabel, row, 2);
        }
    }

    tabLayout->addLayout(gridLayout);

    QHBoxLayout* buttonLayout = new QHBoxLayout;
    QPushButton* saveButton = new QPushButton("保存修改", this);
    QPushButton* exitButton = new QPushButton("退出", this);
    buttonLayout->addStretch();
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(exitButton);
    buttonLayout->addStretch();
    tabLayout->addLayout(buttonLayout);

    connect(saveButton, &QPushButton::clicked, this, [this, algoParams]() {
        // ** 添加确认对话框 **
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "保存确认", "确定要保存当前修改吗？",
                                      QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::Yes) {
            QVariantMap dataToSave;
            for (const QString& fullKey : algoModifiedMap_.keys()) {
                QStringList parts = fullKey.split('.');
                if (parts.size() == 2) {
                    QString section = parts[0];
                    QString param = parts[1];
                    if (!dataToSave.contains(section)) {
                        dataToSave[section] = QVariantMap();
                    }
                    QVariantMap currentSectionMap = dataToSave[section].toMap();
                    currentSectionMap[param] = algoModifiedMap_[fullKey];
                    dataToSave[section] = currentSectionMap;
                }
            }
            // 从原始参数中复制单位
            for (auto it = algoParams.begin(); it != algoParams.end(); ++it) {
                const QString& sectionKey = it.key();
                const QVariantMap originalSectionMap = it.value().toMap();
                if (originalSectionMap.contains("单位")) {
                    QVariantMap currentDataToSaveSection = dataToSave[sectionKey].toMap();
                    currentDataToSaveSection["单位"] = originalSectionMap["单位"];
                    dataToSave[sectionKey] = currentDataToSaveSection;
                }
            }

            FileOperator::writeJsonMap(algoPath, dataToSave);
            QMessageBox::information(this, "保存成功", "算法参数已成功保存！");
        }
    });

    connect(exitButton, &QPushButton::clicked, this, &ParaWidget::closeWindow);
}


void ParaWidget::closeWindow()
{
    close(); // 关闭窗口
}
