#ifndef COMMON_H
#define COMMON_H
#include <stdexcept>
#include <array>
#include <opencv2/core/mat.hpp>
#include "infer.h"

#define WORKNUM 4

enum class XS_NGReults//线扫相机NG输出
{
    OK,
    AX_Err,
    CMAX_Err,
    HS_Err,
    ZW_Err,
    FK_Err,
    SJ_Err,
    JM_Err,
    NG,
};
enum class NY_NGReults//捺印字符
{
    OK,
    NULL_Err,
    CQ_Err,
    GS_Err,
    HS_Err,
    QP_Err,
    YH_Err,
    ZW_Err,
    XT_ZF_Err,
    XT_Color_Err,
};
enum class Crop_Bottom_NGReults//电容底部
{
    OK,
    LKPS_Err,
    JMPS_Err,
    WBTC_Err,
    NG,
};

//enum HXDetectionRsult
//{
//    HXSIZE_Err,
//    HXAREA_Err,
//    HXPY_Err,
//    TBDIS_Err,
//};

struct Params 
{
    bool cuda;
	bool LearnFlag = false;
    //线扫相机参数
    std::string XS_PositionModel;
    std::string SegModel;
    float XS_conf_thresh;
    float XS_nms_thresh;
    int XS_timeout;
    //捺印字符相关参数
    std::string NY_SegModel;
    std::string NY_StampPositionModel;
    std::string NY_DefectBboxModel;
    std::string OCR_ModelFile;
    std::string OCR_keyFile;
    float NY_conf_thresh;
    float NY_nms_thresh;
    int NY_timeout;
    int m_FitCirclePointsNum;
    int m_FitCircleMaxIter;
    int m_FitCircleErrorThreshold;
    int m_RadiusReduce;
    int AngleNum;
    bool isOCR;
    int logolabelindex;
    //型替学习相关
    int ColorNum = 0;
	std::vector<std::string> OCR_str;
    //电容底部检测
    std::string LB_SegModel;
    std::string RB_SegModel;
    float B_conf_thresh;
    float B_nms_thresh;
    int B_timeout;
    //模型加载
    OVYOLOV8 XS_yoloDetector;
    OVYOLOV8Seg XS_SegDetector;
    OVYOLOV8Seg NY_SegmodelDetector;
    OVYOLOV8 NY_yoloStampDetector;//字符框检测
    OVYOLOV8 NY_DefectBboxDetector;//捺印面瑕疵检测
    OVCRNN NY_OCRDetector;
    OVYOLOV8Seg LB_SegDetector;
    OVYOLOV8Seg RB_SegDetector;
};

struct InThresholdParam
{
    //线扫胶帽和束胶检测
    float SJ_WidthMin = 1;
    float SJ_WidthMax = 3;
    float JM_Height = 0.4;
    int JM_Thresholdup = 56;
    int JM_Thresholddown = 29;
	float XS_BDparam = 7;
    //瑕疵检测
    float AX_max = 0.3;
    float CMAX_max = 0.2;
    float HS_max = 0.2;
    float ZW_max = 0.08;
    //捺印相关输入控制参数
    float CQ_AreaMin = 20;
    float GS_AreaMin = 20;
    float HS_AreaMin = 20;
    float QP_AreaMin = 20;
    float YH_AreaMin = 20;
    float ZW_AreaMin = 10;
	float NY_SeCha_Ratio = 0.105;
    float NY_BDparam = 5;
    //模型严格程度控制参数
    float OCR_Control = 0.6;
	//型替字符颜色过滤参数
	int XT_ZF_ColorThreshold = 60;
	//型替字符相似度控制参数
	float XT_Similarity_Threshold = 1;
	//型替字符HSV颜色范围
    int NY_H = 80;
	int NY_S = 150;
    //底面相机
    float LK_min = 1;
    float JM_min = 1;
    float WB_min = 1;
    float LK_BOTTOM_BDparam = 5;
    float JM_BOTTOM_BDparam = 5;
};
struct XSResult
{
    XS_NGReults NGResult;
    cv::Mat dstImg;
    //JM相关检测项
    float JM_height = 0.0f;
    float SHUJIAO_Width = 0.0f;
    //瑕疵检测项
    float ZW_ALLArea = 0.0f;//脏污所有面积
    float ZW_MaxArea = 0.0f;//脏污所有面积
    float AX_MaxArea = 0.0f;//凹陷面积
    float CMAX_MaxArea = 0.0f;//凹陷面积
    float HS_MaxArea = 0.0f;//划伤面积
};
struct NYResult
{
	NY_NGReults NGResult_single;
    unsigned char NGResult = 0x00;
    cv::Mat dstImg;
    //捺印缺陷检测框信息
	float CQ_Area = 0.0f;
	float GS_Area = 0.0f;
	float HS_Area = 0.0f;
	float QP_Area = 0.0f;
	float YH_Area = 0.0f;
    float ZW_Area = 0.0f;
};
struct Crop_BottomResult
{
    Crop_Bottom_NGReults NGResult;
    cv::Mat JM_dstImg;
    cv::Mat LK_dstImg;
    //JM相关检测项
    float JM_PSallArea = 0.0f;
    float JM_PSmaxArea = 0.0f;//最大面积
    float WB_MaxArea = 0.0f;//丸棒最大面积
    //铝壳瑕疵检测项
    float LK_PSallArea = 0.0f;
    float LK_PSmaxArea = 0.0f;//最大面积
};


struct ALLResult
{
    XSResult xsResult;
    NYResult nyResult;
    Crop_BottomResult crop_bootomResult;
};

struct CFrameResult
{
    int sig;
    int camera;
    bool result;
};

struct CameraSw {
    int m_nSource;
    int m_nComposite;
    int m_nOK;
    int m_nNG;
    CameraSw(int ng = 0, int ok = 0, int composite = 0, int source = 0)
        : m_nSource(source), m_nComposite(composite), m_nOK(ok), m_nNG(ng) {}
};

struct SaveSw
{
    CameraSw m_cSave1, m_cSave2, m_cSave3, m_cSave4, m_cSave5, m_cSave6;

    CameraSw& getCameraSw(int index) {
        switch(index) {
        case 1: return m_cSave1;
        case 2: return m_cSave2;
        case 3: return m_cSave3;
        case 4: return m_cSave4;
        case 5: return m_cSave5;
        case 6: return m_cSave6;
        default: throw std::out_of_range("Invalid group index");
        }
    }
};
// 成员指针类型别名
using CameraSwMember = int CameraSw::*;

inline constexpr std::array<CameraSwMember, 4> cameraSwMembers = {
    &CameraSw::m_nSource,
    &CameraSw::m_nComposite,
    &CameraSw::m_nOK,
    &CameraSw::m_nNG
};

// 帧信息
// frame imformation
class CFrameInfo
{
public:
    CFrameInfo()
    {
        m_pImageBuf = NULL;
        m_nBufferSize = 0;
        m_nWidth = 0;
        m_nHeight = 0;
        m_nPaddingX = 0;
        m_nPaddingY = 0;
        m_nTimeStamp = 0;
    }

    ~CFrameInfo()
    {
    }

public:
    std::shared_ptr<unsigned char> m_pImageBuf;
    int				m_nBufferSize;
    int				m_nWidth;
    int				m_nHeight;
    int				m_nPaddingX;
    int				m_nPaddingY;
    uint64_t		m_nTimeStamp;
};
#endif // COMMON_H
