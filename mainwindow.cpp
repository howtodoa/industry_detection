#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "cameralmenu.h"
#include "cameral.h"
#include "login.h"
#include "public.h"
#include "parawidget.h"
#include "syspara.h"
#include "fileoperator.h"
#include "cameralabelwidget.h"
#include "addcameradialog.h"
#include "MZ_ADOConn.h"
#include "MZ_VC3000.h"
#include "tcp_client.h"
#include "rezultinfo_nayin.h"
#include <opencv2/opencv.hpp>
#include "MsvDeviceLib.h"
#include <QThread>
#include <windows.h>
#include <tlhelp32.h>
#include <dwmapi.h>
#include <CapacitanceProgram.h>
#include "rezultinfo_plate.h"
#include "rezultinfo_lift.h"
#include "rezultinfo_abut.h"
#include "rezultinfo_top.h"
#include "rezultinfo_side.h"
#include "rezultinfo_pin.h"
#include "algoclass_lift.h"
#include "algoclass_nayin.h"
#include "algoclass_plate.h"
#include "algoclass_pin.h"
#include "algoclass_side.h"
#include "algoclass_top.h"
#include "aboutwidget.h"
#include "algoclass_abut.h"
#include "rezultinfo_flowerpin.h"
#include "Api_FlowerPinDetection.h"
#include "rezultinfo_look.h"
#include "outputhread_flower.h"
#include "displayinfowidget_flower.h"
#include "rezultinfo_Xs.h"
#include "rezultinfo_ny.h"
#include "rezultinfo_bottom.h"

namespace AppConfig
{
    std::string GetIoOutputMode() { return "NPN"; }


    int camera1Output = 1;
    int camera2Output = 2;
    int camera3Output = 3;
    int camera4Output = 4;
    int camera5Output = 5;
    int camera6Output = 6;
    int camera7Output = 7;

    int runningOutput = 8;
    int ErrorOutput = 9;

    int location1lightValue = 100;
    int location2lightValue = 100;
    int location3lightValue = 100;
    int location4lightValue = 100;


}



void MainWindow::prospect(int code,std::string temppath)
{
    // cv::Mat temp=loadMatFromFile(QString::fromStdString(temppath));
    // test.RunStamp(code,temp,0);
    // cv::Mat dstImg;
    // OutStampResParam out;
    // test.ResultOutStamp(dstImg,out);
    // test.ReleaseStamp(code);

}

void MainWindow::test_show()
{
        // QPixmap pixmap(":/images/resources/images/image.jpg");
    //cameraLabels[4]->displayimg(pixmap);

    QPixmap pixmap1(":/images/resources/images/test.jpg");
    cameraLabels[0]->displayimg(pixmap1);




    std::string folderPathname = "E:/1.jpg";
    cv::Mat temp = cv::imread(folderPathname);

     ExportSpace::InitializeAbut();
    InAbutParam inParam
        {
            inParam.angleNum = 0,
            inParam.p_pin_range_min = 0,
            inParam.p_pin_range_max = 0.43,
            inParam.b_pln_range_min = 0.45,
            inParam.b_pln_range_max = 0.75,
            inParam.n_pin_range_min = 0.78,
            inParam.n_pin_range_max = 1,
            inParam.plateOffsetY = 130,
            inParam.plateHeight = 100,
            inParam.waistOffsetY = 200,
            inParam.waistHeight = 50,
            inParam.gauss_ksize = 5,
            inParam.bin_thres_value = 60,
            inParam.prod_Area_Min = 0.1,
            inParam.pin_Mc_height = 20,
            inParam.pln_Mc_height = 20,
        };

    cams[1]->RI->printProcessedData();
    cams[2]->RI->printProcessedData();
  //  int judgmentResult1 = cams[2]->RI->judge_stamp(OutResParam);
   OutStampResParam testResult;
    testResult.phi = -15;
    testResult.posWidth = 5;
    testResult.posErr = 0.08f;
    testResult.negWidth = 7;
    testResult.negErr = 0.03f;
    testResult.textNum = 3;
    testResult.logoScores = 0.1f;
    testResult.textScores = {0.8f, 0.7f,0.3f};
   int judgmentResult1 = cams[2]->RI->judge_stamp(testResult);

    if (judgmentResult1 == 0) {
        qDebug() << "第一次判断结果：                   所有参数都在范围内 。";
    } else {
        qDebug() << "第一次判断结果：                    有参数不在范围内。";
    }


    OutAbutResParam test_data_pass;
    test_data_pass.isHaveProd = true;
    test_data_pass.isHavePpin = true;
    test_data_pass.isHaveNpin = true;
    test_data_pass.isHaveBpln = true;
    test_data_pass.Pin_C = 12.0f;         // 范围 10-15
    test_data_pass.shuyao_width = 6.0f;   // 范围 5-8
    test_data_pass.plate_width = 11.0f;   // 范围 10-12
    test_data_pass.p_pin_over_pln = 0.5f; // 范围 0-1
    test_data_pass.n_pin_over_pln = 0.5f; // 范围 0-1
    test_data_pass.p_pin_H = 0.5f;        // 范围 0-1
    test_data_pass.n_pin_H = 0.5f;        // 范围 0-1
    test_data_pass.p_n_height_diff = 0.5f; // 范围 0-1
    test_data_pass.p_pin_Angle = 90.0f;   // 范围 85-95
    test_data_pass.n_pin_Angle = 90.0f;   // 范围 85-95
    test_data_pass.p_pin_Mc = 0.0f;       // 上限 0
    test_data_pass.n_pin_Mc = 0.0f;       // 上限 0
    test_data_pass.b_pln_Mc = 0.0f;       // 上限 0
     cams[4]-> RI->printProcessedData();
    judgmentResult1=cams[4]-> RI->judge_abut(test_data_pass);
    if (judgmentResult1 == 0) {
        qDebug() << "第一次判断结果：                   所有参数都在范围内 。";
    } else {
        qDebug() << "第一次判断结果：                    有参数不在范围内。";
    }

}

void MainWindow::setupOutPutThread()
{
#ifdef USE_MAIN_WINDOW_FLOWER
    m_outputThread = new OutPutThread_Flower(this);
    m_outputThread->setObjectName("OutPutThread_Flower");
    m_outputThread->start();

#else
    m_outputThread = new OutPutThread(this);
    m_outputThread->setObjectName("OutPutThread");
    m_outputThread->start();
#endif
    qDebug() << "OutPutThread started successfully.";
}

void MainWindow::loadjson_layer(const QString& filePath)
{
    // 使用 FileOperator 读取 JSON 文件为 QVariantMap
    QVariantMap configMap = FileOperator::readJsonMap(filePath);


    if (configMap.isEmpty()) {
        qWarning() << "Failed to load configuration from" << filePath;
        return;
    }

    // 从 configMap 中解析每个 ParamDetail，并将其 '值' 赋给 SystemPara 的静态成员
    // 检查每个键是否存在，以避免潜在的崩溃并提供更好的错误提示

    if (configMap.contains("CameraType")) {
        ParamDetail detail(configMap.value("CameraType").toMap());
        QString strValue = detail.value.toString().trimmed();
        if (strValue == "HIKVISION") {
            GlobalPara::cameraType = GlobalPara::HIKVISION;
        } else if (strValue == "DHUA") {
            GlobalPara::cameraType = GlobalPara::DHUA;
        } else {
            qWarning() << "Unknown CameraType:" << strValue;
        }
    } else {
        qWarning() << "JSON file missing 'CameraType' entry.";
    }

    if (configMap.contains("IoType")) {
        ParamDetail detail(configMap.value("IoType").toMap());
        QString strValue = detail.value.toString().trimmed();
        if (strValue == "VC3000") {
            GlobalPara::ioType = GlobalPara::VC3000;
        } else if (strValue == "VC3000H") {
            GlobalPara::ioType = GlobalPara::VC3000H;
        } else if (strValue == "PCI1230") {
            GlobalPara::ioType = GlobalPara::PCI1230;
        } else {
            qWarning() << "Unknown IoType:" << strValue;
        }
    } else {
        qWarning() << "JSON file missing 'IoType' entry.";
    }

    if (configMap.contains("Envirment")) {
        ParamDetail detail(configMap.value("Envirment").toMap());
        QString strValue = detail.value.toString().trimmed();
        if (strValue == "LocationEn") {
            GlobalPara::envirment = GlobalPara::LocationEn;
        } else if (strValue == "IPCEn") {
            GlobalPara::envirment = GlobalPara::IPCEn;
        } else {
            qWarning() << "Unknown Envirment:" << strValue;
        }
    } else {
        qWarning() << "JSON file missing 'Envirment' entry.";
    }

    if (configMap.contains("VERSION")) {
        ParamDetail detail(configMap.value("VERSION").toMap());
        SystemPara::VERSION = detail.value.toString().trimmed();
    }
    else {
        qWarning() << "JSON file missing 'VERSION' entry.";
    }

    if (configMap.contains("RunPoint")) {
        ParamDetail detail(configMap.value("RunPoint").toMap());
        GlobalPara::RunPoint = detail.value.toInt();
        qDebug() << "Parsed RunPoint:" << GlobalPara::RunPoint;
    }
    else {
        qWarning() << "JSON file missing 'RunPoint' entry.";
    }

    if (configMap.contains("PULSE")) {
        ParamDetail detail(configMap.value("PULSE").toMap());
        GlobalPara::pulse = detail.value.toBool();
        qDebug() << "Parsed pulse:" << GlobalPara::pulse;
    }
    else {
        qWarning() << "JSON file missing 'pulse' entry.";
    }

    if (configMap.contains("OK_DIR")) {
        ParamDetail detail(configMap.value("OK_DIR").toMap());
        SystemPara::OK_DIR = detail.value.toString();
        qDebug() << "Parsed OK_DIR:" << SystemPara::OK_DIR;
    } else {
        qWarning() << "JSON file missing 'OK_DIR' entry.";
    }

    if (configMap.contains("NG_DIR")) {
        ParamDetail detail(configMap.value("NG_DIR").toMap());
        SystemPara::NG_DIR = detail.value.toString();
        qDebug() << "Parsed NG_DIR:" << SystemPara::NG_DIR;
    } else {
        qWarning() << "JSON file missing 'NG_DIR' entry.";
    }

    if (configMap.contains("LOG_DIR")) {
        ParamDetail detail(configMap.value("LOG_DIR").toMap());
        SystemPara::LOG_DIR = detail.value.toString();
        qDebug() << "Parsed LOG_DIR:" << SystemPara::LOG_DIR;
    } else {
        qWarning() << "JSON file missing 'LOG_DIR' entry.";
    }

    if (configMap.contains("DATA_DIR")) {
        ParamDetail detail(configMap.value("DATA_DIR").toMap());
        SystemPara::DATA_DIR = detail.value.toString();
        qDebug() << "Parsed DATA_DIR:" << SystemPara::DATA_DIR;
    } else {
        qWarning() << "JSON file missing 'DATA_DIR' entry.";
    }

    if (configMap.contains("DeviceId")) {
        ParamDetail detail(configMap.value("DeviceId").toMap());
        GlobalPara::DeviceId = detail.value.toString();
        qDebug() << "Parsed DATA_DIR:" << GlobalPara::DeviceId;
    }
    else {
        qWarning() << "JSON file missing 'DATA_DIR' entry.";
    }

    
    if (configMap.contains("MERGENUM")) {
        ParamDetail detail(configMap.value("MERGENUM").toMap());
        GlobalPara::MergePointNum = detail.value.toInt();
        qDebug() << "Parsed MergePointNum" << GlobalPara::MergePointNum;
    }
    else {
        qWarning() << "JSON file missing 'DATA_DIR' entry.";
    }

    if (configMap.contains("LIGHT1")) {
        ParamDetail detail(configMap.value("LIGHT1").toMap());
        GlobalPara::Light1 = detail.value.toInt();
        qDebug() << "Parsed Light1" << GlobalPara::Light1;
    }
    else {
        qWarning() << "JSON file missing 'LIGHT1' entry.";
    }

    if (configMap.contains("LIGHT2")) {
        ParamDetail detail(configMap.value("LIGHT2").toMap());
        GlobalPara::Light2 = detail.value.toInt();
        qDebug() << "Parsed Light2" << GlobalPara::Light2;
    }
    else {
        qWarning() << "JSON file missing 'Light2' entry.";
    }

    if (configMap.contains("LIGHT3")) {
        ParamDetail detail(configMap.value("LIGHT3").toMap());
        GlobalPara::Light3 = detail.value.toInt();
        qDebug() << "Parsed Light3" << GlobalPara::Light3;
    }
    else {
        qWarning() << "JSON file missing 'LIGHT3' entry.";
    }

    if (configMap.contains("LIGHT4")) {
        ParamDetail detail(configMap.value("LIGHT4").toMap());
        GlobalPara::Light4 = detail.value.toInt();
        qDebug() << "Parsed Light3" << GlobalPara::Light4;
    }
    else {
        qWarning() << "JSON file missing 'Light4' entry.";
    }


    if (configMap.contains("ROOT_DIR")) {
        ParamDetail detail(configMap.value("ROOT_DIR").toMap());
        SystemPara::ROOT_DIR = detail.value.toString();
        qDebug() << "Parsed ROOT_DIR:" << SystemPara::ROOT_DIR;
    } else {
        qWarning() << "JSON file missing 'ROOT_DIR' entry.";
    }
    qDebug() << "JSON parsing complete and SystemPara members populated.";
}

void MainWindow::loadjson_layer2(const QString& filePath)
{
    // 使用 FileOperator 读取 JSON 文件为 QVariantMap
    QVariantMap configMap = FileOperator::readJsonMap(filePath);

    // 检查是否成功读取到配置
    if (configMap.isEmpty()) {
        qWarning() << "Failed to load configuration from" << filePath;
        return;
    }

    // 遍历 configMap 中的所有顶层键（例如 "相机1"）
    for (auto it = configMap.begin(); it != configMap.end(); ++it) {
        if (it.value().type() == QVariant::Map) {
            QVariantMap cameraData = it.value().toMap(); // 获取 "相机1" 下面的所有数据

            Camerinfo currentCamera; // 创建一个 Camerinfo 对象来存储当前相机的信息

            // 相机是否可见,如果不可见直接跳过
            if (cameraData.contains("检测")) {
                ParamDetail nameDetail(cameraData.value("检测").toMap());
                currentCamera.check = nameDetail.value.toString();
                if(currentCamera.check=="false") continue;
            } else {
                qWarning() << "Missing '相机名称' for camera:" << it.key();
            }

            // 解析 "相机名称"
            if (cameraData.contains("相机名称")) {
                ParamDetail nameDetail(cameraData.value("相机名称").toMap());
                currentCamera.name = nameDetail.value.toString();
            } else {
                qWarning() << "Missing '相机名称' for camera:" << it.key();
            }

            // 解析 "相机映射"
            if (cameraData.contains("相机映射")) {
                ParamDetail nameDetail(cameraData.value("相机映射").toMap());
                currentCamera.mapping = nameDetail.value.toString();
            } else {
                qWarning() << "Missing '相机映射' for camera:" << it.key();
            }

            // 解析 "相机线路"
            if (cameraData.contains("Route编号")) {
                ParamDetail routeDetail(cameraData.value("Route编号").toMap());
                currentCamera.rounte = routeDetail.value.toInt();
            } else {
                qWarning() << "Missing 'Route编号' for camera:" << it.key();
            }

            // 解析 "相机ip"
            if (cameraData.contains("相机ip")) {
                ParamDetail ipDetail(cameraData.value("相机ip").toMap());
                currentCamera.ip = ipDetail.value.toString();
            } else {
                qWarning() << "Missing '相机ip' for camera:" << it.key();
            }

            // 解析 "补偿值"
            if (cameraData.contains("补偿值")) {
                ParamDetail fixDetail(cameraData.value("补偿值").toMap());
                currentCamera.fix = fixDetail.value.toDouble();
            }
            else {
                qWarning() << "Missing '相机ip' for camera:" << it.key();
            }

           // 解析"触发点位"
            if (cameraData.contains("触发点位")) {
                ParamDetail pointNumber(cameraData.value("触发点位").toMap());
                currentCamera.pointNumber = pointNumber.value.toInt();
            } else {
                qWarning() << "Missing '触发点位' for camera:" << it.key();
            }

            //解析“存图开关”
            if (cameraData.contains("存图开关")) {
                ParamDetail saveflagDetail(cameraData.value("存图开关").toMap());
                currentCamera.saveflag = saveflagDetail.value.toInt();
            } else {
                qWarning() << "Missing '存图开关' for camera:" << it.key();
            }

            //解析 “标定值”
            if (cameraData.contains("标定值")) {
                ParamDetail scaleFactorDetail(cameraData.value("标定值").toMap());
                currentCamera.scaleFactor=scaleFactorDetail.value.toDouble();
            } else {
                qWarning() << "Missing '标定值' for camera:" << it.key();
            }

            //解析 “角度”
            if (cameraData.contains("角度")) {
                ParamDetail AngleFactorDetail(cameraData.value("角度").toMap());
                currentCamera.Angle = AngleFactorDetail.value.toInt();
            }
            else {
                qWarning() << "Missing '角度' for camera:" << it.key();
            }
            //解析 “屏蔽开关”
            if (cameraData.contains("屏蔽开关")) {
                ParamDetail SheildDetail(cameraData.value("屏蔽开关").toMap());
                currentCamera.Shield = SheildDetail.value.toBool();
            }
            else {
                qWarning() << "Missing '屏蔽开关' for camera:" << it.key();
            }

            //解析 “空料OK”
            if (cameraData.contains("空料OK")) {
                ParamDetail EmptyIsOKDetail(cameraData.value("空料OK").toMap());
                currentCamera.EmptyIsOK = EmptyIsOKDetail.value.toBool();
            }
            else {
                qWarning() << "Missing '空料OK' for camera:" << it.key();
            }

            // 解析 "相机配置文件路径"
            if (cameraData.contains("配置文件路径")) {
                ParamDetail snDetail(cameraData.value("配置文件路径").toMap());
                currentCamera.path= snDetail.value.toString();
            } else {
                qWarning() << "Missing '配置文件路径' for camera:" << it.key();
            }
            // 将解析好的 Camerinfo 对象添加到 QVector 中
            caminfo.append(currentCamera);
            qDebug() << "Parsed Camera Info for" << it.key()
                     << ": Name=" << currentCamera.name
                     << ", IP=" << currentCamera.ip
                     << ",pointNumber=" << currentCamera.pointNumber
                     <<",saveflag="<<currentCamera.saveflag
                     <<",Angle="<<currentCamera.Angle
                     <<",scalefator="<<currentCamera.scaleFactor
                     <<",path="<<currentCamera.path
                     << ",EmptyIsOK=" <<currentCamera.EmptyIsOK
                     << ",Shield=" <<currentCamera.Shield;
        } else {
            qWarning() << "Top-level key '" << it.key() << "' is not a map. Skipping.";
        }
    }
    qDebug() << "Camera configuration parsing complete.";
}

