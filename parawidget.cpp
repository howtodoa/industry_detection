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
#include <QScrollArea>
#include <QtGlobal>
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
    setupDebugTab(tabWidget);//第四个选项卡：调试功能



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

    // 清空 map，防止重复添加。
    m_paramValueEdits.clear();
    m_paramCheckboxes.clear();

    const RangeParameters& allRangeParameters = m_rangeSettings->getRangeParameters();

    if (allRangeParameters.detectionProjects.isEmpty()) {
        QLabel* infoLabel = new QLabel("未找到任何范围参数，请检查配置文件。", this);
        infoLabel->setStyleSheet("color: blue;");
        mainLayout->addWidget(infoLabel);
        return;
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
                // 优化范围解析，以便正确处理负数范围，例如 "-100-10"
                QString minStr;
                QString maxStr;
                int lastDashIndex = paramDetail.range.lastIndexOf('-');

                // 检查是否有至少一个 '-'
                if (lastDashIndex == -1) {
                    qWarning() << "Range format invalid (no dash):" << paramDetail.range;
                    // 不设置 validator，或设置一个宽松的，但后续保存校验会捕获
                } else if (lastDashIndex == 0) { // 比如 "-10"，被认为是无效格式，因为只有min没有max
                    qWarning() << "Range format invalid (dash at start, no max):" << paramDetail.range;
                }
                else {
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
                            qWarning() << "Failed to create validator for param:" << paramName << " type:" << paramDetail.type;
                        }
                    } else {
                        qWarning() << "Failed to parse range values for param:" << paramName << " range:" << paramDetail.range;
                    }
                }
            } else {
                qDebug() << "Range is empty for param:" << paramName << ". No validator applied.";
            }

            // 将 QLineEdit 和 QCheckBox 指针存储到各自的成员变量中
            m_paramValueEdits[projectName].insert(paramName, valueEdit);
            m_paramCheckboxes[projectName].insert(paramName, checkCheckBox);

            // 将控件添加到网格布局，明确指定列索引和对齐方式
            gridLayout->addWidget(nameLabel,         row, 0, Qt::AlignRight | Qt::AlignVCenter);
            gridLayout->addWidget(valueEdit,         row, 1, Qt::AlignLeft | Qt::AlignVCenter);
            gridLayout->addWidget(unitLabel,         row, 2, Qt::AlignLeft | Qt::AlignVCenter);
            gridLayout->addWidget(rangeLabel,        row, 3, Qt::AlignRight | Qt::AlignVCenter);
            gridLayout->addWidget(rangeValueLabel,   row, 4, Qt::AlignLeft | Qt::AlignVCenter);
            gridLayout->addWidget(checkLabel,        row, 5, Qt::AlignRight | Qt::AlignVCenter);
            gridLayout->addWidget(checkCheckBox,     row, 6, Qt::AlignLeft | Qt::AlignVCenter);
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
        // 任何转换失败都应中断并通知用户
        for (auto projectIt = m_paramValueEdits.begin(); projectIt != m_paramValueEdits.end(); ++projectIt)
        {
            const QString& projectName = projectIt.key();
            const QMap<QString, QLineEdit*>& paramEdits = projectIt.value();
            const QMap<QString, QCheckBox*>& paramCheckboxes = m_paramCheckboxes[projectName];

            for (auto paramEditIt = paramEdits.begin(); paramEditIt != paramEdits.end(); ++paramEditIt)
            {
                const QString& paramName = paramEditIt.key();
                QLineEdit* valueEdit = paramEditIt.value();
                QCheckBox* checkBox = paramCheckboxes.value(paramName, nullptr);

                QString textValue = valueEdit->text();
                QVariant convertedValue;

                // 获取参数的详细信息，以便进行类型转换
                const ParamDetail* detail = m_rangeSettings->getParamDetail(projectName, paramName);
                if (detail) {
                    bool conversionOk = false;
                    if (detail->type.toLower() == "int") {
                        convertedValue = textValue.toInt(&conversionOk);
                        if (!conversionOk) {
                            QMessageBox::warning(this, "输入格式错误", QString("参数 '%1' (项目: %2) 的值 '%3' 不是有效的整数。请修正。")
                                                                                 .arg(paramName).arg(projectName).arg(textValue));
                            return; // 中断保存
                        }
                    } else if (detail->type.toLower() == "float") { // 假设配置文件中使用 "float" 表示 double
                        convertedValue = textValue.toDouble(&conversionOk);
                        if (!conversionOk) {
                            QMessageBox::warning(this, "输入格式错误", QString("参数 '%1' (项目: %2) 的值 '%3' 不是有效的浮点数。请修正。")
                                                                                 .arg(paramName).arg(projectName).arg(textValue));
                            return; // 中断保存
                        }
                    } else if (detail->type.toLower() == "bool") {
                        // 对于布尔类型，根据 "true"/"false" 或 "0"/"1" 判断
                        convertedValue = (textValue.toLower() == "true" || textValue == "1");
                        conversionOk = true; // 布尔转换通常不会失败
                    } else {
                        convertedValue = textValue; // 默认为字符串类型
                        conversionOk = true; // 字符串转换不会失败
                    }

                    // 如果类型转换失败，则停止保存
                    if (!conversionOk) {
                        qWarning() << "ParaWidget::saveRangeParams: 无法将" << textValue << "转换为指定类型，参数：" << paramName << " 类型:" << detail->type;
                        return; // 已经通过 QMessageBox 提示用户，这里直接返回
                    }
                } else {
                    convertedValue = textValue; // 如果未找到 ParamDetail，也默认为字符串
                    qWarning() << "ParaWidget::saveRangeParams: 未找到参数详情，无法确定类型进行转换。参数：" << projectName << "/" << paramName;
                }

                m_rangeSettings->updateParamValue(projectName, paramName, convertedValue);

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
        // 重新获取更新后的参数，确保校验的是最新的数据
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
                if (paramDetail.range.isEmpty()) continue; // 没有范围的参数不进行校验

                // 优化范围解析，以便正确处理负数范围，例如 "-100-10"
                QString minStr;
                QString maxStr;
                int lastDashIndex = paramDetail.range.lastIndexOf('-');

                // 检查是否有至少一个 '-'
                if (lastDashIndex == -1) {
                    errorMessage = QString("参数 '%1' (项目: %2) 的范围格式无效: '%3'。应为 'min-max'，缺少分隔符。")
                                       .arg(paramName).arg(projectName).arg(paramDetail.range);
                    validationFailed = true;
                    break;
                }

                // 如果最后一个 '-' 在字符串的开头，例如 "-10"，这被视为无效格式
                if (lastDashIndex == 0) {
                    errorMessage = QString("参数 '%1' (项目: %2) 的范围格式无效: '%3'。应为 'min-max'。")
                                       .arg(paramName).arg(projectName).arg(paramDetail.range);
                    validationFailed = true;
                    break;
                }

                // minStr 是从开头到最后一个 '-' 之前的部分
                minStr = paramDetail.range.left(lastDashIndex);
                // maxStr 是从最后一个 '-' 之后的部分
                maxStr = paramDetail.range.mid(lastDashIndex + 1);

                // 额外的空字符串检查，避免类似 "-10-" 或 "--" 这样的异常情况
                if (minStr.isEmpty() && !minStr.startsWith('-') && minStr != "0") { // 0是个特殊情况
                    errorMessage = QString("参数 '%1' (项目: %2) 的范围格式无效: '%3'。min值解析为空或不符合规范。")
                                       .arg(paramName).arg(projectName).arg(paramDetail.range);
                    validationFailed = true;
                    break;
                }
                if (maxStr.isEmpty()) {
                    errorMessage = QString("参数 '%1' (项目: %2) 的范围格式无效: '%3'。max值解析为空。")
                                       .arg(paramName).arg(projectName).arg(paramDetail.range);
                    validationFailed = true;
                    break;
                }


                bool okMin, okMax;
                double minValDouble = minStr.toDouble(&okMin);
                double maxValDouble = maxStr.toDouble(&okMax);


                if (!okMin || !okMax) {
                    errorMessage = QString("参数 '%1' (项目: %2) 的范围值 '%3' 无法解析为数字。")
                                       .arg(paramName).arg(projectName).arg(paramDetail.range);
                    validationFailed = true;
                    break;
                }

                bool valueConversionOk; // 检查 QVariant 内部转换是否成功
                if (paramDetail.type.toLower() == "int") {
                    double value = paramDetail.value.toDouble(&valueConversionOk);
                    if (!valueConversionOk || value < minValDouble || value > maxValDouble) {
                        errorMessage = QString("参数 '%1' (项目: %2) 的值 '%3' 超出范围 [%4 - %5] 或格式不正确。")
                                           .arg(paramName).arg(projectName).arg(paramDetail.value.toString())
                                           .arg(QString::number(minValDouble, 'f', 0)).arg(QString::number(maxValDouble, 'f', 0));
                        validationFailed = true;
                        break;
                    }
                } else if (paramDetail.type.toLower() == "float") {
                    double value = paramDetail.value.toDouble(&valueConversionOk);
                    if (!valueConversionOk || value < minValDouble || value > maxValDouble) {
                        errorMessage = QString("参数 '%1' (项目: %2) 的值 '%3' 超出范围 [%4 - %5] 或格式不正确。")
                                           .arg(paramName).arg(projectName).arg(paramDetail.value.toString())
                                           .arg(minValDouble).arg(maxValDouble);
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
        qWarning() << "ParaWidget::setupCameralTab: m_cameralSettings is null. Cannot set up Cameral Tab.";
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

    m_cameralValueEdits.clear();
    m_cameralCheckboxes.clear();

    const Parameters& allCameralParameters = m_cameralSettings->getParameters();

    if (allCameralParameters.detectionProjects.isEmpty()) {
        QLabel* infoLabel = new QLabel("未找到任何相机参数，请检查配置文件。", this);
        infoLabel->setStyleSheet("color: blue;");
        mainLayout->addWidget(infoLabel);
        return;
    }

    for (auto projectIt = allCameralParameters.detectionProjects.begin();
         projectIt != allCameralParameters.detectionProjects.end();
         ++projectIt)
    {
        const QString& projectName = projectIt.key();
        const DetectionProject& cameralProject = projectIt.value();

        QWidget* subTabContent = new QWidget;
        QVBoxLayout* subTabContentLayout = new QVBoxLayout(subTabContent);
        QGridLayout* gridLayout = new QGridLayout;
        gridLayout->setVerticalSpacing(5);
        gridLayout->setHorizontalSpacing(10);

        int row = 0;

        m_cameralValueEdits[projectName] = QMap<QString, QLineEdit*>();
        m_cameralCheckboxes[projectName] = QMap<QString, QCheckBox*>();

        for (auto paramIt = cameralProject.params.begin();
             paramIt != cameralProject.params.end();
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

            nameLabel->setMinimumWidth(100);
            valueEdit->setMaximumWidth(80);
            unitLabel->setMinimumWidth(30);
            rangeLabel->setMinimumWidth(40);
            rangeValueLabel->setMinimumWidth(100);
            checkLabel->setMinimumWidth(40);
            checkCheckBox->setMinimumWidth(20);

            // ====== 校验器设置调试开始 ======
            if (!paramDetail.range.isEmpty()) {
                QStringList rangeParts = paramDetail.range.split('-');
                if (rangeParts.size() == 2) {
                    bool okMin, okMax;
                    double minVal = rangeParts.at(0).toDouble(&okMin);
                    double maxVal = rangeParts.at(1).toDouble(&okMax);

                    qDebug() << QString("  UI Setup: Param '%1', Type '%2', Range string '%3'")
                                    .arg(paramName).arg(paramDetail.type).arg(paramDetail.range);
                    qDebug() << QString("    Parsed Range: min=%1 (OK: %2), max=%3 (OK: %4)")
                                    .arg(minVal).arg(okMin).arg(maxVal).arg(okMax);

                    if (okMin && okMax) {
                        QValidator* validator = nullptr;
                        // 确保 minVal 不大于 maxVal，这是校验器正确工作的基本前提
                        if (minVal <= maxVal) {
                            if (paramDetail.type.toLower() == "int") {
                                validator = new QIntValidator(static_cast<int>(minVal), static_cast<int>(maxVal), this);
                                qDebug() << "    Created QIntValidator with range [" << static_cast<int>(minVal) << ", " << static_cast<int>(maxVal) << "]";
                            } else if (paramDetail.type.toLower() == "float") {
                                validator = new QDoubleValidator(minVal, maxVal, 3, this); // 3 为小数位数
                                qDebug() << "    Created QDoubleValidator with range [" << minVal << ", " << maxVal << "]";
                            }
                            // 如果有其他类型需要实时校验，可以在这里添加

                            if (validator) {
                                valueEdit->setValidator(validator);
                                qDebug() << "    Validator successfully applied to '" << paramName << "'.";
                            } else {
                                qWarning() << "    [WARNING] Failed to create specific validator for '" << paramName << "'. Input will be unrestricted.";
                            }
                        } else {
                            qWarning() << "    [WARNING] Calculated minVal (" << minVal << ") is greater than maxVal (" << maxVal << ") for param:" << paramName << ". No validator applied.";
                        }
                    } else {
                        qWarning() << "  [WARNING] Failed to parse numeric range for '" << paramName << "'. No validator applied.";
                    }
                } else {
                    qWarning() << "  [WARNING] Invalid range format for '" << paramName << "'. Expected 'min-max'. No validator applied.";
                }
            } else {
                qDebug() << "  No range specified for '" << paramName << "'. No validator applied, input is unrestricted.";
            }
            // ====== 校验器设置调试结束 ======

            m_cameralValueEdits[projectName].insert(paramName, valueEdit);
            m_cameralCheckboxes[projectName].insert(paramName, checkCheckBox);

            gridLayout->addWidget(nameLabel,       row, 0, Qt::AlignRight | Qt::AlignVCenter);
            gridLayout->addWidget(valueEdit,       row, 1, Qt::AlignLeft | Qt::AlignVCenter);
            gridLayout->addWidget(unitLabel,       row, 2, Qt::AlignLeft | Qt::AlignVCenter);
            gridLayout->addWidget(rangeLabel,      row, 3, Qt::AlignRight | Qt::AlignVCenter);
            gridLayout->addWidget(rangeValueLabel, row, 4, Qt::AlignLeft | Qt::AlignVCenter);
            gridLayout->addWidget(checkLabel,      row, 5, Qt::AlignRight | Qt::AlignVCenter);
            gridLayout->addWidget(checkCheckBox,   row, 6, Qt::AlignLeft | Qt::AlignVCenter);
            row++;
        }

        gridLayout->setColumnStretch(0, 5);
        gridLayout->setColumnStretch(1, 0);
        gridLayout->setColumnStretch(2, 1);
        gridLayout->setColumnStretch(3, 1);
        gridLayout->setColumnStretch(4, 1);
        gridLayout->setColumnStretch(5, 1);
        gridLayout->setColumnStretch(6, 0);
        subTabContentLayout->addLayout(gridLayout);
        subTabContentLayout->addStretch(1);

        QScrollArea* scrollArea = new QScrollArea(this);
        scrollArea->setWidgetResizable(true);
        scrollArea->setWidget(subTabContent);

        subTabWidget->addTab(scrollArea, projectName);
    }

    QHBoxLayout* buttonLayout = new QHBoxLayout;
    QPushButton* saveButton = new QPushButton("保存相机修改", this);
    QPushButton* exitButton = new QPushButton("退出", this);

    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(exitButton);
    buttonLayout->addStretch(1);

    mainLayout->addLayout(buttonLayout);
    mainLayout->addStretch(1);

    connect(saveButton, &QPushButton::clicked, this, [this]() {
        QMessageBox::StandardButton reply = QMessageBox::question(this, "保存确认", "确定要保存当前相机参数修改吗？",
                                                                  QMessageBox::Yes | QMessageBox::No);
        if (reply != QMessageBox::Yes) {
            qDebug() << "保存相机参数操作被用户取消。";
            return;
        }

        qDebug() << "--- 开始保存和校验相机参数 ---";

        // --- 步骤 1: 将所有 QLineEdit 和 QCheckBox 的当前值同步到 m_cameralSettings ---
        for (auto projectIt = m_cameralValueEdits.begin(); projectIt != m_cameralValueEdits.end(); ++projectIt)
        {
            const QString& projectName = projectIt.key();
            const QMap<QString, QLineEdit*>& paramEdits = projectIt.value();
            const QMap<QString, QCheckBox*>& paramCheckboxes = m_cameralCheckboxes[projectName];

            for (auto paramEditIt = paramEdits.begin(); paramEditIt != paramEdits.end(); ++paramEditIt)
            {
                const QString& paramName = paramEditIt.key();
                QLineEdit* valueEdit = paramEditIt.value();
                QCheckBox* checkBox = paramCheckboxes.value(paramName, nullptr);

                QString textValue = valueEdit->text();
                QVariant convertedValue;

                const ParamDetail* detail = m_cameralSettings->getParamDetail(projectName, paramName);
                if (detail) {
                    qDebug() << QString("  步骤1: 项目: %1, 参数: %2, UI输入值: '%3', 预期类型: '%4'")
                                    .arg(projectName).arg(paramName).arg(textValue).arg(detail->type);

                    if (detail->type.toLower() == "int") {
                        bool ok;
                        convertedValue = textValue.toInt(&ok);
                        if (!ok) qWarning() << "    [警告] 无法将 UI 值 '" << textValue << "' 转换为 int 类型。";
                    } else if (detail->type.toLower() == "float") {
                        bool ok;
                        convertedValue = textValue.toDouble(&ok);
                        if (!ok) qWarning() << "    [警告] 无法将 UI 值 '" << textValue << "' 转换为 float 类型。";
                    } else if (detail->type.toLower() == "bool") {
                        convertedValue = (textValue.toLower() == "true" || textValue == "1");
                    } else {
                        convertedValue = textValue;
                    }
                } else {
                    convertedValue = textValue;
                    qWarning() << "    [警告] 未找到相机参数详情，无法确定类型进行转换。参数：" << projectName << "/" << paramName;
                }
                qDebug() << "    步骤1: 转换为 QVariant: " << convertedValue << " (类型: " << convertedValue.typeName() << ")";

                m_cameralSettings->updateParamValue(projectName, paramName, convertedValue);

                if (checkBox) {
                    m_cameralSettings->updateParamCheck(projectName, paramName, checkBox->isChecked());
                } else {
                    qWarning() << "    [警告] 未找到相机参数 '" << paramName << "' 的 QCheckBox。";
                }
            }
        }

        // --- 步骤 2: 对更新后的 m_cameralSettings 数据进行最终的范围校验 ---
        QString errorMessage;
        bool validationFailed = false;
        const Parameters& updatedCameralParams = m_cameralSettings->getParameters();

        qDebug() << "--- 步骤2: 开始校验已更新的相机参数 ---";

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

                qDebug() << QString("  步骤2: 校验项目: %1, 参数: %2, 当前值: %3 (类型: %4), 范围: %5")
                                .arg(projectName).arg(paramName).arg(paramDetail.value.toString())
                                .arg(paramDetail.value.typeName()).arg(paramDetail.range);


                if (!paramDetail.visible) continue;
                if (paramDetail.range.isEmpty()) {
                    qDebug() << "    步骤2: 无范围定义，跳过校验。";
                    continue;
                }

                QStringList rangeParts = paramDetail.range.split('-');
                if (rangeParts.size() != 2) {
                    errorMessage = QString("相机参数 '%1' (项目: %2) 的范围格式无效: '%3'。应为 'min-max'。")
                                       .arg(paramName).arg(projectName).arg(paramDetail.range);
                    validationFailed = true;
                    qDebug() << "    步骤2: [错误] 范围格式无效。";
                    break;
                }

                bool okMin, okMax;
                double minVal = rangeParts.at(0).toDouble(&okMin);
                double maxVal = rangeParts.at(1).toDouble(&okMax);

                if (!okMin || !okMax) {
                    errorMessage = QString("相机参数 '%1' (项目: %2) 的范围值 '%3' 无法解析为数字。")
                                       .arg(paramName).arg(projectName).arg(paramDetail.range);
                    validationFailed = true;
                    qDebug() << "    步骤2: [错误] 范围值无法解析为数字。";
                    break;
                }
                qDebug() << QString("    步骤2: 范围解析结果: minVal = %1, maxVal = %2").arg(minVal).arg(maxVal);

                // 检查 minVal 是否小于或等于 maxVal，否则校验器可能行为异常
                if (minVal > maxVal) {
                    errorMessage = QString("相机参数 '%1' (项目: %2) 的最小范围值 '%3' 大于最大范围值 '%4'。")
                                       .arg(paramName).arg(projectName).arg(minVal).arg(maxVal);
                    validationFailed = true;
                    qDebug() << "    步骤2: [错误] 最小范围值大于最大范围值。";
                    break;
                }

                bool valueConversionOk;
                if (paramDetail.type.toLower() == "int") {
                    long long value = paramDetail.value.toLongLong(&valueConversionOk);
                    qDebug() << QString("    步骤2: 尝试转换为 int: 结果 = %1, 转换成功 = %2").arg(value).arg(valueConversionOk);
                    if (!valueConversionOk || value < minVal || value > maxVal) {
                        errorMessage = QString("相机参数 '%1' (项目: %2) 的值 '%3' 超出范围 [%4 - %5] 或格式不正确。")
                                           .arg(paramName).arg(projectName).arg(paramDetail.value.toString())
                                           .arg(static_cast<long long>(minVal)).arg(static_cast<long long>(maxVal));
                        validationFailed = true;
                        qDebug() << "    步骤2: [错误] int 类型值校验失败！条件：(!valueConversionOk || " << value << " < " << minVal << " || " << value << " > " << maxVal << ")";
                        break;
                    }
                } else if (paramDetail.type.toLower() == "float") {
                    double value = paramDetail.value.toDouble(&valueConversionOk);
                    qDebug() << QString("    步骤2: 尝试转换为 float: 结果 = %1, 转换成功 = %2").arg(value).arg(valueConversionOk);
                    if (!valueConversionOk || value < minVal || value > maxVal) {
                        errorMessage = QString("相机参数 '%1' (项目: %2) 的值 '%3' 超出范围 [%4 - %5] 或格式不正确。")
                                           .arg(paramName).arg(projectName).arg(paramDetail.value.toString())
                                           .arg(minVal).arg(maxVal);
                        validationFailed = true;
                        qDebug() << "    步骤2: [错误] float 类型值校验失败！条件：(!valueConversionOk || " << value << " < " << minVal << " || " << value << " > " << maxVal << ")";
                        break;
                    }
                } else {
                    qDebug() << "    步骤2: 非数字类型，跳过数值范围校验。";
                }
            }
            if (validationFailed) break;
        }

        qDebug() << "--- 步骤2: 校验结果: validationFailed = " << validationFailed << " ---";

        if (validationFailed) {
            QMessageBox::warning(this, "相机参数校验失败", "以下相机参数值超出规定范围或格式错误：\n" + errorMessage + "\n\n请修正后再保存。");
            qDebug() << "相机参数校验失败，已提示用户并阻止保存。";
            return;
        }

        if (m_cameralSettings->saveParams()) {
            QMessageBox::information(this, "保存成功", "相机参数已成功保存！");
            qDebug() << "相机参数保存成功！";
        } else {
            QMessageBox::warning(this, "保存失败", "无法保存相机参数！请检查文件路径或权限。");
            qDebug() << "相机参数保存失败！";
        }
    });

    connect(exitButton, &QPushButton::clicked, this, &ParaWidget::closeWindow);
}


void ParaWidget::setupAlgorithmTab(QTabWidget* tabWidget)
{
    if (m_algoSettings == nullptr) {
        qWarning() << "ParaWidget::setupAlgorithmTab: m_algoSettings is null. Cannot set up Algorithm Tab.";
        QWidget* errorTab = new QWidget;
        tabWidget->addTab(errorTab, "算法参数 (错误)"); // 修改标签文本
        QVBoxLayout* errorLayout = new QVBoxLayout(errorTab);
        errorLayout->addWidget(new QLabel("错误：算法参数管理器未正确初始化。", this)); // 修改错误信息
        return;
    }

    QWidget* algoTab = new QWidget;
    tabWidget->addTab(algoTab, "算法参数"); // 修改标签文本

    QVBoxLayout* mainLayout = new QVBoxLayout(algoTab);
    QTabWidget* subTabWidget = new QTabWidget(this);
    mainLayout->addWidget(subTabWidget);

    // 清空 map，防止重复添加。
    // 注意：这里使用 m_algoValueEdits 和 m_algoCheckboxes
    m_algoValueEdits.clear();
    m_algoCheckboxes.clear();

    // 获取所有算法参数
    const Parameters& allAlgorithmParameters = m_algoSettings->getParameters();

    if (allAlgorithmParameters.detectionProjects.isEmpty()) { // 使用 detectionProjects，与 Parameters 结构体一致
        QLabel* infoLabel = new QLabel("未找到任何算法参数，请检查配置文件。", this); // 修改信息文本
        infoLabel->setStyleSheet("color: blue;");
        mainLayout->addWidget(infoLabel);
        goto end_setup_ui_algo; // 保留 goto
    }

    // 遍历所有算法项目 (例如 "项目A", "项目B")
    for (auto projectIt = allAlgorithmParameters.detectionProjects.begin(); // 使用 detectionProjects
         projectIt != allAlgorithmParameters.detectionProjects.end();
         ++projectIt)
    {
        const QString& projectName = projectIt.key();
        const DetectionProject& algorithmProject = projectIt.value(); // 这里仍是 DetectionProject 类型

        // 为每个项目创建一个子标签页
        QWidget* subTab = new QWidget;
        QVBoxLayout* subTabLayout = new QVBoxLayout(subTab);
        QGridLayout* gridLayout = new QGridLayout;
        gridLayout->setVerticalSpacing(5); // 增加垂直间距
        gridLayout->setHorizontalSpacing(10); // 增加水平间距

        int row = 0;

        // 为当前项目创建内层 QMap
        m_algoValueEdits[projectName] = QMap<QString, QLineEdit*>();
        m_algoCheckboxes[projectName] = QMap<QString, QCheckBox*>();

        // 遍历当前算法项目下的所有具体参数
        for (auto paramIt = algorithmProject.params.begin();
             paramIt != algorithmProject.params.end();
             ++paramIt)
        {
            const QString& paramName = paramIt.key();
            const ParamDetail& paramDetail = paramIt.value(); // 这里仍是 ParamDetail 类型

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
                            qWarning() << "Failed to create validator for algorithm param:" << paramName << " type:" << paramDetail.type;
                        }
                    } else {
                        qWarning() << "Failed to parse range values for algorithm param:" << paramName << " range:" << paramDetail.range;
                    }
                } else {
                    qWarning() << "Invalid range format for algorithm param:" << paramName << " range:" << paramDetail.range;
                }
            } else {
                qDebug() << "Range is empty for algorithm param:" << paramName << ". No validator applied.";
            }

            // 将 QLineEdit 和 QCheckBox 指针存储到各自的成员变量中
            m_algoValueEdits[projectName].insert(paramName, valueEdit);
            m_algoCheckboxes[projectName].insert(paramName, checkCheckBox);

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
        // 注意：根据您提供的最新 setupCameralTab 模板，这里不添加 setColumnStretch 和滚动区域
        subTabLayout->addLayout(gridLayout);
        subTabWidget->addTab(subTab, projectName);
    }

