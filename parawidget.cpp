#include "parawidget.h"
#include "role.h"
#include <QVBoxLayout>
#include <QTabWidget>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>

ParaWidget::ParaWidget(QWidget *parent)
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

    // 创建默认参数对象
    RangePara defaultRangeParams;     // 默认构造
    CameralPara defaultCameralParams; // 默认构造
    AlgorithmPara defaultAlgoParams;  // 默认构造

    // 设置选项卡，使用默认对象
    setupRangeTab(tabWidget, defaultRangeParams);
    setupCameralTab(tabWidget, defaultCameralParams);
    setupAlgorithmTab(tabWidget, defaultAlgoParams);

    // 设置主布局
    setLayout(mainLayout);
}

ParaWidget::ParaWidget(RangePara& rangeParams, CameralPara& cameralParams, AlgorithmPara& algoParams, QWidget *parent)
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

void ParaWidget::setupRangeTab(QTabWidget* tabWidget, RangePara& rangeParams)
{
    QWidget* standardTab = new QWidget;
    tabWidget->addTab(standardTab, "范围参数");

    QVBoxLayout* tabLayout = new QVBoxLayout(standardTab);
    QGridLayout* gridLayout = new QGridLayout;

    QStringList paramNamesLower = {"负极针长下限", "负极针宽下限", "正极针长下限", "针总长下限", "正针弯角度最小", "负针弯角度最小"};
    QStringList paramNamesUpper = {"负极针长上限", "负极针宽上限", "正极针长上限", "针总长上限", "正针弯角度最大", "负针弯角度最大"};
    QList<float RangePara::*> fieldsLower = {&RangePara::nePinLengthLower, &RangePara::nePinWidthLower, &RangePara::poPinLengthLower,
                                             &RangePara::piTotalLengthLower, &RangePara::poBendAngleMin, &RangePara::neBendAngleMin};
    QList<float RangePara::*> fieldsUpper = {&RangePara::nePinLengthUpper, &RangePara::nePinWidthUpper, &RangePara::poPinLengthUpper,
                                             &RangePara::piTotalLengthUpper, &RangePara::poBendAngleMax, &RangePara::neBendAngleMax};
    QStringList units = {"mm", "mm", "mm", "mm", "角度", "角度"};

    QList<QLineEdit*> lowerEdits;
    QList<QLineEdit*> upperEdits;

    for (int i = 0; i < paramNamesLower.size(); ++i) {
        QLabel* nameLabelLower = new QLabel(paramNamesLower[i], standardTab);
        nameLabelLower->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        gridLayout->addWidget(nameLabelLower, i, 0);

        QLineEdit* valueEditLower = new QLineEdit(QString::number(rangeParams.*fieldsLower[i]), standardTab);
        valueEditLower->setReadOnly(false);
        valueEditLower->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        valueEditLower->setStyleSheet("QLineEdit { border: 1px solid gray; padding: 2px; }");
        gridLayout->addWidget(valueEditLower, i, 1);
        lowerEdits.append(valueEditLower);

        QLabel* unitLabelLower = new QLabel(units[i], standardTab);
        unitLabelLower->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        gridLayout->addWidget(unitLabelLower, i, 2);

        QLabel* nameLabelUpper = new QLabel(paramNamesUpper[i], standardTab);
        nameLabelUpper->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        gridLayout->addWidget(nameLabelUpper, i, 3);

        QLineEdit* valueEditUpper = new QLineEdit(QString::number(rangeParams.*fieldsUpper[i]), standardTab);
        valueEditUpper->setReadOnly(false);
        valueEditUpper->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        valueEditUpper->setStyleSheet("QLineEdit { border: 1px solid gray; padding: 2px; }");
        gridLayout->addWidget(valueEditUpper, i, 4);
        upperEdits.append(valueEditUpper);

        QLabel* unitLabelUpper = new QLabel(units[i], standardTab);
        unitLabelUpper->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        gridLayout->addWidget(unitLabelUpper, i, 5);
    }

    gridLayout->setColumnStretch(0, 2);
    gridLayout->setColumnStretch(1, 1);
    gridLayout->setColumnStretch(2, 1);
    gridLayout->setColumnStretch(3, 2);
    gridLayout->setColumnStretch(4, 1);
    gridLayout->setColumnStretch(5, 1);

    tabLayout->addLayout(gridLayout);
    tabLayout->addStretch();

    QHBoxLayout* buttonLayout = new QHBoxLayout;
    QPushButton* saveButton = new QPushButton("保存修改", standardTab);
    QPushButton* exitButton = new QPushButton("退出", standardTab);
    buttonLayout->addStretch();
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(exitButton);
    buttonLayout->addStretch();
    tabLayout->addLayout(buttonLayout);

    // 动态保存逻辑
    connect(saveButton, &QPushButton::clicked, this, [this, &rangeParams, lowerEdits, upperEdits, fieldsLower, fieldsUpper, paramNamesLower, paramNamesUpper]() {
        // 检查下限输入
        for (int i = 0; i < lowerEdits.size(); ++i) {
            bool ok;
            float value = lowerEdits[i]->text().toFloat(&ok);
            if (!ok) {
                QMessageBox::warning(this, "输入错误",
                                     QString("参数 '%1' 的输入值 '%2' 不是有效的数字！")
                                         .arg(paramNamesLower[i])
                                         .arg(lowerEdits[i]->text()));
                return;
            }
            rangeParams.*fieldsLower[i] = value;
        }

        // 检查上限输入
        for (int i = 0; i < upperEdits.size(); ++i) {
            bool ok;
            float value = upperEdits[i]->text().toFloat(&ok);
            if (!ok) {
                QMessageBox::warning(this, "输入错误",
                                     QString("参数 '%1' 的输入值 '%2' 不是有效的数字！")
                                         .arg(paramNamesUpper[i])
                                         .arg(upperEdits[i]->text()));
                return;
            }
            rangeParams.*fieldsUpper[i] = value;
        }

        // 如果所有输入都有效，则保存
        saveRangeChanges("F:/Industry_Detection/ini/Cameral1.txt", rangeParams);
        QMessageBox::information(this, "保存成功", "范围参数已成功保存！");
    });

    connect(exitButton, &QPushButton::clicked, this, &ParaWidget::closeWindow);
}