void MainWindow::init_algo()
{
    int ret;
    for (int i = 0; i < cams.size(); i++)
    {
 
        //捺印
      if(cams[i]->indentify=="NaYin")  ret = ExportSpace::InitializeStamp();


        //座板

      if (cams[i]->indentify == "Plate")  ret = ExportSpace::InitializePlate();
        if (ret == 1) std::cout << "InitializePlate fail" << std::endl;
        else std::cout << "InitializePlate successful" << std::endl;

        qDebug() << "ret:   =" << ret;

        //翘脚
        if(cams[i]->indentify=="Lift")ret = ExportSpace::InitializeLift();

        if (ret == 1) std::cout << "InitializeLift fail" << std::endl;
        else std::cout << "InitializeLift successful" << std::endl;

        qDebug() << "ret:   =" << ret;


        //拱脚正光
        if (cams[i]->indentify == "YYGJ")ret = ExportSpace::InitializeGJ();

        if (ret == 1) std::cout << "InitializeGJ fail" << std::endl;
        else std::cout << "InitializeGJ successful" << std::endl;
        qDebug() << "ret:   =" << ret;
    }


      std::string temppath = "../../../resources/images/Carrier_NaYin.jpg";
      cv::Mat pic = cv::imread(temppath);   
      for (int i = 0; i < cams.size(); i++)
      {
          if (cams[i]->indentify == "Carrier_NaYin")
          {
              ret = ExportSpace::RunStamp(pic, 0, 1, LearnPara::inParam1);
              cv::Mat mat;
              OutStampResParam para;
              ExportSpace::ResultOutStamp(mat, para, 1);
             LearnPara::inNum = para.textNum;
#ifdef  ADAPTATEION
#else
              for (int j = LearnPara::inNum; j < 15; j++)
              {
                  cams[i]->RI->m_PaintData[7 + j].check = false;
              }
#endif
              break;
          }
      }

      temppath = "../../../resources/images/NaYin.jpg";
      pic = cv::imread(temppath);
      for (int i = 0; i < cams.size(); i++)
      {
          if (cams[i]->indentify == "NaYin")
          {
              ret = ExportSpace::RunStamp(pic, 0, 0, LearnPara::inParam2);
              cv::Mat mat;
              OutStampResParam para;
              ExportSpace::ResultOutStamp(mat, para, 0);
              LearnPara::inNum = para.textNum;
#ifdef  ADAPTATEION

#else
              for (int j = 0; j < LearnPara::inNum; j++)
              {
                  cams[i]->RI->m_PaintData[7 + j].check = true;
              }
              for (int j = LearnPara::inNum; j < 15; j++)
              {
                  cams[i]->RI->m_PaintData[7+j].check = false;
              }
#endif
              break;
          }

      }
      

}

void MainWindow::init_algo_Braider()
{
    // 侧面 
   int ret = BraidedTapeSpace::InitializeSide();
    if (ret == 1)
        std::cout << "InitializeSide fail" << std::endl;
    else
        std::cout << "InitializeSide successful" << std::endl;
    qDebug() << "ret:   =" << ret;

    // 顶部
    ret = BraidedTapeSpace::InitializeTop();
    if (ret == 1)
        std::cout << "InitializeTop fail" << std::endl;
    else
        std::cout << "InitializeTop successful" << std::endl;
    qDebug() << "ret:   =" << ret;

    // 引脚
    ret = BraidedTapeSpace::InitializePin();
    if (ret == 1)
        std::cout << "InitializePin fail" << std::endl;
    else
        std::cout << "InitializePin successful" << std::endl;
    qDebug() << "ret:   =" << ret;

    std::string temppath = "../../../resources/images/Top.jpg";
    cv::Mat pic = cv::imread(temppath);
    for (int i = 0; i < cams.size(); i++)
    {
        if (cams[i]->indentify == "Top")
        {
            std::string temppath = "../../../resources/images/Top.jpg";
            cv::Mat pic = cv::imread(temppath);
            InTopParam inpara=LearnPara::inParam3;
            inpara.al_core = true;
            ret = BraidedTapeSpace::RunTop(pic, inpara);

            OutTopParam outpara;

            BraidedTapeSpace::ResultOutTop(pic, outpara);

        }
        if (cams[i]->indentify == "Side")
        {
            std::string temppath = "../../../resources/images/Side.jpg";
            cv::Mat pic = cv::imread(temppath);
            InSideParam inpara = LearnPara::inParam4;
            inpara.al_core = true;
            ret = BraidedTapeSpace::RunSide(pic, inpara);

            OutSideParam outpara;

            BraidedTapeSpace::ResultOutSide(pic, outpara);

        }
        if (cams[i]->indentify == "Pin")
        {
            std::string temppath = "../../../resources/images/Pin.jpg";
            bool exists = QFile::exists(QString::fromStdString(temppath));
            if (exists == false) continue;
            cv::Mat pic = cv::imread(temppath);
            InPinParam inpara = LearnPara::inParam5;
            inpara.al_core = true;
            BraidedTapeSpace::RunPin(pic, inpara);
             
            OutPinParam outpara;
            BraidedTapeSpace::ResultOutPin(pic, outpara);
        }
    }

}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //ui->mainToolBar->setVisible(false);
    setWindowTitle("电容视觉检测系统V2.0.0_Beta");

    setWindowIcon(QIcon(":/images/resources/images/oo.ico"));

//    setTearOffEnabled(false);

    HWND hwnd = (HWND)this->winId();
    BOOL useDarkMode = TRUE;


    HRESULT result = DwmSetWindowAttribute(
        hwnd,
        20, // DWMWA_USE_IMMERSIVE_DARK_MODE for Windows 10 1809+
        &useDarkMode,
        sizeof(useDarkMode)
    );


    this->setStyleSheet(
        "QMainWindow {"
        "    background-color: rgb(24, 26, 32);"
        "}"
        "QMenuBar {"
        "    background-color: rgb(24, 26, 32);"
        "    color: white;"
        "}"
        "QMenuBar::item:selected {"
        "    background-color: #343844; /* 一个稍亮的颜色用于悬停 */"
        "}"
        "QToolBar#mainToolBar {" 
        "    background-color: rgb(24, 26, 32);"
        "    border: none;"
        "    min-height: 40px; /* 增加工具栏高度 */"
        "    max-height: 40px;"
        "}"
        "QStatusBar {"
        "    background-color: rgb(24, 26, 32);"
        "    color: white;"
        "}"
    );

    // 获取主屏幕的分辨率
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry(); // 获取屏幕的几何信息
    int screenWidth = screenGeometry.width();  // 屏幕宽度
    int screenHeight = screenGeometry.height(); // 屏幕高度

    int windowWidth = screenWidth * 1;
    int windowHeight = screenHeight * 1;

    resize(windowWidth, windowHeight);

    // 将窗口居中
    int x = (screenWidth - windowWidth) / 2;
    int y = (screenHeight - windowHeight) / 2;
    move(x, y);
    loadjson_layer(SystemPara::GLOBAL_DIR);
    loadjson_layer2(SystemPara::CAMERAL_DIR);
    if (GlobalPara::envirment == GlobalPara::IPCEn && GlobalPara::ioType == GlobalPara::VC3000H) initPCI_VC3000H();
    else if (GlobalPara::envirment == GlobalPara::IPCEn && GlobalPara::ioType == GlobalPara::VC3000) initPCI_VC3000();
    init_log();
    init_cap();
    initcams(caminfo.size());
    initSqlite3Db_Plater();
#ifdef FOURBRADER
	init_algo_FourBrader();
#else
    init_algo();
#endif
    setupImageSaverThread();
    CreateMenu();
    CreateImageGrid(caminfo.size());
    updateCameraStats();
    setupMonitorThread();
    if (GlobalPara::envirment == GlobalPara::IPCEn) this->onStartAllCamerasClicked();
    initCameralPara();
    setupUpdateTimer();
#ifdef ADAPTATEION || FOURBRADER
    if (GlobalPara::MergePointNum > 0)
    {
        for(int i=0;i< GlobalPara::MergePointNum;i++)
        {
            QString camId = QString::fromStdString(cams[i]->indentify);
            int maxSize = 1;
            MyDeque<int> temp_deque(maxSize);
            MergePointVec.insert(camId, std::move(temp_deque));
		}
        GlobalPara::MergePoint = cams[0]->pointNumber;
        setupOutPutThread();
    }
#endif // ADAPTATEION

    Sleep(50);
    this->onPhotoAllCamerasClicked();
}

        MainWindow::MainWindow(QString str, QWidget * parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow)
    {
        ui->setupUi(this);

        //ui->mainToolBar->setVisible(false);
        setWindowTitle("钉卷机视觉检测系统V2.0.0_Beta");

        setWindowIcon(QIcon(":/images/resources/images/oo.ico"));


        HWND hwnd = (HWND)this->winId();
        BOOL useDarkMode = TRUE;


        HRESULT result = DwmSetWindowAttribute(
            hwnd,
            20, // DWMWA_USE_IMMERSIVE_DARK_MODE for Windows 10 1809+
            &useDarkMode,
            sizeof(useDarkMode)
        );


        this->setStyleSheet(
            "QMainWindow {"
            "    background-color: rgb(24, 26, 32);"
            "}"
            "QMenuBar {"
            "    background-color: rgb(24, 26, 32);"
            "    color: white;"
            "}"
            "QMenuBar::item:selected {"
            "    background-color: #343844; /* 一个稍亮的颜色用于悬停 */"
            "}"
            "QToolBar#mainToolBar {"
            "    background-color: rgb(24, 26, 32);"
            "    border: none;"
            "    min-height: 40px; /* 增加工具栏高度 */"
            "    max-height: 40px;"
            "}"
            "QStatusBar {"
            "    background-color: rgb(24, 26, 32);"
            "    color: white;"
            "}"
        );

        // ------------------------------------------------------------------
        // 【核心修改区域】：使用 availableGeometry 设置窗口尺寸和最大限制
        // ------------------------------------------------------------------
        QScreen* screen = QGuiApplication::primaryScreen();
        if (screen) {
            // 获取屏幕的可用几何尺寸（排除了任务栏/Dock栏等）
            QRect availableGeometry = screen->availableGeometry();

            // 1. 强制限制主窗口的最大尺寸为可用尺寸，防止布局溢出
            this->setMaximumSize(availableGeometry.size());

            // 2. 设置初始尺寸为可用尺寸，让窗口占据整个可用屏幕
            int windowWidth = availableGeometry.width();
            int windowHeight = availableGeometry.height();
            resize(windowWidth, windowHeight);

            // 3. 将窗口移动到可用屏幕的左上角 (通常是 0, 0)
            // 这确保窗口占据整个可用空间
            move(availableGeometry.topLeft());
        }
        // ------------------------------------------------------------------


        SystemPara::CAMERAL_DIR = "../../../ini/globe/cameral_Flower.json";
        loadjson_layer(SystemPara::GLOBAL_DIR);
        loadjson_layer2(SystemPara::CAMERAL_DIR);
        if (GlobalPara::envirment == GlobalPara::IPCEn && GlobalPara::ioType == GlobalPara::VC3000H) initPCI_VC3000H();
        init_log();
        init_cap();
        initcams(caminfo.size());
        init_algo_Flower();
        setupImageSaverThread();
        CreateMenu();
        CreateImageGrid_Braider(caminfo.size());
        setupMonitorThread();
        setupUpdateTimer();
        initSqlite3Db_Brader();
        if (GlobalPara::MergePointNum > 0)
        {
            for (int i = 0; i < GlobalPara::MergePointNum; i++)
            {
                QString camId = QString::fromStdString(cams[i]->indentify);

                std::wstring wstrCamId = camId.toStdWString();

                // 2. 将格式化后的日志消息也用 std::wstring 构造
                std::wstring logMsg = L"Cam ID is: " + wstrCamId;

                // 3. 使用您的 LOG_DEBUG 宏
                LOG_DEBUG(GlobalLog::logger, logMsg.c_str());

                int maxSize = 4;
                MyDeque<int> temp_deque(maxSize);
                MergePointVec.insert(camId, std::move(temp_deque));
            }
            GlobalPara::MergePoint = cams[0]->pointNumber;
            setupOutPutThread();
        }
        initCameralPara();
        if (GlobalPara::envirment == GlobalPara::IPCEn) this->onStartAllCamerasClicked();
    }
void MainWindow::init_algo_FourBrader()
{
    g_detector = Obj<WeldingDetector>::GetInstance();
    g_detector->Initialize(&g_detector->m_params);
	//g_detector->m_params = g_params;
}

MainWindow::MainWindow(int mode,QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //ui->mainToolBar->setVisible(false);
    setWindowTitle("编带机视觉检测系统V2.0.0_Beta");

    setWindowIcon(QIcon(":/images/resources/images/oo.ico"));


    HWND hwnd = (HWND)this->winId();
    BOOL useDarkMode = TRUE;


    HRESULT result = DwmSetWindowAttribute(
        hwnd,
        20, // DWMWA_USE_IMMERSIVE_DARK_MODE for Windows 10 1809+
        &useDarkMode,
        sizeof(useDarkMode)
    );


    this->setStyleSheet(
        "QMainWindow {"
        "    background-color: rgb(24, 26, 32);"
        "}"
        "QMenuBar {"
        "    background-color: rgb(24, 26, 32);"
        "    color: white;"
        "}"
        "QMenuBar::item:selected {"
        "    background-color: #343844; /* 一个稍亮的颜色用于悬停 */"
        "}"
        "QToolBar#mainToolBar {"
        "    background-color: rgb(24, 26, 32);"
        "    border: none;"
        "    min-height: 40px; /* 增加工具栏高度 */"
        "    max-height: 40px;"
        "}"
        "QStatusBar {"
        "    background-color: rgb(24, 26, 32);"
        "    color: white;"
        "}"
    );
    // 获取主屏幕的分辨率
    QScreen* screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry(); // 获取屏幕的几何信息
    int screenWidth = screenGeometry.width();  // 屏幕宽度
    int screenHeight = screenGeometry.height(); // 屏幕高度

    int windowWidth = screenWidth * 1;
    int windowHeight = screenHeight * 1;

    resize(windowWidth, windowHeight);

    // 将窗口居中
    int x = (screenWidth - windowWidth) / 2;
    int y = (screenHeight - windowHeight) / 2;
    move(x, y);
    SystemPara::CAMERAL_DIR = "../../../ini/globe/cameral_Braide.json";
    loadjson_layer(SystemPara::GLOBAL_DIR);
    loadjson_layer2(SystemPara::CAMERAL_DIR);
    if (GlobalPara::envirment == GlobalPara::IPCEn && GlobalPara::ioType == GlobalPara::VC3000H) initPCI_VC3000H();
    init_log();
    init_cap();
    initcams(caminfo.size());
    init_algo_Braider();
    setupImageSaverThread();
    CreateMenu();
    CreateImageGrid_Braider(caminfo.size());
    setupMonitorThread();
    setupUpdateTimer();
    initSqlite3Db_Brader();
    if (GlobalPara::envirment == GlobalPara::IPCEn) this->onStartAllCamerasClicked();
    initCameralPara();

}


void MainWindow::init_algo_Flower()
{
    ExportFlowerSpace::InitializePosFlowerPin();
    ExportFlowerSpace::InitializeNegFlowerPin();
    ExportFlowerSpace::InitializeLookFlowerPin();
}

void MainWindow::initCameralPara()
{
    for (int i=0;i<cams.size();i++)
    {
        cams[i]->CC->init();
    }
}

int MainWindow::initPCI_VC3000()
{

        int result = 0;
        AppConfig::runningOutput = GlobalPara::RunPoint;
        result = VC3000DLL::pci().InitialSystem();
        if (result != 0)
        {
            std::cout << "[ERROR] 系统初始化失败，错误码：" << result << std::endl;

        }

        result = VC3000DLL::pci().setPNP(2);
        if (result != 0)
        {
            std::cout << "[ERROR] setPNP失败，错误码：" << result << std::endl;

        }

        result = VC3000DLL::pci().openseral("Com12");
        if (result != 0)
        {
            std::cout << "[ERROR] openseral失败，错误码：" << result << std::endl;

        }

        result = VC3000DLL::pci().setoutput(AppConfig::camera1Output, false);
        if (result != 0)
        {
            std::cout << "[ERROR] setoutput失败，端口: 错误码：" << AppConfig::camera1Output << result << std::endl;

        }

        // 设置 camera2Output
        result = VC3000DLL::pci().setoutput(AppConfig::camera2Output, false);
        if (result != 0)
        {
            std::cout << "[ERROR] setoutput失败，端口: " << AppConfig::camera2Output << " 错误码：" << result << std::endl;

        }

        // 设置 camera3Output
        result = VC3000DLL::pci().setoutput(AppConfig::camera3Output, false);
        if (result != 0)
        {
            std::cout << "[ERROR] setoutput失败，端口: " << AppConfig::camera3Output << " 错误码：" << result << std::endl;

        }

        // 设置 camera4Output
        result = VC3000DLL::pci().setoutput(AppConfig::camera4Output, false);
        if (result != 0)
        {
            std::cout << "[ERROR] setoutput失败，端口: " << AppConfig::camera4Output << " 错误码：" << result << std::endl;

        }

        // 设置 camera5Output
        result = VC3000DLL::pci().setoutput(AppConfig::camera5Output, false);
        if (result != 0)
        {
            std::cout << "[ERROR] setoutput失败，端口: " << AppConfig::camera5Output << " 错误码：" << result << std::endl;

        }

        // 设置 camera6Output
        result = VC3000DLL::pci().setoutput(AppConfig::camera6Output, false);
        if (result != 0)
        {
            std::cout << "[ERROR] setoutput失败，端口: " << AppConfig::camera6Output << " 错误码：" << result << std::endl;

        }

        // 设置 camera7Output
        result = VC3000DLL::pci().setoutput(AppConfig::camera7Output, false);
        if (result != 0)
        {
            std::cout << "[ERROR] setoutput失败，端口: " << AppConfig::camera7Output << " 错误码：" << result << std::endl;

        }

        // 设置 runningOutput
        result = VC3000DLL::pci().setoutput(AppConfig::runningOutput, false);
        if (result != 0)
        {
            std::cout << "[ERROR] setoutput失败，端口: " << AppConfig::runningOutput << " 错误码：" << result << std::endl;

        }

        // 设置 ErrorOutput
        result = VC3000DLL::pci().setoutput(AppConfig::ErrorOutput, false);
        if (result != 0)
        {
            std::cout << "[ERROR] setoutput失败，端口: " << AppConfig::ErrorOutput << " 错误码：" << result << std::endl;

        }

        for (int channel = 1; channel <= 4; ++channel) {
            // 所有通道亮度固定为 100
            if (VC3000DLL::pci().setlight(channel, 100, 100, 0, true) != 0) {
                std::cout << "[ERROR] " << channel << "通道光源设置失败，请检查光源线路或光源模块。" << std::endl;
            }
            else {
                std::cout << "[INFO] " << channel << "通道光源设置成功。" << std::endl;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(50));  // 保持 50ms 延时
        }

        return 0;

}

