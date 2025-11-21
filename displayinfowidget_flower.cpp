#include "displayinfowidget_flower.h"
#include "displayinfowidget.h"
#include <QGridLayout>
#include <QLabel>
#include <QFont>


const QString NgStyle = ""; // 浅红背景，深红文字
const QString DefaultStyle = ""; // 默认样式

DisplayInfoWidget_Flower::DisplayInfoWidget_Flower(const AllUnifyParams& params, int flowernum, QWidget* parent)
    : DisplayInfoWidget(params, parent)
    , m_flowernum(flowernum)
{
    buildUIFromUnifyParams(params);
}

DisplayInfoWidget_Flower::~DisplayInfoWidget_Flower() = default;


void DisplayInfoWidget_Flower::buildUIFromUnifyParams(const AllUnifyParams& params)
{
    // 1. 清理旧 UI 和数据
    QLayoutItem* item;
    while ((item = m_gridLayout->takeAt(0)) != nullptr) {
        if (item->widget()) delete item->widget();
        delete item;
    }
    m_uiRows.clear();
    m_ngCounters.clear();

    // --- 提取特殊键的初始上下限 (保留此逻辑，即使隐藏) ---
    QString flowerInitialLowerStr = "--";
    QString flowerInitialUpperStr = "--";
    const QStringList flowerKeys = { "allFlowerLength", "flowetLength", "flowerArea" };

    for (auto it = params.constBegin(); it != params.constEnd(); ++it) {
        const QString& keyName = it.key();
        const UnifyParam& p = it.value();
        if (flowerKeys.contains(keyName) && p.check) {
            flowerInitialLowerStr = (p.lowerLimit == UNIFY_UNSET_VALUE) ? "--" : QString::number(p.lowerLimit, 'f', 2);
            flowerInitialUpperStr = (p.upperLimit == UNIFY_UNSET_VALUE) ? "--" : QString::number(p.upperLimit, 'f', 2);
        }
    }
    // ------------------------------------------

    // 2. 创建表头 (恢复 5 列)
    QStringList headers = { "参数名称", "下限值", "上限值", "实测值", "不符数" };
    QFont headerFont("微软雅黑", 12, QFont::Bold);
    for (int i = 0; i < headers.size(); ++i) {
        auto headerLabel = new QLabel(headers[i]);
        headerLabel->setFont(headerFont);
        headerLabel->setStyleSheet("background-color: #444; padding: 5px; border-radius: 3px; color: white;");
        headerLabel->setAlignment(Qt::AlignCenter);

        // 【关键修改 1】: 隐藏 "下限值" (i=1) 和 "上限值" (i=2) 的表头
        if (i == 1 || i == 2) {
            headerLabel->setVisible(false);
        }

        m_gridLayout->addWidget(headerLabel, 0, i);
    }

    QFont valueFont("Arial", 12, QFont::Bold);
    int currentRow = 1;

    // 3. 创建花瓣行 (N行)
    for (int i = 1; i <= m_flowernum; ++i) {
        QString baseName = QString("花瓣 %1").arg(i);

        // 创建 UI 控件 (恢复 5 个)
        auto nameLabel = new QLabel(baseName);
        auto lowerLimitLabel = new QLabel(flowerInitialLowerStr);
        auto upperLimitLabel = new QLabel(flowerInitialUpperStr);
        auto measuredValueLabel = new QLabel("-");
        auto ngCountLabel = new QLabel("0");

        // 【关键修改 2】: 隐藏上下限标签
        lowerLimitLabel->setVisible(false);
        upperLimitLabel->setVisible(false);

        // 设置样式
        nameLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        lowerLimitLabel->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
        upperLimitLabel->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
        measuredValueLabel->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
        ngCountLabel->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
        ngCountLabel->setStyleSheet("color: #DC3545;");

        // 加入布局 (恢复 5 列)
        m_gridLayout->addWidget(nameLabel, currentRow, 0);
        m_gridLayout->addWidget(lowerLimitLabel, currentRow, 1);
        m_gridLayout->addWidget(upperLimitLabel, currentRow, 2);
        m_gridLayout->addWidget(measuredValueLabel, currentRow, 3);
        m_gridLayout->addWidget(ngCountLabel, currentRow, 4);

        // 存储引用 (恢复 5 列，不再需要 nullptr)
        m_uiRows[baseName] = { nameLabel, lowerLimitLabel, upperLimitLabel, measuredValueLabel, ngCountLabel };
        m_ngCounters[baseName] = 0;
        currentRow++;
    }

    // 4. 创建所有 UnifyParam 行 (正常解析显示)，排除特殊键
    QList<UnifyParam> orderedParams = params.values();
    const QStringList ignoredKeys = { "allFlowerLength", "flowetLength", "flowerArea" };

    for (auto it = params.constBegin(); it != params.constEnd(); ++it) {
        const UnifyParam& p = it.value();

        if (ignoredKeys.contains(it.key())) continue;
        if (!p.visible || p.extraData.type() == qMetaTypeId<QList<double>>()) continue;

        const QString& baseName = p.label;

        // 格式化上下限 (保留逻辑)
        QString lowerStr = (p.lowerLimit == UNIFY_UNSET_VALUE) ? "--" : QString::number(p.lowerLimit, 'f', 2);
        QString upperStr = (p.upperLimit == UNIFY_UNSET_VALUE) ? "--" : QString::number(p.upperLimit, 'f', 2);

        // 创建 UI 控件 (恢复 5 个)
        auto nameLabel = new QLabel(baseName);
        auto lowerLimitLabel = new QLabel(lowerStr);
        auto upperLimitLabel = new QLabel(upperStr);
        auto measuredValueLabel = new QLabel("-");
        auto ngCountLabel = new QLabel(QString::number(p.ng_count));

        // 【关键修改 3】: 隐藏上下限标签
        lowerLimitLabel->setVisible(false);
        upperLimitLabel->setVisible(false);

        nameLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        lowerLimitLabel->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
        upperLimitLabel->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
        measuredValueLabel->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
        ngCountLabel->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
        ngCountLabel->setStyleSheet("color: #DC3545;");

        // 加入布局 (恢复 5 列)
        m_gridLayout->addWidget(nameLabel, currentRow, 0);
        m_gridLayout->addWidget(lowerLimitLabel, currentRow, 1);
        m_gridLayout->addWidget(upperLimitLabel, currentRow, 2);
        m_gridLayout->addWidget(measuredValueLabel, currentRow, 3);
        m_gridLayout->addWidget(ngCountLabel, currentRow, 4);

        // 存储引用 (恢复 5 列，不再需要 nullptr)
        m_uiRows[baseName] = { nameLabel, lowerLimitLabel, upperLimitLabel, measuredValueLabel, ngCountLabel };
        m_ngCounters[baseName] = p.ng_count;
        currentRow++;
    }

    // 5. 设置列伸展
    // 【关键修改 4】: 设置 5 列伸展，但将隐藏列 (1 和 2) 的权重设为 0
    m_gridLayout->setColumnStretch(0, 5); // 参数名称
    m_gridLayout->setColumnStretch(1, 0); // 下限值 (隐藏)
    m_gridLayout->setColumnStretch(2, 0); // 上限值 (隐藏)
    m_gridLayout->setColumnStretch(3, 4); // 实测值
    m_gridLayout->setColumnStretch(4, 2); // 不符数

    m_gridLayout->setColumnMinimumWidth(1, 0);
    m_gridLayout->setColumnMinimumWidth(2, 0);
}