void ParaWidget::setupCameralTab(QTabWidget* tabWidget, CameralPara& cameralParams)
{
    QWidget* cameralTab = new QWidget;
    tabWidget->addTab(cameralTab, "相机参数");

    QVBoxLayout* tabLayout = new QVBoxLayout(cameralTab);
    QGridLayout* gridLayout = new QGridLayout;

    QStringList names = {"相机名称", "相机序列号", "相机IP", "相机类型", "相机工位号", "相机AD", "相机增益", "相机曝光"};
    QStringList values = {cameralParams.Cameral_Name, cameralParams.Cameral_SN, cameralParams.Cameral_IP,
                          QString::number(cameralParams.Cameral_Type), QString::number(cameralParams.Cameral_Station),
                          QString::number(cameralParams.Cameral_AD), QString::number(cameralParams.Cameral_Gain),
                          QString::number(cameralParams.Cameral_Exposure)};

    QList<QLineEdit*> edits; // 保存编辑框

    for (int i = 0; i < names.size(); ++i) {
        QLabel* nameLabel = new QLabel(names[i], cameralTab);
        nameLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        gridLayout->addWidget(nameLabel, i, 0);

        QLineEdit* valueEdit = new QLineEdit(values[i], cameralTab);
        valueEdit->setReadOnly(false);
        valueEdit->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        valueEdit->setStyleSheet("QLineEdit { border: 1px solid gray; padding: 2px; }");
        gridLayout->addWidget(valueEdit, i, 1);
        edits.append(valueEdit); // 存储编辑框

        QLabel* unitLabel = new QLabel("", cameralTab);
        unitLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        gridLayout->addWidget(unitLabel, i, 2);
    }

    gridLayout->setColumnStretch(0, 2);
    gridLayout->setColumnStretch(1, 1);
    gridLayout->setColumnStretch(2, 1);

    tabLayout->addLayout(gridLayout);
    tabLayout->addStretch();

    QHBoxLayout* buttonLayout = new QHBoxLayout;
    QPushButton* saveButton = new QPushButton("保存修改", cameralTab);
    QPushButton* exitButton = new QPushButton("退出", cameralTab);
    buttonLayout->addStretch();
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(exitButton);
    buttonLayout->addStretch();
    tabLayout->addLayout(buttonLayout);

    // 保存修改到 cameralParams
    connect(saveButton, &QPushButton::clicked, this, [this, &cameralParams, edits, names]() {
        // 检查整数参数 (索引 3,4,5: Cameral_Type, Cameral_Station, Cameral_AD)
        int intIndices[] = {3, 4, 5};
        for (int idx : intIndices) {
            bool ok;
            int value = edits[idx]->text().toInt(&ok);
            if (!ok) {
                QMessageBox::warning(this, "输入错误",
                                     QString("参数 '%1' 的输入值 '%2' 不是有效的整数！")
                                         .arg(names[idx])
                                         .arg(edits[idx]->text()));
                return;
            }
            // 暂时存储整数值
            switch (idx) {
            case 3: cameralParams.Cameral_Type = value; break;
            case 4: cameralParams.Cameral_Station = value; break;
            case 5: cameralParams.Cameral_AD = value; break;
            }
        }

        // 检查浮点数参数 (索引 6,7: Cameral_Gain, Cameral_Exposure)
        int floatIndices[] = {6, 7};
        for (int idx : floatIndices) {
            bool ok;
            float value = edits[idx]->text().toFloat(&ok);
            if (!ok) {
                QMessageBox::warning(this, "输入错误",
                                     QString("参数 '%1' 的输入值 '%2' 不是有效的数字！")
                                         .arg(names[idx])
                                         .arg(edits[idx]->text()));
                return;
            }
            // 暂时存储浮点数值
            switch (idx) {
            case 6: cameralParams.Cameral_Gain = value; break;
            case 7: cameralParams.Cameral_Exposure = value; break;
            }
        }

        // 处理字符串参数 (索引 0,1,2: Cameral_Name, Cameral_SN, Cameral_IP)
        cameralParams.Cameral_Name = edits[0]->text();
        cameralParams.Cameral_SN = edits[1]->text();
        cameralParams.Cameral_IP = edits[2]->text();

        // 如果所有输入都有效，则保存
        saveCameralChanges("F:/Industry_Detection/ini/Cameral1.txt", cameralParams);
        QMessageBox::information(this, "保存成功", "相机参数已成功保存！");
    });

    connect(exitButton, &QPushButton::clicked, this, &ParaWidget::closeWindow);
}

