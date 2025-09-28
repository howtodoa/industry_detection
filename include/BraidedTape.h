#pragma once
#include <opencv2/core.hpp> 
#include "BraidedTapeParam.h"

#ifndef MYLIBRARY_EXPORTS
#define MYLIBRARY_API __declspec(dllimport)
#else
#define MYLIBRARY_API __declspec(dllexport)
#endif


namespace BraidedTapeSpace {
    
        // 预处理
    MYLIBRARY_API cv::Mat RotateImg(cv::Mat image, double angle);

        // 初始化
    MYLIBRARY_API int InitializeSide();
    MYLIBRARY_API int InitializeTop();
    MYLIBRARY_API int InitializePin();

        // 执行函数
    MYLIBRARY_API int RunSide( cv::Mat& SrcImg, InSideParam& inSideParam);
    MYLIBRARY_API int RunTop(cv::Mat& src, InTopParam& inSideParam);
    MYLIBRARY_API int RunPin(cv::Mat& src, InPinParam& inPinParam);


        //侧面  定位模型输出图
    MYLIBRARY_API void GetLocatImgSide(cv::Mat& expectmask);
    MYLIBRARY_API void ResultOutSide(cv::Mat& dstImg, OutSideParam& OutResParam);

        //顶部  检测模型输出图
    MYLIBRARY_API void GetTopDetectionImg(cv::Mat& expectmask);
        //顶部  分割模型输出图
    MYLIBRARY_API void GetTopSegmentImg(cv::Mat& expectmask);
    MYLIBRARY_API void ResultOutTop(cv::Mat& dstImg, OutTopParam& OutResParam);

    MYLIBRARY_API void ResultOutPin(cv::Mat& dstImg, OutPinParam& outPinParam);

        // 释放资源
    MYLIBRARY_API void ReleaseSide(int al_core);
    MYLIBRARY_API void ReleaseTop();
    MYLIBRARY_API void ReleasePin();
}