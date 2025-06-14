#include "addcameradialog.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QScrollBar>


AddCameraDialog::AddCameraDialog(const QString& configFilePath, QWidget *parent)
    : QDialog(parent), m_configFilePath(configFilePath)
{
    setWindowTitle("相机配置 - " + QFileInfo(configFilePath).fileName());
    setMinimumSize(800, 600);
    setupUI();
    loadCameras(); // 在这里加载并显示配置文件内容
}

AddCameraDialog::~AddCameraDialog()
{
    // If needed, add auto-save logic here
}

void AddCameraDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    scrollAreaContent = new QWidget();
    camerasLayout = new QVBoxLayout(scrollAreaContent);
    camerasLayout->setContentsMargins(10, 10, 10, 10);
    camerasLayout->setSpacing(15);
    scrollArea->setWidget(scrollAreaContent);

    mainLayout->addWidget(scrollArea);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *addNewCamButton = new QPushButton("新增相机", this);
    QPushButton *saveButton = new QPushButton("保存配置", this);
    QPushButton *closeButton = new QPushButton("关闭", this);

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
    QLayoutItem *item;
    // 清空现有UI控件
    while ((item = camerasLayout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            item->widget()->deleteLater(); // 使用 deleteLater 安全删除
        }
        delete item;
    }
    m_cameraWidgets.clear();

    if (m_cameras.isEmpty()) {
        QLabel *noCamLabel = new QLabel("当前没有相机配置，点击'新增相机'添加。", scrollAreaContent);
        noCamLabel->setAlignment(Qt::AlignCenter);
        camerasLayout->addWidget(noCamLabel);
        return;
    }

    for (int i = 0; i < m_cameras.size(); ++i)
    {
        const Camerinfo& info = m_cameras[i];

        QGroupBox *cameraGroupBox = new QGroupBox(QString("相机%1").arg(i + 1), scrollAreaContent);
        cameraGroupBox->setStyleSheet("QGroupBox { border: 1px solid gray; border-radius: 5px; margin-top: 1ex; }"
                                      "QGroupBox::title { subcontrol-origin: margin; subcontrol-position: top center; padding: 0 3px; }");
        QVBoxLayout *groupLayout = new QVBoxLayout(cameraGroupBox);
        groupLayout->setContentsMargins(10, 15, 10, 10);
        groupLayout->setSpacing(8);

        CameraInputWidgets widgets;
        widgets.cameraIndex = i;

        // 相机名称
        QHBoxLayout *nameLayout = new QHBoxLayout();
        nameLayout->addWidget(new QLabel("相机名称", cameraGroupBox));
        widgets.nameEdit = new QLineEdit(info.name, cameraGroupBox); // 直接使用 info.name (QString)
        nameLayout->addWidget(widgets.nameEdit);
        groupLayout->addLayout(nameLayout);

        // 相机IP
        QHBoxLayout *ipLayout = new QHBoxLayout();
        ipLayout->addWidget(new QLabel("相机IP", cameraGroupBox));
        widgets.ipEdit = new QLineEdit(info.ip, cameraGroupBox); // 直接使用 info.ip (QString)
        ipLayout->addWidget(widgets.ipEdit);
        groupLayout->addLayout(ipLayout);

        // 相机端口
        QHBoxLayout *portLayout = new QHBoxLayout();
        portLayout->addWidget(new QLabel("相机端口", cameraGroupBox));
        widgets.portEdit = new QLineEdit(QString::number(info.port), cameraGroupBox); // 直接使用 info.port (int)
        widgets.portEdit->setValidator(new QIntValidator(1, 65535, widgets.portEdit));
        portLayout->addWidget(widgets.portEdit);
        groupLayout->addLayout(portLayout);

        // 相机SN
        QHBoxLayout *snLayout = new QHBoxLayout();
        snLayout->addWidget(new QLabel("相机SN", cameraGroupBox));
        widgets.snEdit = new QLineEdit(info.SN, cameraGroupBox); // 直接使用 info.SN (QString)
        snLayout->addWidget(widgets.snEdit);
        groupLayout->addLayout(snLayout);

        // 配置文件路径
        QHBoxLayout *pathLayout = new QHBoxLayout();
        pathLayout->addWidget(new QLabel("配置文件路径", cameraGroupBox));
        widgets.pathEdit = new QLineEdit(info.path, cameraGroupBox); // 直接使用 info.path (QString)
        pathLayout->addWidget(widgets.pathEdit);
        groupLayout->addLayout(pathLayout);

        // 检测
        QHBoxLayout *checkLayout = new QHBoxLayout();
        widgets.checkCheckBox = new QCheckBox("检测", cameraGroupBox);
        widgets.checkCheckBox->setChecked(info.check.toLower() == "true"); // 直接使用 info.check (QString)
        checkLayout->addWidget(widgets.checkCheckBox);
        checkLayout->addStretch();
        groupLayout->addLayout(checkLayout);

        widgets.deleteButton = new QPushButton("删除此相机", cameraGroupBox);
        widgets.deleteButton->setProperty("cameraIndex", i);
        connect(widgets.deleteButton, &QPushButton::clicked, this, &AddCameraDialog::deleteCamera);
        groupLayout->addWidget(widgets.deleteButton, 0, Qt::AlignRight);

        // 连接信号槽，当输入改变时更新数据
        // 注意：这里连接的是 QLineEdit 的 textChanged 信号和 QCheckBox 的 stateChanged 信号
        connect(widgets.nameEdit, &QLineEdit::textChanged, this, &AddCameraDialog::onInputChanged);
        connect(widgets.ipEdit, &QLineEdit::textChanged, this, &AddCameraDialog::onInputChanged);
        connect(widgets.portEdit, &QLineEdit::textChanged, this, &AddCameraDialog::onInputChanged);
        connect(widgets.snEdit, &QLineEdit::textChanged, this, &AddCameraDialog::onInputChanged);
        connect(widgets.pathEdit, &QLineEdit::textChanged, this, &AddCameraDialog::onInputChanged);
        connect(widgets.checkCheckBox, &QCheckBox::stateChanged, this, &AddCameraDialog::onInputChanged);

        m_cameraWidgets.append(widgets);
        camerasLayout->addWidget(cameraGroupBox);
    }
    camerasLayout->addStretch();
}