void ParaWidget::setupAlgorithmTab(QTabWidget* tabWidget, AlgorithmPara& algoParams)
{
    QWidget* algorithmTab = new QWidget;
    tabWidget->addTab(algorithmTab, "算法参数");

    QVBoxLayout* tabLayout = new QVBoxLayout(algorithmTab);
    QGridLayout* gridLayout = new QGridLayout;

    QStringList names = {"算法名称", "参数名称列表", "参数值列表"};
    using Setter = std::function<void(AlgorithmPara&, const QString&)>;
    QList<Setter> setters = {
        [](AlgorithmPara& p, const QString& v) { p.Algorithm_Name = v; },
        [](AlgorithmPara& p, const QString& v) { p.Value_Names = v.split(", ", Qt::SkipEmptyParts); },
        [](AlgorithmPara& p, const QString& v) { p.Value_Values = v.split(", ", Qt::SkipEmptyParts); }
    };
    QStringList values = {algoParams.Algorithm_Name, algoParams.Value_Names.join(", "), algoParams.Value_Values.join(", ")};

    QList<QLineEdit*> edits;

    for (int i = 0; i < names.size(); ++i) {
        QLabel* nameLabel = new QLabel(names[i], algorithmTab);
        nameLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        gridLayout->addWidget(nameLabel, i, 0);

        QLineEdit* valueEdit = new QLineEdit(values[i], algorithmTab);
        valueEdit->setReadOnly(false);
        valueEdit->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        valueEdit->setStyleSheet("QLineEdit { border: 1px solid gray; padding: 2px; }");
        gridLayout->addWidget(valueEdit, i, 1);
        edits.append(valueEdit);

        QLabel* unitLabel = new QLabel("", algorithmTab);
        unitLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        gridLayout->addWidget(unitLabel, i, 2);
    }

    gridLayout->setColumnStretch(0, 2);
    gridLayout->setColumnStretch(1, 1);
    gridLayout->setColumnStretch(2, 1);

    tabLayout->addLayout(gridLayout);
    tabLayout->addStretch();

    QHBoxLayout* buttonLayout = new QHBoxLayout;
    QPushButton* saveButton = new QPushButton("保存修改", algorithmTab);
    QPushButton* exitButton = new QPushButton("退出", algorithmTab);
    buttonLayout->addStretch();
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(exitButton);
    buttonLayout->addStretch();
    tabLayout->addLayout(buttonLayout);

    connect(saveButton, &QPushButton::clicked, this, [this, &algoParams, edits, setters, names]() {
        // 获取参数名称列表
        QStringList valueNames = edits[1]->text().split(", ", Qt::SkipEmptyParts);

        // 获取参数值列表并验证每个值是否为浮点数
        QStringList valueValues = edits[2]->text().split(", ", Qt::SkipEmptyParts);
        for (int i = 0; i < valueValues.size(); ++i) {
            bool ok;
            valueValues[i].toFloat(&ok);
            if (!ok) {
                QMessageBox::warning(this, "输入错误",
                                     QString("参数值列表中的第 %1 个值 '%2' 不是有效的数字！")
                                         .arg(i + 1)
                                         .arg(valueValues[i]));
                return;
            }
        }

        // 检查参数名称和参数值的数量是否匹配
        if (valueNames.size() != valueValues.size()) {
            QMessageBox::warning(this, "输入错误",
                                 QString("参数名称数量 (%1) 和参数值数量 (%2) 不匹配！")
                                     .arg(valueNames.size())
                                     .arg(valueValues.size()));
            return;
        }

        // 如果验证通过，应用所有修改
        setters[0](algoParams, edits[0]->text()); // Algorithm_Name
        setters[1](algoParams, edits[1]->text()); // Value_Names
        setters[2](algoParams, edits[2]->text()); // Value_Values

        saveAlgorithmChanges("F:/Industry_Detection/ini/Cameral1.txt", algoParams);
        QMessageBox::information(this, "保存成功", "算法参数已成功保存！");
    });

    connect(exitButton, &QPushButton::clicked, this, &ParaWidget::closeWindow);
}

void ParaWidget::saveCameralChanges(const QString& filePath,const CameralPara& cam)
{
    FileOperator::writeCameralPara(filePath, cam);
}

void ParaWidget::saveAlgorithmChanges(const QString& filePath,const AlgorithmPara& algo)
{
    FileOperator::writeAlgorithmPara(filePath, algo);
}

void ParaWidget::saveRangeChanges(const QString& filePath,const RangePara& range)
{
    FileOperator::writeRangePara(filePath, range);
}


void ParaWidget::closeWindow()
{
    close(); // 关闭窗口
}
