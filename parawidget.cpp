#include "parawidget.h"
#include "role.h"
#include <QVBoxLayout>
#include <QTabWidget>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QIntValidator>
#include <QDoubleValidator>
#include <QScrollArea>

ParaWidget::ParaWidget(RangeClass* RC, CameralClass* CC, AlgoClass* AC, QWidget *parent)
    : QWidget{parent}
{
    // 设置窗口属性
    setWindowFlags(Qt::Window);
    setWindowTitle("参数设置");
    setAttribute(Qt::WA_DeleteOnClose);
    resize(800, 600);

    // 设置全局字体（放大一倍）
    QFont font = this->font();
    font.setPointSize(font.pointSize() * 2); // 字体放大两倍
    setFont(font);

    // 创建主布局
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // 创建选项卡控件
    QTabWidget* tabWidget = new QTabWidget;
    mainLayout->addWidget(tabWidget); // 将tabWidget添加到主布局

    // 赋值传入的参数管理类指针
    this->m_algoSettings = AC;
    this->m_rangeSettings = RC;
    this->m_cameralSettings = CC;

    // 设置各个选项卡
    setupRangeTab(tabWidget);    // 第一个选项卡：范围参数
    setupCameralTab(tabWidget);  // 第二个选项卡：相机参数
    setupAlgorithmTab(tabWidget); // 第三个选项卡：算法参数




    if (Role::GetCurrentRole() != "厂商" && Role::GetCurrentRole() != "机修")
    {
        // 隐藏第二个和第三个选项卡的标题
        tabWidget->setTabVisible(1, false); // 隐藏 CameralTab 的标题 (索引从0开始)
        tabWidget->setTabVisible(2, false); // 隐藏 AlgorithmTab 的标题
    }

    // 设置主布局
    setLayout(mainLayout);
}

ParaWidget::~ParaWidget()
{

}