void MainWindow::setupImageSaverThread()
{
    GlobalLog::logger.Mz_AddLog(L"MainWindow: Setting up ImageSaverWorker in a new QThread.");

      m_workerThread = new QThread();

    m_workerThread->setObjectName("saveimage");
    //    将 MainWindow 的 m_saveQueue 引用传递给 ImageSaverWorker 的构造函数。
    m_imageSaverWorker = new ImageSaverWorker(m_saveQueue, nullptr);


    m_imageSaverWorker->moveToThread(m_workerThread);

#ifdef QIMAGE
    QObject::connect(m_workerThread, &QThread::started, m_imageSaverWorker, &ImageSaverWorker::saveLoop_QImage);
#else 
    QObject::connect(m_workerThread, &QThread::started, m_imageSaverWorker, &ImageSaverWorker::saveLoop);
#endif
    //概率二次释放
    //     当 m_workerThread 完成时，安全地清理 m_imageSaverWorker 对象。
    QObject::connect(m_workerThread, &QThread::finished, m_imageSaverWorker, &QObject::deleteLater);

    //     当 m_workerThread 完成时，安全地清理 m_workerThread 对象本身。
    QObject::connect(m_workerThread, &QThread::finished, m_workerThread, &QObject::deleteLater);


    m_workerThread->start();

    GlobalLog::logger.Mz_AddLog(L"MainWindow: ImageSaverWorker thread started successfully.");
}


void MainWindow::CreateImageGrid_Braider(int camnumber)
{
    QWidget* centralWidget = ui->centralWidget;
    if (!centralWidget) {
        centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);
    }
    centralWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QHBoxLayout* mainHorizontalLayout = new QHBoxLayout(centralWidget);
    mainHorizontalLayout->setContentsMargins(1, 1, 1, 1);
    mainHorizontalLayout->setSpacing(1);

    QWidget* leftPanelWidget = new QWidget(centralWidget);
    QVBoxLayout* leftPanelVLayout = new QVBoxLayout(leftPanelWidget);
    leftPanelVLayout->setContentsMargins(1, 1, 1, 1);
    leftPanelVLayout->setSpacing(1);

    QWidget* cameraGridContainer = new QWidget(leftPanelWidget);
    QGridLayout* cameraGridLayout = new QGridLayout(cameraGridContainer);
    cameraGridLayout->setContentsMargins(1, 1, 1, 1);
    cameraGridLayout->setSpacing(10);

    // 清空旧的相机标签，确保每次调用时重新创建
    for (CameraLabelWidget* label : cameraLabels) {
        if (label) {
            label->deleteLater();
        }
    }
    cameraLabels.clear();

    int maxDisplayCams = 3; // 左侧固定显示3个相机

    // 清理旧的 DisplayInfoWidget
    for (DisplayInfoWidget* widget : m_displayInfoWidgets) {
        if (widget) {
            widget->deleteLater();
        }
    }
    m_displayInfoWidgets.clear();

    


    // display info 容器和布局
    QWidget* displayInfoContainer = new QWidget(leftPanelWidget);
    QHBoxLayout* displayInfoHLayout = new QHBoxLayout(displayInfoContainer);
    displayInfoHLayout->setContentsMargins(5, 5, 5, 5);
    displayInfoHLayout->setSpacing(5);

    // 统一循环创建相机标签和绑定的参数展示组件
    for (int idx = 0; idx < maxDisplayCams && idx < cams.size(); ++idx) {
        // 创建 CameraLabelWidget
        CameraLabelWidget* cameraLabel = new CameraLabelWidget(
            cams[idx],
            idx + 1,
            caminfo[idx].name,
            &m_saveQueue,
            cameraGridContainer
        );
        cameraLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        cameraLabels.append(cameraLabel);
        cameraGridLayout->addWidget(cameraLabel, 0, idx);

#ifdef USE_MAIN_WINDOW_BRADER
        // 创建并绑定对应的 DisplayInfoWidget
        DisplayInfoWidget* infoWidget = new DisplayInfoWidget(&cams[idx]->RC->m_parameters, displayInfoContainer);
        displayInfoHLayout->addWidget(infoWidget);
        m_displayInfoWidgets.append(infoWidget);
        connect(cameraLabel->m_imageProcessor, &ImageProcess::PaintSend, infoWidget, &DisplayInfoWidget::onPaintSend);
        connect(cams[idx]->RI, &RezultInfo::UpdateParameters, infoWidget, &DisplayInfoWidget::onUpdateParameters);
        //设置拉伸因子保持均匀分布
        displayInfoHLayout->setStretchFactor(infoWidget, 1);
#else
        DisplayInfoWidget* infoWidget = nullptr;
     //   DisplayInfoWidget* infoWidget = new DisplayInfoWidget(cams[idx]->RI->unifyParams, displayInfoContainer);
      if(cams[idx]->indentify=="FlowerLook") infoWidget = new DisplayInfoWidget(cams[idx]->RI->unifyParams, displayInfoContainer);
      else  infoWidget = new DisplayInfoWidget_Flower(cams[idx]->RI->unifyParams, 4, displayInfoContainer);
        displayInfoHLayout->addWidget(infoWidget);
        m_displayInfoWidgets.append(infoWidget);
        connect(cameraLabel->m_imageProcessor, &ImageProcess::UpdateRealtimeData, infoWidget, &DisplayInfoWidget::onUpdateRealtimeData);
        connect(cams[idx]->RI, &RezultInfo::BuildUIFromUnifyParameters, infoWidget, &DisplayInfoWidget::onBuildUIFromUnifyParameters);
       


        //设置拉伸因子保持均匀分布
        displayInfoHLayout->setStretchFactor(infoWidget, 1);
#endif
        connect(cameraLabel->m_imageProcessor, &ImageProcess::StopDevice, this, &MainWindow::onStopAllCamerasClicked);

    }

    // 保证 3 列布局美观
    for (int col = 0; col < 3; ++col) {
        cameraGridLayout->setColumnStretch(col, 1);
    }
    cameraGridLayout->setRowStretch(0, 1);

    leftPanelVLayout->addWidget(cameraGridContainer);
    leftPanelVLayout->addWidget(displayInfoContainer);

    // 设置拉伸比例
    leftPanelVLayout->setStretchFactor(cameraGridContainer, 1);
    leftPanelVLayout->setStretchFactor(displayInfoContainer, 1);

    // 最后加入主布局
    mainHorizontalLayout->addWidget(leftPanelWidget);

    // 实例化 RightControlPanel
    m_rightControlPanel = new RightControlPanel(this->cams,this->caminfo,centralWidget);

    connect(m_rightControlPanel, &RightControlPanel::photoAllClicked, this, &MainWindow::onPhotoAllCamerasClicked);

    connect(m_rightControlPanel, &RightControlPanel::startAllClicked, this, &MainWindow::onStartAllCamerasClicked);

    connect(m_rightControlPanel, &RightControlPanel::stopAllClicked, this, &MainWindow::onStopAllCamerasClicked);

    connect(m_rightControlPanel, &RightControlPanel::clearAllClicked, this, &MainWindow::onClearAllCameraClicked);

    connect(m_rightControlPanel, &RightControlPanel::parameterButtonClicked, this, &MainWindow::onParamClicked);
   
    connect(m_rightControlPanel, &RightControlPanel::AllSheild, this, &MainWindow::onAllSheild);
    mainHorizontalLayout->addWidget(m_rightControlPanel);

    mainHorizontalLayout->setStretchFactor(leftPanelWidget, 3);
    mainHorizontalLayout->setStretchFactor(m_rightControlPanel, 1);
}

void MainWindow::onAllSheild(bool check)
{
    for (auto cam : cams)
    {
        cam->DI.Shield = check;
    }
}

void MainWindow::init_cap()
{

    if (GlobalPara::cameraType == GlobalPara::HIKVISION)
    {
        if (Mz_CameraConn::MsvInitLib() != 0)
        {
            GlobalLog::logger.Mz_AddLog(L"cameral init fail");
        }
    }

     MsvDeviceId camera;
  //   camera.m_ip = "192.168.1.100";
     camera.m_type = static_cast<DeviceType>(GlobalPara::cameraType);
  //   camera.m_id = "BE26765AAK00052";
  //   camera.configFile = "";

     int ret;
     std::vector<MsvDeviceId> deviceList;
     deviceList.push_back(camera);
     ret=Mz_CameraConn::MsvEnumerateDevics(deviceList);
     //std::cout << "MsvEnumerateDevics  ret=" << ret << std::endl;
     std::cout << "device num: " << deviceList.size() << std::endl;

     std::cout << "deviceList[0].m_id  " << deviceList[0].m_id << std::endl;
     std::cout << "deviceList[0].m_ip  " << deviceList[0].m_ip << std::endl;
     std::cout << "deviceList[0].configFile  " << deviceList[0].configFile << std::endl;


}


void MainWindow::init_log()
{
    GlobalLog::logger.Mz_GetInstance(L"app.log");

    // 写入日志
    LOG_DEBUG(GlobalLog::logger, L"Application started.");
      GlobalLog::logger.Mz_AddLog(L"hello");

    GlobalLog::logger.Mz_CheckDateTime();

    // 释放资源（删除所有日志文件！）
    //    logger.Mz_Realse();

}


void MainWindow::initcams(int camnumber)
{
     QString rootpath = SystemPara::ROOT_DIR;
   for(int i=1;i<=camnumber;i++)
    {
         Cameral * cam=new Cameral(this);
       // cam->algopath = rootpath +"" QString::number(i) + "/algo.json";
       // cam->cameralpath = rootpath + QString::number(i) + "/cameral.json";
       // cam->rangepath = rootpath + QString::number(i) + "/range.json";
         cam->rangepath=caminfo[i-1].path+ "/range.json";
         cam->cameralpath=caminfo[i-1].path+"/cameral.json";
         cam->algopath=caminfo[i-1].path+ "/algo.json";

         QString dateStr = QDate::currentDate().toString("yyyyMMdd");
        // cam->ok_path=SystemPara::ROOT_DIR+"/data/"+caminfo[i-1].mapping+"/OK/"+dateStr;
         cam->ok_path = SystemPara::DATA_DIR + "/" + caminfo[i - 1].mapping + "/OK/" + dateStr;
         qDebug()<<"  cam->ok_path:     "<<  cam->ok_path;

         if (!QDir(cam->ok_path).exists()) {
             QDir dir;
             if (dir.mkpath(cam->ok_path)) {
                 qDebug() << "路径创建成功:" << cam->ok_path;
             } else {
                 qDebug() << "路径创建失败:" << cam->ok_path;
             }
         } else {
             qDebug() << "路径已存在:" << cam->ok_path;
         }


        // cam->ng_path=SystemPara::ROOT_DIR+"/data/"+caminfo[i-1].mapping+"/NG/"+dateStr;
         cam->ng_path = SystemPara::DATA_DIR + "/" + caminfo[i - 1].mapping + "/NG/" + dateStr;

         qDebug()<<"  cam->ng_path:     "<<  cam->ng_path;

         if (!QDir(cam->ng_path).exists()) {
             QDir dir;
             if (dir.mkpath(cam->ng_path)) {
                 qDebug() << "路径创建成功:" << cam->ng_path;
             } else {
                 qDebug() << "路径创建失败:" << cam->ng_path;
             }
         } else {
             qDebug() << "路径已存在:" << cam->ng_path;
         }

         //cam->localpath=SystemPara::ROOT_DIR+"/data/"+caminfo[i-1].mapping+"/LOCAL/"+dateStr;
         cam->localpath = SystemPara::DATA_DIR + "/" + caminfo[i - 1].mapping + "/LOCAL/" + dateStr;
         qDebug()<<"  cam->localpath:     "<<  cam->localpath;

         if (!QDir(cam->localpath).exists()) {
             QDir dir;
             if (dir.mkpath(cam->localpath)) {
                 qDebug() << "路径创建成功:" << cam->localpath;
             } else {
                 qDebug() << "路径创建失败:" << cam->localpath;
             }
         } else {
             qDebug() << "路径已存在:" << cam->localpath;
         }

         cam->DI.saveflag=caminfo[i-1].saveflag;
         cam->DI.scaleFactor=caminfo[i-1].scaleFactor;
         cam->DI.PicDir = cam->localpath;
         cam->DI.Angle = caminfo[i - 1].Angle;
         cam->DI.fix = caminfo[i - 1].fix;
         cam->DI.EmptyIsOK = caminfo[i - 1].EmptyIsOK;
         cam->DI.Shield = caminfo[i - 1].Shield;
       cam->RC=new RangeClass(cam->rangepath,nullptr);
 
    
       
       
       qWarning()<<"caminfo[i-1].mapping: "<<caminfo[i-1].mapping;

       if(caminfo[i-1].mapping=="NaYin")
       {
#ifdef ADAPTATEION
           cam->RI = new RezultInfo_NaYin(&cam->RC->m_parameters, nullptr);
           cam->AC = new AlgoClass_NaYin(cam->algopath, 0, &cam->DI.Angle, nullptr);

           cam->indentify = caminfo[i - 1].mapping.toStdString();
           float temp = caminfo[i - 1].Angle;


           LearnPara::inParam2.angleNum = caminfo[i - 1].Angle;
           qDebug() << LearnPara::inParam2.angleNum;

           cam->unifyParams = RangeClass::loadUnifiedParameters(cam->rangepath);
           cam->RI = new RezultInfo_NaYin(cam->unifyParams, nullptr);
#else

           cam->RI=new RezultInfo_NaYin(&cam->RC->m_parameters,nullptr);
           cam->AC = new AlgoClass_NaYin(cam->algopath,0,&cam->DI.Angle,nullptr);

           cam->indentify=caminfo[i-1].mapping.toStdString();
           float temp = caminfo[i - 1].Angle;
        

           LearnPara::inParam2.angleNum = caminfo[i - 1].Angle;
           qDebug()<< LearnPara::inParam2.angleNum;
#endif
       }
       else if(caminfo[i-1].mapping=="Plate")
       {
           cam->RI=new RezultInfo_Plate(&cam->RC->m_parameters,nullptr);
           cam->AC = new AlgoClass_Plate(cam->algopath, 0, &cam->DI.Angle, nullptr);
           cam->indentify=caminfo[i-1].mapping.toStdString();
#ifdef ADAPTATEION
           cam->unifyParams = RangeClass::loadUnifiedParameters(cam->rangepath);
           cam->RI = new RezultInfo_Plate(cam->unifyParams, nullptr);
#else
           cam->ScalePath = caminfo[i - 1].path + "/scale-plater.json";
           cam->ScaleArray = cam->RI->initScale(cam->ScalePath);
           cam->RI->updatePaintDataFromScaleArray(cam->ScaleArray);
#endif
       }
       else if(caminfo[i-1].mapping=="Lift")
       {
#ifdef ADAPTATEION
           cam->RI = new RezultInfo_Lift(&cam->RC->m_parameters, nullptr);
           cam->AC = new AlgoClass_Lift(cam->algopath, 0, &cam->DI.Angle, nullptr);
           cam->indentify = caminfo[i - 1].mapping.toStdString();
		   cam->unifyParams = RangeClass::loadUnifiedParameters(cam->rangepath);
		   cam->RI = new RezultInfo_Lift(cam->unifyParams, nullptr);
#else
           cam->RI=new RezultInfo_Lift(&cam->RC->m_parameters,nullptr);
           cam->AC = new AlgoClass_Lift(cam->algopath, 0, &cam->DI.Angle, nullptr);
            cam->indentify=caminfo[i-1].mapping.toStdString();
#endif
       }
       else if(caminfo[i-1].mapping=="YYGJ")
       {
           cam->RI=new RezultInfo(&cam->RC->m_parameters,nullptr);
           cam->AC = new AlgoClass_Lift(cam->algopath, 0, &cam->DI.Angle, nullptr);
            cam->indentify=caminfo[i-1].mapping.toStdString();
			//cam->unifyParams = RangeClass::loadUnifiedParameters(cam->rangepath);
       }
       else if (caminfo[i - 1].mapping == "Abut")
       {
           cam->AC = new AlgoClass_Abut(cam->algopath, 0, &cam->DI.Angle, nullptr);
           cam->indentify = caminfo[i - 1].mapping.toStdString();
		   LearnPara::inParam6.angleNum = caminfo[i - 1].Angle; 
           cam->unifyParams = RangeClass::loadUnifiedParameters(cam->rangepath);
           cam->RI = new RezultInfo_Abut(cam->unifyParams , nullptr);
       }
       else if(caminfo[i-1].mapping=="Carrier_NaYin")
       {
#ifdef ADAPTATEION
           cam->RI = new RezultInfo_NaYin(&cam->RC->m_parameters, nullptr);
           cam->AC = new AlgoClass_NaYin(cam->algopath, 1, &cam->DI.Angle, nullptr);
           cam->indentify = caminfo[i - 1].mapping.toStdString();
           LearnPara::inParam1.angleNum = caminfo[i - 1].Angle;
           qDebug() << "After: " << LearnPara::inParam1.angleNum;

           cam->unifyParams = RangeClass::loadUnifiedParameters(cam->rangepath);
           cam->RI = new RezultInfo_NaYin(cam->unifyParams, nullptr);
#else
            cam->RI=new RezultInfo_NaYin(&cam->RC->m_parameters,nullptr);
            cam->AC = new AlgoClass_NaYin(cam->algopath, 1, &cam->DI.Angle, nullptr);
            cam->indentify=caminfo[i-1].mapping.toStdString();
            LearnPara::inParam1.angleNum = caminfo[i - 1].Angle;
            qDebug() << "After: " << LearnPara::inParam1.angleNum;
#endif
       }
       else if(caminfo[i-1].mapping=="Carrier_Plate")
       {
           cam->RI = new RezultInfo_Plate(&cam->RC->m_parameters, nullptr);
           cam->AC = new AlgoClass_Plate(cam->algopath, 0, &cam->DI.Angle, nullptr);
           cam->indentify = caminfo[i - 1].mapping.toStdString();
#ifdef ADAPTATEION
           cam->unifyParams = RangeClass::loadUnifiedParameters(cam->rangepath);
           cam->RI = new RezultInfo_Plate(cam->unifyParams, nullptr);
#else
           cam->ScalePath = caminfo[i - 1].path + "/scale-plater.json";
           cam->ScaleArray = cam->RI->initScale(cam->ScalePath);
           cam->RI->updatePaintDataFromScaleArray(cam->ScaleArray);
#endif
       }
       else if(caminfo[i-1].mapping=="Rift")
       {
#ifdef ADAPTATEION
           cam->RI = new RezultInfo_Lift(&cam->RC->m_parameters, nullptr);
           cam->AC = new AlgoClass_Lift(cam->algopath, 0, &cam->DI.Angle, nullptr);
           cam->indentify = caminfo[i - 1].mapping.toStdString();
           cam->unifyParams = RangeClass::loadUnifiedParameters(cam->rangepath);
           cam->RI = new RezultInfo_Lift(cam->unifyParams, nullptr);
#else
           cam->RI = new RezultInfo_Lift(&cam->RC->m_parameters, nullptr);
           cam->AC = new AlgoClass_Lift(cam->algopath, 0, &cam->DI.Angle, nullptr);
           cam->indentify = caminfo[i - 1].mapping.toStdString();
#endif
       }
       else  if (caminfo[i - 1].mapping == "Top")
       {
           cam->RI = new RezultInfo_Top(&cam->RC->m_parameters, nullptr);
           cam->AC = new AlgoClass_Top(cam->algopath, 1, &cam->DI.Angle, nullptr);
           LearnPara::inParam3.angleNum = caminfo[i - 1].Angle;
           cam->indentify = caminfo[i - 1].mapping.toStdString();
           cam->ScalePath = caminfo[i - 1].path + "/scale-top.json";
           cam->ScaleArray = cam->RI->initScale(cam->ScalePath);
           cam->RI->updatePaintDataFromScaleArray(cam->ScaleArray);
       }
       else  if (caminfo[i - 1].mapping == "Side")
       {
           cam->RI = new RezultInfo_Side(&cam->RC->m_parameters, nullptr);
           cam->AC = new AlgoClass_Side(cam->algopath, 0, &cam->DI.Angle, nullptr);
           LearnPara::inParam4.imgAngleNum = caminfo[i - 1].Angle;
           cam->indentify = caminfo[i - 1].mapping.toStdString();
           cam->ScalePath = caminfo[i - 1].path + "/scale-side.json";
           cam->ScaleArray=cam->RI->initScale(cam->ScalePath);
           cam->RI->updatePaintDataFromScaleArray(cam->ScaleArray);
       }
       else  if (caminfo[i - 1].mapping == "Pin")
       {
           cam->RI = new RezultInfo_Pin(&cam->RC->m_parameters, nullptr);
           cam->AC = new AlgoClass_Pin(cam->algopath, 1, &cam->DI.Angle, nullptr);
           LearnPara::inParam5.angleNum = caminfo[i - 1].Angle;
           cam->indentify = caminfo[i - 1].mapping.toStdString();
           cam->ScalePath = caminfo[i - 1].path + "/scale-pin.json";
           cam->ScaleArray = cam->RI->initScale(cam->ScalePath);
           cam->RI->updatePaintDataFromScaleArray(cam->ScaleArray);
       }
       else  if (caminfo[i - 1].mapping == "null")
       {
           cam->RI = new RezultInfo_Side(&cam->RC->m_parameters, nullptr);
           cam->AC = new AlgoClass_Side(cam->algopath, 0, &cam->DI.Angle, nullptr);
          // LearnPara::inParam4.imgAngleNum = caminfo[i - 1].Angle;
           cam->indentify = caminfo[i - 1].mapping.toStdString();
           cam->ScalePath = caminfo[i - 1].path + "/scale-side.json";
           cam->ScaleArray = cam->RI->initScale(cam->ScalePath);
           cam->RI->updatePaintDataFromScaleArray(cam->ScaleArray);
       }
       else if (caminfo[i - 1].mapping == "FlowerPin")
       {
           cam->AC = new AlgoClass_Plate(cam->algopath, 0, &cam->DI.Angle, nullptr);
           cam->indentify = caminfo[i - 1].mapping.toStdString();
		   LearnPara::inParam7.imgAngleNum = caminfo[i - 1].Angle;
           cam->unifyParams = RangeClass::loadUnifiedParameters(cam->rangepath);
           cam->RI = new RezultInfo_FlowerPin(cam->unifyParams, nullptr);
         }
       else if (caminfo[i - 1].mapping == "FlowerPinNeg")
       {
           cam->AC = new AlgoClass_Plate(cam->algopath, 0, &cam->DI.Angle, nullptr);
           cam->indentify = caminfo[i - 1].mapping.toStdString();
           LearnPara::inParam8.imgAngleNum = caminfo[i - 1].Angle;
           cam->unifyParams = RangeClass::loadUnifiedParameters(cam->rangepath);
           cam->RI = new RezultInfo_FlowerPin(cam->unifyParams, nullptr);
           }
       else if (caminfo[i - 1].mapping == "FlowerLook")
       {
           cam->AC = new AlgoClass_Plate(cam->algopath, 0, &cam->DI.Angle, nullptr);
           cam->indentify = caminfo[i - 1].mapping.toStdString();
           LearnPara::inParam9.imgAngleNum = caminfo[i - 1].Angle;
           cam->unifyParams = RangeClass::loadUnifiedParameters(cam->rangepath);
           cam->RI = new RezultInfo_Look(cam->unifyParams, nullptr);
           }
       else if (caminfo[i - 1].mapping == "Xs")
       {
           cam->AC = new AlgoClass_Plate(cam->algopath, 0, &cam->DI.Angle, nullptr);
           cam->indentify = caminfo[i - 1].mapping.toStdString();
           cam->unifyParams = RangeClass::loadUnifiedParameters(cam->rangepath);
           cam->RI = new RezultInfo_Xs(cam->unifyParams, nullptr);
           }
       else if (caminfo[i - 1].mapping == "Ny")
       {
           cam->AC = new AlgoClass_Plate(cam->algopath, 0, &cam->DI.Angle, nullptr);
           cam->indentify = caminfo[i - 1].mapping.toStdString();
           cam->unifyParams = RangeClass::loadUnifiedParameters(cam->rangepath);
           cam->RI = new RezultInfo_Ny(cam->unifyParams, nullptr);
           }
       else if (caminfo[i - 1].mapping == "Bottom1")
       {
           cam->AC = new AlgoClass_Plate(cam->algopath, 0, &cam->DI.Angle, nullptr);
           cam->indentify = caminfo[i - 1].mapping.toStdString();
           cam->unifyParams = RangeClass::loadUnifiedParameters(cam->rangepath);
           cam->RI = new RezultInfo_Bottom(cam->unifyParams, nullptr);
           }
       else if (caminfo[i - 1].mapping == "Bottom2")
       {
           cam->AC = new AlgoClass_Plate(cam->algopath, 0, &cam->DI.Angle, nullptr);
           cam->indentify = caminfo[i - 1].mapping.toStdString();
           cam->unifyParams = RangeClass::loadUnifiedParameters(cam->rangepath);
           cam->RI = new RezultInfo_Bottom(cam->unifyParams, nullptr);
           }
       cam->cameral_name=caminfo[i-1].name;
       cam->rounte=caminfo[i-1].rounte;
       cam->pointNumber.store(caminfo[i-1].pointNumber);


       MsvDeviceId camera;
       camera.m_ip = caminfo[i-1].ip.toStdString();
    //   camera.m_ip="192.168.1.100";
       camera.m_type = static_cast<DeviceType>(GlobalPara::cameraType);
       qDebug() << "camera.m_type (integer value):" << static_cast<int>(camera.m_type);
   //    camera.m_id = "BE26765AAK00052";
   //    camera.configFile = "";
       cam->camOp = new Mz_CameraConn::COperation(camera);
       cam->CC = new CameralClass(cam->cameralpath, cam->camOp);
  //     cam->CC->init();
     //  renameJpgFiles(cam->localpath);
          GlobalLog::logger.Mz_AddLog(L"hello this iscam");
        cams.push_back(cam);

   }
  // QString filepath="F:/Industry_Detection/data/Lift/LOCAL/20250701";
 //  renameJpgFiles(cam->local);

}

