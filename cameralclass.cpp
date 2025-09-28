#include "CameralClass.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QMessageBox>
#include <QtConcurrent/QtConcurrent>

CameralClass::CameralClass(QObject *parent)
    : QObject(parent)
{
}

CameralClass::CameralClass(QString cameralpath, Mz_CameraConn::COperation* camOp, QObject *parent)
    : QObject(parent), m_cameralPath(cameralpath),m_camOp(camOp)
{

    QVariantMap jsonMap = FileOperator::readJsonMap(m_cameralPath);
    if (!jsonMap.isEmpty()) {
        // 将JSON数据解析到 m_param 结构体
        m_param.width = jsonMap.value("图像宽度").toMap().value("值").toInt();
        m_param.height = jsonMap.value("图像高度").toMap().value("值").toInt();
        m_param.offsetx = jsonMap.value("图像偏移X").toMap().value("值").toInt();
        m_param.offsety = jsonMap.value("图像偏移Y").toMap().value("值").toInt();
        m_param.gain = jsonMap.value("增益").toMap().value("值").toInt();
        m_param.exposure = jsonMap.value("曝光时间").toMap().value("值").toFloat();
        m_param.gama = jsonMap.value("gama值").toMap().value("值").toFloat();
        m_param.fps = jsonMap.value("帧率").toMap().value("值").toInt();

        qDebug() << "配置文件加载成功，参数已初始化。";
    }
    else {
        qDebug() << "配置文件加载失败或为空。";
    }
    


}