void ParaWidget::setupRangeTab(QTabWidget* tabWidget)
{
    if (m_rangeSettings == nullptr) {
        qWarning() << "ParaWidget::setupRangeTab: m_rangeSettings is null. Cannot set up Range Tab.";
        QWidget* errorTab = new QWidget;
        tabWidget->addTab(errorTab, "范围参数 (错误)");
        QVBoxLayout* errorLayout = new QVBoxLayout(errorTab);
        errorLayout->addWidget(new QLabel("错误：范围参数管理器未正确初始化。", this));
        return;
    }

    QWidget* rangeTab = new QWidget;
    tabWidget->addTab(rangeTab, "范围参数");

    QVBoxLayout* mainLayout = new QVBoxLayout(rangeTab);
    QTabWidget* subTabWidget = new QTabWidget(this);
    mainLayout->addWidget(subTabWidget);

    // 清空 map，防止重复添加。此操作在 ParaWidget 构造函数中或加载参数前执行一次更合适。
    // 但是，为了确保当前函数的独立性，如果每次 setupRangeTab 都会重新生成UI，则清空是必要的。
    // 如果您在ParaWidget的构造函数中调用这些setup函数，并且ParaWidget只创建一次，那么可以考虑移除这里的clear。
    m_paramValueEdits.clear();
    m_paramCheckboxes.clear();

    const RangeParameters& allRangeParameters = m_rangeSettings->getRangeParameters();

    if (allRangeParameters.detectionProjects.isEmpty()) {
        QLabel* infoLabel = new QLabel("未找到任何范围参数，请检查配置文件。", this);
        infoLabel->setStyleSheet("color: blue;");
        mainLayout->addWidget(infoLabel);
        return; // 直接返回，不再使用 goto
    }

    for (auto projectIt = allRangeParameters.detectionProjects.begin();
         projectIt != allRangeParameters.detectionProjects.end();
         ++projectIt)
    {
        const QString& projectName = projectIt.key();
        const DetectionProject& detectionProject = projectIt.value();

        // 为每个项目创建一个包含滚动区域的子标签页
        QWidget* subTabContent = new QWidget; // 这个 widget 将作为滚动区域的内容
        QVBoxLayout* subTabContentLayout = new QVBoxLayout(subTabContent); // subTabContent 的布局
        QGridLayout* gridLayout = new QGridLayout;
        gridLayout->setVerticalSpacing(5); // 增加垂直间距
        gridLayout->setHorizontalSpacing(10); // 增加水平间距


        int row = 0;

        // 为当前项目创建内层 QMap
        m_paramValueEdits[projectName] = QMap<QString, QLineEdit*>();
        m_paramCheckboxes[projectName] = QMap<QString, QCheckBox*>();

        for (auto paramIt = detectionProject.params.begin();
             paramIt != detectionProject.params.end();
             ++paramIt)
        {
            const QString& paramName = paramIt.key();
            const ParamDetail& paramDetail = paramIt.value();

            if (!paramDetail.visible) {
                continue;
            }

            QLabel* nameLabel = new QLabel(paramName, this);
            QLineEdit* valueEdit = new QLineEdit(paramDetail.value.toString(), this);
            QLabel* unitLabel = new QLabel(QString("单位: %1").arg(paramDetail.unit), this);
            QLabel* rangeLabel = new QLabel("范围:", this);
            QLabel* rangeValueLabel = new QLabel(paramDetail.range, this);

            QLabel* checkLabel = new QLabel("检测:", this);
            QCheckBox* checkCheckBox = new QCheckBox(this);
            checkCheckBox->setChecked(paramDetail.check);

            // 设置控件样式和大小
            nameLabel->setMinimumWidth(100);
            valueEdit->setMaximumWidth(80);
            unitLabel->setMinimumWidth(30);
            rangeLabel->setMinimumWidth(40);
            rangeValueLabel->setMinimumWidth(100);
            checkLabel->setMinimumWidth(40);
            checkCheckBox->setMinimumWidth(20);

            // 设置验证器 (实时输入限制)
            if (!paramDetail.range.isEmpty()) {
                QStringList rangeParts = paramDetail.range.split('-');
                if (rangeParts.size() == 2) {
                    bool okMin, okMax;
                    double minVal = rangeParts.at(0).toDouble(&okMin);
                    double maxVal = rangeParts.at(1).toDouble(&okMax);

                    if (okMin && okMax) {
                        QValidator* validator = nullptr;
                        if (paramDetail.type.toLower() == "int") {
                            validator = new QIntValidator(static_cast<int>(minVal), static_cast<int>(maxVal), this);
                        } else if (paramDetail.type.toLower() == "float") {
                            validator = new QDoubleValidator(minVal, maxVal, 3, this);
                        }
                        if (validator) {
                            valueEdit->setValidator(validator);
                        } else {
                            qWarning() << "Failed to create validator for param:" << paramName << " type:" << paramDetail.type;
                        }
                    } else {
                        qWarning() << "Failed to parse range values for param:" << paramName << " range:" << paramDetail.range;
                    }
                } else {
                    qWarning() << "Invalid range format for param:" << paramName << " range:" << paramDetail.range;
                }
            } else {
                qDebug() << "Range is empty for param:" << paramName << ". No validator applied.";
            }

            // 将 QLineEdit 和 QCheckBox 指针存储到各自的成员变量中
            m_paramValueEdits[projectName].insert(paramName, valueEdit);
            m_paramCheckboxes[projectName].insert(paramName, checkCheckBox);

            // 将控件添加到网格布局，明确指定列索引和对齐方式
            gridLayout->addWidget(nameLabel,       row, 0, Qt::AlignRight | Qt::AlignVCenter);
            gridLayout->addWidget(valueEdit,       row, 1, Qt::AlignLeft | Qt::AlignVCenter);
            gridLayout->addWidget(unitLabel,       row, 2, Qt::AlignLeft | Qt::AlignVCenter);
            gridLayout->addWidget(rangeLabel,      row, 3, Qt::AlignRight | Qt::AlignVCenter);
            gridLayout->addWidget(rangeValueLabel, row, 4, Qt::AlignLeft | Qt::AlignVCenter);
            gridLayout->addWidget(checkLabel,      row, 5, Qt::AlignRight | Qt::AlignVCenter);
            gridLayout->addWidget(checkCheckBox,   row, 6, Qt::AlignLeft | Qt::AlignVCenter);
            row++;
        }

        gridLayout->setColumnStretch(0, 5);  // 参数名称：加大拉伸比例
        gridLayout->setColumnStretch(1, 0);  // 值输入框：保持固定宽度
        gridLayout->setColumnStretch(2, 1);  // 单位
        gridLayout->setColumnStretch(3, 1);  // "范围" 文本
        gridLayout->setColumnStretch(4, 1);  // 范围值
        gridLayout->setColumnStretch(5, 1);  // 检测文本
        gridLayout->setColumnStretch(6, 0);  // 复选框：固定宽度
        subTabContentLayout->addLayout(gridLayout);
        subTabContentLayout->addStretch(1); // 在gridLayout下方添加一个拉伸，确保gridLayout内容靠顶部

        // 创建 QScrollArea 并将 subTabContent 设置为其内容
        QScrollArea* scrollArea = new QScrollArea(this);
        scrollArea->setWidgetResizable(true); // 允许滚动区域调整其内容部件的大小
        scrollArea->setWidget(subTabContent); // 设置内容部件

        // 将带有滚动条的 scrollArea 添加到 subTabWidget
        subTabWidget->addTab(scrollArea, projectName);
    }

    QHBoxLayout* buttonLayout = new QHBoxLayout;
    QPushButton* saveButton = new QPushButton("保存修改", this);
    QPushButton* exitButton = new QPushButton("退出", this);

    // 按钮布局调整使其按钮靠左排列，并垂直对齐到顶部
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(exitButton);
    buttonLayout->addStretch(1); // 按钮行右侧添加拉伸，使按钮靠左

    mainLayout->addLayout(buttonLayout);
    mainLayout->addStretch(1); // 确保整个 mainLayout 的内容靠顶部

    // 保存按钮的槽函数 (需要读取 QCheckBox 的状态)
    connect(saveButton, &QPushButton::clicked, this, [this]() {
        QMessageBox::StandardButton reply = QMessageBox::question(this, "保存确认", "确定要保存当前修改吗？",
                                                                  QMessageBox::Yes | QMessageBox::No);
        if (reply != QMessageBox::Yes) {
            return;
        }

        // --- 步骤 1: 将所有 QLineEdit 和 QCheckBox 的当前值同步到 m_rangeSettings ---
        for (auto projectIt = m_paramValueEdits.begin(); projectIt != m_paramValueEdits.end(); ++projectIt)
        {
            const QString& projectName = projectIt.key();
            const QMap<QString, QLineEdit*>& paramEdits = projectIt.value();
            const QMap<QString, QCheckBox*>& paramCheckboxes = m_paramCheckboxes[projectName]; // 获取当前 project 的 QCheckBox

            for (auto paramEditIt = paramEdits.begin(); paramEditIt != paramEdits.end(); ++paramEditIt)
            {
                const QString& paramName = paramEditIt.key();
                QLineEdit* valueEdit = paramEditIt.value();
                QCheckBox* checkBox = paramCheckboxes.value(paramName, nullptr);

                m_rangeSettings->updateParamValue(projectName, paramName, valueEdit->text());

                if (checkBox) {
                    m_rangeSettings->updateParamCheck(projectName, paramName, checkBox->isChecked());
                } else {
                    qWarning() << "No QCheckBox found for param:" << paramName << " in project:" << projectName;
                }
            }
        }

        // --- 步骤 2: 对更新后的 m_rangeSettings 数据进行最终的范围校验 ---
        QString errorMessage;
        bool validationFailed = false;
        const RangeParameters& currentParams = m_rangeSettings->getRangeParameters();

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

                QStringList rangeParts = paramDetail.range.split('-');
                if (rangeParts.size() != 2) {
                    errorMessage = QString("参数 '%1' (项目: %2) 的范围格式无效: '%3'。应为 'min-max'。")
                                       .arg(paramName).arg(projectName).arg(paramDetail.range);
                    validationFailed = true;
                    break;
                }

                bool okMin, okMax;
                double minVal = rangeParts.at(0).toDouble(&okMin);
                double maxVal = rangeParts.at(1).toDouble(&okMax);

                if (!okMin || !okMax) {
                    errorMessage = QString("参数 '%1' (项目: %2) 的范围值 '%3' 无法解析为数字。")
                                       .arg(paramName).arg(projectName).arg(paramDetail.range);
                    validationFailed = true;
                    break;
                }

                bool valueConversionOk;
                if (paramDetail.type.toLower() == "int") {
                    long long value = paramDetail.value.toLongLong(&valueConversionOk);
                    if (!valueConversionOk || value < minVal || value > maxVal) {
                        errorMessage = QString("参数 '%1' (项目: %2) 的值 '%3' 超出范围 [%4 - %5]。")
                                           .arg(paramName).arg(projectName).arg(paramDetail.value.toString())
                                           .arg(static_cast<long long>(minVal)).arg(static_cast<long long>(maxVal));
                        validationFailed = true;
                        break;
                    }
                } else if (paramDetail.type.toLower() == "float") {
                    double value = paramDetail.value.toDouble(&valueConversionOk);
                    if (!valueConversionOk || value < minVal || value > maxVal) {
                        errorMessage = QString("参数 '%1' (项目: %2) 的值 '%3' 超出范围 [%4 - %5]。")
                                           .arg(paramName).arg(projectName).arg(paramDetail.value.toString())
                                           .arg(minVal).arg(maxVal);
                        validationFailed = true;
                        break;
                    }
                } else {
                    qDebug() << "Skipping range validation for non-numeric type:" << paramName << " Type:" << paramDetail.type;
                }
            }
            if (validationFailed) break;
        }

        if (validationFailed) {
            QMessageBox::warning(this, "参数校验失败", "以下参数值超出规定范围或格式错误：\n" + errorMessage + "\n\n请修正后再保存。");
            return;
        }

        // --- 步骤 3: 如果所有校验通过，则执行保存操作 ---
        if (m_rangeSettings->saveParams()) {
            QMessageBox::information(this, "保存成功", "范围参数已成功保存！");
        } else {
            QMessageBox::warning(this, "保存失败", "无法保存范围参数！请检查文件路径或权限。");
        }
    });

    connect(exitButton, &QPushButton::clicked, this, &ParaWidget::closeWindow);
}



