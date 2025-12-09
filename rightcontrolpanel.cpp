#include "rightcontrolpanel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QFont>
#include<QProcess>
#include "cameral.h" 
#include "role.h"

RightControlPanel::RightControlPanel(const QVector<Cameral*>& cams, const QVector<Camerinfo>& caminfo, QWidget* parent)
    : QWidget(parent), m_cams(cams)
{
    setupUi(caminfo);
}

RightControlPanel::~RightControlPanel()
{
}

void RightControlPanel::setupUi(const QVector<Camerinfo>& caminfo)
{
    // 调整GroupBox标题样式
    this->setStyleSheet(
        "QGroupBox {"
        "    font-family: '微软雅黑';"
        "    font-size: 16px;" 
        "    background-color: #2E2E2E;"
        "    border: 1px solid #555;"
        "    border-radius: 4px;"
        "    margin-top: 1ex;"
        "}"
        "QGroupBox::title {"
        "    subcontrol-origin: margin;"
        "    subcontrol-position: top center;"
        "    padding: 0 5px;"
        "    color: #FFFFFF;" 
        "}"
        "QLabel {"
        "    font-family: '微软雅黑';"
        "    font-size: 15px;" 
        "    color: white;"
        "}"
    );

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(2, 10, 10, 10);
    //增大主模块之间的垂直间距
    mainLayout->setSpacing(6);

    mainLayout->addWidget(createMachineParamsArea());
    mainLayout->addWidget(createAllShieldButtonArea());
    mainLayout->addWidget(createParameterButtonsArea(caminfo));
    mainLayout->addWidget(createStatisticsArea(caminfo), 1); 
    mainLayout->addWidget(createBottomButtonsArea(), 0);
}


void RightControlPanel::updateStatistics()
{
    if (m_cams.isEmpty() || m_yieldRateLabels.isEmpty()) return;

    qint64 total_sum_count = 0;
    qint64 total_error_count = 0;
    int numCameras = qMin(m_cams.size(), m_totalCountLabels.size());

    for (int i = 0; i < numCameras; ++i) {
        if (!m_cams[i]) continue;

        qint64 current_sum = m_cams[i]->sum_count.load();
        qint64 current_error = m_cams[i]->error_count.load();

        m_totalCountLabels[i]->setText(QString::number(current_sum));
        m_ngCountLabels[i]->setText(QString::number(current_error));
        double pass_rate = (current_sum > 0) ? (static_cast<double>(current_sum - current_error) / current_sum * 100.0) : 0.0;
        m_yieldRateLabels[i]->setText(QString::asprintf("%.2f%%", pass_rate));

        total_sum_count += current_sum;
        total_error_count += current_error;
    }

    if (m_totalCountLabels.size() > numCameras) {
        m_totalCountLabels.last()->setText(QString::number(total_sum_count));
        m_ngCountLabels.last()->setText(QString::number(total_error_count));
        double total_pass_rate = (total_sum_count > 0) ? (static_cast<double>(total_sum_count - total_error_count) / total_sum_count * 100.0) : 0.0;
        m_yieldRateLabels.last()->setText(QString::asprintf("%.2f%%", total_pass_rate));
    }
}

void RightControlPanel::setMachineId(const QString& id) {
    m_machineIdLabel->setText(id);
}

// --- UI 创建函数 ---

QGroupBox* RightControlPanel::createMachineParamsArea() {
    QGroupBox* groupBox = new QGroupBox("机台参数");

    m_machineIdEdit = new QLineEdit(GlobalPara::DeviceId);
    m_machineIdEdit->setFont(QFont("Arial", 16)); 
    m_machineIdEdit->setFrame(false);         
    m_machineIdEdit->setReadOnly(false);      
    m_machineIdEdit->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_machineIdEdit->setFixedHeight(20);      
    m_machineIdEdit->setStyleSheet(
        "QLineEdit { background: transparent; border: none; }"
        "QLineEdit:focus { outline: none; }"
    );

  
    connect(m_machineIdEdit, &QLineEdit::editingFinished, this, [this]() {
        GlobalPara::DeviceId = m_machineIdEdit->text().trimmed();
        });


    QFormLayout* layout = new QFormLayout(groupBox);
    layout->setSpacing(6);                   
    layout->setContentsMargins(0, 0, 0, 0);   
    layout->addRow("机台编号:", m_machineIdEdit);

    return groupBox;
}


