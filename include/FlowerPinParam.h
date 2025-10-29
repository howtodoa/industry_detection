#pragma once
#include <vector>

struct InFlowerPinParam
{
	bool al_core = 0;                      //学习模式  true：学习  false：正常
	float imgAngleNum = 0;                 //旋转角度
	int FoilThresh = 180;                  //裂箔检测二值化参数
	int Mode = 0;                          //花瓣的检测模式：0面积 1整体对角 2单个花瓣对角
	int flowerNum = 0;                     //花瓣个数
};

struct OutFlowerPinResParam
{
	int flowerNum = 0;                   //花瓣个数
	std::vector<float> flowerArea;       //单朵花瓣的面积
	std::vector<float> flowetLength;     //单朵花瓣的对角线长度
	std::vector<float> allFlowerLength;  //单朵花的对角线长度
	float areaFoil;                              //箔裂面积
	float disFlw2L;                              //花到L2距离
	float disL2Pin;                              //L2到针距离
	float disFlw2Pin;                            //花到针距离
	float disPinAngle;                              //针的角度
	float disL2Heigh;                            //L2的高度
};

struct InLookPinParam
{
	bool al_core = 0;                      //学习模式  true：学习  false：正常
	float imgAngleNum = 0;                 //旋转角度
};

struct OutLookPinResParam
{

};