MainWindow::~MainWindow()
{

#ifdef USE_MAIN_WINDOW_CAPACITY
    updateDB_Plater();
#else
    updateDB_Brader();
    updateDeviceId();
#endif // USE_MAIN_WINDOW_CAPACITY

    int result = 0;
     qDebug() << "MainWindow()析构函数被调用";
    delete ui;
    delete menuExecute; // "执行"菜单
    delete menuRecord;  // "记录"菜单
    delete menuTools;   // "工具"菜单
    delete menuUser;    // "用户"菜单

    if (GlobalPara::cameraType == GlobalPara::HIKVISION)
    {
        if (Mz_CameraConn::MsvCloseLib() != 0)
        {
            GlobalLog::logger.Mz_AddLog(L"cameral close fail");
        }
    }

    if (GlobalPara::ioType == GlobalPara::VC3000H)
    {
        for (int channel = 1; channel <= 4; ++channel) {
            // 所有通道亮度固定为 100
            if (PCI::pci().setlight(channel, 0, 100, 1, false, channel) != 0) {
                std::cerr << "[ERROR] " << channel << "通道光源设置失败，请检查光源线路或光源模块。" << std::endl;
            }
            else {
                std::cout << "[INFO] " << channel << "通道光源设置成功。" << std::endl;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(20));         }

        // 依次设置输出模式
        int result;

        if ((result = PCI::pci().setOutputMode(AppConfig::camera1Output, false, 1000)) != 0) {
            QString errorMsg = QString("设置 camera1Output 模式失败，错误码：%1").arg(result);
            LOG_DEBUG(GlobalLog::logger, errorMsg.toStdWString().c_str());
        }
        else {
            QString successMsg = QString("设置 camera1Output 模式成功，返回值：%1").arg(result);
            LOG_DEBUG(GlobalLog::logger, successMsg.toStdWString().c_str());
        }

        if ((result = PCI::pci().setOutputMode(AppConfig::camera2Output, false, 1000)) != 0) {
            QString errorMsg = QString("设置 camera2Output 模式失败，错误码：%1").arg(result);
            LOG_DEBUG(GlobalLog::logger, errorMsg.toStdWString().c_str());
        }
        else {
            QString successMsg = QString("设置 camera2Output 模式成功，返回值：%1").arg(result);
            LOG_DEBUG(GlobalLog::logger, successMsg.toStdWString().c_str());
        }

        if ((result = PCI::pci().setOutputMode(AppConfig::camera3Output, false, 1000)) != 0) {
            QString errorMsg = QString("设置 camera3Output 模式失败，错误码：%1").arg(result);
            LOG_DEBUG(GlobalLog::logger, errorMsg.toStdWString().c_str());
        }
        else {
            QString successMsg = QString("设置 camera3Output 模式成功，返回值：%1").arg(result);
            LOG_DEBUG(GlobalLog::logger, successMsg.toStdWString().c_str());
        }

        if ((result = PCI::pci().setOutputMode(AppConfig::camera4Output, false, 300)) != 0) {
            QString errorMsg = QString("设置 camera4Output 模式失败，错误码：%1").arg(result);
            LOG_DEBUG(GlobalLog::logger, errorMsg.toStdWString().c_str());
        }
        else {
            QString successMsg = QString("设置 camera4Output 模式成功，返回值：%1").arg(result);
            LOG_DEBUG(GlobalLog::logger, successMsg.toStdWString().c_str());
        }

        if ((result = PCI::pci().setOutputMode(AppConfig::camera5Output, false, 300)) != 0) {
            QString errorMsg = QString("设置 camera5Output 模式失败，错误码：%1").arg(result);
            LOG_DEBUG(GlobalLog::logger, errorMsg.toStdWString().c_str());
        }
        else {
            QString successMsg = QString("设置 camera5Output 模式成功，返回值：%1").arg(result);
            LOG_DEBUG(GlobalLog::logger, successMsg.toStdWString().c_str());
        }

        if ((result = PCI::pci().setOutputMode(AppConfig::camera6Output, false, 300)) != 0) {
            QString errorMsg = QString("设置 camera6Output 模式失败，错误码：%1").arg(result);
            LOG_DEBUG(GlobalLog::logger, errorMsg.toStdWString().c_str());
        }
        else {
            QString successMsg = QString("设置 camera6Output 模式成功，返回值：%1").arg(result);
            LOG_DEBUG(GlobalLog::logger, successMsg.toStdWString().c_str());
        }

        if ((result = PCI::pci().setOutputMode(AppConfig::camera7Output, false, 300)) != 0) {
            QString errorMsg = QString("设置 camera7Output 模式失败，错误码：%1").arg(result);
            LOG_DEBUG(GlobalLog::logger, errorMsg.toStdWString().c_str());
        }
        else {
            QString successMsg = QString("设置 camera7Output 模式成功，返回值：%1").arg(result);
            LOG_DEBUG(GlobalLog::logger, successMsg.toStdWString().c_str());
        }

        if ((result = PCI::pci().setOutputMode(AppConfig::runningOutput, false, 300)) != 0) {
            QString errorMsg = QString("设置 runningOutput 模式失败，错误码：%1").arg(result);
            LOG_DEBUG(GlobalLog::logger, errorMsg.toStdWString().c_str());
        }
        else {
            QString successMsg = QString("设置 runningOutput 模式成功，返回值：%1").arg(result);
            LOG_DEBUG(GlobalLog::logger, successMsg.toStdWString().c_str());
        }


        PCI::pci().SaveParam();

    }



    for (QWidget* widget : cameraLabels)
    {
        delete widget;
    }
    {
        std::unique_lock<std::mutex> lock(m_saveQueue.mutex);
        m_saveQueue.stopFlag.store(true);
    }
       m_saveQueue.cond.notify_all();

        if (m_workerThread && m_workerThread->isRunning())
        {
            qDebug() << "MainWindow: Requesting save worker thread quit and wait.";
            m_workerThread->quit(); // 请求线程事件循环退出
            m_workerThread->wait(5000); // 等待线程完成，设置超时时间，防止无限阻塞
            if (m_workerThread->isRunning()) {
                qDebug() << "MainWindow: Save worker thread did not stop gracefully, terminating.";
                m_workerThread->terminate(); // 如果超时仍未停止，强制终止
                m_workerThread->wait(); // 再次等待终止完成
            }
        }

        if (m_monitorThread && m_monitorThread->isRunning())
        {
            qDebug() << "Requesting monitor thread to quit...";
            m_monitorThread->quit(); // 1. 请求线程事件循环退出

            // 2. 等待线程完成，设置5秒超时
            if (!m_monitorThread->wait(5000))
            {
                qDebug() << "Monitor thread did not stop gracefully in 5s, terminating...";
                m_monitorThread->terminate(); // 3. 如果超时仍未停止，强制终止
                m_monitorThread->wait();      // 等待强制终止完成
            }
            else {
                qDebug() << "Monitor thread stopped gracefully.";
            }
        }

        if (m_imageSaverWorker) {
            // 先断开所有连接，防止在删除时触发未预期的信号或槽
            QObject::disconnect(m_workerThread, nullptr, m_imageSaverWorker, nullptr);
          //  delete m_imageSaverWorker;
         //   m_imageSaverWorker = nullptr;  自然释放，避免提前释放导致崩溃
        }

        //删除 QThread 对象本身
        if (m_workerThread) {
            delete m_workerThread;
            m_workerThread = nullptr;
        }


        for(auto cam:cams)
        {
            delete cam;
        }
        //const std::string myExecutableName = "Industry_Detection.exe";
        //terminateProcessByName(myExecutableName);
}

void MainWindow::setupMonitorThread()
{
    for (int i = 0; i < cams.size(); i++)
    {
        m_pathsToWatch.push_back(SystemPara::DATA_DIR);
    }
    // 创建线程对象，指定父对象以便随主窗口销毁
    m_monitorThread = new QThread(this);
    m_monitorThread->setObjectName("MonitorThread");

    // 创建 Worker 对象，不指定父对象以便移动到新线程
    m_monitorWorker = new DirectoryMonitorWorker(m_pathsToWatch, m_saveQueue, nullptr);

    // 将 Worker 的所有权和事件处理移交到新线程
    m_monitorWorker->moveToThread(m_monitorThread);

    // 当线程启动后，调用 Worker 的 startMonitoring 方法来初始化其内部的定时器
    connect(m_monitorThread, &QThread::started, m_monitorWorker, &DirectoryMonitorWorker::startMonitoring);

    // 启动线程，使其进入事件循环
    m_monitorThread->start();

    qDebug() << "MainWindow: DirectoryMonitorWorker thread has been started.";
}

void MainWindow::showAboutWidget()
{
    AboutWidget* about = new AboutWidget(this);
    about->setWindowTitle("公司介绍");
    about->setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
    about->setAttribute(Qt::WA_DeleteOnClose);  // 关闭时自动释放
    about->setWindowModality(Qt::ApplicationModal); // 模态窗口
    about->show();
}

