#include "parawidget.h"
#include "role.h"
#include <QVBoxLayout>
#include <QTabWidget>
#include <QGridLayout>
#include <QLabel>
#include<QGroupBox>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QIntValidator>
#include <QDoubleValidator>
#include <QFileDialog>
#include <QScrollArea>
#include <QSpinBox>
#include<QComboBox>
#include <QFuture>
#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrentRun>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QtGlobal>
#include <QVBoxLayout>
#include <QFormLayout>
#include "imageviewerwindow.h"
#include <opencv2/opencv.hpp>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
// 以及我们之前用到的其他头文件
#include <QDir>
#include <QCoreApplication>
#include <QFileInfo>
#include <QMetaObject>


ParaWidget::ParaWidget(RangeClass* RC, CameralClass* CC, AlgoClass* AC, Cameral* cam, DebugInfo* DI, QWidget* parent)
    : QWidget{ parent }
{
    // 设置窗口属性
    setWindowFlags(Qt::Window);
    setWindowTitle("参数设置");
    setAttribute(Qt::WA_DeleteOnClose);
    resize(1000, 750);

    // 设置全局字体（放大一倍）
    QFont font = this->font();
    font.setPointSize(font.pointSize() * 2); // 字体放大两倍
    setFont(font);

    // 创建主布局
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // 创建选项卡控件
    tabWidget = new QTabWidget;
    mainLayout->addWidget(tabWidget); // 将tabWidget添加到主布局

    // 赋值传入的参数管理类指针
    this->m_cam = cam;
    this->m_algoSettings = AC;
    this->m_rangeSettings = RC;
    this->m_cameralSettings = CC;
    this->DI = DI;

    connect(m_algoSettings, &AlgoClass::TransMat, this, &ParaWidget::onTransMat);

#ifndef USE_MAIN_WINDOW_CAPACITY
    // (情况 1: 普通版)
    qDebug() << "Constructor: Using Normal setup functions.";
    setupRangeTab(tabWidget); // 索引 0
    setupScaleTab(tabWidget); // 索引 1
#else
    // (情况 2: EX 版)
    qDebug() << "Constructor: Using EX setup functions.";
    setupRangeTab_EX(tabWidget); // 索引 0
    setupScaleTabEX(tabWidget);  // 索引 1
#endif // USE_MAIN_WINDOW_CAPACITY

    setupAlgorithmTab(tabWidget); // 索引 2
    setupDebugTab(tabWidget);     // 索引 3
    setupCameralTab(tabWidget);   // 索引 4


    // --- 2. 根据角色设置可见性和可编辑性 ---
    const QString role = Role::GetCurrentRole();
    qDebug() << "Constructor: Current role is:" << role;

    if (role == "厂商" || role == "机修")
    {
        qDebug() << "Constructor: Manufacturer/Mechanic role. All tabs visible.";
        // “厂商”和“机修”权限：所有选项卡默认可见且可用。
        // 无需执行任何操作。
    }
    else
    {
        // 其他所有角色 (如 “操作员” 等)
        qDebug() << "Constructor: Other role detected. Applying restrictions...";

#ifndef USE_MAIN_WINDOW_CAPACITY
        // --- 情况 1 (普通版) 逻辑 ---
        // 显示: 范围(禁用), 标定(索引1), 算法, 调试, 相机
        // 隐藏: 无 (根据您的新要求，不隐藏标定)

            // 【修改】注释掉隐藏标定页的代码
            // tabWidget->setTabVisible(1, false);
        qDebug() << "Constructor (Normal): Scale tab (1) remains visible.";


        // 禁用 "范围参数" (索引 0)
        qDebug() << "Constructor (Normal): Disabling Range tab (0).";
        QWidget* rangePageWidget = tabWidget->widget(0);
        if (rangePageWidget)
        {
            rangePageWidget->setEnabled(false);
        }

#else
        // --- 情况 2 (EX 版) 逻辑 ---
        // 显示: 范围, 标定(索引1), 调试, 相机
        // 隐藏: 算法 (索引 2)

            // 【修改】注释掉隐藏标定页的代码
            // tabWidget->setTabVisible(1, false); // 隐藏 ScaleTab
        qDebug() << "Constructor (EX): Scale tab (1) remains visible.";

        qDebug() << "Constructor (EX): Hiding Algorithm tab (2).";
        tabWidget->setTabVisible(2, false); // 隐藏 AlgorithmTab

#endif // USE_MAIN_WINDOW_CAPACITY
    }

    // --- 针对特定角色的额外处理 ---
    if (role == "操作员") {
        qDebug() << "Constructor: Operator role detected. Hiding Cameral tab (4).";
        tabWidget->setTabVisible(4, false); // 隐藏 CameralTab (索引 4)
    }

    // 设置主布局
    setLayout(mainLayout);
}

ParaWidget::~ParaWidget()
{

}


void ParaWidget::onTransMat(cv::Mat mat)
{
    QPixmap pix = convertMatToPixmap(mat);
    imageViewer_algo->setPixmap(pix);
}