end_setup_ui_algo:; // goto 标签

    // 算法参数的保存和退出按钮
    QHBoxLayout* buttonLayout = new QHBoxLayout;
    QPushButton* saveButton = new QPushButton("保存算法修改", this); // 修改按钮文本
    QPushButton* exitButton = new QPushButton("退出", this); // 这里的退出是退出整个 ParaWidget

    // 按钮布局调整使其按钮居中
    buttonLayout->addStretch();
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(exitButton);
    buttonLayout->addStretch();
    mainLayout->addLayout(buttonLayout);

    // 连接保存按钮的槽函数
    connect(saveButton, &QPushButton::clicked, this, [this]() {
        QMessageBox::StandardButton reply = QMessageBox::question(this, "保存确认", "确定要保存当前算法参数修改吗？", // 修改提示文本
                                                                  QMessageBox::Yes | QMessageBox::No);
        if (reply != QMessageBox::Yes) {
            return;
        }

        // --- 步骤 1: 将所有 QLineEdit 和 QCheckBox 的当前值同步到 m_algoSettings ---
        // 注意：这里获取的是当前 m_algoSettings 中的数据，而不是直接从UI读取，
        // 后面会通过 updateParamValue/Check 更新
        const Parameters& currentAlgorithmParams = m_algoSettings->getParameters();

        // 遍历所有算法项目
        for (auto projectIt = currentAlgorithmParams.detectionProjects.begin(); // 遍历算法参数的 projects
             projectIt != currentAlgorithmParams.detectionProjects.end();
             ++projectIt)
        {
            const QString& projectName = projectIt.key();
            // 从 UI 控件的 map 中获取当前项目的 QLineEdit 和 QCheckBox
            if (m_algoValueEdits.contains(projectName) && m_algoCheckboxes.contains(projectName))
            {
                const QMap<QString, QLineEdit*>& paramEdits = m_algoValueEdits[projectName];
                const QMap<QString, QCheckBox*>& paramCheckboxes = m_algoCheckboxes[projectName];

                // 遍历当前项目下的所有参数
                for (auto paramEditIt = paramEdits.begin(); paramEditIt != paramEdits.end(); ++paramEditIt)
                {
                    const QString& paramName = paramEditIt.key();
                    QLineEdit* valueEdit = paramEditIt.value();
                    QCheckBox* checkBox = paramCheckboxes.value(paramName, nullptr);

                    // 获取参数的详细信息，以便进行类型转换（如果 updateParamValue 内部不处理）
                    // 按照您给的 setupCameralTab 模板，直接传递 QString
                    m_algoSettings->updateParamValue(projectName, paramName, valueEdit->text());

                    // 更新检测状态 (QCheckBox 的 bool 状态)
                    if (checkBox) {
                        m_algoSettings->updateParamCheck(projectName, paramName, checkBox->isChecked());
                    } else {
                        qWarning() << "No QCheckBox found for algorithm param:" << paramName << " in project:" << projectName;
                    }
                }
            } else {
                qWarning() << "UI maps not found for algorithm project:" << projectName;
            }
        }

        // --- 步骤 2: 对更新后的 m_algoSettings 数据进行最终的范围校验 (可选，但推荐) ---
        QString errorMessage;
        bool validationFailed = false;
        // 重新获取更新后的参数，确保校验的是最新的数据
        const Parameters& updatedAlgorithmParams = m_algoSettings->getParameters();

        for (auto projectIt = updatedAlgorithmParams.detectionProjects.begin();
             projectIt != updatedAlgorithmParams.detectionProjects.end();
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
                if (paramDetail.range.isEmpty()) continue; // 没有范围的参数不进行校验

                QStringList rangeParts = paramDetail.range.split('-');
                if (rangeParts.size() != 2) {
                    errorMessage = QString("算法参数 '%1' (项目: %2) 的范围格式无效: '%3'。应为 'min-max'。")
                                       .arg(paramName).arg(projectName).arg(paramDetail.range);
                    validationFailed = true;
                    break; // 跳出内层循环
                }

                bool okMin, okMax;
                double minVal = rangeParts.at(0).toDouble(&okMin);
                double maxVal = rangeParts.at(1).toDouble(&okMax);

                if (!okMin || !okMax) {
                    errorMessage = QString("算法参数 '%1' (项目: %2) 的范围值 '%3' 无法解析为数字。")
                                       .arg(paramName).arg(projectName).arg(paramDetail.range);
                    validationFailed = true;
                    break; // 跳出内层循环
                }

                bool valueConversionOk;
                if (paramDetail.type.toLower() == "int") {
                    long long value = paramDetail.value.toLongLong(&valueConversionOk);
                    if (!valueConversionOk || value < minVal || value > maxVal) {
                        errorMessage = QString("算法参数 '%1' (项目: %2) 的值 '%3' 超出范围 [%4 - %5]。")
                                           .arg(paramName).arg(projectName).arg(paramDetail.value.toString())
                                           .arg(static_cast<long long>(minVal)).arg(static_cast<long long>(maxVal));
                        validationFailed = true;
                        break; // 跳出内层循环
                    }
                } else if (paramDetail.type.toLower() == "float") {
                    double value = paramDetail.value.toDouble(&valueConversionOk);
                    if (!valueConversionOk || value < minVal || value > maxVal) {
                        errorMessage = QString("算法参数 '%1' (项目: %2) 的值 '%3' 超出范围 [%4 - %5]。")
                                           .arg(paramName).arg(projectName).arg(paramDetail.value.toString())
                                           .arg(minVal).arg(maxVal);
                        validationFailed = true;
                        break; // 跳出内层循环
                    }
                } else {
                    // 对于非数字类型，不需要进行范围校验
                    qDebug() << "Skipping range validation for non-numeric algorithm type:" << paramName << " Type:" << paramDetail.type;
                }
            }
            if (validationFailed) break; // 如果内层循环失败，跳出外层循环
        }

        if (validationFailed) {
            QMessageBox::warning(this, "算法参数校验失败", "以下算法参数值超出规定范围或格式错误：\n" + errorMessage + "\n\n请修正后再保存。"); // 修改提示文本
            return;
        }

        // --- 步骤 3: 如果所有校验通过，则执行保存操作 ---
        if (m_algoSettings->saveParams()) {
            QMessageBox::information(this, "保存成功", "算法参数已成功保存！"); // 修改提示文本
        } else {
            QMessageBox::warning(this, "保存失败", "无法保存算法参数！请检查文件路径或权限。"); // 修改提示文本
        }
    });

    connect(exitButton, &QPushButton::clicked, this, &ParaWidget::closeWindow);
}



