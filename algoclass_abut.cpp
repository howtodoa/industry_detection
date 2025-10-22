#include "algoclass_abut.h"
#include <QObject>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include "typdef.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QLineEdit>
#include <QtConcurrent/QtConcurrentRun>
// 构造函数
AlgoClass_Abut::AlgoClass_Abut(QObject* parent)
    : AlgoClass{ parent }
{
    QVariantMap map = FileOperator::readJsonMap(m_cameralPath);  // JSON 文件路径

    if (map.contains("p_pin_range_min"))
    {
        ParamDetail detail(map.value("p_pin_range_min").toMap());
        LearnPara::inParam6.p_pin_range_min = detail.value.toFloat();
    }

    if (map.contains("p_pin_range_max"))
    {
        ParamDetail detail(map.value("p_pin_range_max").toMap());
        LearnPara::inParam6.p_pin_range_max = detail.value.toFloat();
    }

    if (map.contains("b_pln_range_min"))
    {
        ParamDetail detail(map.value("b_pln_range_min").toMap());
        LearnPara::inParam6.b_pln_range_min = detail.value.toFloat();
    }

    if (map.contains("b_pln_range_max"))
    {
        ParamDetail detail(map.value("b_pln_range_max").toMap());
        LearnPara::inParam6.b_pln_range_max = detail.value.toFloat();
    }

    if (map.contains("n_pin_range_min"))
    {
        ParamDetail detail(map.value("n_pin_range_min").toMap());
        LearnPara::inParam6.n_pin_range_min = detail.value.toFloat();
    }

    if (map.contains("n_pin_range_max"))
    {
        ParamDetail detail(map.value("n_pin_range_max").toMap());
        LearnPara::inParam6.n_pin_range_max = detail.value.toFloat();
    }

    if (map.contains("plateOffsetY"))
    {
        ParamDetail detail(map.value("plateOffsetY").toMap());
        LearnPara::inParam6.plateOffsetY = detail.value.toFloat();
    }

    if (map.contains("plateHeight"))
    {
        ParamDetail detail(map.value("plateHeight").toMap());
        LearnPara::inParam6.plateHeight = detail.value.toFloat();
    }

    if (map.contains("waistOffsetY"))
    {
        ParamDetail detail(map.value("waistOffsetY").toMap());
        LearnPara::inParam6.waistOffsetY = detail.value.toFloat();
    }

    if (map.contains("waistHeight"))
    {
        ParamDetail detail(map.value("waistHeight").toMap());
        LearnPara::inParam6.waistHeight = detail.value.toFloat();
    }

    if (map.contains("gauss_ksize"))
    {
        ParamDetail detail(map.value("gauss_ksize").toMap());
        LearnPara::inParam6.gauss_ksize = detail.value.toInt();
    }

    if (map.contains("bin_thres_value"))
    {
        ParamDetail detail(map.value("bin_thres_value").toMap());
        LearnPara::inParam6.bin_thres_value = detail.value.toInt();
    }

    if (map.contains("prod_Area_Min"))
    {
        ParamDetail detail(map.value("prod_Area_Min").toMap());
        LearnPara::inParam6.prod_Area_Min = detail.value.toFloat();
    }

    if (map.contains("pin_Mc_height"))
    {
        ParamDetail detail(map.value("pin_Mc_height").toMap());
        LearnPara::inParam6.pin_Mc_height = detail.value.toFloat();
    }

    if (map.contains("pln_Mc_height"))
    {
        ParamDetail detail(map.value("pln_Mc_height").toMap());
        LearnPara::inParam6.pln_Mc_height = detail.value.toFloat();
    }
}

// 带参数构造函数
AlgoClass_Abut::AlgoClass_Abut(QString algopath, int al_core_param, float* Angle,QObject* parent)
    : AlgoClass(algopath, parent),
    al_core(al_core_param),
	Angle(Angle)
{
}

