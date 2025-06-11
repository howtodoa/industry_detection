#include "parawidget.h"
#include "role.h"
#include <QVBoxLayout>
#include <QTabWidget>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QIntValidator> // For QIntValidator
#include <QDoubleValidator> // For QDoubleValidator

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
    setupCameralTab(tabWidget);  // 第二个选项卡：相机参数 (您需要实现此函数)
    setupAlgorithmTab(tabWidget); // 第三个选项卡：算法参数 (您需要实现此函数)

    // 设置样式表，为 QLineEdit 提供视觉验证反馈
    // 当输入无效时，QLineEdit 会有红色边框和浅红色背景
    this->setStyleSheet(
        "QLineEdit { background-color: white; border: 1px solid gray; }" // 默认边框和背景
        "QLineEdit:invalid { border: 2px solid red; background-color: #FFDDDD; }" // 无效时更粗的红色边框和浅红色背景
        "QLineEdit:valid { border: 1px solid green; background-color: #EEFFEE; }" // 可选：有效时绿色边框和浅绿色背景
        );


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

    // 清空 map，防止重复添加
    m_paramValueEdits.clear();
    m_paramCheckboxes.clear(); // 清空 QCheckBox 的 map

    const RangeParameters& allRangeParameters = m_rangeSettings->getRangeParameters();

    if (allRangeParameters.detectionProjects.isEmpty()) {
        QLabel* infoLabel = new QLabel("未找到任何范围参数，请检查配置文件。", this);
        infoLabel->setStyleSheet("color: blue;");
        mainLayout->addWidget(infoLabel);
        goto end_setup_ui;
    }

    for (auto projectIt = allRangeParameters.detectionProjects.begin();
         projectIt != allRangeParameters.detectionProjects.end();
         ++projectIt)
    {
        const QString& projectName = projectIt.key();
        const DetectionProject& detectionProject = projectIt.value();

        QWidget* subTab = new QWidget;
        QVBoxLayout* subTabLayout = new QVBoxLayout(subTab);
        QGridLayout* gridLayout = new QGridLayout;

        int row = 0;

        // 为当前项目创建内层 QMap
        m_paramValueEdits[projectName] = QMap<QString, QLineEdit*>();
        m_paramCheckboxes[projectName] = QMap<QString, QCheckBox*>(); // 为 QCheckBox 创建内层 QMap

        for (auto paramIt = detectionProject.params.begin();
             paramIt != detectionProject.params.end();
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

            // 新增：检测开关的 QCheckBox
            QLabel* checkLabel = new QLabel("检测:", this);
            QCheckBox* checkCheckBox = new QCheckBox(this);
            checkCheckBox->setChecked(paramDetail.check); // 根据 paramDetail.check 设置初始状态

            // 设置控件样式和大小
            nameLabel->setMinimumWidth(100);
            valueEdit->setMaximumWidth(80);
            unitLabel->setMinimumWidth(30);
            rangeLabel->setMinimumWidth(40);
            rangeValueLabel->setMinimumWidth(100);
            checkLabel->setMinimumWidth(40); // 为检测标签设置最小宽度
            checkCheckBox->setMinimumWidth(20); // 为复选框设置最小宽度

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
            m_paramCheckboxes[projectName].insert(paramName, checkCheckBox); // 存储 QCheckBox 指针

            // 将控件添加到网格布局，明确指定列索引
            // 确保每个addWidget调用都使用 row 和正确的 column
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

end_setup_ui:

    QHBoxLayout* buttonLayout = new QHBoxLayout;
    QPushButton* saveButton = new QPushButton("保存修改", this);
    QPushButton* exitButton = new QPushButton("退出", this);

    buttonLayout->addStretch();
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(exitButton);
    buttonLayout->addStretch();
    mainLayout->addLayout(buttonLayout);

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
            // 注意：这里获取 QCheckBox 的 map 是在外部循环中，确保对当前 project 的 QCheckBox
            const QMap<QString, QCheckBox*>& paramCheckboxes = m_paramCheckboxes[projectName];

            for (auto paramEditIt = paramEdits.begin(); paramEditIt != paramEdits.end(); ++paramEditIt)
            {
                const QString& paramName = paramEditIt.key();
                QLineEdit* valueEdit = paramEditIt.value();
                QCheckBox* checkBox = paramCheckboxes.value(paramName, nullptr); // 获取对应的 QCheckBox，如果不存在则为 nullptr

                // 更新值 (QLineEdit 的文本)
                m_rangeSettings->updateParamValue(projectName, paramName, valueEdit->text());

                // 更新检测状态 (QCheckBox 的 bool 状态)
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

                // 校验条件：如果 paramDetail.visible 为 true，且有 range 定义，才进行范围校验
                if (!paramDetail.visible) {
                    continue;
                }

                if (paramDetail.range.isEmpty()) continue; // 没有范围定义则跳过校验

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

}


void ParaWidget::setupAlgorithmTab(QTabWidget* tabWidget)
{


}


void ParaWidget::closeWindow()
{
    close(); // 关闭窗口
}