void AddCameraDialog::updateDataFromUI()
{
    // 遍历所有 UI 控件，将它们的值同步到 m_cameras 列表中的对应 Camerinfo 对象
    for (const CameraInputWidgets& widgets : m_cameraWidgets) {
        int index = widgets.cameraIndex;
        if (index >= 0 && index < m_cameras.size()) {
            Camerinfo& info = m_cameras[index];

            // 直接更新 Camerinfo 的扁平成员
            info.name = widgets.nameEdit->text();
            info.ip = widgets.ipEdit->text();
            info.port = widgets.portEdit->text().toInt();
            info.SN = widgets.snEdit->text();
            info.path = widgets.pathEdit->text();
            info.check = widgets.checkCheckBox->isChecked() ? "true" : "false";
        }
    }
}

void AddCameraDialog::onInputChanged()
{
    // 任何输入框或复选框内容改变时，立即更新内存中的数据
    updateDataFromUI();
}

void AddCameraDialog::loadCameras()
{
    m_cameras.clear(); // 清空现有数据
    QVariantMap allCamerasMap = FileOperator::readJsonMap(m_configFilePath); // 读取整个 JSON 文件为 QVariantMap

    if (allCamerasMap.isEmpty()) {
        qWarning() << "No cameras found in" << m_configFilePath << "or file failed to load.";
    } else {
        // 遍历 JSON 文件的顶层键值对 (例如 "相机1", "相机2")
        for (auto it = allCamerasMap.begin(); it != allCamerasMap.end(); ++it) {
            // 确保每个相机条目是一个 QVariantMap (JSON 对象)
            if (it.value().type() == QVariant::Map) {
                QVariantMap cameraData = it.value().toMap(); // 获取单个相机的 QVariantMap

                Camerinfo info; // 创建一个扁平的 Camerinfo 结构体实例

                // 从嵌套的 QVariantMap 中提取 "值" 字段到扁平的 Camerinfo 成员
                // 先检查键是否存在且其值是一个 Map，再从该 Map 中获取 "值"
                if (cameraData.contains("相机名称") && cameraData.value("相机名称").type() == QVariant::Map)
                    info.name = cameraData.value("相机名称").toMap().value("值").toString();

                if (cameraData.contains("相机ip") && cameraData.value("相机ip").type() == QVariant::Map)
                    info.ip = cameraData.value("相机ip").toMap().value("值").toString();

                if (cameraData.contains("相机port") && cameraData.value("相机port").type() == QVariant::Map)
                    info.port = cameraData.value("相机port").toMap().value("值").toInt();

                if (cameraData.contains("相机SN") && cameraData.value("相机SN").type() == QVariant::Map)
                    info.SN = cameraData.value("相机SN").toMap().value("值").toString();

                if (cameraData.contains("配置文件路径") && cameraData.value("配置文件路径").type() == QVariant::Map)
                    info.path = cameraData.value("配置文件路径").toMap().value("值").toString();

                if (cameraData.contains("检测") && cameraData.value("检测").type() == QVariant::Map)
                    info.check = cameraData.value("检测").toMap().value("值").toString(); // "true" or "false" string

                // 打印加载的每个字段的值，帮助调试
                qDebug() << "Loaded Camera:";
                qDebug() << "  Name:" << info.name;
                qDebug() << "  IP:" << info.ip;
                qDebug() << "  Port:" << info.port;
                qDebug() << "  SN:" << info.SN;
                qDebug() << "  Path:" << info.path;
                qDebug() << "  Check:" << info.check;

                m_cameras.append(info); // 将解析后的 Camerinfo 添加到列表中
            } else {
                qWarning() << "Top-level item '" << it.key() << "' in JSON is not a camera map (QVariant::Map). Skipping.";
            }
        }
        qDebug() << "Loaded" << m_cameras.size() << "cameras from" << m_configFilePath;
    }
    refreshCameraUI(); // 加载完成后刷新UI以显示数据
}

