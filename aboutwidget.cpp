#include "aboutwidget.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPixmap>

AboutWidget::AboutWidget(QWidget* parent)
    : QWidget{ parent }
{
    this->setFixedSize(600, 300); // 固定窗口大小

    // Logo
    QLabel* logoLabel = new QLabel(this);
    QPixmap logoPixmap("../../../resources/images/oo.ico");
    logoLabel->setPixmap(logoPixmap.scaled(128, 128, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    logoLabel->resize(128, 128);
    logoLabel->move(10, 10);  // 左上角

    // 标题
    QLabel* titleLabel = new QLabel("长沙铭准信息科技有限公司——智能视觉解决方案引领者", this);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #333333;");
    titleLabel->setWordWrap(true);
    titleLabel->setGeometry(150, 10, 430, 60);  // 在 logo 右侧

    // 正文
    QLabel* contentLabel = new QLabel(
        "长沙铭准信息科技有限公司成立于2016年，总部位于湖南省长沙市，是一家聚焦AI机器视觉技术研发与应用的高新技术企业、专精特新企业。"
        "公司以\"相机替代人眼，AI构建识别大脑\"为核心理念，深度融合光学、机械、电子与人工智能技术，"
        "为智能制造领域提供全流程智能化升级解决方案，助力企业实现高效、精准的生产革新。",
        this
    );
    contentLabel->setStyleSheet("font-size: 14px; color: #444444;");
    contentLabel->setWordWrap(true);
    contentLabel->setGeometry(150, 30, 430, 180);
    // 电话号码（右下角）
    QLabel* phoneLabel = new QLabel("联系电话：073184483983", this);
    phoneLabel->setStyleSheet("font-size: 14px; color: #444444;");

    // 计算右下角位置
    int phoneLabelWidth = phoneLabel->fontMetrics().horizontalAdvance("联系电话：073184483983") + 25; // 加 10px 边距
    int phoneLabelHeight = phoneLabel->fontMetrics().height();
    int rightMargin = 20;  // 距离右边距 20px
    int bottomMargin = 20; // 距离下边距 20px

    // 设置位置（右下角）
    phoneLabel->setGeometry(
        this->width() - phoneLabelWidth - rightMargin,  // x 坐标
        this->height() - phoneLabelHeight - bottomMargin, // y 坐标
        phoneLabelWidth,
        phoneLabelHeight
    );
}