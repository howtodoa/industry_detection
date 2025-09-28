#include "initsystem.h"
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QMap>
#include <QMessageBox>
#include <QObject>
#include "FileOperator.h"

InitSystem::InitSystem() {}

InitSystem::InitSystem(const QString& path)
{
    QVariantMap configMap = FileOperator::readJsonMap(path);

    // 如果已经初始化，直接返回
    if (configMap.value("IsInit").toMap().value("值").toString().toLower() == "true")
        return;

    QDialog dialog;
    dialog.setWindowTitle("系统初始化配置");
    QVBoxLayout* mainLayout = new QVBoxLayout(&dialog);

    QMap<QString, QComboBox*> comboBoxes;
    QLineEdit* runPointEdit = new QLineEdit();
    QLineEdit* deviceIdEdit = new QLineEdit();   // 新增：机台号输入框

    // -------- 添加下拉框项 --------
    auto addComboRow = [&](const QString& key, const QString& labelText, const QStringList& options) {
        QHBoxLayout* layout = new QHBoxLayout();
        QLabel* label = new QLabel(labelText);
        QComboBox* combo = new QComboBox();
        combo->addItems(options);
        QString current = configMap.value(key).toMap().value("值").toString().trimmed();
        int index = combo->findText(current);
        if (index >= 0) combo->setCurrentIndex(index);
        layout->addWidget(label);
        layout->addWidget(combo);
        mainLayout->addLayout(layout);
        comboBoxes[key] = combo;
        };

    addComboRow("CameraType", "相机类型", { "HIKVISION", "DHUA" });
    addComboRow("IoType", "IO 类型", { "VC3000", "VC3000H", "PCI1230" });
    addComboRow("Envirment", "运行环境", { "LocationEn", "IPCEn" });

    // -------- 点位输入框 --------
    {
        QHBoxLayout* layout = new QHBoxLayout();
        QLabel* label = new QLabel("运行点位");
        runPointEdit->setText(configMap.value("RunPoint").toMap().value("值").toString());
        layout->addWidget(label);
        layout->addWidget(runPointEdit);
        mainLayout->addLayout(layout);
    }

    // -------- 机台号输入框 --------
    {
        QHBoxLayout* layout = new QHBoxLayout();
        QLabel* label = new QLabel("机台号");
        deviceIdEdit->setText(configMap.value("DeviceId").toMap().value("值").toString());
        layout->addWidget(label);
        layout->addWidget(deviceIdEdit);
        mainLayout->addLayout(layout);
    }

    // -------- 保存按钮 --------
    QPushButton* saveBtn = new QPushButton("保存");
    QObject::connect(saveBtn, &QPushButton::clicked, [&]() {
        for (auto it = comboBoxes.begin(); it != comboBoxes.end(); ++it) {
            QVariantMap fieldMap = configMap.value(it.key()).toMap();
            fieldMap["值"] = it.value()->currentText();
            configMap[it.key()] = fieldMap;
        }

        // 保存 RunPoint
        QVariantMap runPointMap = configMap["RunPoint"].toMap();
        runPointMap["值"] = runPointEdit->text().trimmed();
        configMap["RunPoint"] = runPointMap;

        // 保存 DeviceId
        QVariantMap deviceIdMap = configMap["DeviceId"].toMap();
        deviceIdMap["值"] = deviceIdEdit->text().trimmed();
        configMap["DeviceId"] = deviceIdMap;

        // 设置 IsInit = true
        QVariantMap isInitMap = configMap["IsInit"].toMap();
        isInitMap["值"] = "true";
        configMap["IsInit"] = isInitMap;

        FileOperator::writeJsonMap(path, configMap);
        dialog.accept();
        });

    mainLayout->addWidget(saveBtn);
    dialog.exec();
}