void CameralClass::init()
{
    if (m_camOp == nullptr)
    {
        LOG_DEBUG(GlobalLog::logger, _T("camop is null"));
        return;
    }
    m_camOp->MsvStopImageCapture();
    if (GlobalPara::cameraType == GlobalPara::HIKVISION)
    {
        char Width[] = "Width";
        int ret = m_camOp->SetParam(Width, m_param.width);
        if (ret == 0)
            LOG_DEBUG(GlobalLog::logger, QString("setParam Width successful").toStdWString().c_str());
        else
            LOG_DEBUG(GlobalLog::logger, QString("setParam Width fail: %1").arg(ret).toStdWString().c_str());

        char Height[] = "Height";
        ret = m_camOp->SetParam(Height, m_param.height);
        if (ret == 0)
            LOG_DEBUG(GlobalLog::logger, QString("setParam Height successful").toStdWString().c_str());
        else
            LOG_DEBUG(GlobalLog::logger, QString("setParam Height fail: %1").arg(ret).toStdWString().c_str());

        char OffsetX[] = "OffsetX";
        ret = m_camOp->SetParam(OffsetX, m_param.offsetx);
        if (ret == 0)
            LOG_DEBUG(GlobalLog::logger, QString("setParam OffsetX successful").toStdWString().c_str());
        else
            LOG_DEBUG(GlobalLog::logger, QString("setParam OffsetX fail: %1").arg(ret).toStdWString().c_str());

        char OffsetY[] = "OffsetY";
        ret = m_camOp->SetParam(OffsetY, m_param.offsety);
        if (ret == 0)
            LOG_DEBUG(GlobalLog::logger, QString("setParam OffsetY successful").toStdWString().c_str());
        else
            LOG_DEBUG(GlobalLog::logger, QString("setParam OffsetY fail: %1").arg(ret).toStdWString().c_str());

        // 打开启动开关
        char AcquisitionFrameRateEnable[] = "AcquisitionFrameRateEnable";
        ret = m_camOp->SetParam(AcquisitionFrameRateEnable, true);
        if (ret == 0)
            LOG_DEBUG(GlobalLog::logger, QString("setParam AcquisitionFrameRateControlEnable successful").toStdWString().c_str());
        else
            LOG_DEBUG(GlobalLog::logger, QString("setParam AcquisitionFrameRateControlEnable fail: %1").arg(ret).toStdWString().c_str());

        char GammaEnable[] = "GammaEnable";
        ret = m_camOp->SetParam(GammaEnable, true);
        if (ret == 0)
            LOG_DEBUG(GlobalLog::logger, QString("setParam GammaEnable successful").toStdWString().c_str());
        else
            LOG_DEBUG(GlobalLog::logger, QString("setParam GammaEnable fail: %1").arg(ret).toStdWString().c_str());

        char ExposureAuto[] = "ExposureAuto";
        ret = m_camOp->SetParamEnum(ExposureAuto, 0);
        if (ret == 0)
            LOG_DEBUG(GlobalLog::logger, QString("setParam ExposureAuto successful").toStdWString().c_str());
        else
            LOG_DEBUG(GlobalLog::logger, QString("setParam ExposureAuto fail: %1").arg(ret).toStdWString().c_str());

        char GainAuto[] = "GainAuto";
        ret = m_camOp->SetParam(GainAuto, false);
        if (ret == 0)
            LOG_DEBUG(GlobalLog::logger, QString("setParam GainAuto successful").toStdWString().c_str());
        else
            LOG_DEBUG(GlobalLog::logger, QString("setParam GainAuto fail: %1").arg(ret).toStdWString().c_str());

        // 设置参数
        char FrameRate[] = "AcquisitionFrameRate";
        ret = m_camOp->SetParam(FrameRate, m_param.fps);
        if (ret == 0)
            LOG_DEBUG(GlobalLog::logger, QString("setParam AcquisitionFrameRate successful").toStdWString().c_str());
        else
            LOG_DEBUG(GlobalLog::logger, QString("setParam AcquisitionFrameRate fail: %1").arg(ret).toStdWString().c_str());

        char Gamma[] = "Gamma";
        ret = m_camOp->SetParam(Gamma, m_param.gama);
        if (ret == 0)
            LOG_DEBUG(GlobalLog::logger, QString("setParam Gamma successful").toStdWString().c_str());
        else
            LOG_DEBUG(GlobalLog::logger, QString("setParam Gamma fail: %1").arg(ret).toStdWString().c_str());

        char ExposureTime[] = "ExposureTime";
        ret = m_camOp->SetParam(ExposureTime, m_param.exposure);
        if (ret == 0)
            LOG_DEBUG(GlobalLog::logger, QString("setParam ExposureTime successful").toStdWString().c_str());
        else
            LOG_DEBUG(GlobalLog::logger, QString("setParam ExposureTime fail: %1").arg(ret).toStdWString().c_str());

        char Gain[] = "Gain";
        ret = m_camOp->GetParam(Gain, &m_param.gain);
        if (ret == 0)
            LOG_DEBUG(GlobalLog::logger, QString("setParam Gain successful").toStdWString().c_str());
        else
            LOG_DEBUG(GlobalLog::logger, QString("setParam Gain fail: %1").arg(ret).toStdWString().c_str());

        char  BalanceWhiteAuto[] = "BalanceWhiteAuto";
        ret = m_camOp->SetParamEnum(BalanceWhiteAuto, 0);
        if (ret == 0)
            LOG_DEBUG(GlobalLog::logger, QString("setParam BalanceWhiteAuto successful").toStdWString().c_str());
        else
            LOG_DEBUG(GlobalLog::logger, QString("setParam BalanceWhiteAuto fail: %1").arg(ret).toStdWString().c_str());

    }
    else
    {
        char Width[] = "Width";
        int ret = m_camOp->SetParam(Width, m_param.width);
        if (ret == 0)
            LOG_DEBUG(GlobalLog::logger, QString("setParam Width successful").toStdWString().c_str());
        else
            LOG_DEBUG(GlobalLog::logger, QString("setParam Width fail: %1").arg(ret).toStdWString().c_str());

        char Height[] = "Height";
        ret = m_camOp->SetParam(Height, m_param.height);
        if (ret == 0)
            LOG_DEBUG(GlobalLog::logger, QString("setParam Height successful").toStdWString().c_str());
        else
            LOG_DEBUG(GlobalLog::logger, QString("setParam Height fail: %1").arg(ret).toStdWString().c_str());

        char OffsetX[] = "OffsetX";
        ret = m_camOp->SetParam(OffsetX, m_param.offsetx);
        if (ret == 0)
            LOG_DEBUG(GlobalLog::logger, QString("setParam OffsetX successful").toStdWString().c_str());
        else
            LOG_DEBUG(GlobalLog::logger, QString("setParam OffsetX fail: %1").arg(ret).toStdWString().c_str());

        char OffsetY[] = "OffsetY";
        ret = m_camOp->SetParam(OffsetY, m_param.offsety);
        if (ret == 0)
            LOG_DEBUG(GlobalLog::logger, QString("setParam OffsetY successful").toStdWString().c_str());
        else
            LOG_DEBUG(GlobalLog::logger, QString("setParam OffsetY fail: %1").arg(ret).toStdWString().c_str());

        // 关闭开关
        char AcquisitionFrameRateEnable[] = "AcquisitionFrameRateEnable";
        ret = m_camOp->SetParam(AcquisitionFrameRateEnable, true);
        if (ret == 0)
            LOG_DEBUG(GlobalLog::logger, QString("setParam AcquisitionFrameRateEnable successful").toStdWString().c_str());
        else
            LOG_DEBUG(GlobalLog::logger, QString("setParam AcquisitionFrameRateEnable fail: %1").arg(ret).toStdWString().c_str());

        char ExposureAuto[] = "ExposureAuto";
        ret = m_camOp->SetParamEnum(ExposureAuto, 0);
        if (ret == 0)
            LOG_DEBUG(GlobalLog::logger, QString("setParam ExposureAuto successful").toStdWString().c_str());
        else
            LOG_DEBUG(GlobalLog::logger, QString("setParam ExposureAuto fail: %1").arg(ret).toStdWString().c_str());

        // 设置参数
        char FrameRate[] = "AcquisitionFrameRate";
        ret = m_camOp->SetParam(FrameRate, (float)m_param.fps);
        if (ret == 0)
            LOG_DEBUG(GlobalLog::logger, QString("setParam AcquisitionFrameRate successful").toStdWString().c_str());
        else
            LOG_DEBUG(GlobalLog::logger, QString("setParam AcquisitionFrameRate fail: %1").arg(ret).toStdWString().c_str());

        char Gamma[] = "Gamma";
        ret = m_camOp->SetParam(Gamma, m_param.gama);
        if (ret == 0)
            LOG_DEBUG(GlobalLog::logger, QString("setParam Gamma successful").toStdWString().c_str());
        else
            LOG_DEBUG(GlobalLog::logger, QString("setParam Gamma fail: %1").arg(ret).toStdWString().c_str());

        char ExposureTime[] = "ExposureTime";
        ret = m_camOp->SetParam(ExposureTime, m_param.exposure);
        if (ret == 0)
            LOG_DEBUG(GlobalLog::logger, QString("setParam ExposureTime successful").toStdWString().c_str());
        else
            LOG_DEBUG(GlobalLog::logger, QString("setParam ExposureTime fail: %1").arg(ret).toStdWString().c_str());

        char Gain[] = "GainRaw";
        ret = m_camOp->GetParam(Gain, &m_param.gain);
        if (ret == 0)
            LOG_DEBUG(GlobalLog::logger, QString("setParam Gain successful").toStdWString().c_str());
        else
            LOG_DEBUG(GlobalLog::logger, QString("setParam Gain fail: %1").arg(ret).toStdWString().c_str());
    }
    
    m_camOp->MsvStartImageCapture();
}