void MainWindow::CreateMenu()
{
    // --- 1. 初始化工具栏 ---
    if (ui->mainToolBar) {
        ui->mainToolBar->setMovable(false);
        ui->mainToolBar->setFloatable(false);
        ui->mainToolBar->clear();
        ui->mainToolBar->setMinimumHeight(50);

        // --- 2. 创建一个总的、居中的容器 ---
        QWidget* centralContainer = new QWidget(ui->mainToolBar);
        QHBoxLayout* containerLayout = new QHBoxLayout(centralContainer);
        containerLayout->setContentsMargins(0, 0, 0, 0); // 容器两边的边距
        containerLayout->setSpacing(15);                   // 容器内元素默认间距

        // --- 3. 准备所有要显示的控件 ---
        QLabel* logoLabel = new QLabel(centralContainer);
        logoLabel->setPixmap(QPixmap(":/images/resources/images/oo.ico").scaled(48, 48, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        logoLabel->setFixedSize(48, 48);

        QLabel* versionTextLabel = new QLabel(SystemPara::VERSION, centralContainer);
        versionTextLabel->setStyleSheet("color: white; font-size: 16pt; font-weight: bold;");

        m_runtimeLabel = new QLabel("运行时间: 00:00:00", centralContainer);
        m_roleLabel = new QLabel(role.GetCurrentRole(), centralContainer);
        QString statusLabelStyle = "QLabel { color: #cccccc; font-size: 12pt; }";
        m_runtimeLabel->setStyleSheet(statusLabelStyle);
        m_roleLabel->setStyleSheet(statusLabelStyle);

        // --- 4. 在容器内部进行精细布局 ---
        containerLayout->addWidget(logoLabel);
        containerLayout->addWidget(versionTextLabel);

        // 在这里添加一个弹性间隔
        // 它会占据所有可用空间，从而把版本号和运行时间推开
        containerLayout->addStretch(1);

        containerLayout->addWidget(m_runtimeLabel);
        containerLayout->addWidget(m_roleLabel);
        centralContainer->setLayout(containerLayout);

        // --- 5. 将总容器在工具栏中居中 ---
        QWidget* leftSpacer = new QWidget(ui->mainToolBar);
        leftSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

        QWidget* rightSpacer = new QWidget(ui->mainToolBar);
        rightSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

        ui->mainToolBar->addWidget(leftSpacer);
        ui->mainToolBar->addWidget(centralContainer);
        ui->mainToolBar->addWidget(rightSpacer);
    }
    // --- 4. 创建菜单和Action ---
    QMenuBar* menuBar = this->menuBar();
    menuExecute = menuBar->addMenu("关于软件");
    menuRecord = menuBar->addMenu("记录");
    menuTools = menuBar->addMenu("工具");
    menuUser = menuBar->addMenu("用户");
    menuAbout = menuBar->addAction("公司介绍");

    QAction* menuUpdate = menuExecute->addAction("更新说明");
    QAction* ActionSelfStart = menuTools->addAction("开机自启动（开启中）");
   // QMenu * 
    QAction* actionLogout = menuUser->addAction("注销");
    QAction* loginAction = menuUser->addAction("登录");
    QAction* LogAction = menuRecord->addAction("日志");
    QAction* DataAction = menuRecord->addAction("数据");
    QAction* SystemParaAction = menuTools->addAction("存图路径");
    QAction* CameralSetAction = menuTools->addAction("相机配置");

    // --- 5. 连接所有信号和槽 ---
    QTimer* timer = new QTimer(this);
    QDateTime startTime = QDateTime::currentDateTime();
    connect(timer, &QTimer::timeout, this, [this, startTime]() {
        long long seconds = startTime.secsTo(QDateTime::currentDateTime());
        long long hours = seconds / 3600;
        long long minutes = (seconds % 3600) / 60;
        long long secs = seconds % 60;
        m_runtimeLabel->setText(QString("运行时间: %1:%2:%3")
            .arg(hours, 2, 10, QChar('0'))
            .arg(minutes, 2, 10, QChar('0'))
            .arg(secs, 2, 10, QChar('0')));
        });
    timer->start(1000);

    connect(menuAbout, &QAction::triggered, this, &MainWindow::showAboutWidget);

    connect(ActionSelfStart, &QAction::triggered, this, [=]() {
        this->toggleAutoStart(ActionSelfStart);
        });

    connect(actionLogout, &QAction::triggered, this, [this]() {
        Role::ChangeRole("操作员");
        m_roleLabel->setText(Role::GetCurrentRole());
        });

    connect(loginAction, &QAction::triggered, this, [this]() {
        Login* loginDialog = new Login(this);
        loginDialog->setAttribute(Qt::WA_DeleteOnClose);
        connect(loginDialog, &Login::loginSuccess, this, &Role::ChangeRole);
        connect(loginDialog, &QDialog::accepted, this, [this, loginDialog]() {
            if (!loginDialog->GetPassword().isEmpty()) {
                m_roleLabel->setText(role.GetCurrentRole());
                show();
                Role::StartAutoLogout(600000, this, [this]() {
                    m_roleLabel->setText("操作员");  // 自动更新界面
                    qDebug() << "UI 已更新：角色自动注销为操作员";
                    });
            }
            });
        loginDialog->show();
        });

    connect(menuUpdate, &QAction::triggered, this, [this]() {
        QString filePath = "../../../resources/file/update.txt";
        QFile file(filePath);

        if (file.exists() && file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            QMessageBox::information(this, "更新说明", in.readAll());
            file.close();
        }
        else {
            qDebug() << "文件不存在或无法打开，不执行任何操作：" << filePath;
        }
        });

    connect(LogAction, &QAction::triggered, this, [this]() {
        QString logFolderPath = SystemPara::LOG_DIR;
        if (!QDir(logFolderPath).exists()) return;
        QString selectedFile = QFileDialog::getOpenFileName(this, "选择日志文件", logFolderPath, "Text Files (*.txt)");
        if (!selectedFile.isEmpty()) {
            QDesktopServices::openUrl(QUrl::fromLocalFile(selectedFile));
        }
        });

    connect(DataAction, &QAction::triggered, this, [this]() {
        QString dataFolderPath = SystemPara::DATA_DIR;
        if (!QDir(dataFolderPath).exists()) return;
        QString selectedFile = QFileDialog::getOpenFileName(this, "选择数据文件", dataFolderPath, "Text Files (*.txt)");
        if (!selectedFile.isEmpty()) {
            QDesktopServices::openUrl(QUrl::fromLocalFile(selectedFile));
        }
        });

    connect(CameralSetAction, &QAction::triggered, this, [=]() {
        AddCameraDialog dlg(SystemPara::CAMERAL_DIR, this);
        dlg.exec();
        });

    connect(SystemParaAction, &QAction::triggered, this, [this]() {
        SysPara* syspara = new SysPara(this);
        syspara->show();
        });
}


void MainWindow::terminateProcessByName(const std::string& processName) {
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        return;
    }

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(hProcessSnap, &pe32)) {
        do {
            // 将std::string转换为std::wstring
            std::wstring wProcessName(processName.begin(), processName.end());

            // 使用_wcsicmp进行宽字节字符串比较
            if (_wcsicmp(pe32.szExeFile, wProcessName.c_str()) == 0 && pe32.th32ProcessID != GetCurrentProcessId()) {
                HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pe32.th32ProcessID);
                if (hProcess != NULL) {
                    TerminateProcess(hProcess, 0);
                    CloseHandle(hProcess);
                    qDebug() << "Successfully terminated a residual process:" << QString::fromStdString(processName);
                }
                else {
                    qDebug() << "Failed to open and terminate a residual process:" << QString::fromStdString(processName);
                }
            }
        } while (Process32Next(hProcessSnap, &pe32));
    }

    CloseHandle(hProcessSnap);
}

//void MainWindow::toggleAutoStart(QAction* action) {
// 
//
//    const QString appName = "Industry_Detection";
//    const wchar_t* regPath = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";
//
//    HKEY hKey;
//    // 打开或创建注册表键
//    LONG result = RegCreateKeyExW(HKEY_LOCAL_MACHINE, regPath, 0, nullptr,
//        REG_OPTION_NON_VOLATILE, KEY_WRITE | KEY_READ, nullptr, &hKey, nullptr);
//    if (result != ERROR_SUCCESS) {
//        QMessageBox::warning(nullptr, "错误", "无法访问注册表，可能需要管理员权限！");
//        return;
//    }
//
//    // 检查当前是否已设置
//    bool isEnabled = false;
//    result = RegQueryValueExW(hKey, reinterpret_cast<const wchar_t*>(appName.utf16()),
//        nullptr, nullptr, nullptr, nullptr);
//    isEnabled = (result == ERROR_SUCCESS);
//
//    if (isEnabled) {
//        // 删除启动项
//        result = RegDeleteValueW(hKey, reinterpret_cast<const wchar_t*>(appName.utf16()));
//    }
//    else {
//        // 设置启动项
//        QString appPath = QCoreApplication::applicationFilePath().replace("/", "\\");
//        result = RegSetValueExW(hKey,
//            reinterpret_cast<const wchar_t*>(appName.utf16()),
//            0,
//            REG_SZ,
//            reinterpret_cast<const BYTE*>(appPath.utf16()),
//            (appPath.length() + 1) * sizeof(wchar_t));
//    }
//
//    RegCloseKey(hKey);
//
//    if (result == ERROR_SUCCESS) {
//        bool nowEnabled = !isEnabled;
//        action->setText(nowEnabled ? "开机自启动（开启中）" : "开机自启动（关闭中）");
//        qDebug() << "已设置开机自启动为：" << nowEnabled;
//    }
//    else {
//        QMessageBox::warning(nullptr, "错误", "设置开机自启动失败！");
//    }
//}


void MainWindow::toggleAutoStart(QAction* action) {
    const QString appName = "Industry_Detection";

    // 使用 HKEY_CURRENT_USER 路径，确保在不同电脑上的当前用户环境下都有效
    const wchar_t* regPath = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";

    HKEY hKey;
    // 使用 RegCreateKeyExW 来打开或创建注册表键，确保路径存在
    LONG result = RegCreateKeyExW(HKEY_CURRENT_USER, regPath, 0, nullptr,
        REG_OPTION_NON_VOLATILE, KEY_WRITE | KEY_READ, nullptr, &hKey, nullptr);

    // 检查是否成功打开或创建了注册表键
    if (result != ERROR_SUCCESS) {
        // 如果失败，通常是因为权限不足（虽然 HKEY_CURRENT_USER 很少见）或系统问题
        QMessageBox::warning(nullptr, "错误", "无法访问注册表，设置开机自启动失败！");
        return;
    }

    // 使用更安全的类型转换
    LPCWSTR appNameW = reinterpret_cast<const wchar_t*>(appName.utf16());

    bool isEnabled = false;
    // 检查当前的启动项是否已存在
    result = RegQueryValueExW(hKey, appNameW, nullptr, nullptr, nullptr, nullptr);
    isEnabled = (result == ERROR_SUCCESS);

    if (isEnabled) {
        // 如果已存在，则删除启动项
        result = RegDeleteValueW(hKey, appNameW);
    }
    else {
        // 如果不存在，则设置启动项
        QString appPath = QCoreApplication::applicationFilePath().replace("/", "\\");
        LPCWSTR appPathW = reinterpret_cast<const wchar_t*>(appPath.utf16());

        result = RegSetValueExW(hKey,
            appNameW,
            0,
            REG_SZ,
            reinterpret_cast<const BYTE*>(appPathW),
            (appPath.length() + 1) * sizeof(wchar_t));
    }

    // 关闭注册表键
    RegCloseKey(hKey);

    if (result == ERROR_SUCCESS) {
        bool nowEnabled = !isEnabled;
        action->setText(nowEnabled ? "开机自启动（开启中）" : "开机自启动（关闭中）");
        qDebug() << "已设置开机自启动为：" << nowEnabled;
    }
    else {
        QMessageBox::warning(nullptr, "错误", "设置开机自启动失败！");
    }
}


//void MainWindow::SetupCameraGridLayout(int i, QGridLayout* gridLayout, QVector<CameraLabelWidget*>& cameraLabels, QWidget* window)
//{
//   // auto& Cams = Cameral::Cams;
//    // 输入验证：i 范围 [0, 7]，gridLayout 和 window 非空
//    if (!gridLayout || !window || i < 0 || i > 7)
//        return;
//
//    // 清空旧布局
//    QLayoutItem* item;
//    while ((item = gridLayout->takeAt(0)) != nullptr) {
//        if (item->widget())
//            delete item->widget();
//        delete item;
//    }
//    cameraLabels.clear();
//
//    // 固定 2 行 4 列布局
//    int cols = 4;
//    qDebug()<<"Cams.size():  "<<cams.size();
//    if (cams.size() < i) {
//        qWarning() << "Cameral::Cams 中数据不足，无法初始化 " << i << " 个相机框！";
//        return;
//    }
//
//    for (int idx = 0; idx < i && idx < cams.size(); ++idx) {
//        CameraLabelWidget* cameraLabel = new CameraLabelWidget(
//            cams[idx],       // 使用 idx 而非 i 访问 cams
//            idx + 1,         // 编号从1开始
//            caminfo[idx].name,
//            &m_saveQueue,
//            window
//            );
//        cameraLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
//        cameraLabels.append(cameraLabel);
//        gridLayout->addWidget(cameraLabel, idx / cols, idx % cols);
//    }
//    // 添加第八个固定框
//    QWidget* emptyWidget = CreateEighthFrame(i);
//    emptyWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
//    gridLayout->addWidget(emptyWidget, i / cols, i % cols);
//
//    // 根据 i 动态调整窗口大小
//    int frameWidth = 200;  // 每个框的基准宽度
//    int frameHeight = 50; // 每个框的基准高度
//    int spacing = 10;      // 网格间距
//    int margin = 20;       // 窗口边距
//
//    // 计算使用的行列数
//    int usedSlots = i + 1; // i 个相机框 + 第八个固定框
//    int rows = (usedSlots + 3) / 4; // 向上取整
//    int usedCols = (usedSlots <= 4) ? usedSlots : 4; // 第一行最多 4 列
//
//    // 计算窗口大小
//    int windowWidth = usedCols * frameWidth + (usedCols - 1) * spacing + 2 * margin;
//    int windowHeight = rows * frameHeight + (rows - 1) * spacing + 2 * margin;
//
//    // 调整窗口大小
//    window->resize(windowWidth, windowHeight);
//}

//void MainWindow::CreateImageGrid(int camnumber)
//{
//    // 获取并设置中央控件的可扩展策略
//    QWidget* centralWidget = ui->centralWidget;
//    centralWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
//
//    // 清空并删除旧布局及其中的所有控件
//    QLayout* oldLayout = centralWidget->layout();
//    if (oldLayout) {
//        QLayoutItem* item;
//        while ((item = oldLayout->takeAt(0)) != nullptr) {
//            delete item->widget();
//            delete item;
//        }
//        delete oldLayout;
//    }
//
//    cameraLabels.clear(); // 清空存储相机视图的列表
//
//    // 判断相机数量是奇数还是偶数，以决定布局方案
//    bool isEvenCameraCount = (camnumber % 2 == 0);
//
//    if (isEvenCameraCount) {
//        // 相机数量为偶数时，采用左右分区布局（左3/4，右1/4）
//        qDebug() << "相机数量为偶数，采用左右分区布局。";
//        QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);
//        mainLayout->setContentsMargins(1, 1, 1, 1);
//        mainLayout->setSpacing(1);
//
//        // 创建左侧的相机网格布局
//        QGridLayout* cameraGrid = new QGridLayout();
//        cameraGrid->setContentsMargins(0, 0, 0, 0);
//        cameraGrid->setSpacing(1);
//
//        // 创建右侧的数据统计垂直布局
//        QVBoxLayout* statsLayout = new QVBoxLayout();
//        statsLayout->setContentsMargins(0, 0, 0, 0);
//
//        // 调用函数配置左右布局中的具体控件
//        SetupDualLayout(camnumber, cameraGrid, statsLayout, cameraLabels, centralWidget);
//
//        // 设置左右区域的拉伸因子，实现 3:1 的宽度比例
//        mainLayout->addLayout(cameraGrid, 3);
//        mainLayout->addLayout(statsLayout, 1);
//        centralWidget->setLayout(mainLayout);
//
//    }
//    else {
//        // 相机数量为奇数时，采用全屏网格布局，所有控件平分空间
//        qDebug() << "相机数量为奇数，采用全屏网格布局。";
//        QGridLayout* gridLayout = new QGridLayout(centralWidget);
//        gridLayout->setContentsMargins(1, 1, 1, 1);
//        gridLayout->setSpacing(1);
//        SetupCameraGridLayout(camnumber, gridLayout, cameraLabels, centralWidget);
//    }
//}

/**
 * @brief 奇数相机布局：创建一个 4x2 的网格，填入相机和一个统计框
 * @param camnumber      相机数量
 * @param gridLayout     要填充的网格布局
 * @param cameraLabels   用于存储创建的 CameraLabelWidget
 * @param parentWindow   父窗口指针
 */
void MainWindow::CreateImageGrid(int camnumber)
{
    // 1. 创建一个全新的、干净的 QWidget 作为所有内容的容器
    QWidget* containerWidget = new QWidget();
    containerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    cameraLabels.clear(); // 清空存储相机视图的列表

    // -----------------------------------------------------------------
    // [特殊情况处理] 当相机数量正好为 2 时，采用自定义的“上/下”布局
    // -----------------------------------------------------------------
    if (camnumber == 2)
    {
        qDebug() << "相机数量为 2，采用特殊的“上/下”布局（相机水平并排，统计在下）。";

        // 主布局使用垂直布局 (QVBoxLayout)
        QVBoxLayout* mainLayout = new QVBoxLayout(containerWidget);
        mainLayout->setContentsMargins(1, 1, 1, 1);
        mainLayout->setSpacing(1);

        // 创建一个水平布局 (QHBoxLayout) 用于放置两个相机
        QHBoxLayout* cameraLayout = new QHBoxLayout();
        cameraLayout->setContentsMargins(0, 0, 0, 0);
        cameraLayout->setSpacing(1);

        // 检查相机数据是否足够
        if (cams.size() < 2) {
            qWarning() << "Cameral::Cams 中数据不足，无法初始化 2 个相机框！";
            setCentralWidget(containerWidget); // 设置空窗口
            return;
        }

        // 创建并添加 Camera 1
        CameraLabelWidget* cameraLabel1 = new CameraLabelWidget(cams[0], 1, caminfo[0].name, &m_saveQueue, this);
        cameraLabel1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        cameraLabels.append(cameraLabel1);
        cameraLayout->addWidget(cameraLabel1, 1); // 拉伸因子为 1

        // 创建并添加 Camera 2
        CameraLabelWidget* cameraLabel2 = new CameraLabelWidget(cams[1], 2, caminfo[1].name, &m_saveQueue, this);
        cameraLabel2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        cameraLabels.append(cameraLabel2);
        cameraLayout->addWidget(cameraLabel2, 1); // 拉伸因子为 1

        // 创建统计框
        QWidget* statsWidget = CreateEighthFrame(camnumber);
        // 设置统计框的策略：水平扩展，垂直方向固定或按需 (Preferred)
        statsWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

        // 将相机布局（上）和统计框（下）添加到主布局
        mainLayout->addLayout(cameraLayout, 5); // 上方相机区域，拉伸因子为 1 (占满剩余空间)
        mainLayout->addWidget(statsWidget, 2);  // 下方统计区域，拉伸因子为 0 (保持原有高度)
    }
    // -----------------------------------------------------------------
    // [常规情况处理] 相机数量不为 2 时，执行原有的奇偶判断逻辑
    // -----------------------------------------------------------------
    else
    {
        // 2. 判断相机数量是奇数还是偶数
        bool isEvenCameraCount = (camnumber % 2 == 0);

        if (isEvenCameraCount) {
            // [偶数 > 2] 采用左右分区布局 (左侧相机网格 + 右侧独立统计框)
            qDebug() << "相机数量为偶数 (" << camnumber << ")，采用左右分区布局。";

            QHBoxLayout* mainLayout = new QHBoxLayout(containerWidget);
            mainLayout->setContentsMargins(1, 1, 1, 1);
            mainLayout->setSpacing(1);

            QGridLayout* cameraGrid = new QGridLayout();
            cameraGrid->setContentsMargins(0, 0, 0, 0);
            cameraGrid->setSpacing(1);

            QVBoxLayout* statsLayout = new QVBoxLayout();
            statsLayout->setContentsMargins(0, 0, 0, 0);

            // 调用偶数布局的辅助函数来填充内容
            // (SetupDualLayout 无需修改)
            SetupDualLayout(camnumber, cameraGrid, statsLayout, cameraLabels, this);

            mainLayout->addLayout(cameraGrid, 5);
            mainLayout->addLayout(statsLayout, 1);
        }
        else {
            // [奇数] 采用统一网格布局 (相机和统计框共同参与排列)
            qDebug() << "相机数量为奇数 (" << camnumber << ")，采用统一网格布局。";

            QGridLayout* gridLayout = new QGridLayout(containerWidget);
            gridLayout->setContentsMargins(1, 1, 1, 1);
            gridLayout->setSpacing(1);

            // (SetupCameraGridLayout 无需修改)
            SetupCameraGridLayout(camnumber, gridLayout, cameraLabels, this);
        }
    } // --- 结束 else (处理非 2 的情况) ---

    // 3. 用我们新建的、布局完美的 containerWidget 替换掉旧的中央控件
    setCentralWidget(containerWidget);
}
/**
 * @brief 【偶数相机专用】设置左右双区域布局。
 * 左侧是一个N/2 x 2的相机网格，右侧是独立的统计框。
 */
