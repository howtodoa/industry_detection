#pragma once
#include <iostream>
#include <vector>
#include <fstream>
#include <cstdio>
#include <queue>
#include <opencv2/opencv.hpp>
#include <openvino/openvino.hpp>
#include <chrono>
#include <emmintrin.h>
#include <corecrt_io.h>


#ifndef DLL_IMPORT
#define API __declspec(dllexport)
#else
#define API __declspec(dllimport)
#endif 


typedef bool (*InitSDK_Type)(int i);
typedef bool (*ov_core_getinformation_Type)(int index, int& channels, int& width, int& height, int& outputchannels, int& outputdims);
typedef bool(*ReleaseSDK_Type)();
typedef bool (*ov_core_init_Type)(const wchar_t* model_path, int& index, int& errcode);
typedef bool (*ov_core_Action_Type)(int Index, int TimeOut, void* input_data_ptr, const wchar_t* node_name, float* infer_result, long* timespan_cancel, int& errcode);
typedef bool (*ov_core_release_Type)(int index, int& errcode);


#define PI 3.1415f
#define ERCTANGLERANGLE 180

wchar_t* charToWchar(const char* str);

struct OutputMask
{
	int label = 0;
	cv::Mat mask;
};

struct BBox
{
	float x1;
	float y1;
	float x2;
	float y2;
	float score = -1;
	int label;
};

struct Instance
{
	float x1;
	float y1;
	float x2;
	float y2;
	float score = -1;
	int label;
	cv::Mat mask;
};

struct RBox
{
	float cx;
	float cy;
	float w;
	float h;
	float phi;
	int label;	
	float score = -1;
};

struct OutputPose
{
	cv::Rect_<float> box;
	int label = 0;
	float confidence = 0.0;
	std::vector<float> kpts;
};

class API OVSegment
{
public:
	bool ExternInitial(const char* modelfile, int threadnum);
	bool ExternInfer(cv::Mat& image, OutputMask& outputmask, int timeout = 60);
	void LoadOVModel(const char* xmlfile, bool quant = false);
	void Infer32(cv::Mat& image, OutputMask& outputmask, int timeout = 60);
	void Release();
	size_t m_input_h = 0;
	size_t m_input_w = 0;
	bool m_quant = false;

//private:
	int m_index;
	int m_errcode;

	cv::Mat CropImage(const cv::Mat& srcimg, cv::Size& size);
	ov::Core m_core;
	ov::CompiledModel m_compiled_model;
	std::shared_ptr<ov::Model> m_model;
	ov::InferRequest m_request;

	size_t m_num = 0;
	size_t m_cnum = 0;
	size_t m_dim = 0;
	ov::Tensor m_output;
};

class API OVPoseDetector
{
public:
	void Init();
	void LoadOVModel(const char* modelfile);
	void Infer(cv::Mat& image, std::vector<OutputPose>& outputkpts, float conf_threshold);
	void DrawKeyPoints(const cv::Mat src, const std::vector<OutputPose>& outputkpts, cv::Mat& dst, int thick);
	void ExpandKeyPoint(const cv::Mat& image, const std::vector<OutputPose>& kpts, std::vector<OutputPose>& expandkpts, float expand);
	size_t m_input_h = 0;
	size_t m_input_w = 0;

private:
	cv::Mat LetterBox(cv::Mat& srcimg, cv::Size size, cv::Vec4d& param, int& newh, int& neww, int& top, int& left);
	ov::Core m_core;
	ov::CompiledModel m_model;
	ov::InferRequest m_request;
	size_t m_num = 0;
	size_t m_cnum = 0;
	size_t m_dim = 0;
	ov::Tensor m_output;

	std::vector<std::vector<unsigned int>> KPS_COLORS;
	std::vector<std::vector<unsigned int>> SKELETON;
	std::vector<std::vector<unsigned int>> LIMB_COLORS;
};

class API OVYOLO
{
public:
	bool ExternInitial(const char* modelfile, int threadnum);
	void LoadOVModel(const char* modelfile, int device = -1, bool bin = true);
	//void Infer(cv::Mat& image, std::vector<BBox>& bbox, float conf_threshold = 0.6);

