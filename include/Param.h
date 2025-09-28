#pragma once
#include <vector>

struct InStampParam
{
	float angleNum = 0;               //旋转角度
	int textkernl = 3;                 //字符核大小
	int logoid = 3;                        //负极log标签
	bool textextralenabel = false;             //字符超出部分是否检测
	bool textMissPixEnable = false;            //字符缺失部分是否检测
	bool isRect = false;                       //负极区域是否是矩形
	bool isOCR = true;                         //字符是否检测
};

struct ScoreVector
{
	std::vector<float> scoresPosLimit;         //正极区域分数
    std::vector<float> scoresNegLimit;         //负极区域分数
};


struct OutStampResParam
{
	float phi;                          //旋转角度
	float posWidth;                     //正极宽度
	float posErr;                       //正极缺陷面积
	float negWidth;                     //负极宽度
	float negErr;                       //负极缺陷面积
	int textNum;                      //字符个数
	std::vector<float> textScores;    //各个字符的得分
	float logoScores;                 //logo得分
};

struct OutPlateResParam
{
	bool m_PlatExist;                                          //底座是否存在
	float m_PlateArea;                                         //底座面积
	float m_PlateHeight, m_PlateWid;                           //底座高度、宽度
	float m_PlateHypUpLen, m_PlateHypDownLen;                  //底座上斜边长度，下斜边长度
	float m_PlateErrNum;                                       //底座瑕疵区域个数
	float m_PlateCrushNum;                                     //底座压伤区域个数
	bool m_PlatePinLeftExist, m_PlatePinRightExist;            //底座左引脚、右引脚是否存在
	float m_PlatePinLeftHeight, m_PlatePinRightHeight;         //底座左引脚、右引脚高度
	float m_PlatePinLeftWid, m_PlatePinRightWid;               //底座左引脚、右引脚宽度
	float m_PlatePinLeftAngle, m_PlatePinRightAngle;           //底座左引脚、右引脚角度
	float m_PlatePinLeftBendAngle, m_PlatePinRightBendAngle;   //底座左引脚、右引脚弯脚
	float m_PlatePinLeftParalAngle, m_PlatePinRightParalAngle; //底座左引脚、右引脚平行度
	float m_PlatePinLeftExceLen, m_PlatePinRightExceLen;       //左、右引脚超底座长度
	float m_PlatePinTotalLen;                                  //引脚总长度

};

struct OutLiftResParam
{
	float m_PinWidthLift;                     //引脚宽度
	float m_PinHeightLift;                    //引脚高度
	float m_disLift;                          //引脚到底座距离
	float m_PinAngle;                         //引脚角度
};

struct InAbutParam
{
	float angleNum = 0;                               //旋转角度
	float p_pin_range_min = 0.0;
	float p_pin_range_max = 0.3;
	float b_pln_range_min = 0.32;
	float b_pln_range_max = 0.68;
	float n_pin_range_min = 0.7;
	float n_pin_range_max = 1;
	float plateOffsetY = 130;                    //座板宽度上边平移高度
	float plateHeight = 100;                     //座板宽度检测高度
	float waistOffsetY = 200;                    //腰线检测上边平移高度
	float waistHeight = 50;                      //腰线检测宽度
	int gauss_ksize = 5;                         //高斯滤波参数
	int bin_thres_value = 60;                    //图像二值化参数
	float prod_Area_Min = 0.1;                   //基座有无面积参数
	float pin_Mc_height = 0;                     //引脚毛刺矩形高度
	float pln_Mc_height = 20;                    //基座毛刺矩形高度
};

struct OutAbutResParam
{
	bool isHaveProd = 0;        // 是否有料
	bool isHavePpin = 0;        // 是否有左引脚
	bool isHaveNpin = 0;        // 是否有右引脚
	bool isHaveBpln = 0;        // 是否有座板
	float Pin_C = 0;              // 引脚总长
	float shuyao_width = 0;      // 束腰宽度
	float plate_width = 0;       //底座宽度
	float p_pin_over_pln = 0;    // 左引脚超板
	float n_pin_over_pln = 0;    // 右引脚超板
	float p_pin_H = 0;           // 左引脚翘脚
	float n_pin_H = 0;           // 右引脚翘脚
	float p_n_height_diff = 0;   // 左右引脚高度差
	float p_pin_Angle = 0;       // 左引脚角度
	float n_pin_Angle = 0;       // 右引脚角度
	float p_pin_Mc = 0;          // 左引脚毛刺
	float n_pin_Mc = 0;          // 右引脚毛刺
	float b_pln_Mc = 0;         // 基座毛刺
};