void ParaWidget::setupCameralTab(QTabWidget* tabWidget)
{
    if (m_cameralSettings == nullptr) {
        qWarning() << "ParaWidget::setupCameralTab: m_cameralSettings is null. Cannot set up Camera Tab.";
        QWidget* errorTab = new QWidget;
        tabWidget->addTab(errorTab, "相机参数 (错误)");
        QVBoxLayout* errorLayout = new QVBoxLayout(errorTab);
        errorLayout->addWidget(new QLabel("错误：相机参数管理器未正确初始化。", this));
        return;
    }

    QWidget* cameralTab = new QWidget;
    tabWidget->addTab(cameralTab, "相机参数");

    QVBoxLayout* mainLayout = new QVBoxLayout(cameralTab);
    QTabWidget* subTabWidget = new QTabWidget(this);
    mainLayout->addWidget(subTabWidget);

    // 获取所有相机参数
    const Parameters& allCameralParameters = m_cameralSettings->getParameters();

    if (allCameralParameters.detectionProjects.isEmpty()) { // 使用 detectionProjects，与 Parameters 结构体一致
        QLabel* infoLabel = new QLabel("未找到任何相机参数，请检查配置文件。", this);
        infoLabel->setStyleSheet("color: blue;");
        mainLayout->addWidget(infoLabel);
        goto end_setup_ui;
    }

    // 遍历所有相机项目 (例如 "相机项目1", "相机项目2")
    for (auto projectIt = allCameralParameters.detectionProjects.begin(); // 使用 detectionProjects
         projectIt != allCameralParameters.detectionProjects.end();
         ++projectIt)
    {
        const QString& projectName = projectIt.key();
        const DetectionProject& cameralProject = projectIt.value(); // 这里仍是 DetectionProject 类型

        QWidget* subTab = new QWidget;
        QVBoxLayout* subTabLayout = new QVBoxLayout(subTab);
        QGridLayout* gridLayout = new QGridLayout;

        int row = 0;


        m_paramValueEdits[projectName] = QMap<QString, QLineEdit*>();
        m_paramCheckboxes[projectName] = QMap<QString, QCheckBox*>();

        // 遍历当前相机项目下的所有具体参数
        for (auto paramIt = cameralProject.params.begin();
             paramIt != cameralProject.params.end();
             ++paramIt)
        {
            const QString& paramName = paramIt.key();
            const ParamDetail& paramDetail = paramIt.value();

            // 如果参数不可见，则跳过
            if (!paramDetail.visible) {
                continue;
            }

            // 创建 UI 控件
            QLabel* nameLabel = new QLabel(paramName, this);
            QLineEdit* valueEdit = new QLineEdit(paramDetail.value.toString(), this);
            QLabel* unitLabel = new QLabel(QString("单位: %1").arg(paramDetail.unit), this);
            QLabel* rangeLabel = new QLabel("范围:", this);
            QLabel* rangeValueLabel = new QLabel(paramDetail.range, this);

            // 检测开关的 QCheckBox
            QLabel* checkLabel = new QLabel("检测:", this);
            QCheckBox* checkCheckBox = new QCheckBox(this);
            checkCheckBox->setChecked(paramDetail.check); // 根据 paramDetail.check 设置初始状态

            // 设置控件样式和大小
            nameLabel->setMinimumWidth(100);
            valueEdit->setMaximumWidth(80);
            unitLabel->setMinimumWidth(30);
            rangeLabel->setMinimumWidth(40);
            rangeValueLabel->setMinimumWidth(100);
            checkLabel->setMinimumWidth(40);
            checkCheckBox->setMinimumWidth(20);

            // 设置验证器 (实时输入限制)
            if (!paramDetail.range.isEmpty()) {
                QStringList rangeParts = paramDetail.range.split('-');
                if (rangeParts.size() == 2) {
                    bool okMin, okMax;
                    double minVal = rangeParts.at(0).toDouble(&okMin);
                    double maxVal = rangeParts.at(1).toDouble(&okMax);

                    if (okMin && okMax) {
                        QValidator* validator = nullptr;
                        if (paramDetail.type.toLower() == "int") {
                            validator = new QIntValidator(static_cast<int>(minVal), static_cast<int>(maxVal), this);
                        } else if (paramDetail.type.toLower() == "float") {
                            // 精度设置为 3 位小数，您可以根据需要调整
                            validator = new QDoubleValidator(minVal, maxVal, 3, this);
                        }
                        if (validator) {
                            valueEdit->setValidator(validator);
                        } else {
                            qWarning() << "Failed to create validator for camera param:" << paramName << " type:" << paramDetail.type;
                        }
                    } else {
                        qWarning() << "Failed to parse range values for camera param:" << paramName << " range:" << paramDetail.range;
                    }
                } else {
                    qWarning() << "Invalid range format for camera param:" << paramName << " range:" << paramDetail.range;
                }
            } else {
                qDebug() << "Range is empty for camera param:" << paramName << ". No validator applied.";
            }

            // 将 QLineEdit 和 QCheckBox 指针存储到各自的成员变量中
            m_paramValueEdits[projectName].insert(paramName, valueEdit);
            m_paramCheckboxes[projectName].insert(paramName, checkCheckBox);

            // 将控件添加到网格布局，明确指定列索引
            gridLayout->addWidget(nameLabel,       row, 0, Qt::AlignRight);
            gridLayout->addWidget(valueEdit,       row, 1, Qt::AlignLeft);
            gridLayout->addWidget(unitLabel,       row, 2, Qt::AlignLeft);
            gridLayout->addWidget(rangeLabel,      row, 3, Qt::AlignRight);
            gridLayout->addWidget(rangeValueLabel, row, 4, Qt::AlignLeft);
            gridLayout->addWidget(checkLabel,      row, 5, Qt::AlignRight);
            gridLayout->addWidget(checkCheckBox,   row, 6, Qt::AlignLeft);
            row++;
        }

        subTabLayout->addLayout(gridLayout);
        subTabWidget->addTab(subTab, projectName);
    }

end_setup_ui:;

    // 相机参数的保存和退出按钮 (与范围参数分开，如果需要独立保存相机参数)
    // 如果您希望统一保存所有参数，则可以在主窗口或 ParaWidget 的顶层处理保存按钮
    // 这里为相机参数也添加独立的保存按钮
    QHBoxLayout* buttonLayout = new QHBoxLayout;
    QPushButton* saveButton = new QPushButton("保存相机修改", this);
    QPushButton* exitButton = new QPushButton("退出", this); // 这里的退出是退出整个 ParaWidget

    buttonLayout->addStretch();
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(exitButton);
    buttonLayout->addStretch();
    mainLayout->addLayout(buttonLayout);

    // 连接保存按钮的槽函数
    connect(saveButton, &QPushButton::clicked, this, [this]() {
        QMessageBox::StandardButton reply = QMessageBox::question(this, "保存确认", "确定要保存当前相机参数修改吗？",
                                                                  QMessageBox::Yes | QMessageBox::No);
        if (reply != QMessageBox::Yes) {
            return;
        }

        // --- 步骤 1: 将所有 QLineEdit 和 QCheckBox 的当前值同步到 m_cameralSettings ---
        const Parameters& currentCameralParams = m_cameralSettings->getParameters(); // 获取当前参数状态

        // 遍历所有相机项目
        for (auto projectIt = currentCameralParams.detectionProjects.begin(); // 遍历相机参数的 projects
             projectIt != currentCameralParams.detectionProjects.end();
             ++projectIt)
        {
            const QString& projectName = projectIt.key();
            // 从 UI 控件的 map 中获取当前项目的 QLineEdit 和 QCheckBox
            if (m_paramValueEdits.contains(projectName) && m_paramCheckboxes.contains(projectName))
            {
                const QMap<QString, QLineEdit*>& paramEdits = m_paramValueEdits[projectName];
                const QMap<QString, QCheckBox*>& paramCheckboxes = m_paramCheckboxes[projectName];

                // 遍历当前项目下的所有参数
                for (auto paramEditIt = paramEdits.begin(); paramEditIt != paramEdits.end(); ++paramEditIt)
                {
                    const QString& paramName = paramEditIt.key();
                    QLineEdit* valueEdit = paramEditIt.value();
                    QCheckBox* checkBox = paramCheckboxes.value(paramName, nullptr);

                    // 更新值 (QLineEdit 的文本)
                    m_cameralSettings->updateParamValue(projectName, paramName, valueEdit->text());

                    // 更新检测状态 (QCheckBox 的 bool 状态)
                    if (checkBox) {
                        m_cameralSettings->updateParamCheck(projectName, paramName, checkBox->isChecked());
                    } else {
                        qWarning() << "No QCheckBox found for camera param:" << paramName << " in project:" << projectName;
                    }
                }
            } else {
                qWarning() << "UI maps not found for camera project:" << projectName;
            }
        }

        // --- 步骤 2: 对更新后的 m_cameralSettings 数据进行最终的范围校验 (可选，但推荐) ---
        QString errorMessage;
        bool validationFailed = false;
        const Parameters& updatedCameralParams = m_cameralSettings->getParameters(); // 获取更新后的参数状态

        for (auto projectIt = updatedCameralParams.detectionProjects.begin();
             projectIt != updatedCameralParams.detectionProjects.end();
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

                QStringList rangeParts = paramDetail.range.split('-');
                if (rangeParts.size() != 2) {
                    errorMessage = QString("相机参数 '%1' (项目: %2) 的范围格式无效: '%3'。")
                                       .arg(paramName).arg(projectName).arg(paramDetail.range);
                    validationFailed = true;
                    break;
                }

                bool okMin, okMax;
                double minVal = rangeParts.at(0).toDouble(&okMin);
                double maxVal = rangeParts.at(1).toDouble(&okMax);

                if (!okMin || !okMax) {
                    errorMessage = QString("相机参数 '%1' (项目: %2) 的范围值 '%3' 无法解析为数字。")
                                       .arg(paramName).arg(projectName).arg(paramDetail.range);
                    validationFailed = true;
                    break;
                }

                bool valueConversionOk;
                if (paramDetail.type.toLower() == "int") {
                    long long value = paramDetail.value.toLongLong(&valueConversionOk);
                    if (!valueConversionOk || value < minVal || value > maxVal) {
                        errorMessage = QString("相机参数 '%1' (项目: %2) 的值 '%3' 超出范围 [%4 - %5]。")
                                           .arg(paramName).arg(projectName).arg(paramDetail.value.toString())
                                           .arg(static_cast<long long>(minVal)).arg(static_cast<long long>(maxVal));
                        validationFailed = true;
                        break;
                    }
                } else if (paramDetail.type.toLower() == "float") {
                    double value = paramDetail.value.toDouble(&valueConversionOk);
                    if (!valueConversionOk || value < minVal || value > maxVal) {
                        errorMessage = QString("相机参数 '%1' (项目: %2) 的值 '%3' 超出范围 [%4 - %5]。")
                                           .arg(paramName).arg(projectName).arg(paramDetail.value.toString())
                                           .arg(minVal).arg(maxVal);
                        validationFailed = true;
                        break;
                    }
                }
            }
            if (validationFailed) break;
        }

        if (validationFailed) {
            QMessageBox::warning(this, "相机参数校验失败", "以下相机参数值超出规定范围或格式错误：\n" + errorMessage + "\n\n请修正后再保存。");
            return;
        }

        // --- 步骤 3: 如果所有校验通过，则执行保存操作 ---
        if (m_cameralSettings->saveParams()) {
            QMessageBox::information(this, "保存成功", "相机参数已成功保存！");
        } else {
            QMessageBox::warning(this, "保存失败", "无法保存相机参数！请检查文件路径或权限。");
        }
    });

    connect(exitButton, &QPushButton::clicked, this, &ParaWidget::closeWindow);
}


void ParaWidget::setupAlgorithmTab(QTabWidget* tabWidget)
{


}


void ParaWidget::closeWindow()
{
    close(); // 关闭窗口
}
