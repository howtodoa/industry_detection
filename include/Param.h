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

struct OutStampResParam
{
	float phi = 0.0f;                          //旋转角度
	float posWidth = 0.0f;                     //正极宽度
	float posErr = 0.0f;                       //正极缺陷面积
	float negWidth = 0.0f;                     //负极宽度
	float negErr = 0.0f;                       //负极缺陷面积
	int textNum = 0;                      //字符个数
	std::vector<float> textScores;    //各个字符的得分
	float logoScores = 0.0f;                 //logo得分
};

struct OutPlateResParam
{
	bool m_PlatExist = false;                                          //底座是否存在
	float m_PlateArea = 0.0f;                                         //底座面积
	float m_PlateHeight = 0.0f, m_PlateWid = 0.0f;                           //底座高度、宽度
	float m_PlateHypUpLen = 0.0f, m_PlateHypDownLen = 0.0f;                  //底座上斜边长度，下斜边长度
	float m_PlateErrNum = 0.0f;                                       //底座瑕疵区域个数
	float m_PlateCrushNum = 0.0f;                                     //底座压伤区域个数
	bool m_PlatePinLeftExist = false, m_PlatePinRightExist = false;            //底座左引脚、右引脚是否存在
	float m_PlatePinLeftHeight = 0.0f, m_PlatePinRightHeight = 0.0f;         //底座左引脚、右引脚高度
	float m_PlatePinLeftWid = 0.0f, m_PlatePinRightWid = 0.0f;               //底座左引脚、右引脚宽度
	float m_PlatePinLeftAngle = 0.0f, m_PlatePinRightAngle = 0.0f;           //底座左引脚、右引脚角度
	float m_PlatePinLeftBendAngle = 0.0f, m_PlatePinRightBendAngle = 0.0f;   //底座左引脚、右引脚弯脚
	float m_PlatePinLeftParalAngle = 0.0f, m_PlatePinRightParalAngle = 0.0f; //底座左引脚、右引脚平行度
	float m_PlatePinLeftExceLen = 0.0f, m_PlatePinRightExceLen = 0.0f;       //左、右引脚超底座长度
	float m_PlatePinTotalLen = 0.0f;                                  //引脚总长度

};

struct OutLiftResParam
{
	float m_PinWidthLift = 0.0f;                     //引脚宽度
	float m_PinHeightLift = 0.0f;                    //引脚高度
	float m_disLift = 0.0f;                          //引脚到底座距离
	float m_PinAngle = 0.0f;                         //引脚角度
};

struct InAbutParam
{
	float angleNum = 0;                               //旋转角度
	float p_pin_range_min = 0.0f;
	float p_pin_range_max = 0.3f;
	float b_pln_range_min = 0.32f;
	float b_pln_range_max = 0.68f;
	float n_pin_range_min = 0.7f;
	float n_pin_range_max = 1.0f;                   //底边分段比例
	float plateOffsetY = 130.0f;                    //座板宽度上边平移高度
	float plateHeight = 100.0f;                     //座板宽度检测高度
	float waistOffsetY = 200.0f;                    //腰线检测上边平移高度
	float waistHeight = 50.0f;                      //腰线检测宽度
	int gauss_ksize = 5;                         //高斯滤波参数
	int bin_thres_value = 60;                    //图像二值化参数
	float prod_Area_Min = 0.1f;                   //基座有无面积参数
	float pin_Mc_height = 0.0f;                     //引脚毛刺矩形高度
	float pln_Mc_height = 20.0f;                    //基座毛刺矩形高度
};

struct OutAbutResParam
{
	bool isHaveProd = false;        // 是否有料
	bool isHavePpin = false;        // 是否有左引脚
	bool isHaveNpin = false;        // 是否有右引脚
	bool isHaveBpln = false;        // 是否有座板
	float Pin_C = 0.0f;              // 引脚总长
	float shuyao_width = 0.0f;      // 束腰宽度
	float plate_width = 0.0f;       //底座宽度
	float p_pin_over_pln = 0.0f;    // 左引脚超板
	float n_pin_over_pln = 0.0f;    // 右引脚超板
	float p_pin_H = 0.0f;           // 左引脚翘脚
	float n_pin_H = 0.0f;           // 右引脚翘脚
	float p_n_height_diff = 0.0f;   // 左右引脚高度差
	float p_pin_Angle = 0.0f;       // 左引脚角度
	float n_pin_Angle = 0.0f;       // 右引脚角度
	float p_pin_Mc = 0.0f;          // 左引脚毛刺
	float n_pin_Mc = 0.0f;          // 右引脚毛刺
	float b_pln_Mc = 0.0f;         // 基座毛刺
};

struct InVCParam
{
	float angleNum = 0;                               //旋转角度
	
};

struct OutVCResParam
{
	
};