void CameralClass::change()
{
    init();
}

void CameralClass::saveParamAsync()
{
    qDebug() << "CameralClass: Starting async save to" << m_cameralPath;

    // 直接在函数内部创建并填充 QVariantMap
    QVariantMap mapToSave;
    mapToSave["图像宽度"] = QVariantMap{ {"值", m_param.width} };
    mapToSave["图像高度"] = QVariantMap{ {"值", m_param.height} };
    mapToSave["图像偏移X"] = QVariantMap{ {"值", m_param.offsetx} };
    mapToSave["图像偏移Y"] = QVariantMap{ {"值", m_param.offsety} };
    mapToSave["增益"] = QVariantMap{ {"值", m_param.gain} };
    mapToSave["曝光时间"] = QVariantMap{ {"值", m_param.exposure} };
    mapToSave["gama值"] = QVariantMap{ {"值", m_param.gama} };
    mapToSave["帧率"] = QVariantMap{ {"值", m_param.fps} };

    // 捕获文件路径和数据
    QString filePath = m_cameralPath;
    QVariantMap dataToSave = mapToSave;

    // 运行异步任务
    QtConcurrent::run([filePath, dataToSave]() {
        qDebug() << "Background thread: Executing CameralClass parameter save...";
        if (FileOperator::writeJsonMap(filePath, dataToSave)) {
            qDebug() << "Background thread: Parameters saved successfully to" << filePath;
        }
        else {
            qWarning() << "Background thread: Failed to save parameters to" << filePath;
        }
        });

    qDebug() << "CameralClass: Async save initiated, main thread continues.";
}