void MainWindow::SetupDualLayout(int camnumber, QGridLayout* cameraGrid, QVBoxLayout* statsLayout, QVector<CameraLabelWidget*>& cameraLabels, QWidget* parentWindow)
{
    if (!cameraGrid || !statsLayout || !parentWindow) return;
    int cameraCols = camnumber / 2;
    if (cameraCols == 0) return;
    int cameraRows = (camnumber + cameraCols - 1) / cameraCols;

    for (int idx = 0; idx < camnumber && idx < cams.size(); ++idx) {
        CameraLabelWidget* cameraLabel = new CameraLabelWidget(cams[idx], idx + 1, caminfo[idx].name, &m_saveQueue, parentWindow);
        cameraLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        cameraLabels.append(cameraLabel);
        cameraGrid->addWidget(cameraLabel, idx / cameraCols, idx % cameraCols);
    }
    for (int row = 0; row < cameraRows; ++row) cameraGrid->setRowStretch(row, 1);
    for (int col = 0; col < cameraCols; ++col) cameraGrid->setColumnStretch(col, 1);

    // --- 关键修正 ---
    // 直接添加统计框，并移除 addStretch，让统计框自身来填充整个垂直空间
    QWidget* statsWidget = CreateEighthFrame(camnumber);
    statsLayout->addWidget(statsWidget);
}

/**
 * @brief 【奇数相机专用】设置统一网格布局。
 * 所有相机和统计框共同参与一个4列的网格排列。
 */
void MainWindow::SetupCameraGridLayout(int camnumber, QGridLayout* gridLayout, QVector<CameraLabelWidget*>& cameraLabels, QWidget* parentWindow)
{
    if (!gridLayout || !parentWindow) return;

    int totalWidgets = camnumber + 1;

    // --- 核心逻辑：智能计算列数，以获得最均衡的网格 ---
    // 特殊处理 8 个控件（7相机）的情况，优先形成 4x2 布局
    // 其他情况则通过开方计算最接近正方形的布局
    int cols;
    if (totalWidgets == 8) {
        cols = 4;
    }
    else {
        cols = static_cast<int>(std::ceil(std::sqrt(totalWidgets)));
    }
    qDebug() << "奇数相机模式，总控件数：" << totalWidgets << "，智能计算列数：" << cols;

    if (cams.size() < camnumber) {
        qWarning() << "Cameral::Cams 中数据不足，无法初始化 " << camnumber << " 个相机框！";
        return;
    }

    // 添加所有相机视图和一个数据统计框
    for (int idx = 0; idx < totalWidgets; ++idx) {
        if (idx < camnumber) {
            // 添加相机视图
            CameraLabelWidget* cameraLabel = new CameraLabelWidget(
                cams[idx], idx + 1, caminfo[idx].name, &m_saveQueue, parentWindow
            );
            cameraLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            cameraLabels.append(cameraLabel);
            gridLayout->addWidget(cameraLabel, idx / cols, idx % cols);
        }
        else {
            // 添加最后一个数据统计框
            QWidget* statsWidget = CreateEighthFrame(camnumber);
            statsWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            gridLayout->addWidget(statsWidget, idx / cols, idx % cols);
        }
    }

    // 设置行列拉伸因子，使所有单元格平分空间
    int rows = (totalWidgets + cols - 1) / cols;
    for (int row = 0; row < rows; ++row) {
        gridLayout->setRowStretch(row, 1);
    }
    for (int col = 0; col < cols; ++col) {
        gridLayout->setColumnStretch(col, 1);
    }
}

QString MainWindow::CameralName(int &i)
{
    if(i==0) return QString("正引脚");
    if(i==1) return QString("正引脚");
    if(i==2) return QString("座板");
    if(i==3) return QString("负引脚");
    if(i==4) return QString("捺印");
    if(i==5) return QString("载带");
    if(i==6) return QString("H部");
    if(i==7) return QString("载带座板");
    return QString("");
}


QWidget* MainWindow::CreateEighthFrame(int camnumber)
{
    const QString commonBackgroundColor = "rgb(210, 210, 210)";

    // 最外层的总容器
    QWidget* container = new QWidget(this);
    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    container->setStyleSheet("background-color: " + commonBackgroundColor + "; border: 1px solid black;");

    // 总的垂直布局
    QVBoxLayout* mainVLayout = new QVBoxLayout(container);
    mainVLayout->setContentsMargins(5, 5, 5, 5);
    mainVLayout->setSpacing(5);

    // --- 1. 创建最上方的标题行区域 ---
    QWidget* titleWidget = new QWidget();
    QHBoxLayout* titleLayout = new QHBoxLayout(titleWidget);
    titleLayout->setContentsMargins(0, 0, 0, 0);
    titleLayout->setSpacing(3);

    const char* titles[] = { "相机", "总数", "NG", "良率" };
    for (int col = 0; col < 4; ++col) {
        QLabel* label = new QLabel(titles[col], titleWidget);
        label->setAlignment(col == 0 ? (Qt::AlignLeft | Qt::AlignVCenter) : (Qt::AlignCenter | Qt::AlignVCenter));
        label->setFont(QFont("微软雅黑", 20, QFont::Normal));
        label->setStyleSheet("background-color: transparent; border: none;");
        label->setContentsMargins(5, 0, 5, 0);
        titleLayout->addWidget(label, 1);
    }

    // --- 2. 创建中间的数据表格区域 ---
    QWidget* dataGridWidget = new QWidget();
    dataGridWidget->setStyleSheet("background-color: rgb(210, 210, 210); border-top: 1px solid #555; border-left: 1px solid #555;");

    QGridLayout* gridLayout = new QGridLayout(dataGridWidget);
    gridLayout->setContentsMargins(0, 0, 0, 0);
    gridLayout->setSpacing(0);

  //  qDeleteAll(dataLabels.flatMap([](const QVector<QLabel*>& row) { return row; }));
    dataLabels.clear();

    int dataRows = camnumber + 1;
    qint64 total_sum_count;
    qint64 total_error_count;

    for (int row = 0; row < dataRows; ++row) {
        QVector<QLabel*> currentRowDataLabels;
        for (int col = 0; col < 4; ++col) {
            QLabel* label = new QLabel("", dataGridWidget);
            QString text;
            QFont font("微软雅黑", 16);

            if (row < camnumber) { // 数据行
                int cameraIndex = row;
                if (cameraIndex < caminfo.size() && cameraIndex < cams.size() && cams[cameraIndex]) {
                    switch (col) {
                    case 0: text = caminfo[cameraIndex].name; break;
                    case 1: {
                        qint64 current_sum = cams[cameraIndex]->sum_count.load();
                        text = QString::number(current_sum);
                        total_sum_count += current_sum;
                        break;
                    }
                    case 2: {
                        qint64 current_error = cams[cameraIndex]->error_count.load();
                        text = QString::number(current_error);
                        total_error_count += current_error;
                        break;
                    }
                    case 3: {
                        qint64 current_sum = cams[cameraIndex]->sum_count.load();
                        qint64 current_error = cams[cameraIndex]->error_count.load();
                        double pass_rate = (current_sum > 0) ? ((double)(current_sum - current_error) / current_sum * 100.0) : 0.0;
                        text = QString::asprintf("%.2f%%", pass_rate);
                        break;
                    }
                    }
                }
                else {
                    text = (col == 0) ? QString("未知相机%1").arg(cameraIndex + 1) : "0";
                }
            }
            else { // "总和" 行
                if (col == 0) {
                    text = "总和";
                    font.setBold(true);
                    font.setPointSize(18);
                }
                else if (col == 1) {
                    text = QString::number(total_sum_count);
                }
                else if (col == 2) {
                    text = QString::number(total_error_count);
                }
                else if (col == 3) {
                    double total_pass_rate = (total_sum_count > 0) ? ((double)(total_sum_count - total_error_count) / total_sum_count * 100.0) : 0.0;
                    text = QString::asprintf("%.2f%%", total_pass_rate);
                }
            }
            label->setFont(font);
            label->setAlignment(col == 0 ? (Qt::AlignLeft | Qt::AlignVCenter) : (Qt::AlignRight | Qt::AlignVCenter));
            label->setText(text);
            label->setStyleSheet("background-color: transparent; border-bottom: 1px solid #555; border-right: 1px solid #555;");
            gridLayout->addWidget(label, row, col);
            if (!text.isEmpty() && col > 0) {
                currentRowDataLabels.append(label);
            }
        }
        if (!currentRowDataLabels.isEmpty()) {
            dataLabels.append(currentRowDataLabels);
        }
    }
    for (int col = 0; col < 4; ++col) {
        gridLayout->setColumnStretch(col, 1);
    }

    // --- 3. 创建最下方的按钮区域 ---
    QWidget* buttonWidget = new QWidget();
    QHBoxLayout* buttonLayout = new QHBoxLayout(buttonWidget);
    buttonLayout->setContentsMargins(0, 0, 0, 0);
    buttonLayout->setSpacing(3);

    const char* buttonTexts[] = { "拍照", "启动", "停止", "一键清理" };
    void (MainWindow:: * callbacks[])() = { &MainWindow::onPhotoAllCamerasClicked, &MainWindow::onStartAllCamerasClicked, &MainWindow::onStopAllCamerasClicked, &MainWindow::onClearAllCameraClicked };

    for (int i = 0; i < 4; ++i) {
        QPushButton* button = new QPushButton(buttonTexts[i], buttonWidget);
        button->setFont(QFont("微软雅黑", 18, QFont::Bold));
        button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        button->setStyleSheet("QPushButton { background-color: transparent; border: 1px solid black; padding: 5px; } QPushButton:pressed { background-color: #D8D8D8; padding: 6px 4px 4px 6px; }");
        connect(button, &QPushButton::clicked, this, callbacks[i]);
        buttonLayout->addWidget(button, 1);
    }

    // --- 4. 将上、中、下三个部分按比例拉伸组合 ---
    // 这是最终的解决方案
    mainVLayout->addWidget(titleWidget, 1);
    mainVLayout->addWidget(dataGridWidget, camnumber + 1); // 表格的拉伸比例与其行数相关
    mainVLayout->addWidget(buttonWidget, 1);

    return container;
}


void MainWindow::setLabel(QVBoxLayout *layout, int row, int col)
{
    int counter=0;
    if (row >= 0 && row < 9 && col >= 0 && col < 4) { // 边界检查
        QHBoxLayout *rowLayout = qobject_cast<QHBoxLayout*>(layout->itemAt(row)->layout());
        if (rowLayout) {
            QLabel *label = qobject_cast<QLabel*>(rowLayout->itemAt(col)->widget());
            if (label) {
                label->setText(QString::number(counter++)); // 设置内容为自增的 counter
            }
        }
    }
}


void MainWindow::updateCameraStats()
{
    if (cams.isEmpty() || dataLabels.isEmpty()) {
        return; 
    }

    // 重新计算总和，确保是最新的累加值
    qint64 total_sum_count =0;
    qint64 total_error_count =0;

    // 遍历每个相机的数据行
    // 注意：dataLabels 的外层 QList 的大小应该是 camnumber (相机数据行) + 1 (总和行)
    // caminfo.size() 应该和实际显示的相机行数 camnumber 保持一致
    int actualCamCount = qMin(caminfo.size(), cams.size()); // 取实际相机数量和UI创建数量的最小值

    for (int i = 0; i < actualCamCount; ++i) { // 遍历相机数据行
        if (cams[i]) { // 确保相机对象存在
            qint64 current_sum = cams[i]->sum_count.load();
            qint64 current_error = cams[i]->error_count.load();

            // 更新总数标签 (dataLabels[i][0])
            if (i < dataLabels.size() && dataLabels[i].size() > 0 && dataLabels[i][0]) {
                dataLabels[i][0]->setText(QString::number(current_sum));
            }
            // 更新NG标签 (dataLabels[i][1])
            if (i < dataLabels.size() && dataLabels[i].size() > 1 && dataLabels[i][1]) {
                dataLabels[i][1]->setText(QString::number(current_error));
            }
            // 更新良率标签 (dataLabels[i][2])
            if (i < dataLabels.size() && dataLabels[i].size() > 2 && dataLabels[i][2]) {
                double pass_rate = 0.0;
                if (current_sum > 0) {
                    pass_rate = (double)(current_sum - current_error) / current_sum * 100.0;
                }
                dataLabels[i][2]->setText(QString("%1%").arg(pass_rate, 0, 'f', 2));
            }

            total_sum_count += current_sum;
            total_error_count += current_error;
        }
    }

    // 更新总和行 (dataLabels 的最后一行)
    int totalRowIndex = actualCamCount; // 假设数据行从索引0到actualCamCount-1，总和行是actualCamCount
    if (totalRowIndex < dataLabels.size()) { // 确保总和行标签列表存在
        // 更新总数总和标签 (dataLabels[totalRowIndex][0])
        if (dataLabels[totalRowIndex].size() > 0 && dataLabels[totalRowIndex][0]) {
          //  dataLabels[totalRowIndex][0]->setText(QString::number(total_sum_count));
            dataLabels[totalRowIndex][0]->setText("");
        }
        // 更新NG总和标签 (dataLabels[totalRowIndex][1])
        if (dataLabels[totalRowIndex].size() > 1 && dataLabels[totalRowIndex][1]) {
           // dataLabels[totalRowIndex][1]->setText(QString::number(total_error_count));
            dataLabels[totalRowIndex][1]->setText("");
        }
        // 更新良率总和标签 (dataLabels[totalRowIndex][2])
        if (dataLabels[totalRowIndex].size() > 2 && dataLabels[totalRowIndex][2]) {
            double total_pass_rate = 0.0;
            if (total_sum_count > 0) {
                total_pass_rate = (double)(total_sum_count - total_error_count) / total_sum_count * 100.0;
            }
            dataLabels[totalRowIndex][2]->setText(QString("%1%").arg(total_pass_rate, 0, 'f', 2));
        }
    }
}



bool MainWindow::updateDeviceId()
{
    const QString filePath = SystemPara::GLOBAL_DIR;

    // --- 1. 使用 FileOperator 读取 JSON ---
    QJsonObject rootObj = FileOperator::readJsonObject(filePath);

    if (rootObj.isEmpty()) {
        // readJsonObject 内部（根据您的描述）已经打印了 qCritical 错误
        QString logMsg = QString("Error: 无法读取或解析 JSON 文件: %1").arg(filePath);
        LOG_DEBUG(GlobalLog::logger, logMsg.toStdWString().c_str());
        return false;
    }

    // --- 2. 修改值 ---
    // 检查 "DeviceId" 是否存在且是否为一个对象
    if (!rootObj.contains("DeviceId") || !rootObj["DeviceId"].isObject()) {
        QString logMsg = QString("Error: 在 %1 中未找到 'DeviceId' 键，或者它不是一个对象。").arg(filePath);
        LOG_DEBUG(GlobalLog::logger, logMsg.toStdWString().c_str());
        return false;
    }

    // 获取 DeviceId 对象的副本
    QJsonObject deviceIdObj = rootObj["DeviceId"].toObject();

    // 检查 "值" 键是否存在
    if (!deviceIdObj.contains("值")) {
        LOG_DEBUG(GlobalLog::logger, L"Error: 在 'DeviceId' 中未找到 '值' 键。");
        return false;
    }

    // 更新 "值"
    deviceIdObj["值"] = GlobalPara::DeviceId; // 假设 GlobalPara::DeviceId 可以被 QJsonValue 接受

    // 将修改后的 deviceIdObj 对象放回根对象
    rootObj["DeviceId"] = deviceIdObj;

    // --- 3. 使用 FileOperator 写回文件 ---
    if (!FileOperator::writeJsonObject(filePath, rootObj)) {
        QString logMsg = QString("Error: FileOperator::writeJsonObject 失败，无法写入文件: %1").arg(filePath);
        LOG_DEBUG(GlobalLog::logger, logMsg.toStdWString().c_str());
        return false;
    }

    return true;
}


//void MainWindow::closeEvent(QCloseEvent* event)
//{
//    closeEvent(event);
//}

