#pragma once
#include <opencv2/core.hpp> 
#include "Param.h"

#ifndef MYLIBRARY_EXPORTS
#define MYLIBRARY_API __declspec(dllimport)
#else
#define MYLIBRARY_API __declspec(dllexport)
#endif


namespace ExportSpace {

    // 初始化
    MYLIBRARY_API int InitializeStamp();
    MYLIBRARY_API int InitializePlate();
    MYLIBRARY_API int InitializeLift();
    MYLIBRARY_API int InitializeAbut();
    MYLIBRARY_API int InitializeGJ();
    MYLIBRARY_API int InitializeVC1();
    MYLIBRARY_API int InitializeVC2();
    MYLIBRARY_API int InitializeVC3();
    MYLIBRARY_API int InitializeVC4();

    // 执行函数
    MYLIBRARY_API int RunStamp(cv::Mat& Src, bool update, int al_core, InStampParam& inStampParam);
    MYLIBRARY_API int RunPlate(cv::Mat& SrcImg, int al_core = 1, int angleNum = 0);
    MYLIBRARY_API int RunLift(cv::Mat& src, int al_core, int angleNum = 0, bool PinAnglEnalbe = true);
    MYLIBRARY_API int RunAbut(cv::Mat& src, InAbutParam& inParam);
    MYLIBRARY_API int RunYYGJ(cv::Mat& src, int angleNum, bool PinAnglEnalbe);
    MYLIBRARY_API int RunVC1(cv::Mat& src, InVCParam& inParam);
    MYLIBRARY_API int RunVC2(cv::Mat& src, InVCParam& inParam);
    MYLIBRARY_API int RunVC3(cv::Mat& src, InVCParam& inParam);
    MYLIBRARY_API int RunVC4(cv::Mat& src, InVCParam& inParam);

    // 结果获取
    //捺印  正负极期望mark图（第一个模型输出的crop）
    MYLIBRARY_API void GetStampMaskExpect(cv::Mat& _expectmask, int al_core);
    //捺印  第一个模型输出图
    MYLIBRARY_API void GetStampSegmentImg(cv::Mat& expectmask, int al_core);
    //捺印  第二个模型输出图
    MYLIBRARY_API void GetStampDetectImg(cv::Mat& expectmask, int al_core);
    //捺印  第三个模型输入图
    MYLIBRARY_API void GetStampCropimage(cv::Mat& cropimage, int al_core);
    //捺印  第三个模型输出图
    MYLIBRARY_API void GetStampCropSegmentImg(cv::Mat& expectmask, int al_core);
    MYLIBRARY_API void ResultOutStamp(cv::Mat& dstImg, OutStampResParam& OutResParam, int al_core);

    //座板  第一个模型输出图
    MYLIBRARY_API void GetSegmentPlateImg(cv::Mat& expectmask, int al_core);
    //座板  第二个模型输出图
    MYLIBRARY_API void GetDetectorPlateImg(cv::Mat& expectmask, int al_core);
    MYLIBRARY_API void ResultOutPlate(cv::Mat& dstImg, OutPlateResParam& OutResParam, int al_core);

    //翘脚  第一个模型输出图
    MYLIBRARY_API void GetLiftDetect1Img(cv::Mat& expectmask, int al_core);
    MYLIBRARY_API void ResultOutLift(cv::Mat& dstImg, OutLiftResParam& OutResParam, int al_core);


    //拱脚背光  二值化后的图片
    MYLIBRARY_API void GethresholdImgAbut(cv::Mat& binaryImg);
    MYLIBRARY_API void ResultOutAbut(cv::Mat& dstImg, OutAbutResParam& OutResParam);

    //拱脚正光
    MYLIBRARY_API void ResultOutYYGJ(cv::Mat& dstImg);

    //外观1
    MYLIBRARY_API void ResultOutVC1(cv::Mat& dstImg);

    //外观2
    MYLIBRARY_API void ResultOutVC2(cv::Mat& dstImg);

    //外观3
    MYLIBRARY_API void ResultOutVC3(cv::Mat& dstImg);

    //外观4
    MYLIBRARY_API void ResultOutVC4(cv::Mat& dstImg);

    // 释放资源
    MYLIBRARY_API void ReleaseStamp(int al_core);
    MYLIBRARY_API void ReleasePlate(int al_core);
    MYLIBRARY_API void ReleaseLift(int al_core);
    MYLIBRARY_API void ReleaseAbut();
    MYLIBRARY_API void ReleaseYYGJ();
}