QGroupBox* RightControlPanel::createParameterButtonsArea(const QVector<Camerinfo>& caminfo) {
    QGroupBox* groupBox = new QGroupBox("参数设定");
    QVBoxLayout* layout = new QVBoxLayout(groupBox);
    layout->setSpacing(2); // 按钮间距
    layout->setContentsMargins(10, 15, 10, 0);
    QString buttonStyle = "QPushButton { text-align: center; background-color: #007BFF; border-radius: 4px; padding: 8px; font-size: 16px; } QPushButton:hover { background-color: #0056b3; }";

    int numCameras = caminfo.size();
    for (int i = 0; i < numCameras; ++i) {
        QString buttonText = caminfo[i].name + " - 参数设置";
        QPushButton* button = new QPushButton(buttonText);
        button->setStyleSheet(buttonStyle);

        
        connect(button, &QPushButton::clicked, this, [this, i]() {
            emit parameterButtonClicked(i); 
            });

        layout->addWidget(button);
    }

    return groupBox;
}


QGroupBox* RightControlPanel::createStatisticsArea(const QVector<Camerinfo>& caminfo) {
    QGroupBox* groupBox = new QGroupBox("检测数据");
    QVBoxLayout* mainVLayout = new QVBoxLayout(groupBox);
    // 增加统计模块之间的间距 
    mainVLayout->setSpacing(6);

    const QStringList groupTitles = { "良品率", "不良数", "检测总数" };
    QVector<QFormLayout*> layouts;
    for (const auto& title : groupTitles) {
        QGroupBox* box = new QGroupBox(title);
        QFormLayout* layout = new QFormLayout(box);
        layout->setSpacing(3); // 增加表单内行间距
        layouts.append(layout);
        mainVLayout->addWidget(box);
    }

    int numCameras = caminfo.size();
    m_yieldRateLabels.resize(numCameras + 1);
    m_ngCountLabels.resize(numCameras + 1);
    m_totalCountLabels.resize(numCameras + 1);

    // 创建每个相机的统计行
    for (int i = 0; i < numCameras; ++i) {
     
        m_yieldRateLabels[i] = new QLabel("0.00%");
        m_yieldRateLabels[i]->setFont(QFont("Arial", 16, QFont::Bold));
        m_yieldRateLabels[i]->setStyleSheet("color: #28A745;");
        layouts[0]->addRow(caminfo[i].name + ":", m_yieldRateLabels[i]);

        m_ngCountLabels[i] = new QLabel("0");
        m_ngCountLabels[i]->setFont(QFont("Arial", 16, QFont::Bold));
        m_ngCountLabels[i]->setStyleSheet("color: #DC3545;");
        layouts[1]->addRow(caminfo[i].name + ":", m_ngCountLabels[i]);

        m_totalCountLabels[i] = new QLabel("0");
        m_totalCountLabels[i]->setFont(QFont("Arial", 16, QFont::Bold));
        m_totalCountLabels[i]->setStyleSheet("color: #00BFFF;");
        layouts[2]->addRow(caminfo[i].name + ":", m_totalCountLabels[i]);
    }

    // 创建总和行
    m_yieldRateLabels.last() = new QLabel("0.00%");
    m_yieldRateLabels.last()->setFont(QFont("Arial", 16, QFont::Bold));
    m_yieldRateLabels.last()->setStyleSheet("color: #28A745;");
    layouts[0]->addRow("总和:", m_yieldRateLabels.last());

    m_ngCountLabels.last() = new QLabel("0");
    m_ngCountLabels.last()->setFont(QFont("Arial", 16, QFont::Bold));
    m_ngCountLabels.last()->setStyleSheet("color: #DC3545;");
    layouts[1]->addRow("总和:", m_ngCountLabels.last());

    QLabel* totalLabel2 = new QLabel("总和:");
    m_totalCountLabels.last() = new QLabel("0");
    m_totalCountLabels.last()->setFont(QFont("Arial", 16, QFont::Bold));
    m_totalCountLabels.last()->setStyleSheet("color: #00BFFF;");
    //layouts[2]->addRow("总和:", m_totalCountLabels.last());
    layouts[2]->addRow(totalLabel2, m_totalCountLabels.last());
    m_totalCountLabels.last()->hide();
    totalLabel2->hide();
    m_totalCountLabels.last()->hide();
    return groupBox;
}