void AddCameraDialog::saveCameras()
{
    updateDataFromUI(); // 确保 UI 上的最新数据已同步到 m_cameras

    // 最终的数据验证：确保关键值不为空
    for(const Camerinfo& cam : m_cameras) {
        if (cam.name.isEmpty() ||
            cam.ip.isEmpty() ||
            cam.SN.isEmpty() ||
            cam.path.isEmpty())
        {
            QMessageBox::warning(this, "保存错误", "相机名称、IP、SN或配置文件路径不能为空，请检查！");
            return;
        }
        // 端口号验证
        if (cam.port < 1 || cam.port > 65535) {
            QMessageBox::warning(this, "保存错误", QString("相机 '%1' 的端口号无效，请检查！").arg(cam.name));
            return;
        }
    }

    QVariantMap allCamerasMap;
    for (int i = 0; i < m_cameras.size(); ++i) {
        QString cameraKey = QString("相机%1").arg(i + 1);
        QVariantMap cameraData;

        const Camerinfo& currentCam = m_cameras[i];

        // 手动构建每个属性的嵌套 QVariantMap，以匹配原始 JSON 结构
        // 我们需要为每个字段提供默认的 "单位", "可见", "检测", "类型", "范围"
        // 确保这些默认值与您的原始JSON中的期望一致

        QVariantMap nameDetail;
        nameDetail["值"] = currentCam.name;
        nameDetail["单位"] = "";
        nameDetail["可见"] = "true";
        nameDetail["检测"] = "";
        nameDetail["类型"] = "string";
        nameDetail["范围"] = "";
        cameraData["相机名称"] = nameDetail;

        QVariantMap ipDetail;
        ipDetail["值"] = currentCam.ip;
        ipDetail["单位"] = "ms"; // 假设IP地址单位是ms，根据您JSON中的示例
        ipDetail["可见"] = "true";
        ipDetail["检测"] = "";
        ipDetail["类型"] = "string";
        ipDetail["范围"] = "";
        cameraData["相机ip"] = ipDetail;

        QVariantMap portDetail;
        portDetail["值"] = currentCam.port;
        portDetail["单位"] = "";
        portDetail["可见"] = "true";
        portDetail["检测"] = "";
        portDetail["类型"] = "int";
        portDetail["范围"] = "";
        cameraData["相机port"] = portDetail;

        QVariantMap snDetail;
        snDetail["值"] = currentCam.SN;
        snDetail["单位"] = "";
        snDetail["可见"] = "true";
        snDetail["检测"] = "";
        snDetail["类型"] = "string";
        snDetail["范围"] = "";
        cameraData["相机SN"] = snDetail;

        QVariantMap pathDetail;
        pathDetail["值"] = currentCam.path;
        pathDetail["单位"] = "";
        pathDetail["可见"] = "true";
        pathDetail["检测"] = "";
        pathDetail["类型"] = "string";
        pathDetail["范围"] = "";
        cameraData["配置文件路径"] = pathDetail;

        QVariantMap checkDetail;
        checkDetail["值"] = currentCam.check; // "true" or "false" string
        checkDetail["单位"] = "";
        // 注意：您原始JSON中 "检测" 的 "可见" 字段有时是 "false"
        // 如果您的UI不提供修改这个字段，它将始终是 "true"
        // 您可以根据需要调整这里的逻辑，例如从原始加载的数据中保留这些元数据
        checkDetail["可见"] = "true"; // 默认保存为true，除非有其他逻辑
        checkDetail["检测"] = ""; // 这个字段自身通常为空，因为它描述的是当前参数是否需要检测
        checkDetail["类型"] = "string";
        checkDetail["范围"] = "";
        cameraData["检测"] = checkDetail;

        allCamerasMap[cameraKey] = cameraData;
    }

    // 调用 FileOperator 将 QVariantMap 写入 JSON 文件
    if (FileOperator::writeJsonMap(m_configFilePath, allCamerasMap)) {
        QMessageBox::information(this, "保存成功", "相机配置已成功保存！");
        qDebug() << "Saved" << m_cameras.size() << "cameras to" << m_configFilePath;
    } else {
        QMessageBox::warning(this, "保存失败", "未能保存相机配置，请检查文件路径或权限。");
    }
}

