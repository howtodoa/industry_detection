#pragma once
#include <opencv2/core.hpp> 
#include "FlowerPinParam.h"

#ifndef MYLIBRARY_EXPORTS
#define MYLIBRARY_API __declspec(dllimport)
#else
#define MYLIBRARY_API __declspec(dllexport)
#endif


namespace ExportFlowerSpace {

    // 预处理
    MYLIBRARY_API cv::Mat RotateImg(cv::Mat image, double angle);
    MYLIBRARY_API cv::Mat RotateImageKeepAll(cv::Mat image, double angle);
    //正极
    // 初始化
    MYLIBRARY_API int InitializePosFlowerPin();
    // 执行函数
    //判断有无料
    MYLIBRARY_API int RunPosFlowerPin(cv::Mat& SrcImg, InFlowerPinParam& inSideParam);
    //有料处理函数（0正常，1NG，2花有问题
    MYLIBRARY_API int RunPosFlowerPinDeal(cv::Mat& SrcImg, InFlowerPinParam& inSideParam);
    //红胶带处理函数(0正常，1有红胶带)
    MYLIBRARY_API int RunPosTape(cv::Mat& SrcImg);
    MYLIBRARY_API void GetPosFoilImg(cv::Mat& expectmask);

    MYLIBRARY_API void ResultOutPosFlowerPin(cv::Mat& dstImg, OutFlowerPinResParam& OutResParam);
    // 释放资源
    MYLIBRARY_API void ReleasePosFlowerPin();

    //负极
    MYLIBRARY_API int InitializeNegFlowerPin();
    // 执行函数
    //判断有无料
    MYLIBRARY_API int RunNegFlowerPin(cv::Mat& SrcImg, InFlowerPinParam& inSideParam);
    //有料处理函数（0正常，1NG，2花有问题
    MYLIBRARY_API int RunNegFlowerPinDeal(cv::Mat& SrcImg, InFlowerPinParam& inSideParam);
    //红胶带处理函数(0正常，1有红胶带)
    MYLIBRARY_API int RunNegTape(cv::Mat& SrcImg);
    MYLIBRARY_API void GetNegFoilImg(cv::Mat& expectmask);

    MYLIBRARY_API void ResultOutNegFlowerPin(cv::Mat& dstImg, OutFlowerPinResParam& OutResParam);
    // 释放资源
    MYLIBRARY_API void ReleaseNegFlowerPin();

    //外观
    MYLIBRARY_API int InitializeLookFlowerPin();
    // 执行函数
    MYLIBRARY_API int RunLookFlowerPin(cv::Mat& SrcImg, InLookPinParam& inSideParam);
    MYLIBRARY_API int RunLookFlowerPinDeal(cv::Mat& SrcImg, InLookPinParam& inSideParam);
    MYLIBRARY_API void ResultOutLookFlowerPin(cv::Mat& dstImg, OutLookPinResParam& OutResParam);
    // 释放资源
    MYLIBRARY_API void ReleaseLookFlowerPin();
}