void ParaWidget::closeEvent(QCloseEvent* event)
{
#ifndef USED_MAIN_WINDOW_CAPACITY
    // 获取当前活动页面的索引
    int currentIndex = tabWidget->currentIndex();
    // 获取当前活动页面的文本
    QString currentTabText = tabWidget->tabText(currentIndex);

    // 只有当当前页面是“范围参数”时，才弹出保存确认框
    if (currentTabText != "范围参数") {
        event->accept();
        return;
    }

    // 弹窗询问用户是否保存修改
    QMessageBox::StandardButton reply = QMessageBox::question(this, "保存确认", "确定要保存当前修改吗？",
        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

    if (reply == QMessageBox::No) {
        // 用户选择“不保存”，直接接受关闭事件
        event->accept();
        return;
    }

    if (reply == QMessageBox::Cancel) {
        // 用户选择“取消”，忽略关闭事件
        event->ignore();
        return;
    }

    // 用户选择“保存”，执行保存逻辑和校验
    // --- 步骤 1: 将所有 QLineEdit 和 QCheckBox 的当前值同步到 m_rangeSettings ---
    // 根据角色判断补偿值是否可见和可编辑
    bool isManufacturer = (Role::CurrentRole == "厂商");
    for (auto projectIt = m_paramValueEdits.begin(); projectIt != m_paramValueEdits.end(); ++projectIt)
    {
        const QString& projectName = projectIt.key();
        const QMap<QString, QLineEdit*>& paramEdits = projectIt.value();
        const QMap<QString, QCheckBox*>& paramCheckboxes = m_paramCheckboxes[projectName];
        const QMap<QString, QLineEdit*>& paramCompensationEdits = m_paramCompensationEdits[projectName];

        for (auto paramEditIt = paramEdits.begin(); paramEditIt != paramEdits.end(); ++paramEditIt)
        {
            const QString& paramName = paramEditIt.key();
            QLineEdit* valueEdit = paramEditIt.value();
            QCheckBox* checkBox = paramCheckboxes.value(paramName, nullptr);
            QLineEdit* compensationEdit = paramCompensationEdits.value(paramName, nullptr);

            QString textValue = valueEdit->text();
            QVariant convertedValue;

            const ParamDetail* detail = m_rangeSettings->getParamDetail(projectName, paramName);
            if (detail) {
                bool conversionOk = false;
                if (detail->type.toLower() == "int") {
                    convertedValue = textValue.toInt(&conversionOk);
                }
                else if (detail->type.toLower() == "float") {
                    convertedValue = textValue.toDouble(&conversionOk);
                }
                else if (detail->type.toLower() == "bool") {
                    convertedValue = (textValue.toLower() == "true" || textValue == "1");
                    conversionOk = true;
                }
                else {
                    convertedValue = textValue;
                    conversionOk = true;
                }

                if (!conversionOk) {
                    QMessageBox::warning(this, "输入格式错误", QString("参数 '%1' (项目: %2) 的值 '%3' 不是有效格式。请修正。")
                        .arg(paramName).arg(projectName).arg(textValue));
                    event->ignore(); // 忽略关闭事件
                    return;
                }
            }
            else {
                convertedValue = textValue;
            }

            m_rangeSettings->updateParamValue(projectName, paramName, convertedValue);

            if (checkBox) {
                m_rangeSettings->updateParamCheck(projectName, paramName, checkBox->isChecked());
            }

            if (isManufacturer && compensationEdit) {
                bool compConversionOk;
                double compValue = compensationEdit->text().toDouble(&compConversionOk);
                if (compConversionOk) {
                    m_rangeSettings->updateParamCompensation(projectName, paramName, compValue);
                }
                else {
                    QMessageBox::warning(this, "输入格式错误", QString("参数 '%1' (项目: %2) 的补偿值 '%3' 不是有效的浮点数。请修正。")
                        .arg(paramName).arg(projectName).arg(compensationEdit->text()));
                    event->ignore(); // 忽略关闭事件
                    return;
                }
            }
        }
    }

    //// --- 步骤 2: 对更新后的 m_rangeSettings 数据进行最终的范围校验 ---
    //QString errorMessage;
    //errorMessage = validateParametersRange(m_rangeSettings, "范围");

    //if (!errorMessage.isEmpty()) {
    //    QMessageBox::warning(this, "参数校验失败", "以下参数值超出规定范围或格式错误：\n" + errorMessage + "\n\n请修正后再保存。");
    //    event->ignore(); // 忽略关闭事件
    //    return;
    //}

    // --- 步骤 3: 如果所有校验通过，则执行保存操作并接受关闭事件 ---
    m_rangeSettings->saveParamsAsync();
    m_cam->RI->updateProcessedData(&m_rangeSettings->m_parameters);
    m_cam->RI->updatePaintData(&m_rangeSettings->m_parameters);

    QMessageBox::information(this, "保存成功", "参数已成功保存。");
    event->accept(); // 接受关闭事件

#endif
}


void ParaWidget::setupScaleTabEX(QTabWidget* tabWidget)
{
    // 1. 初始化和安全检查
    if (m_cam == nullptr) {
        return;
    }
    const AllUnifyParams& allParams = m_cam->unifyParams;
    if (allParams.isEmpty()) {
        return;
    }

    // 2. UI 结构设置
    QWidget* scaleTab = new QWidget(this);
    // 【重要】确保选项卡标题是正确的
    tabWidget->addTab(scaleTab, "标定参数配置");
    QVBoxLayout* mainLayout = new QVBoxLayout(scaleTab);

    // --- 【关键】确保这里清除的是 m_scale... Map ---
    m_scaleCheckboxes.clear();    // 清除 scale 的 CheckBox Map
    m_scaleLineEditMap.clear(); // 清除 scale 的 LineEdit Map
    // ---

    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    mainLayout->addWidget(scrollArea);

    // 布局
    QWidget* subTabContent = new QWidget;
    QVBoxLayout* scrollLayout = new QVBoxLayout(subTabContent);
    QGridLayout* gridLayout = new QGridLayout;
    scrollLayout->addLayout(gridLayout);
    scrollLayout->addStretch(1);
    scrollArea->setWidget(subTabContent);

    gridLayout->setVerticalSpacing(8);
    gridLayout->setHorizontalSpacing(10);

    QDoubleValidator* validator = new QDoubleValidator(this);
    validator->setLocale(QLocale::C);
    validator->setDecimals(8);

    // 3. 遍历参数并创建 UI 控件
    int row = 0;
    for (auto it = allParams.constBegin(); it != allParams.constEnd(); ++it)
    {
        const QString& paramName = it.key();
        const UnifyParam& config = it.value();

        QLabel* nameLabel = new QLabel(config.label, this);
        nameLabel->setWordWrap(true);

        QCheckBox* checkCheckBox = new QCheckBox("启用检测", this);
        checkCheckBox->setChecked(config.check);

        QLabel* scaleLabel = new QLabel("标定参数:", this);
        QLineEdit* scaleEdit = new QLineEdit(QString::number(config.scaleFactor, 'f', 6), this);
        scaleEdit->setValidator(validator);

        // 存入专属 Map
        m_scaleCheckboxes.insert(paramName, checkCheckBox);
        m_scaleLineEditMap.insert(paramName, scaleEdit);

        // 布局
        gridLayout->addWidget(nameLabel, row, 0, Qt::AlignRight | Qt::AlignVCenter);
        gridLayout->addWidget(checkCheckBox, row, 1, Qt::AlignLeft | Qt::AlignVCenter);
        gridLayout->addWidget(scaleLabel, row, 2, Qt::AlignRight | Qt::AlignVCenter);
        gridLayout->addWidget(scaleEdit, row, 3, Qt::AlignLeft | Qt::AlignVCenter);

        row++;
    }

    // 4. 调整列拉伸比例
    gridLayout->setColumnStretch(0, 3); gridLayout->setColumnStretch(1, 1);
    gridLayout->setColumnStretch(2, 1); gridLayout->setColumnStretch(3, 2);
    gridLayout->setColumnStretch(4, 3);

    // 5. 添加保存按钮
    QPushButton* saveButton = new QPushButton("保存标定配置", this);
    saveButton->setMinimumHeight(30);
    QHBoxLayout* buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(saveButton);
    mainLayout->addLayout(buttonLayout);

    // 连接到专属的槽函数
    connect(saveButton, &QPushButton::clicked, this, &ParaWidget::onScaleSaveClicked);
}

void ParaWidget::setupScaleTab(QTabWidget* tabWidget)
{
    QWidget* scaleTab = new QWidget(tabWidget);
    QVBoxLayout* mainLayout = new QVBoxLayout(scaleTab);//设置父对象，避免崩溃

    QScrollArea* scrollArea = new QScrollArea(scaleTab);
    scrollArea->setWidgetResizable(true);

    QWidget* contentWidget = new QWidget(scrollArea);
    QVBoxLayout* contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setContentsMargins(10, 10, 10, 10);
    contentLayout->setSpacing(10);
    scrollArea->setWidget(contentWidget);


    qDebug() << "Debug: setupScaleTab called.";
    if (!m_cam) {
        qWarning() << "Error: m_cam is a null pointer.";
        return;
    }

    qDebug() << "Debug: Address of m_cam->ScaleArray:" << &m_cam->ScaleArray;
    qDebug() << "Debug: Size of m_cam->ScaleArray:" << m_cam->ScaleArray.size();

    // 清空成员变量 m_lineEditMap，以防多次调用
    m_lineEditMap.clear();

    for (int i = 0; i < m_cam->ScaleArray.size(); ++i) {
        const SimpleParam& param = m_cam->ScaleArray.at(i);

        qDebug() << QString("Debug: Iteration %1, Processing item: name='%2', value=%3")
            .arg(i).arg(param.name).arg(param.value);

        if (param.name.isNull() || param.name.isEmpty()) {
            qWarning() << "Warning: Detected invalid or empty name at index" << i << ". Skipping.";
            continue;
        }

        if (qIsNaN(param.value) || qIsInf(param.value)) {
            qWarning() << "Warning: Detected invalid double value (NaN/Inf) at index" << i << ". Using 0.0.";
            continue;
        }

        QHBoxLayout* paramLayout = new QHBoxLayout();

        // 将 QLabel 和 QLineEdit 设置为 contentWidget 的子对象
        QLabel* label = new QLabel(param.name, contentWidget);
        QLineEdit* lineEdit = new QLineEdit(QString::number(param.value, 'f', 5), contentWidget);

        label->setFixedWidth(180);
        lineEdit->setFixedWidth(150);

        paramLayout->addWidget(label);
        paramLayout->addWidget(lineEdit);
        paramLayout->addStretch(1);

        contentLayout->addLayout(paramLayout);
        m_lineEditMap.insert(param.name, lineEdit);
    }

    mainLayout->addWidget(scrollArea);

    // 将 saveButton 设置为 scaleTab 的子对象
    QPushButton* saveButton = new QPushButton("保存修改", scaleTab);
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(saveButton);
    mainLayout->addLayout(buttonLayout);

    QObject::connect(saveButton, &QPushButton::clicked, this, [this]() {
        QVector<SimpleParam> tempScaleArray = m_cam->ScaleArray;

        for (SimpleParam& param : tempScaleArray) {
            if (m_lineEditMap.contains(param.name)) {
                QLineEdit* lineEdit = m_lineEditMap.value(param.name);
                bool ok;
                double value = lineEdit->text().toDouble(&ok);
                if (ok) {
                    param.value = value;
                }
            }
        }
        m_cam->RI->updatePaintDataFromScaleArray(tempScaleArray);
        m_cam->ScaleArray = tempScaleArray;
        saveScaleArrayAsync(m_cam->ScalePath, tempScaleArray);

        qDebug() << "Calibration scales updated via m_cam.";
        });

    tabWidget->addTab(scaleTab, "标定参数");
}


void ParaWidget::saveScaleArrayAsync(const QString& filePath, const QVector<SimpleParam>& scaleArray)
{
    // 将 QVector<SimpleParam> 转换为 QVariantMap，以方便 FileOperator::writeJsonMap 使用
    QVariantMap mapToSave;
    for (const SimpleParam& param : scaleArray) {
        QVariantMap paramMap;
        paramMap["值"] = param.value;
        mapToSave[param.name] = paramMap;
    }

    // 在后台线程中执行保存操作
    QFuture<bool> future = QtConcurrent::run([filePath, dataToSave = mapToSave]() {
        qDebug() << "Background thread: Executing scale parameter save...";
        if (FileOperator::writeJsonMap(filePath, dataToSave)) {
            qDebug() << "Background thread: Scale parameters saved successfully to" << filePath;
            return true;
        }
        else {
            qWarning() << "Background thread: Failed to save scale parameters to" << filePath;
            return false;
        }
        });

    qDebug() << "Main thread: Async scale parameter save initiated.";
}


void ParaWidget::setupRangeTab(QTabWidget* tabWidget)
{
    if (m_rangeSettings == nullptr) {
        // 使用新的通用错误处理函数
        handleSettingsInitializationError(tabWidget, "范围参数", "范围参数管理器未正确初始化。");
        return;
    }

    QWidget* rangeTab = new QWidget;
    tabWidget->addTab(rangeTab, "范围参数");

    QVBoxLayout* mainLayout = new QVBoxLayout(rangeTab);
    QTabWidget* subTabWidget = new QTabWidget(this);
    mainLayout->addWidget(subTabWidget);

    QFont defaultFont = rangeTab->font();
    // 创建一个新字体，字号减一
    QFont smallerFont = defaultFont;
    smallerFont.setPointSize(defaultFont.pointSize() - 1);

    // 将新字体应用于 rangeTab 的所有子控件
    rangeTab->setFont(smallerFont);

    // 清空 map，防止重复添加。
    m_paramValueEdits.clear();
    m_paramCheckboxes.clear();
    m_paramCompensationEdits.clear();

    const Parameters& allRangeParameters = m_rangeSettings->getRangeParameters();

    if (allRangeParameters.detectionProjects.isEmpty()) {
        displayNoParametersMessage(mainLayout, "范围");
        return;
    }

    // 根据角色判断补偿值是否可见
    bool isManufacturer = (Role::CurrentRole == "厂商");

    for (auto projectIt = allRangeParameters.detectionProjects.begin();
        projectIt != allRangeParameters.detectionProjects.end();
        ++projectIt)
    {
        const QString& projectName = projectIt.key();
        const DetectionProject& detectionProject = projectIt.value();

        // 为每个项目创建一个包含滚动区域的子标签页
        QWidget* subTabContent = new QWidget;
        QVBoxLayout* subTabContentLayout = new QVBoxLayout(subTabContent);
        QGridLayout* gridLayout = new QGridLayout;
        gridLayout->setVerticalSpacing(5);
        gridLayout->setHorizontalSpacing(10);

        int row = 0;

        // 为当前项目创建内层 QMap
        m_paramValueEdits[projectName] = QMap<QString, QLineEdit*>();
        m_paramCheckboxes[projectName] = QMap<QString, QCheckBox*>();
        m_paramCompensationEdits[projectName] = QMap<QString, QLineEdit*>();

        for (auto paramIt = detectionProject.params.begin();
            paramIt != detectionProject.params.end();
            ++paramIt)
        {
            const QString& paramName = paramIt.key();
            const ParamDetail& paramDetail = paramIt.value();

            if (!paramDetail.visible) {
                continue;
            }

            // 参数名称和值
            QLabel* nameLabel = new QLabel(paramName, this);
            QLineEdit* valueEdit = new QLineEdit(paramDetail.value.toString(), this);
            QLabel* unitLabel = new QLabel(QString("单位: %1").arg(paramDetail.unit), this);
            QLabel* checkLabel = new QLabel("检测:", this);
            QCheckBox* checkCheckBox = new QCheckBox(this);
            checkCheckBox->setChecked(paramDetail.check);

            // 补偿值控件
            QLabel* compensationLabel = new QLabel("补偿值:", this);
            // 确保显示足够的小数位，例如3位
            QLineEdit* compensationEdit = new QLineEdit(QString::number(paramDetail.compensation, 'f', 3), this);

            // 设置控件样式和大小
            nameLabel->setMinimumWidth(100);
            valueEdit->setMaximumWidth(80);
            unitLabel->setMinimumWidth(30);
            checkLabel->setMinimumWidth(40);
            checkCheckBox->setMinimumWidth(20);
            compensationLabel->setMinimumWidth(60);
            compensationEdit->setMaximumWidth(80);

            // 只有当角色是“厂商”时，补偿值相关控件才可见
            compensationLabel->setVisible(isManufacturer);
            compensationEdit->setVisible(isManufacturer);

            // 使用新的通用验证函数，只校验类型，不校验范围
            validateInputType(valueEdit, paramDetail.type);

            // 为补偿值编辑框设置验证器，只允许浮点数
            QDoubleValidator* compensationValidator = new QDoubleValidator(this);
            compensationValidator->setLocale(QLocale::C); // 使用C语言环境，点号作为小数分隔符
            compensationEdit->setValidator(compensationValidator);

            // 将 QLineEdit 和 QCheckBox 指针存储到各自的成员变量中
            m_paramValueEdits[projectName].insert(paramName, valueEdit);
            m_paramCheckboxes[projectName].insert(paramName, checkCheckBox);
            m_paramCompensationEdits[projectName].insert(paramName, compensationEdit);

            // 将控件添加到网格布局
            // 注意：因为移除了“范围”，列索引需要调整
            gridLayout->addWidget(nameLabel, row, 0, Qt::AlignRight | Qt::AlignVCenter);
            gridLayout->addWidget(valueEdit, row, 1, Qt::AlignLeft | Qt::AlignVCenter);
            gridLayout->addWidget(unitLabel, row, 2, Qt::AlignLeft | Qt::AlignVCenter);
            gridLayout->addWidget(checkLabel, row, 3, Qt::AlignRight | Qt::AlignVCenter);
            gridLayout->addWidget(checkCheckBox, row, 4, Qt::AlignLeft | Qt::AlignVCenter);
            gridLayout->addWidget(compensationLabel, row, 5, Qt::AlignRight | Qt::AlignVCenter);
            gridLayout->addWidget(compensationEdit, row, 6, Qt::AlignLeft | Qt::AlignVCenter);
            row++;
        }

        // 调整列拉伸比例
        gridLayout->setColumnStretch(0, 2);
        gridLayout->setColumnStretch(1, 0);
        gridLayout->setColumnStretch(2, 1);
        gridLayout->setColumnStretch(3, 1);
        gridLayout->setColumnStretch(4, 0);
        gridLayout->setColumnStretch(5, 1);
        gridLayout->setColumnStretch(6, 0);
        gridLayout->setColumnStretch(7, 1);

        subTabContentLayout->addLayout(gridLayout);
        subTabContentLayout->addStretch(1);

        // 创建 QScrollArea 并将 subTabContent 设置为其内容
        QScrollArea* scrollArea = new QScrollArea(this);
        scrollArea->setWidgetResizable(true);
        scrollArea->setWidget(subTabContent);

        // 将带有滚动条的 scrollArea 添加到 subTabWidget
        subTabWidget->addTab(scrollArea, projectName);
    }
}

void ParaWidget::setupRangeTab_EX(QTabWidget* tabWidget) // 修改函数定义
{
    qDebug() << "============ setupRangeTab_EX: START ============"; // 保留入口日志
    // 1. 初始化和安全检查
    if (m_cam == nullptr || m_rangeSettings == nullptr) {
        qWarning() << "setupRangeTab_EX: Aborted. m_cam or m_rangeSettings is null.";
        return;
    }
    const AllUnifyParams& allParams = m_cam->unifyParams;
    if (allParams.isEmpty()) {
        qWarning() << "setupRangeTab_EX: Aborted. allParams is empty.";
        return;
    }

    // 2. UI 结构设置
    QWidget* rangeTab = new QWidget(this);
    tabWidget->addTab(rangeTab, "统一参数配置");
    QVBoxLayout* mainLayout = new QVBoxLayout(rangeTab);

    // 【修复】使用专属 Map 并清空
    m_rangeLineEditMap.clear();
    if (!m_rangeCheckboxes.contains("Visible")) {
        m_rangeCheckboxes.insert("Visible", QMap<QString, QCheckBox*>());
    }
    m_rangeCheckboxes["Visible"].clear();

    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    mainLayout->addWidget(scrollArea);

    // 布局
    QWidget* subTabContent = new QWidget;
    QVBoxLayout* scrollLayout = new QVBoxLayout(subTabContent);
    QGridLayout* gridLayout = new QGridLayout;
    scrollLayout->addLayout(gridLayout);
    scrollLayout->addStretch(1);
    scrollArea->setWidget(subTabContent);

    gridLayout->setVerticalSpacing(8);
    gridLayout->setHorizontalSpacing(10);

    int row = 0;
    bool isManufacturer = (Role::CurrentRole == "厂商");
    QDoubleValidator* validator = new QDoubleValidator(this);
    validator->setLocale(QLocale::C);

    QMap<QString, QCheckBox*>& visibleCheckboxesMap = m_rangeCheckboxes["Visible"];

    // 3. 遍历参数并创建 UI 控件
    for (auto it = allParams.constBegin(); it != allParams.constEnd(); ++it)
    {
        const QString& paramName = it.key();
        const UnifyParam& config = it.value();

        // (您的日志已证明此逻辑有效)
        bool isBooleanMode = config.unit.isEmpty();

        if (isBooleanMode)
        {
            // --- A. 布尔模式 (单行布局) ---
            QString labelText = QString("%1 (%2)").arg(config.label).arg(config.unit.isEmpty() ? "N/A" : config.unit);
            QLabel* mainLabel = new QLabel(labelText, this);
            QLabel* valueLabel = new QLabel("期望值:", this);
            QLineEdit* valueEdit = new QLineEdit(QString::number(config.value, 'f', 3), this);
            valueEdit->setValidator(validator);
            m_rangeLineEditMap.insert(paramName + "_Value", valueEdit);

            QCheckBox* visibleCheckBox = new QCheckBox(this);
            visibleCheckBox->setChecked(config.visible);
            visibleCheckboxesMap.insert(paramName, visibleCheckBox);
            QLabel* visibleLabel = new QLabel("可见:", this);

            gridLayout->addWidget(mainLabel, row, 0, Qt::AlignRight | Qt::AlignVCenter);
            gridLayout->addWidget(valueLabel, row, 1, Qt::AlignRight | Qt::AlignVCenter);
            gridLayout->addWidget(valueEdit, row, 2, Qt::AlignLeft | Qt::AlignVCenter);
            gridLayout->addWidget(visibleLabel, row, 5, Qt::AlignRight | Qt::AlignVCenter);
            gridLayout->addWidget(visibleCheckBox, row, 6, Qt::AlignLeft | Qt::AlignVCenter);

            row++;
        }
        else
        {
            // --- B. 范围模式 (双行布局) ---
            QString labelText = QString("%1 (%2)").arg(config.label).arg(config.unit.isEmpty() ? "N/A" : config.unit);
            QLabel* mainLabel = new QLabel(labelText, this);
            QCheckBox* visibleCheckBox = new QCheckBox(this);
            visibleCheckBox->setChecked(config.visible);
            visibleCheckboxesMap.insert(paramName, visibleCheckBox);
            QLabel* visibleLabel = new QLabel("可见:", this);

            // 第一行
            QLabel* upperLabel = new QLabel("上限:", this);
            QLineEdit* upperLimitEdit = new QLineEdit(QString::number(config.upperLimit, 'f', 3), this);
            upperLimitEdit->setValidator(validator);
            m_rangeLineEditMap.insert(paramName + "_Upper", upperLimitEdit);

            // 【恢复】补偿值控件
            QLabel* compLabel1 = new QLabel("补偿值:", this);
            QLineEdit* upfixEdit = new QLineEdit(QString::number(config.upfix, 'f', 3), this);
            upfixEdit->setValidator(validator);
            m_rangeLineEditMap.insert(paramName + "_Upfix", upfixEdit);
            compLabel1->setVisible(isManufacturer); // 【恢复】可见性控制
            upfixEdit->setVisible(isManufacturer); // 【恢复】可见性控制


            // 第二行
            QLabel* lowerLabel = new QLabel("下限:", this);
            QLineEdit* lowerLimitEdit = new QLineEdit(QString::number(config.lowerLimit, 'f', 3), this);
            lowerLimitEdit->setValidator(validator);
            m_rangeLineEditMap.insert(paramName + "_Lower", lowerLimitEdit);

            // 【恢复】补偿值控件
            QLabel* compLabel2 = new QLabel("补偿值:", this);
            QLineEdit* lowfixEdit = new QLineEdit(QString::number(config.lowfix, 'f', 3), this);
            lowfixEdit->setValidator(validator);
            m_rangeLineEditMap.insert(paramName + "_Lowfix", lowfixEdit);
            compLabel2->setVisible(isManufacturer); // 【恢复】可见性控制
            lowfixEdit->setVisible(isManufacturer); // 【恢复】可见性控制


            // 布局
            gridLayout->addWidget(mainLabel, row, 0, 2, 1, Qt::AlignRight | Qt::AlignVCenter);
            gridLayout->addWidget(visibleLabel, row, 5, 2, 1, Qt::AlignRight | Qt::AlignVCenter);
            gridLayout->addWidget(visibleCheckBox, row, 6, 2, 1, Qt::AlignLeft | Qt::AlignVCenter);

            // 第一行布局
            gridLayout->addWidget(upperLabel, row, 1, Qt::AlignRight | Qt::AlignVCenter);
            gridLayout->addWidget(upperLimitEdit, row, 2);
            gridLayout->addWidget(compLabel1, row, 3, Qt::AlignRight | Qt::AlignVCenter); // 【恢复】添加到布局
            gridLayout->addWidget(upfixEdit, row, 4); // 【恢复】添加到布局

            row++;

            // 第二行布局
            gridLayout->addWidget(lowerLabel, row, 1, Qt::AlignRight | Qt::AlignVCenter);
            gridLayout->addWidget(lowerLimitEdit, row, 2);
            gridLayout->addWidget(compLabel2, row, 3, Qt::AlignRight | Qt::AlignVCenter); // 【恢复】添加到布局
            gridLayout->addWidget(lowfixEdit, row, 4); // 【恢复】添加到布局

            row++;
        }

        // 4. 添加分隔线
        if (it + 1 != allParams.constEnd()) {
            QFrame* line = new QFrame(this);
            line->setFrameShape(QFrame::HLine);
            line->setFrameShadow(QFrame::Sunken);
            gridLayout->addWidget(line, row++, 0, 1, 7); // 跨越所有7列
        }
    }

    // 5. 调整列拉伸比例
    // Col 0: 主标签, Col 1: 值标签, Col 2: 值Edit, 
    // Col 3: 补偿标签, Col 4: 补偿Edit, 
    // Col 5: 可见标签, Col 6: 可见Check, Col 7: 额外空间

    // 【修改】调整拉伸因子以适应恢复的补偿值列
    gridLayout->setColumnStretch(0, 4); // 主标签
    gridLayout->setColumnStretch(1, 1); // 值标签
    gridLayout->setColumnStretch(2, 2); // 值输入框
    gridLayout->setColumnStretch(3, 1); // 补偿标签
    gridLayout->setColumnStretch(4, 2); // 补偿输入框
    gridLayout->setColumnStretch(5, 1); // 可见标签
    gridLayout->setColumnStretch(6, 0); // 可见 CheckBox
    gridLayout->setColumnStretch(7, 1); // 额外空间 (减少)

    // 6. 添加保存按钮
    QPushButton* saveButton = new QPushButton("保存配置", this);
    saveButton->setMinimumHeight(30);
    QHBoxLayout* buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(saveButton);
    mainLayout->addLayout(buttonLayout);
    connect(saveButton, &QPushButton::clicked, this, &ParaWidget::onSaveClicked);

    qDebug() << "setupRangeTab_EX: m_rangeLineEditMap size AFTER population:" << m_rangeLineEditMap.size();
    qDebug() << "============ setupRangeTab_EX: END ============";
}


void ParaWidget::onSaveClicked()
{
    qDebug() << "\n============ onSaveClicked: START ============"; // 添加日志标识
    // 1. 安全检查
    if (m_cam == nullptr || m_rangeSettings == nullptr) {
        QMessageBox::critical(this, "错误", "内部组件未初始化，无法保存。");
        return;
    }

    AllUnifyParams updatedParams = m_cam->unifyParams;
    bool hasInvalidInput = false;

    // --- 检查 CheckBox Map 状态 ---
    if (!m_rangeCheckboxes.contains("Visible")) {
        qWarning() << "onSaveClicked: FATAL: m_rangeCheckboxes does not contain 'Visible' key!";
        QMessageBox::critical(this, "内部错误", "界面控件映射丢失 (Visible)，无法保存。");
        return;
    }
    QMap<QString, QCheckBox*>& visibleCheckboxesMap = m_rangeCheckboxes["Visible"];
    if (visibleCheckboxesMap.isEmpty() && !updatedParams.isEmpty()) { // Check against updatedParams size
        qWarning() << "onSaveClicked: FATAL: m_rangeCheckboxes['Visible'] is EMPTY. UI setup likely failed or map cleared.";
        QMessageBox::critical(this, "内部错误", "无法找到界面可见性控件，请重新打开参数窗口。");
        return;
    }
    // --- 检查结束 ---

    // 检查 LineEdit Map
    if (m_rangeLineEditMap.isEmpty() && !updatedParams.isEmpty()) { // Check against updatedParams size
        qWarning() << "onSaveClicked: m_rangeLineEditMap is EMPTY. UI setup likely failed or map cleared.";
        QMessageBox::critical(this, "内部错误", "无法找到界面输入控件，请重新打开参数窗口。");
        return;
    }


    // 3. 遍历副本，从UI控件中读取数据进行更新
    for (auto it = updatedParams.begin(); it != updatedParams.end(); ++it)
    {
        const QString& paramName = it.key();
        UnifyParam& config = it.value(); // 获取引用以便修改
        qDebug() << "\n--- Checking Param:" << config.label << "---"; // 添加日志标识

        // --- 更新可见性 ---
        if (!visibleCheckboxesMap.contains(paramName)) {
            qWarning() << "onSaveClicked: Checkbox key for '" << paramName << "' not found in map. Skipping visibility update.";
        }
        else {
            QCheckBox* cb = visibleCheckboxesMap.value(paramName);
            if (cb == nullptr) {
                qWarning() << "onSaveClicked: Checkbox pointer for '" << paramName << "' is unexpectedly NULL in map. Skipping visibility update.";
            }
            else {
                bool isChecked = cb->isChecked(); // 读取 CheckBox 状态到临时变量
                config.visible = isChecked;       // 赋值给 config
                // --- 【新增调试日志】 ---
                qDebug() << "  -> Read CheckBox state for '" << paramName << "' : " << isChecked << "(Pointer:" << cb << ")";
                // ---
            }
        }
        // --- 可见性更新结束 ---

        // --- 读取数值 (Bug 1 已修复：不再检查 visible 状态) ---
        bool isBooleanMode = config.unit.isEmpty();
        qDebug() << "  -> Determined Mode based on unit.isEmpty():" << (isBooleanMode ? "Boolean" : "Range"); // 添加日志标识

        if (isBooleanMode)
        {
            // --- 布尔模式 ---
            qDebug() << "  -> Mode Processing: Boolean"; // 添加日志标识
            QString key = paramName + "_Value";
            QLineEdit* valueEdit = m_rangeLineEditMap.value(key);

            if (valueEdit == nullptr) {
                qWarning() << "onSaveClicked: FATAL: QLineEdit* for Boolean key '" << key << "' is NULL.";
                hasInvalidInput = true;
                break;
            }

            if (valueEdit->hasAcceptableInput()) {
                config.value = valueEdit->text().toDouble();
            }
            else {
                qWarning() << "onSaveClicked: INVALID INPUT for key '" << key << "'.";
                valueEdit->setFocus();
                hasInvalidInput = true;
                break;
            }
        }
        else // 范围模式
        {
            qDebug() << "  -> Mode Processing: Range"; // 添加日志标识
            // --- 上限/补偿 ---
            QString upperKey = paramName + "_Upper";
            QString upfixKey = paramName + "_Upfix";
            QLineEdit* upperEdit = m_rangeLineEditMap.value(upperKey);
            QLineEdit* upfixEdit = m_rangeLineEditMap.value(upfixKey);

            if (upperEdit == nullptr || upfixEdit == nullptr) {
                qWarning() << "onSaveClicked: FATAL: QLineEdit* for Upper/Upfix is NULL.";
                hasInvalidInput = true;
                break;
            }

            if (!upperEdit->hasAcceptableInput() || !upfixEdit->hasAcceptableInput()) {
                qWarning() << "onSaveClicked: INVALID INPUT for Upper/Upfix.";
                if (!upperEdit->hasAcceptableInput()) upperEdit->setFocus(); else upfixEdit->setFocus();
                hasInvalidInput = true;
                break;
            }

            // --- 下限/补偿 ---
            QString lowerKey = paramName + "_Lower";
            QString lowfixKey = paramName + "_Lowfix";
            QLineEdit* lowerEdit = m_rangeLineEditMap.value(lowerKey);
            QLineEdit* lowfixEdit = m_rangeLineEditMap.value(lowfixKey);

            if (lowerEdit == nullptr || lowfixEdit == nullptr) {
                qWarning() << "onSaveClicked: FATAL: QLineEdit* for Lower/Lowfix is NULL.";
                hasInvalidInput = true;
                break;
            }

            if (!lowerEdit->hasAcceptableInput() || !lowfixEdit->hasAcceptableInput()) {
                qWarning() << "onSaveClicked: INVALID INPUT for Lower/Lowfix.";
                if (!lowerEdit->hasAcceptableInput()) lowerEdit->setFocus(); else lowfixEdit->setFocus();
                hasInvalidInput = true;
                break;
            }

            // 只有全部通过才赋值
            config.upperLimit = upperEdit->text().toDouble();
            config.upfix = upfixEdit->text().toDouble();
            config.lowerLimit = lowerEdit->text().toDouble();
            config.lowfix = lowfixEdit->text().toDouble();
        }
        // --- 数值读取结束 ---
    }


    if (hasInvalidInput) {
        QMessageBox::warning(this, "保存失败", "存在无效的参数输入或内部控件错误，请检查。");
        return;
    }

    // --- 【新增调试日志】 ---
    qDebug() << "\n============ onSaveClicked: Before Commit ============";
    qDebug() << "Final state of 'updatedParams' before assigning to m_cam->unifyParams:";
    // 打印几个关键参数的 visible 状态
    if (updatedParams.contains("Pin_C")) qDebug() << "  -> Pin_C Visible:" << updatedParams["Pin_C"].visible;
    if (updatedParams.contains("isHaveBpln")) qDebug() << "  -> isHaveBpln Visible:" << updatedParams["isHaveBpln"].visible;
    // 您可以根据需要添加更多参数
    // ---

    // 5. 应用更改到内存
    m_cam->unifyParams = updatedParams;
    qDebug() << "onSaveClicked: Committed updatedParams to m_cam->unifyParams."; // 添加日志标识

    // 6. 调用异步保存
    saveParamsToJsonFile(updatedParams);
}


/**
 * @brief 【最终异步版】将参数保存到 JSON 文件，并自动处理UI反馈。
 *
 * 此函数启动一个后台任务。任务完成后，会自动在主线程弹出
 * 成功或失败的提示框。
 *
 * @param paramsToSave 需要保存的参数数据。
 */
void ParaWidget::saveParamsToJsonFile(const AllUnifyParams& paramsToSave)
{
    // 1. 路径解析和目录创建 (此逻辑已验证无误)
    const QString relativePath = m_cam->rangepath;
    if (relativePath.isEmpty()) {
        qWarning() << "Save Failed: Config file path is empty.";
        return ;
    }
    QDir appDir(QCoreApplication::applicationDirPath());
    QString absolutePath = QFileInfo(appDir.filePath(relativePath)).canonicalFilePath();
    qDebug() << "Sync Save: Attempting to save to:" << absolutePath;

    QDir fileDir = QFileInfo(absolutePath).dir();
    if (!fileDir.exists() && !fileDir.mkpath(".")) {
        qWarning() << "Sync Save: CRITICAL: Failed to create directory path:" << fileDir.path();
        return ;
    }

    // 2. 【核心修改】序列化数据到 QJsonObject，并使用中文键名
    qDebug() << "Sync Save: Serializing data with Chinese keys...";
    QJsonObject rootObject;
    for (auto it = paramsToSave.constBegin(); it != paramsToSave.constEnd(); ++it)
    {
        const UnifyParam& item = it.value();
        QJsonObject paramJson;

        // --- 通用字段 ---
        paramJson["单位"] = item.unit;
        paramJson["可见"] = item.visible; // JSON 会自动处理 bool 值
        paramJson["映射变量"] = item.label;
        paramJson["检测"] = item.check;

        // --- 根据模式写入特定字段 ---
        if (item.need_value) {
            // 布尔模式
            // 注意: 您的 UnifyParam 结构体中没有 "布尔值" 字段，这里我们假设用 item.value 来表示
            // 如果 item.value 是 1.0 代表 true, 0.0 代表 false
            paramJson["布尔值"] = (item.value == 1.0);
        }
        else {
            // 范围模式
            paramJson["上限"] = item.upperLimit;
            paramJson["下限"] = item.lowerLimit;
            paramJson["上限补偿值"] = item.upfix;
            paramJson["下限补偿值"] = item.lowfix;
            paramJson["标定值"] = item.value;
        }

        rootObject[it.key()] = paramJson;
    }

    // 3. 使用 QFile 直接写入文件 (此逻辑已验证无误)
    QJsonDocument saveDoc(rootObject);
    QFile saveFile(absolutePath);

    if (!saveFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        qWarning() << "Sync Save: FAILED to open file for writing!";
        qWarning() << "  -> Error:" << saveFile.errorString();
        return ;
    }

    qint64 bytesWritten = saveFile.write(saveDoc.toJson(QJsonDocument::Indented));
    saveFile.close();

    if (bytesWritten > 0) {
        qDebug() << "Sync Save: Successfully wrote" << bytesWritten << "bytes to file.";
        return ;
    }
    else {
        qWarning() << "Sync Save: FAILED to write data to opened file.";
        qWarning() << "  -> Error:" << saveFile.errorString();
        return ;
    }
}

void ParaWidget::setupCameralTab(QTabWidget* tabWidget)
{
    if (!m_cameralSettings) {
        qWarning() << "m_cameralSettings is not initialized!";
        return;
    }

    QWidget* cameralTab = new QWidget();
    QVBoxLayout* mainLayout = new QVBoxLayout(cameralTab);
    QFormLayout* formLayout = new QFormLayout();

    // 使用 QWidget* 显式保存控件指针
    QMap<QString, QWidget*> controlMap;

    CamParamControll& params = m_cameralSettings->getParameters();

    // 图像宽度
    QSpinBox* widthBox = new QSpinBox();
    widthBox->setRange(0, 8192);
    widthBox->setValue(params.width);
    formLayout->addRow("图像宽度:", widthBox);
    controlMap["width"] = widthBox;

    // 图像高度
    QSpinBox* heightBox = new QSpinBox();
    heightBox->setRange(0, 8192);
    heightBox->setValue(params.height);
    formLayout->addRow("图像高度:", heightBox);
    controlMap["height"] = heightBox;

    // 图像偏移X
    QSpinBox* offsetXBox = new QSpinBox();
    offsetXBox->setRange(0, 8192);
    offsetXBox->setValue(params.offsetx);
    formLayout->addRow("图像偏移X:", offsetXBox);
    controlMap["offsetx"] = offsetXBox;

    // 图像偏移Y
    QSpinBox* offsetYBox = new QSpinBox();
    offsetYBox->setRange(0, 8192);
    offsetYBox->setValue(params.offsety);
    formLayout->addRow("图像偏移Y:", offsetYBox);
    controlMap["offsety"] = offsetYBox;

    // 增益
    QDoubleSpinBox* gainBox = new QDoubleSpinBox();
    gainBox->setRange(0, 20);
    gainBox->setDecimals(3);
    gainBox->setValue(params.gain);
    gainBox->setSingleStep(1);
    formLayout->addRow("增益:", gainBox);
    controlMap["gain"] = gainBox;
    gainBox->setEnabled(false);
    // 曝光时间
    QDoubleSpinBox* exposureBox = new QDoubleSpinBox();
    exposureBox->setRange(0.0, 5000.0);
    exposureBox->setDecimals(3);
    exposureBox->setSingleStep(50.0);
    exposureBox->setValue(params.exposure);
    formLayout->addRow("曝光时间:", exposureBox);
    controlMap["exposure"] = exposureBox;

    // gama值
    QDoubleSpinBox* gamaBox = new QDoubleSpinBox();
    gamaBox->setRange(0.0, 6.0);
    gamaBox->setDecimals(3);
    gamaBox->setValue(params.gama);
    formLayout->addRow("gama值:", gamaBox);
    controlMap["gama"] = gamaBox;

    // 帧率
    QSpinBox* fpsBox = new QSpinBox();
    fpsBox->setRange(1, 90);
    fpsBox->setValue(params.fps);
    formLayout->addRow("帧率:", fpsBox);
    controlMap["fps"] = fpsBox;

    // 保存按钮
    QPushButton* saveButton = new QPushButton("保存并应用");
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(saveButton);
    mainLayout->addStretch();

    tabWidget->addTab(cameralTab, "相机参数");

    connect(saveButton, &QPushButton::clicked, [=]() {

        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(cameralTab, "确认保存", "是否保存并应用当前相机参数？",
            QMessageBox::Yes | QMessageBox::No);

        if (reply != QMessageBox::Yes) {
            qDebug() << "用户取消了相机参数保存。";
            return;
        }

        CamParamControll& p = m_cameralSettings->getParameters();

        // 使用 qobject_cast 转换指针类型
        p.width = qobject_cast<QSpinBox*>(controlMap["width"])->value();
        p.height = qobject_cast<QSpinBox*>(controlMap["height"])->value();
        p.offsetx = qobject_cast<QSpinBox*>(controlMap["offsetx"])->value();
        p.offsety = qobject_cast<QSpinBox*>(controlMap["offsety"])->value();
        p.gain = qobject_cast<QDoubleSpinBox*>(controlMap["gain"])->value();
        p.exposure = qobject_cast<QDoubleSpinBox*>(controlMap["exposure"])->value();
        p.gama = qobject_cast<QDoubleSpinBox*>(controlMap["gama"])->value();
        p.fps = qobject_cast<QSpinBox*>(controlMap["fps"])->value();

        m_cameralSettings->change();
        m_cameralSettings->saveParamAsync();
        qDebug() << "相机参数已更新并应用";
        });
}

void ParaWidget::saveScaleParamsToFile(const AllUnifyParams& paramsToSave)
{
    qDebug() << "saveScaleParamsToFile: Starting background task...";
    QtConcurrent::run([this, params = paramsToSave]() {
        // =======================================================
        //  后台线程
        // =======================================================
        qDebug() << "Background Thread (Scale): Task started.";

        // 1. 路径解析和目录创建 (此部分不变)
        const QString relativePath = this->m_cam->rangepath;
        if (relativePath.isEmpty()) {
            qWarning() << "Background Thread (Scale): FAILED. Config file path is empty.";
            QMetaObject::invokeMethod(this, [this]() {
                QMessageBox::critical(this, "保存失败", "(Scale) 配置文件路径未设置。");
                }, Qt::QueuedConnection);
            return;
        }
        QDir appDir(QCoreApplication::applicationDirPath());
        QString absolutePath = QFileInfo(appDir.filePath(relativePath)).canonicalFilePath();
        QDir fileDir = QFileInfo(absolutePath).dir();
        if (!fileDir.exists() && !fileDir.mkpath(".")) {
            qWarning() << "Background Thread (Scale): FAILED. Could not create directory" << fileDir.path();
            QMetaObject::invokeMethod(this, [this]() {
                QMessageBox::critical(this, "保存失败", "(Scale) 无法创建目标目录，请检查权限。");
                }, Qt::QueuedConnection);
            return;
        }

        // 2. 【修复】序列化数据 (使用与 saveParamsToJsonFile 完全一致的中文键和格式)
        qDebug() << "Background Thread (Scale): Serializing data (using specified Chinese keys)...";
        QJsonObject rootObject;
        for (auto it = params.constBegin(); it != params.constEnd(); ++it)
        {
            const UnifyParam& item = it.value();
            QJsonObject paramJson;

            // --- 【核心】使用您JSON示例中的键和格式 ---
            paramJson["映射变量"] = item.label;
            paramJson["单位"] = item.unit;
            paramJson["检测"] = item.check ; // 字符串 "true" / "false"
            paramJson["可见"] = item.visible ; // 字符串 "true" / "false"
            paramJson["标定值"] = item.scaleFactor; // 所有项都有标定值

            // --- 根据 need_value 决定是布尔模式还是范围模式 ---
            if (item.need_value) {
                // A. 布尔模式 (如 isHaveNpin)
                paramJson["布尔值"] = (item.value == 1.0); // true / false
            }
            else {
                // B. 范围模式 (如 n_pin_H)
                paramJson["上限"] = item.upperLimit;
                paramJson["下限"] = item.lowerLimit;
                paramJson["上限补偿值"] = item.upfix;
                paramJson["下限补偿值"] = item.lowfix;
            }
            // (我们不保存 "期望值" "检测总数" "NG总数" 等，因为您的JSON示例中没有)

            rootObject[it.key()] = paramJson;
        }
        qDebug() << "Background Thread (Scale): Serialization complete.";


        // 3. 使用 QFile 直接写入文件 (此部分不变)
        bool success = false;
        QJsonDocument saveDoc(rootObject);
        QFile saveFile(absolutePath);

        if (!saveFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
            qWarning() << "Background Thread (Scale): FAILED to open file for writing!";
            qWarning() << "  -> Error:" << saveFile.errorString();
        }
        else {
            qint64 bytesWritten = saveFile.write(saveDoc.toJson(QJsonDocument::Indented));
            saveFile.close();
            success = (bytesWritten > 0);
        }

        // 4. 将最终的弹窗任务发送回主线程执行 (此部分不变)
        qDebug() << "Background Thread (Scale): Posting result (" << success << ") back to main thread.";
        QMetaObject::invokeMethod(this, [this, success]() {
            qDebug() << "Main Thread: Received scale save result:" << success;
            if (success) {
                QMessageBox::information(this, "保存成功", "标定参数已成功写入配置文件。");
            }
            else {
                QMessageBox::critical(this, "保存失败", "(Scale) 写入配置文件时发生错误。");
            }
            }, Qt::QueuedConnection);
        });
}


void ParaWidget::onScaleSaveClicked()
{
    qDebug() << "\n============ onScaleSaveClicked: START ============";
    // 1. 安全检查
    if (m_cam == nullptr) {
        qWarning() << "onScaleSaveClicked: Aborted. m_cam is null.";
        QMessageBox::critical(this, "错误", "内部组件未初始化，无法保存。");
        return;
    }

    // 2. 获取参数的可编辑副本
    AllUnifyParams updatedParams = m_cam->unifyParams;
    bool hasInvalidInput = false;

    // --- 【新增】检查“可见” CheckBox Map 状态 ---
    if (!m_rangeCheckboxes.contains("Visible")) {
        qWarning() << "onScaleSaveClicked: FATAL: m_rangeCheckboxes does not contain 'Visible' key! Cannot preserve visibility state.";
        // 即使没有Visible Map，仍然尝试保存 Check 和 ScaleFactor
        // return; // 或者根据业务逻辑决定是否中断
    }
    QMap<QString, QCheckBox*>& visibleCheckboxesMap = m_rangeCheckboxes["Visible"];
    // 不检查 visibleCheckboxesMap 是否为空，因为即使为空也应尝试保存 scale 数据
    // --- 检查结束 ---


    qDebug() << "onScaleSaveClicked: Using m_scaleLineEditMap and m_scaleCheckboxes.";

    // 3. 遍历副本，从UI控件中读取数据进行更新
    for (auto it = updatedParams.begin(); it != updatedParams.end(); ++it)
    {
        const QString& paramName = it.key();
        UnifyParam& config = it.value(); // 获取引用

        // --- 【核心修复】首先，从 Range Tab 读取并更新 visible 状态 ---
        if (visibleCheckboxesMap.contains(paramName)) {
            QCheckBox* visCb = visibleCheckboxesMap.value(paramName);
            if (visCb != nullptr) {
                config.visible = visCb->isChecked();
                qDebug() << "  -> Preserving visible state for '" << paramName << "' : " << config.visible;
            }
            else {
                qWarning() << "  -> Warning: Visible checkbox pointer for '" << paramName << "' is NULL in map.";
            }
        }
        else {
            qWarning() << "  -> Warning: Visible checkbox key for '" << paramName << "' not found in map. Preserving existing visible state:" << config.visible;
        }
        // --- 可见状态保留结束 ---


        // A. 读取 "启用检测" (check)
        QCheckBox* checkCheckBox = m_scaleCheckboxes.value(paramName);
        if (checkCheckBox) {
            config.check = checkCheckBox->isChecked();
            qDebug() << "  -> Reading CheckBox state for '" << paramName << "' : " << config.check;
        }
        else {
            qWarning() << "onScaleSaveClicked: CheckBox for" << paramName << "is NULL!";
            // 根据业务逻辑决定是否中断或跳过
        }

        // B. 读取 "标定参数" (scaleFactor)
        QLineEdit* scaleEdit = m_scaleLineEditMap.value(paramName);
        if (scaleEdit) {
            if (scaleEdit->hasAcceptableInput()) {
                config.scaleFactor = scaleEdit->text().toDouble();
                qDebug() << "  -> Reading ScaleFactor for '" << paramName << "' : " << config.scaleFactor;
            }
            else {
                qWarning() << "onScaleSaveClicked: Invalid scale input for" << config.label;
                scaleEdit->setFocus();
                hasInvalidInput = true;
                break; // 中断循环
            }
        }
        else {
            qWarning() << "onScaleSaveClicked: LineEdit for" << paramName << "is NULL!";
            // 根据业务逻辑决定是否中断或跳过
        }
    }

    qDebug() << "\n============ onScaleSaveClicked: END LOOP ============";

    if (hasInvalidInput) {
        QMessageBox::warning(this, "保存失败", "存在无效的标定参数输入，请检查。");
        return;
    }

    // 5. 应用更改到内存
    qDebug() << "onScaleSaveClicked: All inputs valid. Committing to m_cam->unifyParams.";
    m_cam->unifyParams = updatedParams; // 现在 updatedParams 包含了正确的 visible 状态

    // 6. 【重要】调用专属的异步保存函数
    qDebug() << "onScaleSaveClicked: Calling asynchronous save function (saveScaleParamsToFile)...";
    saveScaleParamsToFile(updatedParams); // 使用专属保存函数
}

void ParaWidget::setupAlgorithmTab(QTabWidget* tabWidget)
{
    QWidget* algoPage = new QWidget(tabWidget);
    QHBoxLayout* mainLayout = new QHBoxLayout(algoPage);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

      
    QWidget* leftPanelWidget = m_algoSettings->createLeftPanel(algoPage);  // 虚函数生成面板并绑定事件

    leftPanelWidget->setFixedWidth(280);
    leftPanelWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    mainLayout->addWidget(leftPanelWidget);



    // 图像窗口初始化
    imageViewer_algo = new ImageViewerWindow(algoPage);
    imageViewer_algo->setMinimumSize(100, 100);
    imageViewer_algo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QPixmap blackPixmap(920, 880);  // 右侧初始化黑屏
    blackPixmap.fill(Qt::black);
    imageViewer_algo->setPixmap(blackPixmap);

    mainLayout->addWidget(imageViewer_algo, 1);
    tabWidget->addTab(algoPage, "算法");
}


void ParaWidget::setupDebugTab(QTabWidget* tabWidget)
{
    QWidget* debugPage = new QWidget(tabWidget);
    QHBoxLayout* mainHorizontalLayout = new QHBoxLayout(debugPage);
    mainHorizontalLayout->setContentsMargins(0, 0, 0, 0);
    mainHorizontalLayout->setSpacing(0);

    // === 左侧参数区域的容器 ===
    QWidget* leftPanelWidget = new QWidget(debugPage);
    QVBoxLayout* leftParamsLayout = new QVBoxLayout(leftPanelWidget);
    leftParamsLayout->setContentsMargins(5, 5, 5, 5);
    leftParamsLayout->setSpacing(5);

    // === 标定 + 缩放系数 ===
    QHBoxLayout* calibLayout = new QHBoxLayout;
    QPushButton* calibButton = new QPushButton("标定", debugPage);
    QLineEdit* scaleFactorEdit = new QLineEdit(debugPage);
    scaleFactorEdit->setFixedSize(100, 30);
    scaleFactorEdit->setPlaceholderText("缩放系数");
    scaleFactorEdit->setText(QString::number(DI->scaleFactor));
    calibButton->setFixedSize(160, 30);
    calibLayout->addWidget(calibButton);
    calibLayout->addWidget(scaleFactorEdit);
    calibLayout->addStretch();
    leftParamsLayout->addLayout(calibLayout);

    // === 等级 ===
    QHBoxLayout* flagLayout = new QHBoxLayout;
    QPushButton* flagButton = new QPushButton("存图等级设置", debugPage);
    QComboBox* flagCombo = new QComboBox(debugPage);
    for (int i = 0; i <= 3; ++i) {
        flagCombo->addItem(QString::number(i), i);
    }
    flagCombo->setFixedSize(100, 30);
    flagCombo->setCurrentIndex(DI->saveflag);
    flagButton->setFixedSize(160, 30);
    flagLayout->addWidget(flagButton);
    flagLayout->addWidget(flagCombo);
    flagLayout->addStretch();
    leftParamsLayout->addLayout(flagLayout);

    // === 角度设置 ===
    QHBoxLayout* angleLayout = new QHBoxLayout;
    QPushButton* angleButton = new QPushButton("角度设置", debugPage);
    QLineEdit* angleEdit = new QLineEdit(debugPage);
    angleEdit->setFixedSize(100, 30);
    angleEdit->setPlaceholderText("角度（-180~180）");
    angleEdit->setText(QString::number(DI->Angle));
    angleButton->setFixedSize(160, 30);
    angleLayout->addWidget(angleButton);
    angleLayout->addWidget(angleEdit);
    angleLayout->addStretch();
    leftParamsLayout->addLayout(angleLayout);

    // === 其他按钮 ===
    QSize buttonSize(160, 30);
    QPushButton* roiButton = new QPushButton("ROI", debugPage);
    QPushButton* recipeButton = new QPushButton("配方", debugPage);
    QPushButton* choosepicButton = new QPushButton("选择图片", debugPage);
    QPushButton* learnButton = new QPushButton("学习", debugPage);
    QPushButton* checkButton = new QPushButton("复检", debugPage);

    roiButton->setFixedSize(buttonSize);
    recipeButton->setFixedSize(buttonSize);
    choosepicButton->setFixedSize(buttonSize);
    learnButton->setFixedSize(buttonSize);
    checkButton->setFixedSize(buttonSize);

    QPushButton *shieldButton = new QPushButton(debugPage);
    shieldButton->setCheckable(true);
    shieldButton->setFixedSize(buttonSize);
    shieldButton->setChecked(DI->Shield);
    shieldButton->setText(DI->Shield ? "屏蔽(开启)" : "屏蔽(关闭)");

    QPushButton* emptyOKButton = new QPushButton(debugPage);
    emptyOKButton->setCheckable(true);
    emptyOKButton->setFixedSize(buttonSize);
    emptyOKButton->setChecked(DI->EmptyIsOK);
    emptyOKButton->setText(DI->EmptyIsOK ? "空料OK(开启)" : "空料OK(关闭)");

    leftParamsLayout->addWidget(roiButton);
    leftParamsLayout->addWidget(recipeButton);
    leftParamsLayout->addWidget(choosepicButton);
    leftParamsLayout->addWidget(checkButton);
    leftParamsLayout->addWidget(learnButton);
    leftParamsLayout->addWidget(shieldButton);
    leftParamsLayout->addWidget(emptyOKButton);

    roiButton->setVisible(false);
    recipeButton->setVisible(false);

    QPushButton* saveButton = new QPushButton("保存修改", debugPage);
    saveButton->setFixedSize(160, 40);
    leftParamsLayout->addWidget(saveButton, 0, Qt::AlignLeft);
    leftParamsLayout->addStretch(1);

    leftPanelWidget->setFixedWidth(280);
    leftPanelWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    mainHorizontalLayout->addWidget(leftPanelWidget);

    // === 右侧图片显示窗口 ===
    imageViewer = new ImageViewerWindow(debugPage);
    imageViewer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    imageViewer->setMinimumSize(100, 100);

    QPixmap blackPixmap(1200 - 280, 880);
    blackPixmap.fill(Qt::black);
    imageViewer->setPixmap(blackPixmap);

    mainHorizontalLayout->addWidget(imageViewer, 1);

    tabWidget->addTab(debugPage, "调试");

    calibButton->setFocusPolicy(Qt::NoFocus);
    calibButton->setStyleSheet("QPushButton { background-color: none; border: none; }");
    flagButton->setFocusPolicy(Qt::NoFocus);
    flagButton->setStyleSheet("QPushButton { background-color: none; border: none; }");
    angleButton->setFocusPolicy(Qt::NoFocus);
    angleButton->setStyleSheet("QPushButton { background-color: none; border: none; }");

    // 信号连接
    connect(calibButton, &QPushButton::clicked, this, &ParaWidget::onCalibClicked);
    connect(roiButton, &QPushButton::clicked, this, &ParaWidget::onROIClicked);
    connect(recipeButton, &QPushButton::clicked, this, &ParaWidget::onRecipeClicked);
    connect(choosepicButton, &QPushButton::clicked, this, &ParaWidget::onChooseImageClicked);
    connect(checkButton, &QPushButton::clicked, this, [this]() {
        qDebug() << "复检功能触发！";
        emit Check();
        });
    connect(learnButton, &QPushButton::clicked, this, [this]() {
        qDebug() << "学习功能触发！";
        emit Learn();
        });

    connect(saveButton, &QPushButton::clicked, this, [=]() {
        QString scaleText = scaleFactorEdit->text();
        QString angleText = angleEdit->text();
        int flagValue = flagCombo->currentData().toInt(); // 取下拉框值

        bool ok1 = false, ok2 = false;
        double scaleValue = scaleText.toDouble(&ok1);
        float angleValue = angleText.toFloat(&ok2);

        if (!ok1) {
            QMessageBox::warning(this, "输入错误", "缩放系数必须是浮点数！");
            return;
        }
        if (!ok2 || angleValue < -180 || angleValue > 180) {
            QMessageBox::warning(this, "输入错误", "角度必须是整数（-180 到 180）！");
            return;
        }

        QMessageBox::StandardButton reply = QMessageBox::question(
            this, "确认保存", "确定要保存当前设置吗？",
            QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::Yes) {
            DI->scaleFactor = scaleValue;
            DI->Angle = angleValue;
            DI->saveflag = flagValue;
            if (m_cam->indentify == "Carrier_NaYin") LearnPara::inParam1.angleNum = angleValue;
            if (m_cam->indentify == "NaYin") LearnPara::inParam2.angleNum = angleValue;
            if (m_cam->indentify == "Top") LearnPara::inParam3.angleNum = angleValue;
            if (m_cam->indentify == "Side") LearnPara::inParam4.imgAngleNum = angleValue;
            if (m_cam->indentify == "Pin") LearnPara::inParam5.angleNum = angleValue;
            emit SaveDebugInfo(DI);
        }
        });

    connect(shieldButton, &QPushButton::toggled, this, [=](bool checked) {
        DI->Shield = checked;
        shieldButton->setText(checked ? "屏蔽(开启)" : "屏蔽(关闭)");
        qDebug() << "DI->Shield 设置为：" << checked;
        });
    connect(emptyOKButton, &QPushButton::toggled, this, [=](bool checked) {
        DI->EmptyIsOK = checked;
        emptyOKButton->setText(checked ? "空料OK(开启)" : "空料OK(关闭)");
        qDebug() << "EmptyIsOK 设置为：" << checked;
        });
}


void ParaWidget::onChooseImageClicked()
{
    QString defaultPath = m_cam->localpath;
    QString filePath = QFileDialog::getOpenFileName(
        this,
        "选择图像文件",
        defaultPath,
        "图像文件 (*.png *.jpg *.bmp *.jpeg *.tif *.tiff);;所有文件 (*.*)");

    if (filePath.isEmpty()) {
        return;
    }

   QString m_tempImagePath = filePath;
    qDebug() << "用户选择图片路径：" << filePath;

    cv::Mat image = cv::imread(filePath.toStdString());
    if (image.empty()) {
        QMessageBox::warning(this, "错误", "加载失败！");
        return;
    }

    // 创建 shared_ptr 持有独立图像数据
     std::shared_ptr<cv::Mat> imagePtr(new cv::Mat(image.clone()));
    emit ImageLoaded(imagePtr);
   

    QPixmap pixmap = convertMatToPixmap(*imagePtr);
    imageViewer->setPixmap(pixmap);
}



void ParaWidget::handleSettingsInitializationError(QTabWidget* tabWidget, const QString& tabName, const QString& errorMessage)
{
    qWarning() << "ParaWidget::handleSettingsInitializationError: " << errorMessage;
    QWidget* errorTab = new QWidget;
    tabWidget->addTab(errorTab, tabName + " (错误)");
    QVBoxLayout* errorLayout = new QVBoxLayout(errorTab);
    // 这里使用 this 作为 QLabel 的父对象，确保其生命周期与 ParaWidget 关联
    errorLayout->addWidget(new QLabel("错误：" + errorMessage, this));
}

void ParaWidget::displayNoParametersMessage(QVBoxLayout* mainLayout, const QString& settingType)
{
    // 创建一个 QLabel 显示提示信息
    QLabel* infoLabel = new QLabel(QString("未找到任何%1参数。请检查配置文件路径或格式是否正确。").arg(settingType), this);
    infoLabel->setStyleSheet("color: blue; font-size: 14px;"); 
    // 设置 QLabel 居中对齐
    infoLabel->setAlignment(Qt::AlignCenter);

    // 添加到主布局
    mainLayout->addWidget(infoLabel);

    // 添加一些弹性空间，确保信息在中间
    mainLayout->addStretch(1);
}

template<typename T>
QString ParaWidget::validateParametersRange(T* settingsManager, const QString& settingType) const
{
    if (!settingsManager) {
        return "内部错误：" + settingType + "参数管理器未初始化，无法进行校验。";
    }

    // 通过模板参数 T，我们可以调用 T 类型特有的方法来获取 Parameters
    const Parameters& currentParams = settingsManager->getParameters();

    for (auto projectIt = currentParams.detectionProjects.begin();
         projectIt != currentParams.detectionProjects.end();
         ++projectIt)
    {
        const QString& projectName = projectIt.key();
        const DetectionProject& project = projectIt.value();

        for (auto paramIt = project.params.begin();
             paramIt != project.params.end();
             ++paramIt)
        {
            const QString& paramName = paramIt.key();
            const ParamDetail& paramDetail = paramIt.value();

            if (!paramDetail.visible) continue;
            if (paramDetail.range.isEmpty()) continue;

            QString minStr;
            QString maxStr;
            int lastDashIndex = paramDetail.range.lastIndexOf('-');

            if (lastDashIndex == -1 || lastDashIndex == 0 || lastDashIndex == paramDetail.range.length() -1) {
                return QString("%1参数 '%2' (项目: %3) 的范围格式无效: '%4'。应为 'min-max'。")
                    .arg(settingType).arg(paramName).arg(projectName).arg(paramDetail.range);
            }

            minStr = paramDetail.range.left(lastDashIndex);
            maxStr = paramDetail.range.mid(lastDashIndex + 1);

            bool okMin, okMax;
            double minValDouble = minStr.toDouble(&okMin);
            double maxValDouble = maxStr.toDouble(&okMax);

            if (!okMin || !okMax) {
                return QString("%1参数 '%2' (项目: %3) 的范围值 '%4' 无法解析为数字。")
                    .arg(settingType).arg(paramName).arg(projectName).arg(paramDetail.range);
            }

            // 检查 minValDouble 是否大于 maxValDouble，这会导致校验逻辑混乱
            if (minValDouble > maxValDouble) {
                return QString("%1参数 '%2' (项目: %3) 的最小范围值 '%4' 大于最大范围值 '%5'。")
                    .arg(settingType).arg(paramName).arg(projectName)
                    .arg(QString::number(minValDouble, 'f', (paramDetail.type.toLower() == "int" ? 0 : 3)))
                    .arg(QString::number(maxValDouble, 'f', (paramDetail.type.toLower() == "int" ? 0 : 3)));
            }

            bool valueConversionOk;
            if (paramDetail.type.toLower() == "int") {
                long long value = paramDetail.value.toLongLong(&valueConversionOk);
                if (!valueConversionOk || value < static_cast<long long>(minValDouble) || value > static_cast<long long>(maxValDouble)) {
                    return QString("%1参数 '%2' (项目: %3) 的值 '%4' 超出范围 [%5 - %6] 或格式不正确。")
                        .arg(settingType).arg(paramName).arg(projectName).arg(paramDetail.value.toString())
                        .arg(QString::number(minValDouble, 'f', 0)).arg(QString::number(maxValDouble, 'f', 0));
                }
            } else if (paramDetail.type.toLower() == "float") {
                double value = paramDetail.value.toDouble(&valueConversionOk);
                if (!valueConversionOk || value < minValDouble || value > maxValDouble) {
                    return QString("%1参数 '%2' (项目: %3) 的值 '%4' 超出范围 [%5 - %6] 或格式不正确。")
                        .arg(settingType).arg(paramName).arg(projectName).arg(paramDetail.value.toString())
                        .arg(minValDouble).arg(maxValDouble);
                }
            } else {
                qDebug() << "Skipping range validation for non-numeric " << settingType << " type:" << paramName << " Type:" << paramDetail.type;
            }
        }
    }
    return QString(); // 所有校验通过，返回空字符串
}


// 3. 通用 QLineEdit 验证器设置 (paramNameForLog)
void ParaWidget::setupLineEditValidator(QLineEdit* valueEdit, const ParamDetail& paramDetail, const QString& paramNameForLog)
{
    // 使用 paramNameForLog 进行日志输出，而不是 paramDetail.name
    if (paramDetail.range.isEmpty()) {
        qDebug() << "Range is empty for param:" << paramNameForLog << ". No validator applied.";
        return;
    }

    QString minStr;
    QString maxStr;
    int lastDashIndex = paramDetail.range.lastIndexOf('-');

    if (lastDashIndex == -1 || lastDashIndex == 0 || lastDashIndex == paramDetail.range.length() -1) {
        qWarning() << "Range format invalid for param:" << paramNameForLog << " range:" << paramDetail.range;
        return;
    }

    minStr = paramDetail.range.left(lastDashIndex);
    maxStr = paramDetail.range.mid(lastDashIndex + 1);

    bool okMin, okMax;
    double minValDouble = minStr.toDouble(&okMin);
    double maxValDouble = maxStr.toDouble(&okMax);

    if (okMin && okMax) {
        QValidator* validator = nullptr;
        if (paramDetail.type.toLower() == "int") {
            int minInt = static_cast<int>(minValDouble);
            int maxInt = static_cast<int>(maxValDouble);
            validator = new QIntValidator(minInt, maxInt, this);
        } else if (paramDetail.type.toLower() == "float") {
            validator = new QDoubleValidator(minValDouble, maxValDouble, 3, this); // 3 为小数位数
            static_cast<QDoubleValidator*>(validator)->setNotation(QDoubleValidator::StandardNotation);
        }
        if (validator) {
            valueEdit->setValidator(validator);
        } else {
            qWarning() << "Failed to create validator for param:" << paramNameForLog << " type:" << paramDetail.type;
        }
    } else {
        qWarning() << "Failed to parse range values for param:" << paramNameForLog << " range:" << paramDetail.range;
    }
}

void ParaWidget::validateInputType(QLineEdit* lineEdit, const QString& type)
{
    if (type == "int") {
        QIntValidator* validator = new QIntValidator(lineEdit);
        lineEdit->setValidator(validator);
    }
    else if (type == "double" || type == "float") {
        QDoubleValidator* validator = new QDoubleValidator(lineEdit);
        // 使用C语言环境，以确保小数点为 '.'
        validator->setLocale(QLocale::C);
        lineEdit->setValidator(validator);
    }
    // 如果类型不是 int 或 double，则不设置验证器，允许自由输入
}

void ParaWidget::onCalibClicked()
{
    // TODO: 实现标定操作
    qDebug() << "标定按钮被点击";
}

void ParaWidget::onROIClicked()
{
    // TODO: 实现 ROI 操作
    qDebug() << "ROI 按钮被点击";
}

void ParaWidget::onRecipeClicked()
{
    // TODO: 实现配方操作
    qDebug() << "配方按钮被点击";
}



void ParaWidget::closeWindow()
{
    close(); // 关闭窗口
}