void DisplayInfoWidget_Flower::updateLimitLabelsFromUnifyParams(const AllUnifyParams& params)
{
    // 【修改 11】: 移除整个函数逻辑，因为它只用于更新上下限
    // 由于基类中可能没有实现该函数，这里直接清空以避免编译错误和运行时逻辑
    Q_UNUSED(params);
    // 原函数内容全部移除
}

void DisplayInfoWidget_Flower::updateDataFromUnifyParams(const AllUnifyParams& params)
{
    // --- DEBUG 1: 打印函数入口和参数总数 ---
    qDebug() << "--- DEBUG: updateDataFromUnifyParams called. Total params received:" << params.size();

    const QStringList flowerKeys = { "allFlowerLength", "flowetLength", "flowerArea" };
    QVariantList flowerDataList;
    bool flowerDataFound = false;

    // 1. 遍历所有参数，提取特殊键的数据并更新普通参数
    for (auto it = params.constBegin(); it != params.constEnd(); ++it) {
        const QString& keyName = it.key();
        const UnifyParam& p = it.value();
        const QString& lookupKey = p.label;

        // A. 特殊花瓣键逻辑：提取 QList<double> 数据
        if (flowerKeys.contains(keyName)) {
            // --- DEBUG 2: 打印特殊键状态 ---
            qDebug().nospace() << "DEBUG: Found flower key '" << keyName << "'. Check=" << p.check
                << ", ExtraDataType=" << p.extraData.typeName();

            if (p.check) {
                if (p.extraData.type() == QVariant::List) {
                    flowerDataList = p.extraData.toList();
                    flowerDataFound = true;
                    // --- DEBUG 2.1: 打印成功提取列表 ---
                    qDebug() << "DEBUG: Successfully extracted QList data from" << keyName << ". List size:" << flowerDataList.size();
                }
                else {
                    qDebug() << "DEBUG: ExtraData type is NOT QVariant::List. Skipping list extraction.";
                }
            }
        }

        // B. 普通参数的更新逻辑 (恢复基类的颜色高亮)
        // 确保 UI 中存在该行且不是花瓣摘要行 (花瓣摘要行在下面统一处理)
        if (m_uiRows.contains(lookupKey) && (lookupKey == "花瓣个数" || !lookupKey.startsWith("花瓣 ")))
        {
            const auto& rowWidgets = m_uiRows.value(lookupKey);

            // --- DEBUG 3: 打印普通参数值 ---
            qDebug().nospace() << "DEBUG: Updating normal param '" << lookupKey << "'. Value=" << p.value
                << ", Result=" << p.result;

            // --- 1. 更新实测值 (value) ---
            QString valueStr = (p.value == UNIFY_UNSET_VALUE) ? "-" : QString::number(p.value, 'f', 2);
            rowWidgets.measuredValueLabel->setText(valueStr);

            // --- 2. 更新 NG 计数 (ng_count) ---
            m_ngCounters[lookupKey] = p.ng_count;
            rowWidgets.ngCountLabel->setText(QString::number(p.ng_count));

            // --- 3. 高亮显示 NG 结果 (result) ---
            bool isNg = (p.result == 0);

            // 清除之前的样式
            rowWidgets.nameLabel->setStyleSheet(DefaultStyle);
            // 【修改 12】: 删除上下限标签的样式操作 (清理)
            // rowWidgets.lowerLimitLabel->setStyleSheet(DefaultStyle); 
            // rowWidgets.upperLimitLabel->setStyleSheet(DefaultStyle);
            rowWidgets.measuredValueLabel->setStyleSheet(DefaultStyle);
            rowWidgets.ngCountLabel->setStyleSheet("color: #DC3545;"); // 保持 NG 计数标签的红色

            if (isNg) {
                // 将 NG 样式应用于该行的所有标签，并确保数值标签加粗
                rowWidgets.nameLabel->setStyleSheet(NgStyle);
                // 【修改 13】: 删除上下限标签的样式操作 (NG)
                // rowWidgets.lowerLimitLabel->setStyleSheet(NgStyle); 
                // rowWidgets.upperLimitLabel->setStyleSheet(NgStyle);
                rowWidgets.measuredValueLabel->setStyleSheet(NgStyle + "font-weight: bold;");
            }
        }
    }

    // 2. 将提取到的 QList 数据分配给花瓣摘要行 (只更新值，不进行颜色高亮)
    if (flowerDataFound) {
        // --- DEBUG 4: 开始分配花瓣数据 ---
        qDebug() << "DEBUG: Starting to assign flower data to" << m_flowernum << "flower rows.";

        for (int i = 1; i <= m_flowernum; ++i) {
            QString flowerKey = QString("花瓣 %1").arg(i);

            if (m_uiRows.contains(flowerKey)) {
                const auto& rowWidgets = m_uiRows.value(flowerKey);

                int startIndex = (i - 1) * 4;
                QStringList values;

                // 提取当前花瓣的 4 个值
                for (int j = 0; j < 4; ++j) {
                    int index = startIndex + j;
                    if (index < flowerDataList.size()) {
                        double val = flowerDataList.at(index).toDouble();
                        values.append(QString::number(val, 'f', 2));
                    }
                    else {
                        values.append("-");
                    }
                }

                // --- DEBUG 4.1: 打印分配的花瓣值 ---
                qDebug().nospace() << "DEBUG: Flower row '" << flowerKey << "' values: {" << values.join(", ") << "}";

                // 更新实测值标签 (此处保留逗号分隔，按上次需求)
                rowWidgets.measuredValueLabel->setText(values.join(", ")); // 已修正为 ", " 还是 ","，此处使用 ","

                // 确保花瓣行的样式是 DefaultStyle，不进行 NG 高亮
                rowWidgets.nameLabel->setStyleSheet(DefaultStyle);
                // 【修改 14】: 删除上下限标签的样式操作
                // rowWidgets.lowerLimitLabel->setStyleSheet(DefaultStyle); 
                // rowWidgets.upperLimitLabel->setStyleSheet(DefaultStyle);
                rowWidgets.measuredValueLabel->setStyleSheet(DefaultStyle);
            }
            else {
                qDebug() << "DEBUG: Flower row" << flowerKey << "NOT found in m_uiRows. Skipping.";
            }
        }
    }
    else {
        qDebug() << "DEBUG: flowerDataFound is FALSE. Skipping flower row value update.";
    }

    qDebug() << "--- DEBUG: updateDataFromUnifyParams finished. ---";
}