	void LetterBox(const cv::Mat& srcimg, cv::Mat& resizeimage, cv::Size size, int& newh, int& neww, int& top, int& left);
	void NMS(std::vector<BBox>& input_boxes, float nmsthreshold);
	std::vector<cv::Mat> CropImage(const cv::Mat& img, std::vector<BBox>bboxs);
	cv::Mat CropImage1(const cv::Mat& srcimg, cv::Size& size);
	ov::Core m_core;
	std::shared_ptr<ov::Model> m_model;
	ov::CompiledModel m_compiled_model;
	ov::InferRequest m_request;

//protected:
	int m_index;
	int m_errcode;
	size_t m_input_h = 0;
	size_t m_input_w = 0;
	size_t m_num = 0;
	size_t m_cnum = 0;
	size_t m_dim = 0;
	ov::Tensor m_output;
};

class API OVYOLOV5 : public  OVYOLO
{
public:
	void ExternInfer(cv::Mat& image, std::vector<BBox>& bbox, float& conf_threshold, float& nms_threshold, int timeout = 60);
	void Infer(cv::Mat& image, std::vector<BBox>& bbox, float& conf_threshold, float& nms_threshold, int timeout = 60);
};

class API OVYOLOV8 : public  OVYOLO
{
public:
	void ExternInfer(cv::Mat& image, std::vector<BBox>& bbox, float& conf_threshold, float& nms_threshold, int timeout = 60);
	void Infer(cv::Mat& image, std::vector<BBox>& bbox, float& conf_threshold, float& nms_threshold, int timeout = 60);
};

class API OVYOLOV8Obb : public  OVYOLO
{
public:
	void ExternInfer(cv::Mat& image, std::vector<RBox>& bbox, float& conf_threshold, float& nms_threshold, int timeout = 60);
	void Infer(cv::Mat& image, std::vector<RBox>& bbox, float& conf_threshold, float& nms_threshold, int timeout = 60);
};

class API OVYOLOV8Seg : public  OVYOLO
{
public:
	void ExternInfer(cv::Mat& image, std::vector<Instance>& bbox, float& conf_threshold, float& nms_threshold, int timeout = 60);
	void Infer(cv::Mat& image, std::vector<Instance>& bbox, float& conf_threshold, float& nms_threshold, int timeout = 60);
	void Infer1(cv::Mat& image, std::vector<Instance>& bbox, float& conf_threshold, float& nms_threshold, int timeout = 60);
	void ExternInfer1(cv::Mat& image, OutputMask& outputmask, const wchar_t* outputnode, int timeou = 60);
	void Infer32(cv::Mat& image, OutputMask& outputmask, int timeout = 60);
private:
	void sigmoid_function(float a, float b);
};

class API OVCRNN
{
public:
	void LoadOVModel(const char* onnxfile, const char* keyfile, cv::Size paddingsize);
	void Infer(const cv::Mat& src, std::string& text, float thresh = 0.6);

private:
	cv::Mat Padding(const cv::Mat& srcimg, cv::Size size);

	std::vector<float> SubstractMeanNormalize(cv::Mat& src, const float* meanVals, const float* normVals);

	cv::Mat paddingimage;
	std::vector<std::string> m_keys;
	ov::CompiledModel m_compiled_model;
	std::shared_ptr<ov::Model> m_model;
	ov::InferRequest m_request;

	const float meanValues[3] = { 127.5, 127.5, 127.5 };
	const float normValues[3] = { 1.0 / 127.5, 1.0 / 127.5, 1.0 / 127.5 };

	size_t m_input_h = 0;
	size_t m_input_w = 0;
	size_t m_num = 0;
	size_t m_cnum = 0;
	size_t m_dim = 0;
};

class API GROUNDINGDINO
{
public:
	void LoadOVModel(const char* onnxfile);
	void Infer(const cv::Mat& src, std::vector<BBox>& bboxs, float thresh = 0.6);

private:
	cv::Mat Normalize(cv::Mat& src, const float* meanVals, const float* normVals);

	ov::CompiledModel m_compiled_model;
	std::shared_ptr<ov::Model> m_model;
	ov::InferRequest m_request;

	const float meanValues[3] = { 0.406, 0.456, 0.485 };
	const float normValues[3] = { 0.225, 0.224, 0.229 };

	//const float meanValues[3] = { 127.5, 127.5, 127.5 };
	//const float normValues[3] = { 1.0 / 127.5, 1.0 / 127.5, 1.0 / 127.5 };

	size_t m_input_h = 0;
	size_t m_input_w = 0;
	size_t m_num = 0;
	size_t m_cnum = 0;
	size_t m_dim = 0;


};


