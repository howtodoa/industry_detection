#pragma once
#include <vector>

struct InSideParam
{
	bool al_core = 0;                //学习使能
	float imgAngleNum = 0;               //图像旋转角度
	float charAngleNum = 90;               //字符旋转角度
	int buttonThresh = 120;                //编带下层二值化
	int topThresh = 190;                   //编带上层二值化
	bool isColor = false;                  //套管颜色是否判定，true为判定
};

struct OutSideParam
{
	std::stringstream m_VolumeStr, m_VoltageStr;//容积和电压的字符信息
	bool m_VolumeOK = false, m_VoltageOK = false;               //容积和电压型号匹配
	bool m_PinOK = false;                                       //针脚状态
	bool m_BreakOK = false;                                     //是否破损
	bool m_CaseColor = false;                                   //套管颜色类型
	bool m_CaseOK;                                              //套管状态
	float m_PinDis = false;                                     //两根针的距离
	float m_PinWidth;                                           //针的宽度
	float m_CaseHeigh;                                          //套管宽度
	float m_Case2PinDis;                                        //套筒与针的间距
	float m_CaseYSite;                                          //套管Y轴坐标差
	float m_TapeDis;                                            //胶带间隔
	float m_Case2TapeDis;                                       //套管到胶带间距
};


struct InTopParam
{
	float angleNum;               //旋转角度
	bool al_core;                //学习使能
	bool isExproofExist;          //防爆阀是否存在，true是存在，就要检测
};

struct OutTopParam
{
	bool m_FBFClassOKTop = false;                 //防爆阀类型
	float m_NegAngleTop = -999.0;                 //负极相较水平角度
	float m_CirNumTop = -999.0;                   //顶面防爆阀圆度
	float m_RadiusTop = -999.0;                   //顶面防爆阀半径
};

struct InPinParam
{
	float angleNum = 0;                               //旋转角度
	float pinWidth = 0;                               //针脚宽度
	float Diameter = 0;                               //定位孔直径
	int thresh = 0;                                   //胶带二值化参数
	int PinThresh = 0;                               //胶带上针参数
	bool al_core=false;
};

struct OutPinParam
{
	float m_MaoCiArea;             //毛刺存在
	float m_DisCircle;          //定位圆间距
	float m_DisTopPin;          //上针间距
	float m_DisButtomPin;       //上下针间距
	float m_DisPinWid;          //针最大宽度
	float m_totalArea;             //胶带缺陷总面积
	float m_DisTapeWid;            //胶带宽度
};



