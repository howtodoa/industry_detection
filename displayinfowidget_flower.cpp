#include "displayinfowidget_flower.h"

DisplayInfoWidget_Flower::DisplayInfoWidget_Flower(const AllUnifyParams& params, int flowernum, QWidget* parent)
    : DisplayInfoWidget(params, parent)
    , m_flowernum(flowernum)
{
}

DisplayInfoWidget_Flower::~DisplayInfoWidget_Flower() = default;

void DisplayInfoWidget_Flower::buildUIFromUnifyParams(const AllUnifyParams& params)
{
    // TODO: 在此实现 DisplayInfoWidget_Flower 特有的 UI 构建逻辑
}

void DisplayInfoWidget_Flower::updateLimitLabelsFromUnifyParams(const AllUnifyParams& params)
{
    // TODO: 在此实现 DisplayInfoWidget_Flower 特有的限值标签更新逻辑
}