// 创建左侧控制面板
QWidget* AlgoClass_Abut::createLeftPanel(QWidget* parent)
{
    QWidget* panel = new QWidget(parent);
    QVBoxLayout* layout = new QVBoxLayout(panel);

    // ----------------- 参数编辑区域 -----------------

    // 正引脚最小范围
    QHBoxLayout* pPinMinLayout = new QHBoxLayout;
    QLabel* lblPPinMin = new QLabel("正引脚最小范围:", panel);
    QLineEdit* editPPinMin = new QLineEdit(QString::number(LearnPara::inParam6.p_pin_range_min), panel);
    pPinMinLayout->addWidget(lblPPinMin);
    pPinMinLayout->addWidget(editPPinMin);
    layout->addLayout(pPinMinLayout);

    // 正引脚最大范围
    QHBoxLayout* pPinMaxLayout = new QHBoxLayout;
    QLabel* lblPPinMax = new QLabel("正引脚最大范围:", panel);
    QLineEdit* editPPinMax = new QLineEdit(QString::number(LearnPara::inParam6.p_pin_range_max), panel);
    pPinMaxLayout->addWidget(lblPPinMax);
    pPinMaxLayout->addWidget(editPPinMax);
    layout->addLayout(pPinMaxLayout);

    // 底座最小范围
    QHBoxLayout* bPlnMinLayout = new QHBoxLayout;
    QLabel* lblBPlnMin = new QLabel("底座最小范围:", panel);
    QLineEdit* editBPlnMin = new QLineEdit(QString::number(LearnPara::inParam6.b_pln_range_min), panel);
    bPlnMinLayout->addWidget(lblBPlnMin);
    bPlnMinLayout->addWidget(editBPlnMin);
    layout->addLayout(bPlnMinLayout);

    // 底座最大范围
    QHBoxLayout* bPlnMaxLayout = new QHBoxLayout;
    QLabel* lblBPlnMax = new QLabel("底座最大范围:", panel);
    QLineEdit* editBPlnMax = new QLineEdit(QString::number(LearnPara::inParam6.b_pln_range_max), panel);
    bPlnMaxLayout->addWidget(lblBPlnMax);
    bPlnMaxLayout->addWidget(editBPlnMax);
    layout->addLayout(bPlnMaxLayout);

    // 负引脚最小范围
    QHBoxLayout* nPinMinLayout = new QHBoxLayout;
    QLabel* lblNPinMin = new QLabel("负引脚最小范围:", panel);
    QLineEdit* editNPinMin = new QLineEdit(QString::number(LearnPara::inParam6.n_pin_range_min), panel);
    nPinMinLayout->addWidget(lblNPinMin);
    nPinMinLayout->addWidget(editNPinMin);
    layout->addLayout(nPinMinLayout);

    // 负引脚最大范围
    QHBoxLayout* nPinMaxLayout = new QHBoxLayout;
    QLabel* lblNPinMax = new QLabel("负引脚最大范围:", panel);
    QLineEdit* editNPinMax = new QLineEdit(QString::number(LearnPara::inParam6.n_pin_range_max), panel);
    nPinMaxLayout->addWidget(lblNPinMax);
    nPinMaxLayout->addWidget(editNPinMax);
    layout->addLayout(nPinMaxLayout);

    // 座板上边平移高度
    QHBoxLayout* plateOffsetLayout = new QHBoxLayout;
    QLabel* lblPlateOffset = new QLabel("座板上边平移高度:", panel);
    QLineEdit* editPlateOffset = new QLineEdit(QString::number(LearnPara::inParam6.plateOffsetY), panel);
    plateOffsetLayout->addWidget(lblPlateOffset);
    plateOffsetLayout->addWidget(editPlateOffset);
    layout->addLayout(plateOffsetLayout);

    // 座板高度
    QHBoxLayout* plateHeightLayout = new QHBoxLayout;
    QLabel* lblPlateHeight = new QLabel("座板高度:", panel);
    QLineEdit* editPlateHeight = new QLineEdit(QString::number(LearnPara::inParam6.plateHeight), panel);
    plateHeightLayout->addWidget(lblPlateHeight);
    plateHeightLayout->addWidget(editPlateHeight);
    layout->addLayout(plateHeightLayout);

    // 腰线上边平移高度
    QHBoxLayout* waistOffsetLayout = new QHBoxLayout;
    QLabel* lblWaistOffset = new QLabel("腰线上边平移高度:", panel);
    QLineEdit* editWaistOffset = new QLineEdit(QString::number(LearnPara::inParam6.waistOffsetY), panel);
    waistOffsetLayout->addWidget(lblWaistOffset);
    waistOffsetLayout->addWidget(editWaistOffset);
    layout->addLayout(waistOffsetLayout);

    // 腰线宽度
    QHBoxLayout* waistHeightLayout = new QHBoxLayout;
    QLabel* lblWaistHeight = new QLabel("腰线宽度:", panel);
    QLineEdit* editWaistHeight = new QLineEdit(QString::number(LearnPara::inParam6.waistHeight), panel);
    waistHeightLayout->addWidget(lblWaistHeight);
    waistHeightLayout->addWidget(editWaistHeight);
    layout->addLayout(waistHeightLayout);

    // 高斯滤波参数
    QHBoxLayout* gaussLayout = new QHBoxLayout;
    QLabel* lblGauss = new QLabel("高斯滤波参数:", panel);
    QLineEdit* editGauss = new QLineEdit(QString::number(LearnPara::inParam6.gauss_ksize), panel);
    gaussLayout->addWidget(lblGauss);
    gaussLayout->addWidget(editGauss);
    layout->addLayout(gaussLayout);

    // 图像二值化参数
    QHBoxLayout* binThresLayout = new QHBoxLayout;
    QLabel* lblBinThres = new QLabel("图像二值化参数:", panel);
    QLineEdit* editBinThres = new QLineEdit(QString::number(LearnPara::inParam6.bin_thres_value), panel);
    binThresLayout->addWidget(lblBinThres);
    binThresLayout->addWidget(editBinThres);
    layout->addLayout(binThresLayout);

    // 基座有无面积参数
    QHBoxLayout* prodAreaLayout = new QHBoxLayout;
    QLabel* lblProdArea = new QLabel("基座有无面积参数:", panel);
    QLineEdit* editProdArea = new QLineEdit(QString::number(LearnPara::inParam6.prod_Area_Min), panel);
    prodAreaLayout->addWidget(lblProdArea);
    prodAreaLayout->addWidget(editProdArea);
    layout->addLayout(prodAreaLayout);

    // 引脚毛刺矩形高度
    QHBoxLayout* pinMcLayout = new QHBoxLayout;
    QLabel* lblPinMc = new QLabel("引脚毛刺矩形高度:", panel);
    QLineEdit* editPinMc = new QLineEdit(QString::number(LearnPara::inParam6.pin_Mc_height), panel);
    pinMcLayout->addWidget(lblPinMc);
    pinMcLayout->addWidget(editPinMc);
    layout->addLayout(pinMcLayout);

    // 基座毛刺矩形高度
    QHBoxLayout* plnMcLayout = new QHBoxLayout;
    QLabel* lblPlnMc = new QLabel("基座毛刺矩形高度:", panel);
    QLineEdit* editPlnMc = new QLineEdit(QString::number(LearnPara::inParam6.pln_Mc_height), panel);
    plnMcLayout->addWidget(lblPlnMc);
    plnMcLayout->addWidget(editPlnMc);
    layout->addLayout(plnMcLayout);

    // ----------------- 保存按钮 -----------------
    QPushButton* btnSave = new QPushButton("保存参数", panel);
    layout->addWidget(btnSave);

    connect(btnSave, &QPushButton::clicked, this, [=]() mutable {
        LearnPara::inParam6.p_pin_range_min = editPPinMin->text().toFloat();
        LearnPara::inParam6.p_pin_range_max = editPPinMax->text().toFloat();
        LearnPara::inParam6.b_pln_range_min = editBPlnMin->text().toFloat();
        LearnPara::inParam6.b_pln_range_max = editBPlnMax->text().toFloat();
        LearnPara::inParam6.n_pin_range_min = editNPinMin->text().toFloat();
        LearnPara::inParam6.n_pin_range_max = editNPinMax->text().toFloat();
        LearnPara::inParam6.plateOffsetY = editPlateOffset->text().toFloat();
        LearnPara::inParam6.plateHeight = editPlateHeight->text().toFloat();
        LearnPara::inParam6.waistOffsetY = editWaistOffset->text().toFloat();
        LearnPara::inParam6.waistHeight = editWaistHeight->text().toFloat();
        LearnPara::inParam6.gauss_ksize = editGauss->text().toInt();
        LearnPara::inParam6.bin_thres_value = editBinThres->text().toInt();
        LearnPara::inParam6.prod_Area_Min = editProdArea->text().toFloat();
        LearnPara::inParam6.pin_Mc_height = editPinMc->text().toFloat();
        LearnPara::inParam6.pln_Mc_height = editPlnMc->text().toFloat();
        saveParamAsync(); // 异步保存到 JSON
        });

    layout->addStretch();

    // ----------------- 弯脚算法操作按钮 -----------------
    QPushButton* btnRun = new QPushButton("启动弯脚算法", panel);
    QPushButton* btnThreshold = new QPushButton("弯脚 二值化输出图", panel);
    QPushButton* btnResult = new QPushButton("弯脚 检测结果输出图", panel);

    layout->addWidget(btnRun);
    layout->addWidget(btnThreshold);
    layout->addWidget(btnResult);

    connect(btnRun, &QPushButton::clicked, this, &AlgoClass_Abut::onRunAbut);
    connect(btnThreshold, &QPushButton::clicked, this, &AlgoClass_Abut::onGetThresholdImgAbut);
    connect(btnResult, &QPushButton::clicked, this, &AlgoClass_Abut::onResultOutAbut);

    layout->addStretch();

    return panel;
}