int MainWindow::initPCI_VC3000H()
{
    int result = 0;
    AppConfig::runningOutput = GlobalPara::RunPoint;
    result = PCI::pci().InitialSystem();
    if (result != 0)
    {
        std::cout << "[ERROR] 系统初始化失败，错误码：" << result << std::endl;
        return -1;
    }
    std::cout << "[INFO] 系统初始化成功。" << std::endl;

    result = PCI::pci().openseral("Com3");
    if (result != 0)
    {
        std::cout << "[ERROR] 打开串口 Com3 失败，错误码：" << result << std::endl;
        return -1;
    }
    std::cout << "[INFO] 串口 Com3 打开成功。PLC连接状态：已连接" << std::endl;

    result = PCI::pci().setPNP(0, AppConfig::GetIoOutputMode().c_str());
    if (result != 0)
        std::cout << "[ERROR] 设置 PNP/NPN 模式失败，错误码：" << result << std::endl;
    else
        std::cout << "[INFO] PNP/NPN 模式设置为：" << AppConfig::GetIoOutputMode() << std::endl;

// 依次设置输出模式
    if ((result = PCI::pci().setOutputMode(AppConfig::camera1Output, false, 200)) != 0) {
        QString errorMsg = QString("[ERROR] 设置 camera1Output 模式失败，错误码：%1").arg(result);
        LOG_DEBUG(GlobalLog::logger, errorMsg.toStdWString().c_str());
    }
    else {
        QString successMsg = QString("[INFO] 设置 camera1Output 模式成功。");
        LOG_DEBUG(GlobalLog::logger, successMsg.toStdWString().c_str());
    }

    if ((result = PCI::pci().setOutputMode(AppConfig::camera2Output, false, 200)) != 0) {
        QString errorMsg = QString("[ERROR] 设置 camera2Output 模式失败，错误码：%1").arg(result);
        LOG_DEBUG(GlobalLog::logger, errorMsg.toStdWString().c_str());
    }
    else {
        QString successMsg = QString("[INFO] 设置 camera2Output 模式成功。");
        LOG_DEBUG(GlobalLog::logger, successMsg.toStdWString().c_str());
    }

    if ((result = PCI::pci().setOutputMode(AppConfig::camera3Output, false, 200)) != 0) {
        QString errorMsg = QString("[ERROR] 设置 camera3Output 模式失败，错误码：%1").arg(result);
        LOG_DEBUG(GlobalLog::logger, errorMsg.toStdWString().c_str());
    }
    else {
        QString successMsg = QString("[INFO] 设置 camera3Output 模式成功。");
        LOG_DEBUG(GlobalLog::logger, successMsg.toStdWString().c_str());
    }
#ifdef USE_MAIN_WINDOW_CAPACITY
    if ((result = PCI::pci().setOutputMode(AppConfig::camera4Output, false, 200)) != 0) {
        QString errorMsg = QString("[ERROR] 设置 camera4Output 模式失败，错误码：%1").arg(result);
        LOG_DEBUG(GlobalLog::logger, errorMsg.toStdWString().c_str());
    }
    else {
        QString successMsg = QString("[INFO] 设置 camera4Output 模式成功。");
        LOG_DEBUG(GlobalLog::logger, successMsg.toStdWString().c_str());
    }

    if ((result = PCI::pci().setOutputMode(AppConfig::camera5Output, false, 200)) != 0) {
        QString errorMsg = QString("[ERROR] 设置 camera5Output 模式失败，错误码：%1").arg(result);
        LOG_DEBUG(GlobalLog::logger, errorMsg.toStdWString().c_str());
    }
    else {
        QString successMsg = QString("[INFO] 设置 camera5Output 模式成功。");
        LOG_DEBUG(GlobalLog::logger, successMsg.toStdWString().c_str());
    }

    if ((result = PCI::pci().setOutputMode(AppConfig::camera6Output, false, 200)) != 0) {
        QString errorMsg = QString("[ERROR] 设置 camera6Output 模式失败，错误码：%1").arg(result);
        LOG_DEBUG(GlobalLog::logger, errorMsg.toStdWString().c_str());
    }
    else {
        QString successMsg = QString("[INFO] 设置 camera6Output 模式成功。");
        LOG_DEBUG(GlobalLog::logger, successMsg.toStdWString().c_str());
    }

    if ((result = PCI::pci().setOutputMode(AppConfig::camera7Output, false, 200)) != 0) {
        QString errorMsg = QString("[ERROR] 设置 camera7Output 模式失败，错误码：%1").arg(result);
        LOG_DEBUG(GlobalLog::logger, errorMsg.toStdWString().c_str());
    }
    else {
        QString successMsg = QString("[INFO] 设置 camera7Output 模式成功。");
        LOG_DEBUG(GlobalLog::logger, successMsg.toStdWString().c_str());
    }
#else
    if ((result = PCI::pci().setInputMode(AppConfig::camera5Output, true, 0, 100)) != 0) {
        QString errorMsg = QString("[ERROR] 设置 set input camera5Output 模式失败，错误码：%1").arg(result);
        LOG_DEBUG(GlobalLog::logger, errorMsg.toStdWString().c_str());
    }
    else {
        QString successMsg = QString("[INFO] 设置 set input camera5Output 模式成功。");
        LOG_DEBUG(GlobalLog::logger, successMsg.toStdWString().c_str());
    }


#endif // USE_MAIN_WINDOW_CAPACITY

    if ((result = PCI::pci().setOutputMode(AppConfig::runningOutput, false, 200)) != 0) {
        QString errorMsg = QString("[ERROR] 设置 runningOutput 模式失败，错误码：%1").arg(result);
        LOG_DEBUG(GlobalLog::logger, errorMsg.toStdWString().c_str());
    }
    else {
        QString successMsg = QString("[INFO] 设置 runningOutput 模式成功。");
        LOG_DEBUG(GlobalLog::logger, successMsg.toStdWString().c_str());
    }



    if (PCI::pci().setlight(1, GlobalPara::Light1, 100, 1, false, 1) != 0) {
        std::cerr << "[ERROR] 1通道光源设置失败，请检查光源线路或光源模块。" << std::endl;
    }
    else {
        std::cout << "[INFO] 1通道光源设置成功。" << std::endl;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    // 通道 2
    if (PCI::pci().setlight(2, GlobalPara::Light2, 100, 1, false, 2) != 0) {
        std::cerr << "[ERROR] 2通道光源设置失败，请检查光源线路或光源模块。" << std::endl;
    }
    else {
        std::cout << "[INFO] 2通道光源设置成功。" << std::endl;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    // 通道 3
    if (PCI::pci().setlight(3, 100, 100, 1, false, 3) != 0) {
        std::cerr << "[ERROR] 3通道光源设置失败，请检查光源线路或光源模块。" << std::endl;
    }
    else {
        std::cout << "[INFO] 3通道光源设置成功。" << std::endl;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    // 通道 4
    if (PCI::pci().setlight(4,100, 100, 1, false, 4) != 0) {
        std::cerr << "[ERROR] 4通道光源设置失败，请检查光源线路或光源模块。" << std::endl;
    }
    else {
        std::cout << "[INFO] 4通道光源设置成功。" << std::endl;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(50));


    // 关闭输出

    if ((result = PCI::pci().setoutput(AppConfig::camera1Output, false)) != 0) {
        QString warningMsg = QString("[WARNING] 关闭 camera1Output 失败，错误码：%1").arg(result);
        LOG_DEBUG(GlobalLog::logger, warningMsg.toStdWString().c_str());
    }

    if ((result = PCI::pci().setoutput(AppConfig::camera2Output, false)) != 0) {
        QString warningMsg = QString("[WARNING] 关闭 camera2Output 失败，错误码：%1").arg(result);
        LOG_DEBUG(GlobalLog::logger, warningMsg.toStdWString().c_str());
    }

    if ((result = PCI::pci().setoutput(AppConfig::camera3Output, false)) != 0) {
        QString warningMsg = QString("[WARNING] 关闭 camera3Output 失败，错误码：%1").arg(result);
        LOG_DEBUG(GlobalLog::logger, warningMsg.toStdWString().c_str());
    }

    if ((result = PCI::pci().setoutput(AppConfig::camera4Output, false)) != 0) {
        QString warningMsg = QString("[WARNING] 关闭 camera4Output 失败，错误码：%1").arg(result);
        LOG_DEBUG(GlobalLog::logger, warningMsg.toStdWString().c_str());
    }

    if ((result = PCI::pci().setoutput(AppConfig::camera5Output, false)) != 0) {
        QString warningMsg = QString("[WARNING] 关闭 camera5Output 失败，错误码：%1").arg(result);
        LOG_DEBUG(GlobalLog::logger, warningMsg.toStdWString().c_str());
    }

    if ((result = PCI::pci().setoutput(AppConfig::camera6Output, false)) != 0) {
        QString warningMsg = QString("[WARNING] 关闭 camera6Output 失败，错误码：%1").arg(result);
        LOG_DEBUG(GlobalLog::logger, warningMsg.toStdWString().c_str());
    }

    if ((result = PCI::pci().setoutput(AppConfig::camera7Output, false)) != 0) {
        QString warningMsg = QString("[WARNING] 关闭 camera7Output 失败，错误码：%1").arg(result);
        LOG_DEBUG(GlobalLog::logger, warningMsg.toStdWString().c_str());
    }

    if ((result = PCI::pci().setoutput(AppConfig::runningOutput, false)) != 0) {
        QString warningMsg = QString("[WARNING] 关闭 runningOutput 失败，错误码：%1").arg(result);
        LOG_DEBUG(GlobalLog::logger, warningMsg.toStdWString().c_str());
    }

   // PCI::pci().SaveParam();
    std::cout << "[INFO] 所有初始化和设置流程完成。" << std::endl;
    return 0;
}

void MainWindow::initSqlite3Db_Brader()
{
    // 1. 初始化数据库连接
    QString dbFileName = SystemPara::ROOT_DIR + "resources/file/SQliteDB/" + GlobalPara::DeviceId + ".db";

    int initResult = SqliteDB::DBOperation::Initialize(dbFileName.toUtf8().constData());
    if (initResult != 0) {
        QMessageBox::critical(this, "数据库错误", "无法初始化数据库实例。");
        return;
    }

    // --- 2. 计算当前班次的表名 ---
    QDateTime now = QDateTime::currentDateTime();
    int currentHour = now.time().hour();
    QDate logicalDate = now.date(); // 逻辑日期
    QString suffix = "";

    // 判断逻辑：
    // 08:00 - 19:59 为白班 (_daytime)
    // 20:00 - 07:59 (次日) 为晚班 (_night)
    if (currentHour >= 8 && currentHour < 20) {
        // 白班
        suffix = "_daytime";
    }
    else {
        // 晚班
        suffix = "_night";
        if (currentHour < 8) {
            // 如果是凌晨 00:00-07:59，逻辑上属于前一天的晚班
            logicalDate = logicalDate.addDays(-1);
        }
    }

    QString datePrefix = logicalDate.toString("yyyyMMdd");
    QString summaryTableName = datePrefix + "_All_summary" + suffix;
    QString detailTableName = datePrefix + "_NG_details" + suffix;

    // 3. 确保当前班次的表都存在
    QString summaryCols = QStringLiteral(
        "\"相机名称\" TEXT PRIMARY KEY, "
        "\"总数\" INT, "
        "\"NG数\" INT, "
        "\"良率\" REAL"
    );
    SqliteDB::DBOperation::CreateTable(summaryTableName.toUtf8().constData(), summaryCols.toUtf8().constData());

    QString detailCols = QStringLiteral(
        "\"相机名称\" TEXT, "
        "\"缺陷类型\" TEXT, "
        "\"数量\" INT, "
        "PRIMARY KEY(\"相机名称\", \"缺陷类型\")"
    );
    SqliteDB::DBOperation::CreateTable(detailTableName.toUtf8().constData(), detailCols.toUtf8().constData());

    // --- 4. 为每个相机加载或创建初始数据 ---
    for (const auto& camera : cams) {
        if (!camera) {
            continue;
        }

        const QString& cameraName = camera->cameral_name;

        // --- PART A: 加载总览表 ---
        QString summaryCondition = QString("\"相机名称\" = '%1'").arg(cameraName);
        std::map<std::string, std::variant<int, double, std::string>> dbRecord;
        SqliteDB::DBOperation::GetFullRecord(
            summaryTableName.toUtf8().constData(),
            summaryCondition.toUtf8().constData(),
            dbRecord
        );

        if (!dbRecord.empty()) {
            // **记录已存在**: 继承数据库中的值
            long long totalFromDb = 0;
            long long ngFromDb = 0;
            if (dbRecord.count("总数")) {
                totalFromDb = std::get<int>(dbRecord.at("总数"));
            }
            if (dbRecord.count("NG数")) {
                ngFromDb = std::get<int>(dbRecord.at("NG数"));
            }
            camera->sum_count.store(totalFromDb);
            camera->error_count.store(ngFromDb);
        }
        else {
            // **记录不存在**: 插入初始值为0的新记录
            std::vector<std::variant<int, double, std::string>> valuesToInsert;
            valuesToInsert.push_back(cameraName.toUtf8().toStdString()); // 写入UTF-8
            valuesToInsert.push_back(0);
            valuesToInsert.push_back(0);
            valuesToInsert.push_back(1.0);
            SqliteDB::DBOperation::InsertRecord(summaryTableName.toUtf8().constData(), valuesToInsert);
            // 内存中的计数器保持默认的0即可
        }

        // --- PART B: 加载NG细分表 ---
        bool hasPaintData = (camera->RI && !camera->RI->m_PaintData.isEmpty());
        if (hasPaintData) {
            for (auto& item : camera->RI->m_PaintData) {
                const QString& defectType = item.label;
                QString detailCondition = QString("\"相机名称\" = '%1' AND \"缺陷类型\" = '%2'").arg(cameraName).arg(defectType);

                std::variant<int, double, std::string> dbDefectCount = "";

                SqliteDB::DBOperation::GetRecordValue(
                    detailTableName.toUtf8().constData(),
                    "数量",
                    detailCondition.toUtf8().constData(),
                    dbDefectCount
                );

                if (std::holds_alternative<int>(dbDefectCount)) {
                    item.count = std::get<int>(dbDefectCount);
                }
                else {
                    std::vector<std::variant<int, double, std::string>> defectValuesToInsert;
                    defectValuesToInsert.push_back(cameraName.toUtf8().toStdString()); // 写入UTF-8
                    defectValuesToInsert.push_back(defectType.toUtf8().toStdString()); // 写入UTF-8
                    defectValuesToInsert.push_back(0);
                    SqliteDB::DBOperation::InsertRecord(detailTableName.toUtf8().constData(), defectValuesToInsert);
                    item.count = 0;
                }
            }
        }
    }
}


void MainWindow::updateDB_Brader()
{
    // --- 1. 计算当前班次和上一班次的表名 ---
    QDateTime now = QDateTime::currentDateTime();
    int currentHour = now.time().hour();
    QDate logicalDate = now.date();

    QString currentSuffix;
    QString prevSuffix;
    QDate prevLogicalDate;

    // 判断逻辑：8点和20点为界
    if (currentHour >= 8 && currentHour < 20) {
        // 当前是: 白班 (Today_daytime)
        // 上一班: 昨天的晚班 (Yesterday_night)
        currentSuffix = "_daytime";
        prevSuffix = "_night";
        prevLogicalDate = logicalDate.addDays(-1);
    }
    else {
        // 当前是: 晚班
        currentSuffix = "_night";
        if (currentHour < 8) {
            // 如果是凌晨，归属到前一天
            logicalDate = logicalDate.addDays(-1);
        }

        // 上一班: 今天的白班
        prevSuffix = "_daytime";
        prevLogicalDate = logicalDate;
    }

    QString dateStr = logicalDate.toString("yyyyMMdd");
    QString prevDateStr = prevLogicalDate.toString("yyyyMMdd");

    // 当前表名
    QString summaryTableName = dateStr + "_All_summary" + currentSuffix;
    QString detailTableName = dateStr + "_NG_details" + currentSuffix;

    // 上一班表名 (用于结转)
    QString prevSummaryTableName = prevDateStr + "_All_summary" + prevSuffix;
    QString prevDetailTableName = prevDateStr + "_NG_details" + prevSuffix;

    // --- 2. 确保当前班次的表存在 ---
    QString summaryCols = QStringLiteral(
        "\"相机名称\" TEXT PRIMARY KEY, "
        "\"总数\" INT, "
        "\"NG数\" INT, "
        "\"良率\" REAL"
    );
    SqliteDB::DBOperation::CreateTable(summaryTableName.toUtf8().constData(), summaryCols.toUtf8().constData());

    QString detailCols = QStringLiteral(
        "\"相机名称\" TEXT, "
        "\"缺陷类型\" TEXT, "
        "\"数量\" INT, "
        "PRIMARY KEY(\"相机名称\", \"缺陷类型\")"
    );
    SqliteDB::DBOperation::CreateTable(detailTableName.toUtf8().constData(), detailCols.toUtf8().constData());

    // --- 3. 遍历所有相机进行更新 ---
    for (const auto& camera : cams) {
        if (!camera) continue;

        const QString& cameraName = camera->cameral_name;
        long long currentAbsoluteTotal = camera->sum_count.load();
        long long currentAbsoluteNg = camera->error_count.load();

        QString condition = QString("\"相机名称\" = '%1'").arg(cameraName);

        // 检查当前班次的记录是否存在
        std::map<std::string, std::variant<int, double, std::string>> existingRecord;
        SqliteDB::DBOperation::GetFullRecord(
            summaryTableName.toUtf8().constData(),
            condition.toUtf8().constData(),
            existingRecord
        );

        if (existingRecord.empty()) {
            // --- 换班了：当前班次无记录，执行结转 ---

            // a. 读取上一班次的数据
            std::map<std::string, std::variant<int, double, std::string>> prevRecord;
            SqliteDB::DBOperation::GetFullRecord(
                prevSummaryTableName.toUtf8().constData(),
                condition.toUtf8().constData(),
                prevRecord
            );

            long long prevTotal = 0;
            long long prevNg = 0;
            if (!prevRecord.empty()) {
                if (prevRecord.count("总数")) prevTotal = std::get<int>(prevRecord.at("总数"));
                if (prevRecord.count("NG数")) prevNg = std::get<int>(prevRecord.at("NG数"));
            }

            // b. 计算当前班次的初始增量
            long long currentShiftTotal = currentAbsoluteTotal - prevTotal;
            long long currentShiftNg = currentAbsoluteNg - prevNg;
            double yieldRate = (currentShiftTotal > 0) ? (static_cast<double>(currentShiftTotal - currentShiftNg) / currentShiftTotal) : 0.0;

            // c. 插入新表
            std::vector<std::variant<int, double, std::string>> summaryValues;
            summaryValues.push_back(cameraName.toUtf8().toStdString());
            summaryValues.push_back(static_cast<int>(currentShiftTotal));
            summaryValues.push_back(static_cast<int>(currentShiftNg));
            summaryValues.push_back(yieldRate);
            SqliteDB::DBOperation::InsertRecord(summaryTableName.toUtf8().constData(), summaryValues);

            // d. 【关键】重置内存计数器为当前班次的初始值
            camera->sum_count.store(currentShiftTotal);
            camera->error_count.store(currentShiftNg);

            // e. 结转NG详情表
            bool hasPaintData = (camera->RI && !camera->RI->m_PaintData.isEmpty());
            if (hasPaintData) {
                for (auto& item : camera->RI->m_PaintData) {
                    const QString& defectType = item.label;
                    long long currentAbsoluteDefect = item.count;
                    QString detailCondition = QString("\"相机名称\" = '%1' AND \"缺陷类型\" = '%2'").arg(cameraName).arg(defectType);

                    std::variant<int, double, std::string> prevDefectCountVar = "";
                    SqliteDB::DBOperation::GetRecordValue(
                        prevDetailTableName.toUtf8().constData(), "数量", detailCondition.toUtf8().constData(), prevDefectCountVar
                    );

                    long long prevDefectCount = 0;
                    if (std::holds_alternative<int>(prevDefectCountVar)) {
                        prevDefectCount = std::get<int>(prevDefectCountVar);
                    }

                    long long currentShiftDefect = currentAbsoluteDefect - prevDefectCount;

                    std::vector<std::variant<int, double, std::string>> detailValues;
                    detailValues.push_back(cameraName.toUtf8().toStdString());
                    detailValues.push_back(defectType.toUtf8().toStdString());
                    detailValues.push_back(static_cast<int>(currentShiftDefect));
                    SqliteDB::DBOperation::InsertRecord(detailTableName.toUtf8().constData(), detailValues);

                    // 重置内存中的NG计数
                    item.count = currentShiftDefect;
                }
            }

        }
        else {
            // --- 班次内：常规更新 ---

            double newYieldRate = (currentAbsoluteTotal > 0) ? (static_cast<double>(currentAbsoluteTotal - currentAbsoluteNg) / currentAbsoluteTotal) : 0.0;

            std::map<std::string, std::variant<int, double, std::string>> summaryValues;
            summaryValues["总数"] = static_cast<int>(currentAbsoluteTotal);
            summaryValues["NG数"] = static_cast<int>(currentAbsoluteNg);
            summaryValues["良率"] = newYieldRate;
            SqliteDB::DBOperation::UpdateFullRecord(
                summaryTableName.toUtf8().constData(), summaryValues, condition.toUtf8().constData()
            );

            bool hasPaintData = (camera->RI && !camera->RI->m_PaintData.isEmpty());
            if (hasPaintData) {
                for (auto& item : camera->RI->m_PaintData) {
                    QString detailCondition = QString("\"相机名称\" = '%1' AND \"缺陷类型\" = '%2'").arg(cameraName).arg(item.label);
                    SqliteDB::DBOperation::UpdateRecordValue(
                        detailTableName.toUtf8().constData(),
                        "数量",
                        static_cast<int>(item.count),
                        detailCondition.toUtf8().constData()
                    );
                }
            }
        }
    }
}


void MainWindow::initSqlite3Db_Plater()
{
    // 1. 初始化数据库连接
    QString dbFileName = SystemPara::ROOT_DIR + "resources/file/SQliteDB/" + GlobalPara::DeviceId + ".db";

    int initResult = SqliteDB::DBOperation::Initialize(dbFileName.toUtf8().constData());
    if (initResult != 0) {
        QMessageBox::critical(this, "数据库错误", "无法初始化数据库实例。");
        qDebug() << "错误: 无法初始化数据库实例。" << dbFileName;
        return;
    }
    qDebug() << "数据库连接成功: " << dbFileName;

    // 2. 准备固定的总表名和表结构定义
    const char* summaryTableName = "plater";
    QString summaryCols = QStringLiteral(
        "\"相机名称\" TEXT PRIMARY KEY, "
        "\"总数\" INT, "
        "\"NG数\" INT, "
        "\"良率\" REAL"
    );

    // 3. 确保总表存在
    SqliteDB::DBOperation::CreateTable(summaryTableName, summaryCols.toUtf8().constData());
    qDebug() << "已创建或确认表存在: " << summaryTableName;

    // 4. 遍历所有相机，加载或创建初始数据
    for (const auto& camera : cams) {
        if (!camera) {
            continue;
        }

        const QString& cameraName = camera->cameral_name;
        QString summaryCondition = QString("\"相机名称\" = '%1'").arg(cameraName);
        std::map<std::string, std::variant<int, double, std::string>> dbRecord;

        qDebug() << "正在为相机 '" << cameraName << "' 检查数据库记录...";

        // 尝试从数据库加载现有记录
        SqliteDB::DBOperation::GetFullRecord(
            summaryTableName,
            summaryCondition.toUtf8().constData(),
            dbRecord
        );

        if (!dbRecord.empty()) {
            // 记录已存在：继承数据库中的值
            long long totalFromDb = 0;
            long long ngFromDb = 0;

            auto it_total = dbRecord.find("总数");
            if (it_total != dbRecord.end() && std::holds_alternative<int>(it_total->second)) {
                totalFromDb = std::get<int>(it_total->second);
            }
            auto it_ng = dbRecord.find("NG数");
            if (it_ng != dbRecord.end() && std::holds_alternative<int>(it_ng->second)) {
                ngFromDb = std::get<int>(it_ng->second);
            }
            camera->sum_count.store(totalFromDb);
            camera->error_count.store(ngFromDb);

            qDebug() << "为相机 '" << cameraName << "' 找到现有记录。已继承数据：总数=" << totalFromDb << ", NG数=" << ngFromDb;

        }
        else {
            // 记录不存在：插入初始值为0的新记录
            std::vector<std::variant<int, double, std::string>> valuesToInsert;
            valuesToInsert.push_back(cameraName.toStdString()); // 相机名称
            valuesToInsert.push_back(0);                       // 总数
            valuesToInsert.push_back(0);                       // NG数
            valuesToInsert.push_back(1.0);                     // 良率 (初始为100%)
            SqliteDB::DBOperation::InsertRecord(summaryTableName, valuesToInsert);
            qDebug() << "为相机 '" << cameraName << "' 创建新记录，初始值设为0。";
        }
    }
}


void MainWindow::updateDB_Plater()
{
    const char* summaryTableName = "plater";

    for (const auto& camera : cams) {
        if (!camera) {
            continue;
        }

        const QString& cameraName = camera->cameral_name;
        long long currentTotal = camera->sum_count.load();
        long long currentNg = camera->error_count.load();
        double newYieldRate = (currentTotal > 0) ? (static_cast<double>(currentTotal - currentNg) / currentTotal) : 0.0;

        QString condition = QString("\"相机名称\" = '%1'").arg(cameraName);
        std::map<std::string, std::variant<int, double, std::string>> existingRecord;

        SqliteDB::DBOperation::GetFullRecord(
            summaryTableName,
            condition.toUtf8().constData(),
            existingRecord
        );

        if (existingRecord.empty()) {
            // 记录不存在，插入新记录
            std::vector<std::variant<int, double, std::string>> valuesToInsert;
            valuesToInsert.push_back(cameraName.toStdString());
            valuesToInsert.push_back(static_cast<int>(currentTotal));
            valuesToInsert.push_back(static_cast<int>(currentNg));
            valuesToInsert.push_back(newYieldRate);
            SqliteDB::DBOperation::InsertRecord(summaryTableName, valuesToInsert);
        }
        else {
            // 记录已存在，更新数据
            std::map<std::string, std::variant<int, double, std::string>> summaryValues;
            summaryValues["总数"] = static_cast<int>(currentTotal);
            summaryValues["NG数"] = static_cast<int>(currentNg);
            summaryValues["良率"] = newYieldRate;
            SqliteDB::DBOperation::UpdateFullRecord(
                summaryTableName, summaryValues, condition.toUtf8().constData()
            );
        }
    }
}

void MainWindow::setupUpdateTimer()
{

    m_updateTimer = new QTimer(this); 
    m_databaseTimer = new QTimer(this);
#ifdef USE_MAIN_WINDOW_CAPACITY
    connect(m_updateTimer, &QTimer::timeout, this, &MainWindow::updateCameraStats);

    connect(m_databaseTimer, &QTimer::timeout, this, &MainWindow::updateDB_Plater);

    m_databaseTimer->start(10*60*1000);
#else  
    connect(m_updateTimer, &QTimer::timeout, m_rightControlPanel, &RightControlPanel::updateStatistics);

    connect(m_databaseTimer, &QTimer::timeout, this, &MainWindow::updateDB_Brader);

    m_databaseTimer->start(60 * 1000);
#endif
    connect(m_updateTimer, &QTimer::timeout, this, &MainWindow::AllCameraConnect);

	connect(m_databaseTimer, &QTimer::timeout, this, &MainWindow::RefreshDir);
    m_updateTimer->start(1000);
   
}

void MainWindow::RefreshDir()
{
    QDate today = QDate::currentDate();
    if (today != m_lastDate) {
        m_lastDate = today;
        QString dateStr = today.toString("yyyyMMdd");

        for (int i = 0; i < cams.size(); ++i) {
            // 先把已有路径分割成目录列表
            auto updatePath = [&](QString& path) {
                QDir dir(path);
                QString parent = dir.path();           // 当前路径
                QString baseName = dir.dirName();     // 最后一层目录（原日期）
                parent.chop(baseName.length());       // 去掉原日期
                path = parent + dateStr;              // 拼接新的日期
                };

            updatePath(cams[i]->ok_path);
            updatePath(cams[i]->ng_path);
            updatePath(cams[i]->localpath);

            // 创建目录
            QStringList paths = { cams[i]->ok_path, cams[i]->ng_path, cams[i]->localpath };
            for (const QString& p : paths) {
                if (!QDir(p).exists()) {
                    if (QDir().mkpath(p))
                        qDebug() << "路径创建成功:" << p;
                    else
                        qDebug() << "路径创建失败:" << p;
                }
                else {
                    qDebug() << "路径已存在:" << p;
                }
            }

			cameraLabels[i]->ChangeDateDir(*cams[i]);
            cameraLabels[i]->m_imageProcessor->ChangeDir(*cams[i]);
        }
    }
 

}


void MainWindow::AllCameraConnect()
{
    int result;
    
   
    for (int i=0;i<cams.size();i++)
    {
        if (cams[i]->camOp == nullptr)
        {
            LOG_DEBUG(GlobalLog::logger, L"camOp is null");
            return;
        }
        int ret=cams[i]->camOp->IsDevConnect();
        if(ret==0)
        {
           QString errorMsg = QString("cam is do not connect, camera name: %1").arg(cams[i]->cameral_name);
            LOG_DEBUG(GlobalLog::logger, errorMsg.toStdWString().c_str());
            cameraLabels[i]->onCameraConnect("#8B0000");

            bool outputSignalInvert = false;
            unsigned short durationMs = 100;
            PCI::pci().setOutputMode(AppConfig::runningOutput, outputSignalInvert ? true : false, durationMs);
 


                if (cams[i]->m_cameraWarningShown==false)
                {
                    cams[i]->m_cameraWarningShown = true; // 只弹一次
                    QMessageBox::warning(this, "相机警告", "相机未连接，请重启软件");

                }
              //  cams[i]->camOp->MsvCloseDevice();
        }
        else 
        {
            cameraLabels[i]->onCameraConnect("#3CB371");
        }
    }
}


void MainWindow::onStartAllCamerasClicked()
{
    LOG_DEBUG(GlobalLog::logger, L"收到全部运行的请求");
//重置七个点位
    int result;
    if ((result = PCI::pci().setOutputMode(AppConfig::camera1Output, false, 1000)) != 0) {
        QString errorMsg = QString("[ERROR] 设置 camera1Output 模式失败，错误码：%1").arg(result);
        LOG_DEBUG(GlobalLog::logger, errorMsg.toStdWString().c_str());
    }
    else {
        QString successMsg = QString("[INFO] 设置 camera1Output 模式成功。");
        LOG_DEBUG(GlobalLog::logger, successMsg.toStdWString().c_str());
    }

    if ((result = PCI::pci().setOutputMode(AppConfig::camera2Output, false, 1000)) != 0) {
        QString errorMsg = QString("[ERROR] 设置 camera2Output 模式失败，错误码：%1").arg(result);
        LOG_DEBUG(GlobalLog::logger, errorMsg.toStdWString().c_str());
    }
    else {
        QString successMsg = QString("[INFO] 设置 camera2Output 模式成功。");
        LOG_DEBUG(GlobalLog::logger, successMsg.toStdWString().c_str());
    }

    if ((result = PCI::pci().setOutputMode(AppConfig::camera3Output, false, 1000)) != 0) {
        QString errorMsg = QString("[ERROR] 设置 camera3Output 模式失败，错误码：%1").arg(result);
        LOG_DEBUG(GlobalLog::logger, errorMsg.toStdWString().c_str());
    }
    else {
        QString successMsg = QString("[INFO] 设置 camera3Output 模式成功。");
        LOG_DEBUG(GlobalLog::logger, successMsg.toStdWString().c_str());
    }

    if ((result = PCI::pci().setOutputMode(AppConfig::camera4Output, false, 300)) != 0) {
        QString errorMsg = QString("[ERROR] 设置 camera4Output 模式失败，错误码：%1").arg(result);
        LOG_DEBUG(GlobalLog::logger, errorMsg.toStdWString().c_str());
    }
    else {
        QString successMsg = QString("[INFO] 设置 camera4Output 模式成功。");
        LOG_DEBUG(GlobalLog::logger, successMsg.toStdWString().c_str());
    }

    if ((result = PCI::pci().setOutputMode(AppConfig::camera5Output, false, 300)) != 0) {
        QString errorMsg = QString("[ERROR] 设置 camera5Output 模式失败，错误码：%1").arg(result);
        LOG_DEBUG(GlobalLog::logger, errorMsg.toStdWString().c_str());
    }
    else {
        QString successMsg = QString("[INFO] 设置 camera5Output 模式成功。");
        LOG_DEBUG(GlobalLog::logger, successMsg.toStdWString().c_str());
    }

    if ((result = PCI::pci().setOutputMode(AppConfig::camera6Output, false, 300)) != 0) {
        QString errorMsg = QString("[ERROR] 设置 camera6Output 模式失败，错误码：%1").arg(result);
        LOG_DEBUG(GlobalLog::logger, errorMsg.toStdWString().c_str());
    }
    else {
        QString successMsg = QString("[INFO] 设置 camera6Output 模式成功。");
        LOG_DEBUG(GlobalLog::logger, successMsg.toStdWString().c_str());
    }

    if ((result = PCI::pci().setOutputMode(AppConfig::camera7Output, false, 300)) != 0) {
        QString errorMsg = QString("[ERROR] 设置 camera7Output 模式失败，错误码：%1").arg(result);
        LOG_DEBUG(GlobalLog::logger, errorMsg.toStdWString().c_str());
    }
    else {
        QString successMsg = QString("[INFO] 设置 camera7Output 模式成功。");
        LOG_DEBUG(GlobalLog::logger, successMsg.toStdWString().c_str());
    }


    for (int i = 0; i < cameraLabels.size(); ++i) {
        if (cameraLabels[i] && i < cams.size() && cams[i] && cams[i]->camOp)
        {
            cameraLabels[i]->onCameraStart();
        } 
        else {
            // 如果有任何一个为空，打印警告信息
            qWarning() << "Warning: Cannot trigger run for camera" << i + 1 << ". CameraLabelWidget or Cameral object is null.";
            return;
        }
        bool outputSignalInvert = true;
        unsigned short durationMs = 100;
        PCI::pci().setOutputMode(AppConfig::runningOutput, outputSignalInvert ? true : false, durationMs);
    }
}

void MainWindow::onPhotoAllCamerasClicked()
{
    LOG_DEBUG(GlobalLog::logger, L"收到全部拍照的请求");
    for (int i = 0; i < cameraLabels.size(); ++i) {
        if (cameraLabels[i] && i < cams.size() && cams[i]) {
            cameraLabels[i]->onCameraPhoto();
        }
        else {
            // 如果有任何一个为空，打印警告信息
            qWarning() << "Warning: Cannot trigger photo for camera" << i + 1 << ". CameraLabelWidget or Cameral object is null.";
            return;
        }
    }
}


void MainWindow::onStopAllCamerasClicked() {
    LOG_DEBUG(GlobalLog::logger, L"收到全部停止的请求");
    int result;
    if ((result = PCI::pci().setOutputMode(AppConfig::camera1Output, false, 1000)) != 0) {
        QString errorMsg = QString("[ERROR] 设置 camera1Output 模式失败，错误码：%1").arg(result);
        LOG_DEBUG(GlobalLog::logger, errorMsg.toStdWString().c_str());
    }
    else {
        QString successMsg = QString("[INFO] 设置 camera1Output 模式成功。");
        LOG_DEBUG(GlobalLog::logger, successMsg.toStdWString().c_str());
    }

    if ((result = PCI::pci().setOutputMode(AppConfig::camera2Output, false, 1000)) != 0) {
        QString errorMsg = QString("[ERROR] 设置 camera2Output 模式失败，错误码：%1").arg(result);
        LOG_DEBUG(GlobalLog::logger, errorMsg.toStdWString().c_str());
    }
    else {
        QString successMsg = QString("[INFO] 设置 camera2Output 模式成功。");
        LOG_DEBUG(GlobalLog::logger, successMsg.toStdWString().c_str());
    }

    if ((result = PCI::pci().setOutputMode(AppConfig::camera3Output, false, 1000)) != 0) {
        QString errorMsg = QString("[ERROR] 设置 camera3Output 模式失败，错误码：%1").arg(result);
        LOG_DEBUG(GlobalLog::logger, errorMsg.toStdWString().c_str());
    }
    else {
        QString successMsg = QString("[INFO] 设置 camera3Output 模式成功。");
        LOG_DEBUG(GlobalLog::logger, successMsg.toStdWString().c_str());
    }

    if ((result = PCI::pci().setOutputMode(AppConfig::camera4Output, false, 300)) != 0) {
        QString errorMsg = QString("[ERROR] 设置 camera4Output 模式失败，错误码：%1").arg(result);
        LOG_DEBUG(GlobalLog::logger, errorMsg.toStdWString().c_str());
    }
    else {
        QString successMsg = QString("[INFO] 设置 camera4Output 模式成功。");
        LOG_DEBUG(GlobalLog::logger, successMsg.toStdWString().c_str());
    }

    if ((result = PCI::pci().setOutputMode(AppConfig::camera5Output, false, 300)) != 0) {
        QString errorMsg = QString("[ERROR] 设置 camera5Output 模式失败，错误码：%1").arg(result);
        LOG_DEBUG(GlobalLog::logger, errorMsg.toStdWString().c_str());
    }
    else {
        QString successMsg = QString("[INFO] 设置 camera5Output 模式成功。");
        LOG_DEBUG(GlobalLog::logger, successMsg.toStdWString().c_str());
    }

    if ((result = PCI::pci().setOutputMode(AppConfig::camera6Output, false, 300)) != 0) {
        QString errorMsg = QString("[ERROR] 设置 camera6Output 模式失败，错误码：%1").arg(result);
        LOG_DEBUG(GlobalLog::logger, errorMsg.toStdWString().c_str());
    }
    else {
        QString successMsg = QString("[INFO] 设置 camera6Output 模式成功。");
        LOG_DEBUG(GlobalLog::logger, successMsg.toStdWString().c_str());
    }

    if ((result = PCI::pci().setOutputMode(AppConfig::camera7Output, false, 300)) != 0) {
        QString errorMsg = QString("[ERROR] 设置 camera7Output 模式失败，错误码：%1").arg(result);
        LOG_DEBUG(GlobalLog::logger, errorMsg.toStdWString().c_str());
    }
    else {
        QString successMsg = QString("[INFO] 设置 camera7Output 模式成功。");
        LOG_DEBUG(GlobalLog::logger, successMsg.toStdWString().c_str());
    }

    if ((result = PCI::pci().setOutputMode(AppConfig::runningOutput, false, 300)) != 0) {
        QString errorMsg = QString("[ERROR] 设置 runningOutput 模式失败，错误码：%1").arg(result);
        LOG_DEBUG(GlobalLog::logger, errorMsg.toStdWString().c_str());
    }
    else {
        QString successMsg = QString("[INFO] 设置 runningOutput 模式成功。");
        LOG_DEBUG(GlobalLog::logger, successMsg.toStdWString().c_str());
    }


    for (int i = 0; i < cameraLabels.size(); ++i) {
        if (cameraLabels[i] && i < cams.size() && cams[i]) {
            cams[i]->camOp->MsvStopImageCapture();
          //  cameraLabels[i]->triggerCameraStop(cams[i]);
            cams[i]->video.store(false);
        } else {
            // 如果有任何一个为空，打印警告信息
            qWarning() << "Warning: Cannot trigger stop for camera" << i + 1 << ". CameraLabelWidget or Cameral object is null.";
        }
    }

}

void MainWindow::onClearAllCameraClicked()
{
    for(auto cam :cams)
    {
        cam->sum_count.store(0);
        cam->error_count.store(0);
        qDebug()<<"all cams clear";

    }
#ifndef USE_MAIN_WINDOW_CAPACITY
    for (auto display : m_displayInfoWidgets)
    {
        display->resetCounters();
    }
    for (auto cam : cams)
    {
        for (int i = 0; i < cam->RI->m_PaintData.size();i++)
        {
            cam->RI->m_PaintData[i].count = 0;
        }
    }
#endif // !USE_MAIN_WINDOW_CAPACITY

}

void MainWindow::onParamClicked(int buttonIndex)
{
    this->cameraLabels[buttonIndex]->onParam();
}