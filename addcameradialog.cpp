#include "addcameradialog.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QScrollBar>
#include <cmath>

AddCameraDialog::AddCameraDialog(const QString& configFilePath, QWidget* parent)
    : QDialog(parent), m_configFilePath(configFilePath)
{
    setWindowTitle("相机配置 - " + QFileInfo(configFilePath).fileName());
    setMinimumSize(800, 600);
    setupUI();
    loadCameras(); // 加载配置
}

AddCameraDialog::~AddCameraDialog()
{
}

void AddCameraDialog::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    scrollAreaContent = new QWidget();
    camerasLayout = new QVBoxLayout(scrollAreaContent);
    camerasLayout->setContentsMargins(10, 10, 10, 10);
    camerasLayout->setSpacing(15);
    scrollArea->setWidget(scrollAreaContent);

    mainLayout->addWidget(scrollArea);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    QPushButton* addNewCamButton = new QPushButton("新增相机", this);
    QPushButton* saveButton = new QPushButton("保存配置", this);
    QPushButton* closeButton = new QPushButton("关闭", this);

    buttonLayout->addWidget(addNewCamButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(closeButton);

    mainLayout->addLayout(buttonLayout);

    connect(addNewCamButton, &QPushButton::clicked, this, &AddCameraDialog::addNewCamera);
    connect(saveButton, &QPushButton::clicked, this, &AddCameraDialog::saveCameras);
    connect(closeButton, &QPushButton::clicked, this, &AddCameraDialog::close);
}