QWidget* RightControlPanel::createBottomButtonsArea() {
    QWidget* container = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout(container);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(6);


    QString buttonStyle = "QPushButton { background-color: #6C757D; color: white; border: none; padding: 10px; font-size: 16px; font-weight: bold; border-radius: 4px; } QPushButton:hover { background-color: #5a6268; }";
    QString startStyle = "background-color: #28A745;" + buttonStyle.mid(buttonStyle.indexOf("color:"));
    QString stopStyle = "background-color: #DC3545;" + buttonStyle.mid(buttonStyle.indexOf("color:"));

    QPushButton* photoBtn = new QPushButton("清零");
    photoBtn->setStyleSheet(buttonStyle);
    connect(photoBtn, &QPushButton::clicked, this, &RightControlPanel::clearAllClicked);

    QPushButton* startBtn = new QPushButton("启动");
    startBtn->setStyleSheet(startStyle);
    connect(startBtn, &QPushButton::clicked, this, &RightControlPanel::startAllClicked);

    QPushButton* stopBtn = new QPushButton("停止");
    stopBtn->setStyleSheet(stopStyle);
    connect(stopBtn, &QPushButton::clicked, this, &RightControlPanel::stopAllClicked);

    QPushButton* clearBtn = new QPushButton("统计报表");
    clearBtn->setStyleSheet(buttonStyle);
    connect(clearBtn, &QPushButton::clicked, this, &RightControlPanel::startDBChart);

    layout->addWidget(photoBtn);
    layout->addWidget(startBtn);
    layout->addWidget(stopBtn);
    layout->addWidget(clearBtn);

    return container;
}

QGroupBox* RightControlPanel::createAllShieldButtonArea() {
    QGroupBox* groupBox = new QGroupBox("机台参数");
    QVBoxLayout* layout = new QVBoxLayout(groupBox);
    layout->setContentsMargins(10, 10, 10, 10);

    // 将按钮声明为可切换状态
    QPushButton* allShieldBtn = new QPushButton("调机模式");
    allShieldBtn->setCheckable(true);

    // 初始样式设置
    QString baseStyle = "QPushButton { text-align: center; border-radius: 4px; padding: 8px; font-size: 16px; } QPushButton:hover { background-color: #0056b3; }";
    QString checkedStyle = "QPushButton { background-color: #FF0000; }"; // 红色
    QString uncheckedStyle = "QPushButton { background-color: #007BFF; }"; // 蓝色

    // 根据初始状态设置颜色
    allShieldBtn->setStyleSheet(uncheckedStyle + baseStyle);

    // 使用 connect 连接 toggled 信号
    connect(allShieldBtn, &QPushButton::toggled, this, [this, allShieldBtn, checkedStyle, uncheckedStyle, baseStyle](bool checked) {
		//if (Role::GetCurrentRole() == "操作员") return; // 操作员无权限切换调机模式
        if (checked) {
            // 如果按钮被选中，设置为红色样式，并发射 true 信号
            allShieldBtn->setStyleSheet(checkedStyle + baseStyle);
            emit AllSheild(true);
            qDebug() << "进入调机模式 (红色)";
        }
        else {
            // 如果按钮未被选中，设置为蓝色样式，并发射 false 信号
            allShieldBtn->setStyleSheet(uncheckedStyle + baseStyle);
            emit AllSheild(false);
            qDebug() << "退出调机模式 (蓝色)";
        }
        });

    layout->addWidget(allShieldBtn);

    QPushButton* learnBtn = new QPushButton("一键学习");
    learnBtn->setCheckable(false); // 一键学习通常不需要 Check 状态

    // 复用 baseStyle，只给它一个绿色背景风格
    QString learnStyle = "QPushButton { background-color: #007BFF; }";
    learnBtn->setStyleSheet(learnStyle + baseStyle);

    // 连接信号
    connect(learnBtn, &QPushButton::clicked, this, [this]() {
        emit AllLearn();
        });

  //  learnBtn->hide();

    layout->addWidget(learnBtn);

    return groupBox;
}

void RightControlPanel::startDBChart()
{
    QProcess::startDetached("DBChart.exe");
}