#pragma once
#include "common.h"
#include <thread>
#include <functional>
#include <vector>
//#include <nlohmann/json.hpp>
#include <direct.h>
#include <regex>
#include <cmath>

#ifndef MYLIBRARY_EXPORTS
#define MYLIBRARY_API __declspec(dllimport)
#else
#define MYLIBRARY_API __declspec(dllexport)
#endif

template<class T>
class Obj
{
private:
	static bool _instance;
public:

	static std::shared_ptr<T> GetInstance()
	{
		static std::shared_ptr<T> instance(new T);
		return instance;
	}
};

inline std::string GetProgramDir()
{
	char buf[1024] = "";
	std::string path = std::string();
	_getcwd(buf, sizeof(buf));
	std::regex reg("\\\\");
	path = regex_replace(buf, reg, "/");
	return path;

}

class MYLIBRARY_API WeldingDetector
{
public:
	WeldingDetector();
	bool Initialize(Params* params);
	void Report(std::function<void(ALLResult)> callback);
	void ProducerFunc();
	void ConsumerFunc(int id);
	cv::Mat resize_with_padding(const cv::Mat& input, int target_width, int target_height,
		const cv::Scalar& padding_color);
	cv::Mat resize_with_paddingPlus(const cv::Mat& input, int target_width, int target_height,
		const cv::Scalar& padding_color);
	bool getContourArea(int id, cv::Mat& image, ALLResult& result);
	//线扫相机相关
	void waveletDecomposition(const cv::Mat& src, std::vector<cv::Mat>& coeffs, int levels);
	void waveletReconstruction(const std::vector<cv::Mat>& coeffs, cv::Mat& dst);
	//捺印算法相关
	cv::Mat RotateImageKeepAll(const cv::Mat& image, double angle);
	//印花区域检测与拟合，印花图像裁剪，不同区域分割和分割线提取
	bool ProcessImage(cv::Mat& src, const cv::Mat& mask, cv::Mat& cropimage,
		cv::Mat& cropmask, cv::Mat& positivemask, cv::Mat& negativemask);
	bool Fitcircle_LeastSquares(const std::vector<cv::Point2f>& points, float& center_x, float& center_y, double& radius);
	bool CircleCrop(const cv::Mat& src, cv::Mat& dst, cv::Point center, int radius, int& offsetX, int& offsetY, int substride = 1, bool YellowFillUp = false);
	bool CutStampCircle(const cv::Mat& circlemask, int thred1, int thred2, cv::Mat& positivemask, cv::Mat& negativemask, cv::Mat& boundaryline);
	bool FitLine_Ransac(const std::vector<cv::Point2f>& points, cv::Vec4f& line, int iterations = 100, double sigma = 1., double k_min = -270., double k_max = 270.);
	cv::Vec4i GetLine(const cv::Mat& src, const cv::Vec4f& lineParams);
	//根据给定的边界线方向，对输入图像 (cropimage) 进行旋转校正
	bool RotateStampCrop(const cv::Mat& cropimage, cv::Mat& rotatecropimage, float& phi, const cv::Vec4i& boundary);
	int CaculateCols(const std::vector<BBox>& bboxs, int& meanwidth, int& meanheight);
	int CaculateRows(const std::vector<BBox>& bboxs, std::vector<cv::Rect>& rowRects);
	//对目标检测框（BBox）进行多级排序，先垂直方向（列）分组，再对每列内的检测框按水平方向（行）排序，最终输出一个结构化的检测框列表
	void SortBBoxs(const std::vector<BBox>& bboxs, std::vector<BBox>& sortbboxs, std::vector<std::vector<BBox>>& sortcolsbboxs, int cols);
	//绘制轮廓函数
	void DrawContoursOnImage(const cv::Mat& showimg, const std::vector<cv::Point>& contour, cv::Point2i Original_start, const cv::Scalar& color);

	std::queue<std::pair<cv::Mat, int>> m_Images;
	std::queue<std::pair<cv::Mat, int>> m_ImageTeams;
	std::vector<std::thread> m_Consumers;
	std::thread m_Producer;
	Params m_params;
	InThresholdParam InParam{};

	int m_PY_x, m_PY_y;//电容位置抠图偏移坐标

public:
	void Process(int id, const cv::Mat& src, ALLResult& result);
	void SegandObjDetec(ALLResult& result);

private:
	std::mutex mtx;
	std::condition_variable cvr;
	std::thread m_ConsumerThread;
	std::thread m_ProductorThread;
	std::queue<ALLResult> m_Result;
	ALLResult result;
	bool m_initalized;
};

