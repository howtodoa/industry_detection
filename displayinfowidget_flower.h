#ifndef DISPLAYINFOWIDGET_FLOWER_H
#define DISPLAYINFOWIDGET_FLOWER_H

#include "displayinfowidget.h"

class DisplayInfoWidget_Flower : public DisplayInfoWidget
{
    Q_OBJECT

public:
	//子类只能在自己的构造函数中调用重写的函数，无法在基类构造函数中调用子类的重写函数
    explicit DisplayInfoWidget_Flower(const AllUnifyParams& params,int flowernum,QWidget* parent = nullptr);
    ~DisplayInfoWidget_Flower() override;

private:
	// 重写 buildUIFromUnifyParams
    void buildUIFromUnifyParams(const AllUnifyParams& params) override;

    // 重写 updateLimitLabelsFromUnifyParams
    void updateLimitLabelsFromUnifyParams(const AllUnifyParams& params) override;

	int m_flowernum;
};

#endif // DISPLAYINFOWIDGET_FLOWER_H