void AddCameraDialog::refreshCameraUI()
{
    QLayoutItem* item;
    while ((item = camerasLayout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }
    m_cameraWidgets.clear();

    if (m_cameras.isEmpty()) {
        QLabel* noCamLabel = new QLabel("当前没有相机配置，点击'新增相机'添加。", scrollAreaContent);
        noCamLabel->setAlignment(Qt::AlignCenter);
        camerasLayout->addWidget(noCamLabel);
        return;
    }

    for (int i = 0; i < m_cameras.size(); ++i)
    {
        const Camerinfo& info = m_cameras[i];

        QGroupBox* cameraGroupBox = new QGroupBox(QString("相机%1").arg(i + 1), scrollAreaContent);
        cameraGroupBox->setStyleSheet("QGroupBox { border: 1px solid gray; border-radius: 5px; margin-top: 1ex; }"
            "QGroupBox::title { subcontrol-origin: margin; subcontrol-position: top center; padding: 0 3px; }");
        QVBoxLayout* groupLayout = new QVBoxLayout(cameraGroupBox);
        groupLayout->setContentsMargins(10, 15, 10, 10);
        groupLayout->setSpacing(8);

        CameraInputWidgets widgets;
        widgets.cameraIndex = i;

        // 相机名称
        QHBoxLayout* nameLayout = new QHBoxLayout();
        nameLayout->addWidget(new QLabel("相机名称", cameraGroupBox));
        widgets.nameEdit = new QLineEdit(info.name, cameraGroupBox);
        nameLayout->addWidget(widgets.nameEdit);
        groupLayout->addLayout(nameLayout);

        // 相机IP
        QHBoxLayout* ipLayout = new QHBoxLayout();
        ipLayout->addWidget(new QLabel("相机IP", cameraGroupBox));
        widgets.ipEdit = new QLineEdit(info.ip, cameraGroupBox);
        ipLayout->addWidget(widgets.ipEdit);
        groupLayout->addLayout(ipLayout);

        // 配置路径
        QHBoxLayout* pathLayout = new QHBoxLayout();
        pathLayout->addWidget(new QLabel("配置文件路径", cameraGroupBox));
        widgets.pathEdit = new QLineEdit(info.path, cameraGroupBox);
        pathLayout->addWidget(widgets.pathEdit);
        groupLayout->addLayout(pathLayout);

        // 映射
        QHBoxLayout* mappingLayout = new QHBoxLayout();
        mappingLayout->addWidget(new QLabel("相机映射", cameraGroupBox));
        widgets.mappingEdit = new QLineEdit(info.mapping, cameraGroupBox);
        mappingLayout->addWidget(widgets.mappingEdit);
        groupLayout->addLayout(mappingLayout);

        // 触发点位
        QHBoxLayout* triggerPointLayout = new QHBoxLayout();
        triggerPointLayout->addWidget(new QLabel("输出点位", cameraGroupBox));
        widgets.triggerPointEdit = new QLineEdit(QString::number(info.pointNumber), cameraGroupBox);
        widgets.triggerPointEdit->setValidator(new QIntValidator(0, 999, widgets.triggerPointEdit));
        triggerPointLayout->addWidget(widgets.triggerPointEdit);
        groupLayout->addLayout(triggerPointLayout);

        // Route编号
        QHBoxLayout* routeLayout = new QHBoxLayout();
        routeLayout->addWidget(new QLabel("Route编号", cameraGroupBox));
        widgets.rounteEdit = new QLineEdit(QString::number(info.rounte), cameraGroupBox);
        widgets.rounteEdit->setValidator(new QIntValidator(0, 10000, widgets.rounteEdit));
        routeLayout->addWidget(widgets.rounteEdit);
        groupLayout->addLayout(routeLayout);

        // 检测
        QHBoxLayout* checkLayout = new QHBoxLayout();
        widgets.checkCheckBox = new QCheckBox("检测", cameraGroupBox);
        widgets.checkCheckBox->setChecked(info.check.toLower() == "true");
        checkLayout->addWidget(widgets.checkCheckBox);
        checkLayout->addStretch();
        groupLayout->addLayout(checkLayout);

        // 删除按钮
        widgets.deleteButton = new QPushButton("删除此相机", cameraGroupBox);
        widgets.deleteButton->setProperty("cameraIndex", i);
        connect(widgets.deleteButton, &QPushButton::clicked, this, &AddCameraDialog::deleteCamera);
        groupLayout->addWidget(widgets.deleteButton, 0, Qt::AlignRight);

        // 绑定槽
        connect(widgets.nameEdit, &QLineEdit::textChanged, this, &AddCameraDialog::onInputChanged);
        connect(widgets.ipEdit, &QLineEdit::textChanged, this, &AddCameraDialog::onInputChanged);
        connect(widgets.pathEdit, &QLineEdit::textChanged, this, &AddCameraDialog::onInputChanged);
        connect(widgets.mappingEdit, &QLineEdit::textChanged, this, &AddCameraDialog::onInputChanged);
        connect(widgets.triggerPointEdit, &QLineEdit::textChanged, this, &AddCameraDialog::onInputChanged);
        connect(widgets.rounteEdit, &QLineEdit::textChanged, this, &AddCameraDialog::onInputChanged);
        connect(widgets.checkCheckBox, &QCheckBox::stateChanged, this, &AddCameraDialog::onInputChanged);

        m_cameraWidgets.append(widgets);
        camerasLayout->addWidget(cameraGroupBox);
    }

    camerasLayout->addStretch();
}

void AddCameraDialog::updateDataFromUI()
{
    for (const CameraInputWidgets& widgets : m_cameraWidgets) {
        int index = widgets.cameraIndex;
        if (index >= 0 && index < m_cameras.size()) {
            Camerinfo& info = m_cameras[index];

            info.name = widgets.nameEdit->text();
            info.ip = widgets.ipEdit->text();
            info.path = widgets.pathEdit->text();
            info.check = widgets.checkCheckBox->isChecked() ? "true" : "false";
            info.mapping = widgets.mappingEdit->text();
            info.pointNumber = widgets.triggerPointEdit->text().toInt();
            info.rounte = widgets.rounteEdit->text().toInt();
        }
    }
}

void AddCameraDialog::onInputChanged()
{
    updateDataFromUI();
}

void AddCameraDialog::loadCameras()
{
    m_cameras.clear();

    QVariantMap allCamerasMap = FileOperator::readJsonMap(m_configFilePath);

    auto getValue = [](const QVariantMap& cameraData, const QString& key) -> QVariant {
        if (cameraData.contains(key) && cameraData.value(key).type() == QVariant::Map) {
            return cameraData.value(key).toMap().value("值");
        }
        return QVariant();
        };

    for (auto it = allCamerasMap.begin(); it != allCamerasMap.end(); ++it) {
        if (it.value().type() == QVariant::Map) {
            QVariantMap cameraData = it.value().toMap();
            Camerinfo info;

            info.name = getValue(cameraData, "相机名称").toString();
            info.ip = getValue(cameraData, "相机ip").toString();
            info.path = getValue(cameraData, "配置文件路径").toString();
            info.check = getValue(cameraData, "检测").toString();
            info.mapping = getValue(cameraData, "相机映射").toString();
            info.pointNumber = getValue(cameraData, "触发点位").toInt();
            info.scaleFactor = std::round(getValue(cameraData, "标定值").toDouble() * 1000.0) / 1000.0;
            info.saveflag = getValue(cameraData, "存图开关").toInt();
            info.rounte = getValue(cameraData, "Route编号").toInt();

            m_cameras.append(info);
        }
    }

    refreshCameraUI();
}

void AddCameraDialog::saveCameras() {

    updateDataFromUI();

   
    for (const Camerinfo& cam : m_cameras) {
        if (cam.name.isEmpty() || cam.ip.isEmpty() || cam.path.isEmpty()) {
            QMessageBox::warning(this, "保存错误", "相机名称、IP或配置文件路径不能为空！");
            return;
        }
    }

    // 重新加载完整的配置文件内容
    // 将完整的原始数据（包括未在UI上显示的字段）读入内存
    QVariantMap allCamerasMap = FileOperator::readJsonMap(m_configFilePath);

    // 遍历m_cameras，并根据m_cameras的顺序来更新allCamerasMap
    QVariantMap updatedCamerasMap;

    auto createDetailMap = [](const QVariant& value, const QString& unit = "", const QString& visible = "true", const QString& check = "", const QString& type = "", const QString& range = "") -> QVariantMap {
        QVariantMap detail;
        detail["值"] = value;
        detail["单位"] = unit;
        detail["可见"] = visible;
        detail["检测"] = check;
        detail["类型"] = type.isEmpty() ? (value.type() == QVariant::Int ? "int" : (value.type() == QVariant::Double ? "double" : "string")) : type;
        detail["范围"] = range;
        return detail;
    };

    for (int i = 0; i < m_cameras.size(); ++i) {
        QString cameraKey = QString("相机%1").arg(i + 1);
        const Camerinfo& cam = m_cameras[i];
        
        // 从已加载的原始数据中获取对应相机的配置
        QVariantMap cameraData;
        if (allCamerasMap.contains(cameraKey)) {
            cameraData = allCamerasMap[cameraKey].toMap();
        }
        // 如果是新添加的相机，cameraData将为空，下面的代码会创建所有字段

        // 更新UI上相关的字段，同时保留cameraData中已有的其他字段
        // 对于已有的键，会用新的值覆盖；对于不存在的键，会新添加。
        cameraData["相机名称"] = createDetailMap(cam.name);
        cameraData["相机ip"] = createDetailMap(cam.ip);
        cameraData["配置文件路径"] = createDetailMap(cam.path);
        cameraData["检测"] = createDetailMap(cam.check);
        cameraData["相机映射"] = createDetailMap(cam.mapping);
        cameraData["触发点位"] = createDetailMap(cam.pointNumber);
        cameraData["标定值"] = createDetailMap(std::round(cam.scaleFactor * 10000.0) / 10000.0, "", "true", "", "double");
        cameraData["存图开关"] = createDetailMap(cam.saveflag);
        cameraData["Route编号"] = createDetailMap(cam.rounte);
        
        // 将更新后的相机配置放入新的QVariantMap
        updatedCamerasMap[cameraKey] = cameraData;
    }
    
    // 写入更新后的完整数据到文件
    if (FileOperator::writeJsonMap(m_configFilePath, updatedCamerasMap)) {
        QMessageBox::information(this, "保存成功", "相机配置已成功保存！");
    } else {
        QMessageBox::warning(this, "保存失败", "无法保存配置文件，请检查路径或权限。");
    }
}

void AddCameraDialog::addNewCamera()
{
    m_cameras.append(Camerinfo());
    refreshCameraUI();
    scrollArea->verticalScrollBar()->setValue(scrollArea->verticalScrollBar()->maximum());
}

void AddCameraDialog::deleteCamera()
{
    QPushButton* senderButton = qobject_cast<QPushButton*>(sender());
    if (!senderButton) return;

    int indexToDelete = senderButton->property("cameraIndex").toInt();
    if (indexToDelete >= 0 && indexToDelete < m_cameras.size()) {
        if (QMessageBox::question(this, "确认删除", QString("确定删除相机 '%1' 吗？").arg(m_cameras[indexToDelete].name), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
            m_cameras.removeAt(indexToDelete);
            refreshCameraUI();
        }
    }
}