void AddCameraDialog::addNewCamera()
{
    // 创建一个默认的 Camerinfo 对象，其成员将由 Camerinfo 默认构造函数初始化
    m_cameras.append(Camerinfo());
    refreshCameraUI(); // 刷新 UI 以显示新添加的相机
    // 滚动到最底部，以便用户看到新添加的相机
    scrollArea->verticalScrollBar()->setValue(scrollArea->verticalScrollBar()->maximum());
    qDebug() << "Added new empty camera. Total cameras:" << m_cameras.size();
}

void AddCameraDialog::deleteCamera()
{
    QPushButton *senderButton = qobject_cast<QPushButton*>(sender());
    if (!senderButton) return;

    int indexToDelete = senderButton->property("cameraIndex").toInt();

    if (indexToDelete >= 0 && indexToDelete < m_cameras.size()) {
        // 弹出确认对话框
        if (QMessageBox::question(this, "确认删除", QString("确定要删除相机 '%1' 吗？")
                                                            .arg(m_cameras[indexToDelete].name.isEmpty() ? QString("相机%1").arg(indexToDelete + 1) : m_cameras[indexToDelete].name),
                                  QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
            m_cameras.removeAt(indexToDelete); // 从列表中移除相机
            refreshCameraUI(); // 刷新 UI
            qDebug() << "Deleted camera at index:" << indexToDelete;
        }
    }
}