void ParaWidget::setupDebugTab(QTabWidget* tabWidget)
{
    // 创建调试页
    QWidget* debugPage = new QWidget(tabWidget);
    QVBoxLayout* layout = new QVBoxLayout(debugPage);


    QPushButton* calibButton = new QPushButton("标定", debugPage);
    QPushButton* roiButton = new QPushButton("ROI", debugPage);
    QPushButton* recipeButton = new QPushButton("配方", debugPage);
     QPushButton* choosepicButton = new QPushButton("选择图片", debugPage);
     QPushButton* shieldButton = new QPushButton("屏蔽", debugPage);

    QSize buttonSize(160, 30);
    calibButton->setFixedSize(buttonSize);
    roiButton->setFixedSize(buttonSize);
    recipeButton->setFixedSize(buttonSize);
    choosepicButton->setFixedSize(buttonSize);
    shieldButton->setFixedSize(buttonSize);

    // 添加按钮到布局
    layout->addWidget(calibButton);
    layout->addWidget(roiButton);
    layout->addWidget(recipeButton);
    layout->addWidget(choosepicButton);
    layout->addWidget(shieldButton);

    // 占位符：连接槽函数（后续实现）
    connect(calibButton, &QPushButton::clicked, this, &ParaWidget::onCalibClicked);
    connect(roiButton, &QPushButton::clicked, this, &ParaWidget::onROIClicked);
    connect(recipeButton, &QPushButton::clicked, this, &ParaWidget::onRecipeClicked);

    // 添加调试页到 tabWidget
    tabWidget->addTab(debugPage, "调试");
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