// 获取弯脚二值化图
void AlgoClass_Abut::onGetThresholdImgAbut()
{
    if (image.empty()) {
        qWarning() << "onGetThresholdImgAbut: member variable 'image' is empty. Please set input image first.";
        return;
    }

    ExportSpace::GethresholdImgAbut(image);

    emit TransMat(image);
}

// 输出弯脚检测结果
void AlgoClass_Abut::onResultOutAbut()
{
    if (image.empty()) {
        qWarning() << "onResultOutAbut: member variable 'image' is empty. Please set input image first.";
        return;
    }

    OutAbutResParam outParam;

    ExportSpace::ResultOutAbut(image, outParam);

    emit TransMat(image);
}

// 启动弯脚算法（加载图片并执行检测）
void AlgoClass_Abut::onRunAbut()
{
    QString defaultPath = "../../../data";
    QString filePath = QFileDialog::getOpenFileName(
        nullptr,
        "选择图像文件",
        defaultPath,
        "图像文件 (*.png *.jpg *.bmp *.jpeg *.tif *.tiff);;所有文件 (*.*)");

    if (filePath.isEmpty()) {
        return;
    }

    qDebug() << "用户选择图片路径：" << filePath;

    cv::Mat temp = cv::imread(filePath.toStdString());
    if (temp.empty()) {
        QMessageBox::warning(nullptr, "错误", "加载失败！");
        return;
    }

    // 假设 ExportSpace 中存在 RunAbut 接口
    if (ExportSpace::RunAbut) {
        ExportSpace::RunAbut(temp,LearnPara::inParam6);
    }
    else {
        qWarning() << "ExportSpace::RunAbut is not defined. Skipping run.";
    }

    image = temp;
    emit TransMat(temp);
}

