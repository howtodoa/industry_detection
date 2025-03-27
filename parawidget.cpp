#include "parawidget.h"
#include <QVBoxLayout>
#include <QTabWidget>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

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

    // 设置空选项卡
    setupRangeTab(tabWidget, nullptr);
    setupCameralTab(tabWidget, nullptr);
    setupAlgorithmTab(tabWidget, nullptr);

    // 设置主布局
    setLayout(mainLayout);
}

ParaWidget::ParaWidget(RangePara* rangeParams, CameralPara* cameralParams, AlgorithmPara* algoParams, QWidget *parent)
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
    setupRangeTab(tabWidget, rangeParams);
    setupCameralTab(tabWidget, cameralParams);
    setupAlgorithmTab(tabWidget, algoParams);

    // 设置主布局
    setLayout(mainLayout);
}

ParaWidget::~ParaWidget()
{

}

void ParaWidget::setupRangeTab(QTabWidget* tabWidget, RangePara* rangeParams)
{
    QWidget* standardTab = new QWidget;
    tabWidget->addTab(standardTab, "范围参数");

    QVBoxLayout* tabLayout = new QVBoxLayout(standardTab);
    QGridLayout* gridLayout = new QGridLayout;

    if (rangeParams) {
        QStringList paramNamesLower = {"负极针长下限", "负极针宽下限", "正极针长下限", "针总长下限", "正针弯角度最小", "负针弯角度最小"};
        QStringList paramNamesUpper = {"负极针长上限", "负极针宽上限", "正极针长上限", "针总长上限", "正针弯角度最大", "负针弯角度最大"};
        QStringList valuesLower = {QString::number(rangeParams->nePinLengthLower), QString::number(rangeParams->nePinWidthLower),
                                   QString::number(rangeParams->poPinLengthLower), QString::number(rangeParams->piTotalLengthLower),
                                   QString::number(rangeParams->poBendAngleMin), QString::number(rangeParams->neBendAngleMin)};
        QStringList valuesUpper = {QString::number(rangeParams->nePinLengthUpper), QString::number(rangeParams->nePinWidthUpper),
                                   QString::number(rangeParams->poPinLengthUpper), QString::number(rangeParams->piTotalLengthUpper),
                                   QString::number(rangeParams->poBendAngleMax), QString::number(rangeParams->neBendAngleMax)};
        QStringList units = {"mm", "mm", "mm", "mm", "角度", "角度"};

        for (int i = 0; i < paramNamesLower.size(); ++i) {
            QLabel* nameLabelLower = new QLabel(paramNamesLower[i], standardTab);
            nameLabelLower->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
            gridLayout->addWidget(nameLabelLower, i, 0);

            QLineEdit* valueEditLower = new QLineEdit(valuesLower[i], standardTab);
            valueEditLower->setReadOnly(false);
            valueEditLower->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            valueEditLower->setStyleSheet("QLineEdit { border: 1px solid gray; padding: 2px; }");
            gridLayout->addWidget(valueEditLower, i, 1);

            QLabel* unitLabelLower = new QLabel(units[i], standardTab);
            unitLabelLower->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            gridLayout->addWidget(unitLabelLower, i, 2);

            QLabel* nameLabelUpper = new QLabel(paramNamesUpper[i], standardTab);
            nameLabelUpper->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
            gridLayout->addWidget(nameLabelUpper, i, 3);

            QLineEdit* valueEditUpper = new QLineEdit(valuesUpper[i], standardTab);
            valueEditUpper->setReadOnly(true); // 暂时只读
            valueEditUpper->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            valueEditUpper->setStyleSheet("QLineEdit { border: 1px solid gray; padding: 2px; }");
            gridLayout->addWidget(valueEditUpper, i, 4);

            QLabel* unitLabelUpper = new QLabel(units[i], standardTab);
            unitLabelUpper->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            gridLayout->addWidget(unitLabelUpper, i, 5);
        }
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

    connect(saveButton, &QPushButton::clicked, this, &ParaWidget::saveChanges);
    connect(exitButton, &QPushButton::clicked, this, &ParaWidget::closeWindow);
}

void ParaWidget::setupCameralTab(QTabWidget* tabWidget, CameralPara* cameralParams)
{
    QWidget* cameralTab = new QWidget;
    tabWidget->addTab(cameralTab, "相机参数");

    QVBoxLayout* tabLayout = new QVBoxLayout(cameralTab);
    QGridLayout* gridLayout = new QGridLayout;

    if (cameralParams) {
        QStringList names = {"相机名称", "相机序列号", "相机IP", "相机类型", "相机工位号", "相机AD", "相机增益", "相机曝光"};
        QStringList values = {cameralParams->Cameral_Name, cameralParams->Cameral_SN, cameralParams->Cameral_IP,
                              QString::number(cameralParams->Cameral_Type), QString::number(cameralParams->Cameral_Station),
                              QString::number(cameralParams->Cameral_AD), QString::number(cameralParams->Cameral_Gain),
                              QString::number(cameralParams->Cameral_Exposure)};

        for (int i = 0; i < names.size(); ++i) {
            QLabel* nameLabel = new QLabel(names[i], cameralTab);
            nameLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
            gridLayout->addWidget(nameLabel, i, 0);

            QLineEdit* valueEdit = new QLineEdit(values[i], cameralTab);
            valueEdit->setReadOnly(false);
            valueEdit->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            valueEdit->setStyleSheet("QLineEdit { border: 1px solid gray; padding: 2px; }");
            gridLayout->addWidget(valueEdit, i, 1);

            QLabel* unitLabel = new QLabel("", cameralTab);
            unitLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            gridLayout->addWidget(unitLabel, i, 2);
        }
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

    connect(saveButton, &QPushButton::clicked, this, &ParaWidget::saveChanges);
    connect(exitButton, &QPushButton::clicked, this, &ParaWidget::closeWindow);
}

void ParaWidget::setupAlgorithmTab(QTabWidget* tabWidget, AlgorithmPara* algoParams)
{
    QWidget* algorithmTab = new QWidget;
    tabWidget->addTab(algorithmTab, "算法参数");

    QVBoxLayout* tabLayout = new QVBoxLayout(algorithmTab);
    QGridLayout* gridLayout = new QGridLayout;

    if (algoParams) {
        QLabel* algoNameLabel = new QLabel("算法名称", algorithmTab);
        algoNameLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        gridLayout->addWidget(algoNameLabel, 0, 0);

        QLineEdit* algoNameEdit = new QLineEdit(algoParams->Algorithm_Name, algorithmTab);
        algoNameEdit->setReadOnly(false);
        algoNameEdit->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        algoNameEdit->setStyleSheet("QLineEdit { border: 1px solid gray; padding: 2px; }");
        gridLayout->addWidget(algoNameEdit, 0, 1);

        QLabel* algoNameUnit = new QLabel("", algorithmTab);
        algoNameUnit->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        gridLayout->addWidget(algoNameUnit, 0, 2);

        QLabel* valueNamesLabel = new QLabel("参数名称列表", algorithmTab);
        valueNamesLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        gridLayout->addWidget(valueNamesLabel, 1, 0);

        QLineEdit* valueNamesEdit = new QLineEdit(algoParams->Value_Names.join(", "), algorithmTab);
        valueNamesEdit->setReadOnly(false);
        valueNamesEdit->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        valueNamesEdit->setStyleSheet("QLineEdit { border: 1px solid gray; padding: 2px; }");
        gridLayout->addWidget(valueNamesEdit, 1, 1);

        QLabel* valueNamesUnit = new QLabel("", algorithmTab);
        valueNamesUnit->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        gridLayout->addWidget(valueNamesUnit, 1, 2);

        QLabel* valueValuesLabel = new QLabel("参数值列表", algorithmTab);
        valueValuesLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        gridLayout->addWidget(valueValuesLabel, 2, 0);

        QLineEdit* valueValuesEdit = new QLineEdit(algoParams->Value_Values.join(", "), algorithmTab);
        valueValuesEdit->setReadOnly(false);
        valueValuesEdit->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        valueValuesEdit->setStyleSheet("QLineEdit { border: 1px solid gray; padding: 2px; }");
        gridLayout->addWidget(valueValuesEdit, 2, 1);

        QLabel* valueValuesUnit = new QLabel("", algorithmTab);
        valueValuesUnit->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        gridLayout->addWidget(valueValuesUnit, 2, 2);
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

    connect(saveButton, &QPushButton::clicked, this, &ParaWidget::saveChanges);
    connect(exitButton, &QPushButton::clicked, this, &ParaWidget::closeWindow);
}

void ParaWidget::saveChanges()
{
    // 占位函数，目前留空，等待后续需求
    // qDebug() << "保存修改按钮被点击";
}

void ParaWidget::closeWindow()
{
    close(); // 关闭窗口
}