void AlgoClass_Abut::saveParamAsync()
{
    // 创建并填充 QVariantMap
    QVariantMap mapToSave;

    mapToSave["p_pin_range_min"] = QVariantMap{ {"值", LearnPara::inParam6.p_pin_range_min} };
    mapToSave["p_pin_range_max"] = QVariantMap{ {"值", LearnPara::inParam6.p_pin_range_max} };
    mapToSave["b_pln_range_min"] = QVariantMap{ {"值", LearnPara::inParam6.b_pln_range_min} };
    mapToSave["b_pln_range_max"] = QVariantMap{ {"值", LearnPara::inParam6.b_pln_range_max} };
    mapToSave["n_pin_range_min"] = QVariantMap{ {"值", LearnPara::inParam6.n_pin_range_min} };
    mapToSave["n_pin_range_max"] = QVariantMap{ {"值", LearnPara::inParam6.n_pin_range_max} };
    mapToSave["plateOffsetY"] = QVariantMap{ {"值", LearnPara::inParam6.plateOffsetY} };
    mapToSave["plateHeight"] = QVariantMap{ {"值", LearnPara::inParam6.plateHeight} };
    mapToSave["waistOffsetY"] = QVariantMap{ {"值", LearnPara::inParam6.waistOffsetY} };
    mapToSave["waistHeight"] = QVariantMap{ {"值", LearnPara::inParam6.waistHeight} };
    mapToSave["gauss_ksize"] = QVariantMap{ {"值", LearnPara::inParam6.gauss_ksize} };
    mapToSave["bin_thres_value"] = QVariantMap{ {"值", LearnPara::inParam6.bin_thres_value} };
    mapToSave["prod_Area_Min"] = QVariantMap{ {"值", LearnPara::inParam6.prod_Area_Min} };
    mapToSave["pin_Mc_height"] = QVariantMap{ {"值", LearnPara::inParam6.pin_Mc_height} };
    mapToSave["pln_Mc_height"] = QVariantMap{ {"值", LearnPara::inParam6.pln_Mc_height} };

    // 捕获文件路径和数据
    QString filePath = m_cameralPath;
    QVariantMap dataToSave = mapToSave;

    // 运行异步任务
    QtConcurrent::run([filePath, dataToSave]() {
        if (FileOperator::writeJsonMap(filePath, dataToSave)) {
            // 保存成功，可选日志输出
        }
        else {
            // 保存失败，可选日志输出
        }
        });
}
