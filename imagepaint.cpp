#include "imagepaint.h"
#include <QPainter>
#include <QDebug>
#include <QFontMetrics>
#include "typdef.h"

void ImagePaint::prepareImageForDrawing(QImage& src)
{
    // 不创建 dst，直接检查 src
    if (src.format() != QImage::Format_RGB32 && src.format() != QImage::Format_ARGB32) {
        // 直接将转换后的结果赋值回 src
        src = src.convertToFormat(QImage::Format_RGB32);
    }
}

void ImagePaint::drawResultOnPixmap(QPixmap& sourcePixmap, int result)

{
    // 检查原始图片是否有效
    if (sourcePixmap.isNull()) {
        qDebug() << "错误：传入的 QPixmap 是空的，无法绘制结果。";
        return;
    }

    // 创建一个 QPainter 对象，直接在传入的 QPixmap 上进行绘制
    QPainter painter(&sourcePixmap);

    // 启用抗锯齿以获得更平滑的文本边缘
    painter.setRenderHint(QPainter::Antialiasing);

    // 1. 根据结果值准备绘制文本和颜色
    QString text;
    QColor color;

    if (result == 0) {
        text = "OK";
        color = Qt::green;
    } else if (result == -1) {
        text = "NG";
        color = Qt::red;
    } else {
        // 对于其他值，不绘制任何内容，直接返回
        return;
    }

    // 2. 设置字体
    QFont font;

    font.setWeight(QFont::Light); // 或者 QFont::Thin
    // 根据图片高度动态调整字体大小
    font.setPixelSize(static_cast<int>(sourcePixmap.height() * 0.08));
    painter.setFont(font);
    painter.setPen(color); // 设置画笔颜色

    // 3. 计算文本绘制位置
    // 创建一个QFontMetrics对象来获取文本尺寸
    QFontMetrics fm(font);
    int textWidth = fm.horizontalAdvance(text);
    int textHeight = fm.height();

    // 在右下角绘制文本，并留出一些边距
    int margin = 20;
    int x = sourcePixmap.width() - textWidth - margin;
    int y = sourcePixmap.height() - textHeight - margin;


    // 4. 绘制文本
    painter.setPen(color); // 重新设置文本颜色
    painter.drawText(x, y + textHeight, text);
}

void ImagePaint::drawDetectionResult(cv::Mat& image, const DetectInfo& info)
{
    if (image.empty()) return;

    int imgW = image.cols;
    int imgH = image.rows;

    // ---------- 字体设置 ----------
    // Cost Time 用字体（图高5%）
    int baseFontHeight = static_cast<int>(imgH * 0.05);
    double fontScaleNormal = baseFontHeight / 30.0;
    int thicknessNormal = std::max(1, baseFontHeight / 10);

    // OK/NG 用字体（图高7%）
    int resultFontHeight = static_cast<int>(imgH * 0.07);
    double fontScaleResult = resultFontHeight / 30.0;
    int thicknessResult = std::max(2, resultFontHeight / 9);

    int fontFace = cv::FONT_HERSHEY_SIMPLEX;
    int baseline = 0;

    // ---------- 左下角文本 ----------
    std::string timeText = "Cost Time: " + info.timeStr;
    cv::Size timeTextSize = cv::getTextSize(timeText, fontFace, fontScaleNormal, thicknessNormal, &baseline);
    cv::Point timeOrg(10, imgH - 10);  // 左下角

    cv::putText(image, timeText, timeOrg, fontFace, fontScaleNormal, cv::Scalar(0, 255, 0), thicknessNormal);

    // ---------- 右下角 OK / NG ----------
    std::string resultText = (info.ret == 0) ? "OK" : "NG";
    cv::Scalar resultColor = (info.ret == 0) ? cv::Scalar(0, 255, 0) : cv::Scalar(0, 0, 255);

    cv::Size resultTextSize = cv::getTextSize(resultText, fontFace, fontScaleResult, thicknessResult, &baseline);
    cv::Point resultOrg(imgW - resultTextSize.width - 10, imgH - 10);  // 右下角

    cv::putText(image, resultText, resultOrg, fontFace, fontScaleResult, resultColor, thicknessResult);
}

void ImagePaint::drawDetectionResultEx(cv::Mat& image, const DetectInfo& info)
{
    if (image.empty()) return;

    int imgW = image.cols;
    int imgH = image.rows;

    // ---------- 1. 字体和通用参数设置 ----------
    int baseFontHeight = static_cast<int>(imgH * 0.05);
    double fontScaleNormal = baseFontHeight / 30.0;
    int thicknessNormal = std::max(1, baseFontHeight / 10);

    int resultFontHeight = static_cast<int>(imgH * 0.07);
    double fontScaleResult = resultFontHeight / 30.0;
    int thicknessResult = std::max(2, resultFontHeight / 9);

    int fontFace = cv::FONT_HERSHEY_SIMPLEX;
    int baseline = 0;
    const int margin = 10; // 统一的边缘距离
    const int gap = 15;    // 文本之间的最小间隙

    // ---------- 2. 准备文本和计算尺寸 ----------
    // 左下角文本
    std::string timeText = "Cost Time: " + info.timeStr;
    cv::Size timeTextSize = cv::getTextSize(timeText, fontFace, fontScaleNormal, thicknessNormal, &baseline);

    // 右下角文本
    std::string resultText = (info.ret == 0) ? "OK" : "NG";
    cv::Scalar resultColor = (info.ret == 0) ? cv::Scalar(0, 255, 0) : cv::Scalar(0, 0, 255);
    cv::Size resultTextSize = cv::getTextSize(resultText, fontFace, fontScaleResult, thicknessResult, &baseline);

    // ---------- 3. 重叠判断与绘制 ----------
    // 计算默认情况下的位置
    cv::Point timeOrg_default(margin, imgH - margin);
    cv::Point resultOrg_default(imgW - resultTextSize.width - margin, imgH - margin);

    // 判断左边文本的右边缘是否会与右边文本的左边缘重叠（加上一个间隙）
    if ((timeOrg_default.x + timeTextSize.width + gap) > resultOrg_default.x) {
        // *会重叠，启用堆叠模式 (全部靠右绘制)

        // "OK/NG" 在下
        cv::Point resultOrg_stacked(imgW - resultTextSize.width - margin, imgH - margin);
        cv::putText(image, resultText, resultOrg_stacked, fontFace, fontScaleResult, resultColor, thicknessResult);

        // "Cost Time" 在上
        // Y坐标 = "OK/NG"的Y坐标 - "OK/NG"文本的高度 - 垂直间隙
        int timeY_stacked = resultOrg_stacked.y - resultTextSize.height - 5;
        cv::Point timeOrg_stacked(imgW - timeTextSize.width - margin, timeY_stacked);
        cv::putText(image, timeText, timeOrg_stacked, fontFace, fontScaleNormal, cv::Scalar(0, 255, 0), thicknessNormal);

    }
    else {
        // 不会重叠，使用默认的左右角模式
        cv::putText(image, timeText, timeOrg_default, fontFace, fontScaleNormal, cv::Scalar(0, 255, 0), thicknessNormal);
        cv::putText(image, resultText, resultOrg_default, fontFace, fontScaleResult, resultColor, thicknessResult);
    }
}


void ImagePaint::drawResultOnPixmap(QPixmap& sourcePixmap, int result,
    const QVector<PaintDataItem>& paintDataList,
    QSize displaySize) {
    // 1. 参数校验和默认值处理 
    if (sourcePixmap.isNull()) {
        qDebug() << "错误：传入的 sourcePixmap 是空的，无法绘制结果。";
        return;
    }

    if (displaySize.isEmpty() || displaySize.width() <= 0 || displaySize.height() <= 0) {
        qWarning() << "警告：传入的 displaySize 无效或为空 (" << displaySize << ")。将使用默认尺寸 480x480 进行绘制。";
        displaySize = QSize(480, 480);
    }

    // 2. 创建绘图目标 
    QPixmap targetPixmap(displaySize);
    targetPixmap.fill(Qt::black);

    QPainter painter(&targetPixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    // 3. 将原始 sourcePixmap 绘制为居中背景 
    QPixmap scaledSourcePixmap = sourcePixmap.scaled(displaySize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    int bgX = (displaySize.width() - scaledSourcePixmap.width()) / 2;
    int bgY = (displaySize.height() - scaledSourcePixmap.height()) / 2;
    painter.drawPixmap(bgX, bgY, scaledSourcePixmap);

    // 定义布局比例常量，而不是固定像素值
    //    所有尺寸都将根据 displaySize 动态计算，这些比例是调整视觉效果的核心

    // --- 可调整的核心比例参数 ---
    const double TEXT_HEIGHT_RATIO = 0.04;     // 普通文本高度占 displaySize 高度的比例 (例如: 4%)
    const double OK_NG_HEIGHT_RATIO = 0.06;    // OK/NG 标记高度占 displaySize 高度的比例 (例如: 12%)
    const double X_MARGIN_RATIO = 0.05;        // 左右边距占 displaySize 宽度的比例
    const double Y_MARGIN_RATIO = 0.05;        // 顶部边距占 displaySize 高度的比例
    const double ROW_SPACING_RATIO = 0.01;     // 额外行间距占 displaySize 高度的比例
    const double COLUMN_WIDTH_RATIO = 0.45;    // 每列宽度占 displaySize 宽度的比例 (允许两列并留有空隙)
    const double OK_NG_MARGIN_RATIO = 0.015;    // OK/NG 标记边距占 displaySize 较小边的比例

    // --- 动态计算实际像素值 ---
    // 添加 qMax 来保证在 displaySize 极小的情况下，字体不会小于一个可读的最小值 (如12px)
    const int textPixelSize = qMax(12, static_cast<int>(displaySize.height() * TEXT_HEIGHT_RATIO));
    const int okNgPixelSize = qMax(24, static_cast<int>(displaySize.height() * OK_NG_HEIGHT_RATIO));
    const int xMargin = static_cast<int>(displaySize.width() * X_MARGIN_RATIO);
    const int yMargin = static_cast<int>(displaySize.height() * Y_MARGIN_RATIO);
    const int rowSpacingExtra = static_cast<int>(displaySize.height() * ROW_SPACING_RATIO);
    const int columnWidth = static_cast<int>(displaySize.width() * COLUMN_WIDTH_RATIO);
    const int okNgMargin = static_cast<int>(qMin(displaySize.width(), displaySize.height()) * OK_NG_MARGIN_RATIO);


    // 5. 设置普通文本字体
    QFont font;
    font.setWeight(QFont::Medium);
    font.setPixelSize(textPixelSize); // 使用动态计算的像素大小
    painter.setFont(font);

    // 6. 计算实际行距
    QFontMetrics fmText(font);
    int currentRowSpacing = fmText.height() + rowSpacingExtra; // 字体实际高度 + 额外间距

    // 7. 计算每列可以容纳的最大行数
    // 预留底部空间给 OK/NG 标记
    QFont tempBigFontForHeight;
    tempBigFontForHeight.setPixelSize(okNgPixelSize);
    QFontMetrics fmBigTemp(tempBigFontForHeight);
    int bottomReservedHeight = fmBigTemp.height() + okNgMargin;

    int availableHeightForText = displaySize.height() - yMargin - bottomReservedHeight;
    int maxRowsPerCol = (currentRowSpacing > 0) ? (availableHeightForText / currentRowSpacing) : 1;
    if (maxRowsPerCol <= 0) {
        maxRowsPerCol = 1;
        qWarning() << "警告: 可用绘制高度不足，每列最大行数设为1。";
    }

    // 8. 遍历并绘制每个检测项文本 (逻辑与原版类似，但使用动态计算的边距和列宽)
    int drawIndex = 0;
    for (const auto& item : paintDataList) {
        if (!item.check)
            continue;

        QColor color = (item.result == 1) ? Qt::green : Qt::blue;
        painter.setPen(color);

        int col = drawIndex / maxRowsPerCol;
        int row = drawIndex % maxRowsPerCol;

        int x = xMargin + col * columnWidth;
        int y = yMargin + row * currentRowSpacing + fmText.ascent();

        QString text = QString("%1: %2").arg(item.label, item.value);
        painter.drawText(x, y, text);

        ++drawIndex;
    }

    // 9. 绘制右下角 OK / NG 标记 (使用动态计算的字体大小和边距)
    QString totalText = (result == 0) ? "OK" : (result == -1 ? "NG" : "");
    if (!totalText.isEmpty()) {
#ifdef USE_MAIN_WINDOW_CAPACITY
        QColor totalColor = (result == 0) ? Qt::green : Qt::blue;
#else
        QColor totalColor = (result == 0) ? Qt::green : Qt::red;
#endif

        QFont bigFont;
        bigFont.setWeight(QFont::Bold);
        bigFont.setPixelSize(okNgPixelSize); // 使用动态计算的像素大小
        painter.setFont(bigFont);
        painter.setPen(totalColor);

        QFontMetrics fmBig(bigFont);
        int textWidth = fmBig.horizontalAdvance(totalText);

        // 使用动态计算的边距
        int x = displaySize.width() - textWidth - okNgMargin;
        int y = displaySize.height() - okNgMargin; // y 坐标定位在基线位置

        painter.drawText(x, y, totalText);
    }

    // 10. 将绘制好的 targetPixmap 赋值回 sourcePixmap (与原版相同)
    sourcePixmap = targetPixmap;
}


void ImagePaint::drawPaintData(QPixmap& sourcePixmap,
    QVector<PaintDataItem>& paintDataList,
    QSize displaySize)
{
    // 1. 安全检查
    if (sourcePixmap.isNull()) {
        qWarning() << "传入的 pixmap 为空，无法绘制。";
        LOG_DEBUG(GlobalLog::logger, _T("m_pParaDock ptr null"));
        
        return;
    }

    // 2. 创建一个与原图等大的高质量画布，避免直接操作原QPixmap可能带来的格式问题。
    //    这一步是保证画质无损的关键之一。
QImage safeImage = sourcePixmap.toImage().convertToFormat(QImage::Format_ARGB32_Premultiplied);
QPixmap targetPixmap = QPixmap::fromImage(safeImage);
QPainter painter(&targetPixmap);
painter.setRenderHint(QPainter::Antialiasing);

// 3. 如果传入的UI控件尺寸无效，则将其视为与原图等大（此时无缩放）。
if (displaySize.isEmpty() || !displaySize.isValid()) {
    displaySize = sourcePixmap.size();
}

// 4. 计算原图在保持宽高比的情况下，真实占满UI控件后的“实际”显示尺寸。
//    这是解决因宽高比不同导致缩放比例不一致问题的关键。
QSize actualOnScreenSize = sourcePixmap.size().scaled(displaySize, Qt::KeepAspectRatio);

const QSize canvasSize = targetPixmap.size(); // 画布尺寸即为原图尺寸

// 5. 定义视觉设计参数
constexpr int TARGET_VISUAL_FONT_SIZE = 12;   // 期望文字最终在屏幕上呈现的像素大小
constexpr double ROW_SPACING_VISUAL_PX = 4.0; // 期望行间距最终在屏幕上呈现的像素大小

// 6. 定义布局比例（相对于画布尺寸）
constexpr double X_MARGIN_RATIO = 0.00;
constexpr double Y_MARGIN_RATIO = 0.05;
constexpr double FIRST_COLUMN_WIDTH_RATIO = 0.50;
constexpr double FIRST_COL_HEIGHT_RATIO = 0.70;
constexpr double COLUMN_GAP_RATIO = 0.0001;

// 7. 根据“实际”显示尺寸，计算出字体/元素在高清画布上应有的像素大小
if (actualOnScreenSize.height() == 0) return; // 防止除零错误

double fontHeightRatio = static_cast<double>(TARGET_VISUAL_FONT_SIZE) / actualOnScreenSize.height();
int fontSizeOnCanvas = qRound(canvasSize.height() * fontHeightRatio);
if (fontSizeOnCanvas < 1) return; // 字体过小，没有绘制意义

double rowSpacingRatio = ROW_SPACING_VISUAL_PX / actualOnScreenSize.height();
int rowSpacingOnCanvas = qRound(canvasSize.height() * rowSpacingRatio);

// 8. 设置字体
QFont font;
font.setWeight(QFont::Medium);
font.setPixelSize(fontSizeOnCanvas);
painter.setFont(font);

// 9. 计算布局参数
QFontMetrics fm(font);
int lineHeightOnCanvas = fm.height() + rowSpacingOnCanvas;
if (lineHeightOnCanvas <= 0) lineHeightOnCanvas = 1;

int xMarginOnCanvas = qRound(canvasSize.width() * X_MARGIN_RATIO);
int yMarginOnCanvas = qRound(canvasSize.height() * Y_MARGIN_RATIO);
int firstColWidthOnCanvas = qRound(canvasSize.width() * FIRST_COLUMN_WIDTH_RATIO);
int columnGapOnCanvas = qRound(canvasSize.width() * COLUMN_GAP_RATIO);

int maxRowsFirstCol = static_cast<int>((canvasSize.height() * FIRST_COL_HEIGHT_RATIO - yMarginOnCanvas) / lineHeightOnCanvas);
if (maxRowsFirstCol <= 0) maxRowsFirstCol = 1;
int maxRowsSecondCol = (canvasSize.height() - yMarginOnCanvas) / lineHeightOnCanvas;
if (maxRowsSecondCol <= 0) maxRowsSecondCol = 1;

// 10. 循环绘制所有数据项
int drawIndex = 0;
for (const auto& item : paintDataList) {
    if (!item.check)
        continue;

    QColor color = (item.result == 1) ? Qt::green : Qt::blue;
    painter.setPen(color);

    int x, y;
    QString text = QString("%1: %2").arg(item.label, item.value);

    if (drawIndex < maxRowsFirstCol) {
        // 第一列
        x = xMarginOnCanvas;
        y = yMarginOnCanvas + drawIndex * lineHeightOnCanvas;
        painter.drawText(QRect(x, y, firstColWidthOnCanvas, lineHeightOnCanvas), Qt::AlignLeft | Qt::AlignVCenter, text);
    }
    else {
        // 第二列
        int secondColIndex = drawIndex - maxRowsFirstCol;
        if (secondColIndex >= maxRowsSecondCol) {
            break;
        }
        x = xMarginOnCanvas + firstColWidthOnCanvas + columnGapOnCanvas;
        y = yMarginOnCanvas + secondColIndex * lineHeightOnCanvas;
        int secondColWidthOnCanvas = canvasSize.width() - x - xMarginOnCanvas;
        if (secondColWidthOnCanvas > 0) {
            painter.drawText(QRect(x, y, secondColWidthOnCanvas, lineHeightOnCanvas), Qt::AlignLeft | Qt::AlignVCenter, text);
        }
    }
    ++drawIndex;
}

// 11. 用绘制完成的高质量画布，替换掉原始的sourcePixmap
sourcePixmap = targetPixmap;
}

void ImagePaint::drawPaintDataEx_QImage(QImage& image, QVector<PaintDataItem> paintDataList, QSize displaySize)
{
    if (image.isNull() || paintDataList.isEmpty()) return;

    prepareImageForDrawing(image);

    QPainter painter(&image);
    if (!painter.isActive()) return;

    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);

    if (displaySize.isEmpty() || !displaySize.isValid()) {
        displaySize = image.size();
    }

    QSize actualOnScreenSize = image.size().scaled(displaySize, Qt::KeepAspectRatio);
    if (actualOnScreenSize.height() <= 0) return; // 防呆：避免除零

    const QSize canvasSize = image.size();
    constexpr int TARGET_VISUAL_FONT_SIZE = 16;

    // 计算字体在画布上的实际像素大小
    double fontHeightRatio = static_cast<double>(TARGET_VISUAL_FONT_SIZE) / actualOnScreenSize.height();
    int fontSizeOnCanvas = std::max(1, qRound(canvasSize.height() * fontHeightRatio));

    QFont font;
    font.setWeight(QFont::Medium);
    font.setPixelSize(fontSizeOnCanvas);
    painter.setFont(font);

    for (const auto& item : paintDataList) {
        if (!item.check) continue;

        // 核心逻辑：只寻找并绘制第一个 NG (result != 1) 的项
        if (item.result != 1) {
#ifdef USE_MAIN_WINDOW_CAPACITY
            QColor color = Qt::blue;
#else
            QColor color = Qt::red;
#endif
            painter.setPen(color);

            QString text = QString("%1: %2").arg(item.label, item.value);

            // 布局计算
            int xMarginOnCanvas = qRound(canvasSize.width() * 0.01);
            int yMarginOnCanvas = qRound(canvasSize.height() * 0.05);

            // 绘制区域：限制在画布内，防止坐标溢出
            QRect drawRect(xMarginOnCanvas,
                yMarginOnCanvas,
                canvasSize.width() - 2 * xMarginOnCanvas,
                canvasSize.height() - 2 * yMarginOnCanvas);

            painter.drawText(drawRect, Qt::AlignLeft | Qt::AlignTop, text);

            return; // 找到并绘制第一个 NG 后立即退出
        }
    }
}

void ImagePaint::drawPaintDataEx(QPixmap& pixmap,
    QVector<PaintDataItem> paintDataList,
    QSize displaySize)
{
    // 1. 安全检查
    if (pixmap.isNull()) {
        qWarning() << "传入的 pixmap 为空，无法绘制。";
        LOG_DEBUG(GlobalLog::logger, _T("m_pParaDock ptr null"));
        return;
    }

    // 2. 直接在源 Pixmap 上创建 QPainter 进行绘制
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    // 3. 如果传入的UI控件尺寸无效，则将其视为与原图等大（此时无缩放）。
    if (displaySize.isEmpty() || !displaySize.isValid()) {
        displaySize = pixmap.size();
    }

    // 4. 计算原图在保持宽高比的情况下，真实占满UI控件后的“实际”显示尺寸。
    QSize actualOnScreenSize = pixmap.size().scaled(displaySize, Qt::KeepAspectRatio);
    const QSize canvasSize = pixmap.size();

    // 5. 定义视觉设计参数
    constexpr int TARGET_VISUAL_FONT_SIZE = 12;
    constexpr double ROW_SPACING_VISUAL_PX = 4.0;
    constexpr double X_MARGIN_RATIO = 0.01;
    constexpr double Y_MARGIN_RATIO = 0.05;

    // 6. 根据“实际”显示尺寸，计算出字体/元素在高清画布上应有的像素大小
    if (actualOnScreenSize.height() == 0) return;

    double fontHeightRatio = static_cast<double>(TARGET_VISUAL_FONT_SIZE) / actualOnScreenSize.height();
    int fontSizeOnCanvas = qRound(canvasSize.height() * fontHeightRatio);
    if (fontSizeOnCanvas < 1) return;

    double rowSpacingRatio = ROW_SPACING_VISUAL_PX / actualOnScreenSize.height();
    int rowSpacingOnCanvas = qRound(canvasSize.height() * rowSpacingRatio);

    // 7. 设置字体
    QFont font;
    font.setWeight(QFont::Medium);
    font.setPixelSize(fontSizeOnCanvas);
    painter.setFont(font);

    // 8. **移除预计算，直接开始循环**
    //    这里的逻辑是：只寻找并绘制第一个满足条件的项
    for (const auto& item : paintDataList) {
        // 确保项是有效的
        if (!item.check) {
            continue;
        }

        // --- 核心改动：寻找第一个 "NG" (result != 1) 的项目 ---
        if (item.result != 1) {
            // 设置画笔颜色（NG项）
#ifdef USE_MAIN_WINDOW_CAPACITY
            QColor color = Qt::blue;
#else
            QColor color = Qt::red;
#endif
            painter.setPen(color);

            // 构建要绘制的文本
            QString text = QString("%1: %2").arg(item.label, item.value);

            // 计算绘制位置
            int xMarginOnCanvas = qRound(canvasSize.width() * X_MARGIN_RATIO);
            int yMarginOnCanvas = qRound(canvasSize.height() * Y_MARGIN_RATIO);

            int x = xMarginOnCanvas;
            int y = yMarginOnCanvas;

            // 绘制文本
            painter.drawText(QRect(x, y, canvasSize.width() - 2 * xMarginOnCanvas, canvasSize.height() - 2 * yMarginOnCanvas),
                Qt::AlignLeft | Qt::AlignTop, text);

            // 找到并绘制第一个后，立即退出循环
            return;
        }
    }

    // 如果循环结束都没有找到NG项，则什么都不绘制
}

void ImagePaint::drawPaintDataEx_V(QPixmap& pixmap,
    QVector<PaintDataItem> paintDataList,
    QSize displaySize)
{
    // 1. 安全检查，与原函数相同
    if (pixmap.isNull()) {
        qWarning() << "传入的 pixmap 为空，无法绘制。";
        LOG_DEBUG(GlobalLog::logger, _T("m_pParaDock ptr null"));
        return;
    }

    // 2. 直接在源 Pixmap 上创建 QPainter 进行绘制，与原函数相同
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    // 3. 尺寸处理，与原函数相同
    if (displaySize.isEmpty() || !displaySize.isValid()) {
        displaySize = pixmap.size();
    }

    // 4. 计算缩放比例，与原函数相同
    QSize actualOnScreenSize = pixmap.size().scaled(displaySize, Qt::KeepAspectRatio);
    const QSize canvasSize = pixmap.size();

    // 5. 定义视觉设计参数，与原函数相同
    constexpr int TARGET_VISUAL_FONT_SIZE = 20;
    constexpr double ROW_SPACING_VISUAL_PX = 4.0;
    constexpr double X_MARGIN_RATIO = 0.01;
    constexpr double Y_MARGIN_RATIO = 0.05;
    constexpr double COLUMN_GAP_RATIO = 0.02;

    // 6. 字体尺寸计算，与原函数相同
    if (actualOnScreenSize.height() == 0) return;

    double fontHeightRatio = static_cast<double>(TARGET_VISUAL_FONT_SIZE) / actualOnScreenSize.height();
    int fontSizeOnCanvas = qRound(canvasSize.height() * fontHeightRatio);
    if (fontSizeOnCanvas < 1) return;

    double rowSpacingRatio = ROW_SPACING_VISUAL_PX / actualOnScreenSize.height();
    int rowSpacingOnCanvas = qRound(canvasSize.height() * rowSpacingRatio);

    // 7. 设置字体，与原函数相同
    QFont font;
    font.setWeight(QFont::DemiBold);
    font.setPixelSize(fontSizeOnCanvas);
    painter.setFont(font);

    // 8. **核心改动：预计算需要绘制的总数（使用新的兼容过滤逻辑）**
    int itemsToDraw = 0;
    for (const auto& item : paintDataList) {
        if (!item.check) {
            continue;
        }

        // 新的兼容过滤逻辑：只绘制包含“容积”或“电压”字符的项目
        if (item.label.contains("容积字符信息") || item.label.contains("电压字符信息")) {
            itemsToDraw++;
        }
    }

    if (itemsToDraw == 0) {
        return;
    }

    // 9. 动态计算布局参数，与原函数相同
    QFontMetrics fm(font);
    int lineHeightOnCanvas = fm.height() + rowSpacingOnCanvas;
    if (lineHeightOnCanvas <= 0) return;

    int xMarginOnCanvas = qRound(canvasSize.width() * X_MARGIN_RATIO);
    int yMarginOnCanvas = qRound(canvasSize.height() * Y_MARGIN_RATIO);

    int availableHeight = canvasSize.height() - 2 * yMarginOnCanvas;
    int maxRowsPerColumn = availableHeight / lineHeightOnCanvas;
    if (maxRowsPerColumn <= 0) return;

    int numColumns = 1;
    int rowsInFirstColumn = itemsToDraw;
    int columnWidth = canvasSize.width() - 2 * xMarginOnCanvas;
    int columnGapOnCanvas = 0;

    if (itemsToDraw > maxRowsPerColumn) {
        numColumns = 2;
        rowsInFirstColumn = static_cast<int>(std::ceil(static_cast<double>(itemsToDraw) / 2.0));
        columnGapOnCanvas = qRound(canvasSize.width() * COLUMN_GAP_RATIO);
        columnWidth = (canvasSize.width() - 2 * xMarginOnCanvas - columnGapOnCanvas) / 2;
    }

    if (columnWidth <= 0) return;

    // 10. **核心改动：循环绘制并修改显示的标签**
    int drawIndex = 0;
    for (const auto& item : paintDataList) {
        if (!item.check) {
            continue;
        }

        // 过滤逻辑 (必须与计数循环中的逻辑保持一致)
        if (!item.label.contains("容积字符信息") && !item.label.contains("电压字符信息")) {
            continue;
        }

        // 颜色判断，与原函数相同
#ifdef USE_MAIN_WINDOW_CAPACITY
        QColor color = (item.result == 1) ? Qt::green : Qt::blue;
#else
        QColor color = (item.result == 1) ? Qt::green : Qt::red;
#endif
        painter.setPen(color);

        // 新的标签显示逻辑：根据条件修改 text
        QString text = "";
        if (item.label.contains("容积字符信息")) {
            text = QString("容积: %1").arg(item.value.left(4));
        }
        else if (item.label.contains("电压字符信息")) {
            text = QString("电压: %1").arg(item.value.left(4));
        }

        // 确保 text 不为空，如果前面的 if/else if 都未匹配
        if (text.isEmpty()) {
            text = QString("%1: %2").arg(item.label, item.value);
        }

        int currentColumn, currentRow;

        if (drawIndex < rowsInFirstColumn) {
            currentColumn = 0;
            currentRow = drawIndex;
        }
        else {
            currentColumn = 1;
            currentRow = drawIndex - rowsInFirstColumn;
        }

        int x = xMarginOnCanvas + currentColumn * (columnWidth + columnGapOnCanvas);
        int y = yMarginOnCanvas + currentRow * lineHeightOnCanvas;

        painter.drawText(QRect(x, y, columnWidth, lineHeightOnCanvas), Qt::AlignLeft | Qt::AlignVCenter, text);

        ++drawIndex;
    }
}

void ImagePaint::drawPaintDataEx(QPixmap& pixmap,
    const AllUnifyParams unifyParams,
    QSize displaySize)
{
    // 1. 安全检查
    if (pixmap.isNull()) {
        qWarning() << "传入的 pixmap 为空，无法绘制。";
        return;
    }

    // 2. 直接在源 Pixmap 上创建 QPainter 进行绘制
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    // 3. 如果传入的UI控件尺寸无效，则将其视为与原图等大（此时无缩放）。
    if (displaySize.isEmpty() || !displaySize.isValid()) {
        displaySize = pixmap.size();
    }

    // 4. 计算缩放比率和画布尺寸
    // actualOnScreenSize 用于计算为了让人眼看着舒服，字体在原图上应该画多大
    QSize actualOnScreenSize = pixmap.size().scaled(displaySize, Qt::KeepAspectRatio);
    const QSize canvasSize = pixmap.size();

    // 5. 定义视觉设计参数
    int TARGET_VISUAL_FONT_SIZE = GlobalPara::FontSize;
    constexpr double ROW_SPACING_VISUAL_PX = 4.0;
    constexpr double X_MARGIN_RATIO = 0.01;
    constexpr double Y_MARGIN_RATIO = 0.05;

    // 6. 根据“实际”显示尺寸，计算出字体/元素在高清画布上应有的像素大小
    if (actualOnScreenSize.height() == 0) return;

    // 字体高度和行间距的画布尺寸计算
    double fontHeightRatio = static_cast<double>(TARGET_VISUAL_FONT_SIZE) / actualOnScreenSize.height();
    int fontSizeOnCanvas = qRound(canvasSize.height() * fontHeightRatio);
    if (fontSizeOnCanvas < 1) fontSizeOnCanvas = 1; // 防止字体过小

    double rowSpacingRatio = ROW_SPACING_VISUAL_PX / actualOnScreenSize.height();
    int rowSpacingOnCanvas = qRound(canvasSize.height() * rowSpacingRatio);

    // 每行文本占据的高度：字体大小 + 行间距
    int totalRowHeight = fontSizeOnCanvas + rowSpacingOnCanvas;

    // 7. 设置字体
    QFont font;
    font.setWeight(QFont::Medium);
    font.setPixelSize(fontSizeOnCanvas);
    painter.setFont(font);

    // 8. 绘制颜色和边距
    int xMarginOnCanvas = qRound(canvasSize.width() * X_MARGIN_RATIO);
    int yMarginOnCanvas = qRound(canvasSize.height() * Y_MARGIN_RATIO);

    // 默认画笔颜色
    QColor defaultColor = Qt::green;

    // --- 统计需要绘制的项 ---
    int totalItemsToDraw = 0;
    for (auto it = unifyParams.constBegin(); it != unifyParams.constEnd(); ++it)
    {
        const UnifyParam& item = it.value();
        if ((item.check && item.visible) || item.result != 1)
        {
            totalItemsToDraw++;
        }
    }

    // --- 计算布局参数 (优先填满第一列) ---

    // 图像可用于绘制文本的垂直高度
    int availableHeight = canvasSize.height() - 2 * yMarginOnCanvas;

    // 一列能容纳的最大行数 (物理极限)
    int maxRowsInOneColumn = availableHeight / totalRowHeight;
    if (maxRowsInOneColumn <= 0) maxRowsInOneColumn = 1; // 防止极小图片导致的除零

    // 默认是单列
    int numColumns = 1;

    // 【修改点】：每列最大项数直接等于屏幕物理能容纳的最大行数
    // 只有超过这个数，才会去第二列
    int maxItemsPerColumn = maxRowsInOneColumn;

    // 如果总数超过一列能容纳的最大行数，则开启第二列
    if (totalItemsToDraw > maxRowsInOneColumn) {
        numColumns = 2;
    }

    // 计算第二列的起始 X 坐标
    int columnWidth = canvasSize.width() / (numColumns > 0 ? numColumns : 1);
    int secondColumnStartX = columnWidth + xMarginOnCanvas;
    // 确保第二列的起始 X 坐标不会太靠左，至少是图像宽度的一半
    if (numColumns == 2) {
        secondColumnStartX = qMax(secondColumnStartX, canvasSize.width() / 2);
    }

    // 9. 初始化起始绘制位置和行计数器
    int currentRow = 0;
    int currentColumn = 1;
    int currentX = xMarginOnCanvas;

    // 10. **核心查找和绘制逻辑：遍历 Map 绘制所有满足条件的项**
    for (auto it = unifyParams.constBegin(); it != unifyParams.constEnd(); ++it)
    {
        const UnifyParam& item = it.value();

        // 核心条件：只有当 (item.check && item.visible) 或 item.result != 1 时才绘制
        if ((item.check && item.visible) || item.result != 1)
        {
            // --- 【修改点】：检查是否需要换列 ---
            // 逻辑：如果当前行号已经达到了单列最大容量，并且是双列模式，且当前还在第一列
            if (currentRow >= maxItemsPerColumn && numColumns == 2 && currentColumn == 1) {
                // 换到第二列
                currentRow = 0;             // 重置行号
                currentColumn = 2;          // 标记为第二列
                currentX = secondColumnStartX; // X坐标移到右侧
            }

            // 安全检查：如果第二列也满了，或者单列模式下满了，停止绘制
            if (currentRow >= maxItemsPerColumn) {
                break;
            }

            // 构建要绘制的文本
            QString formattedValue = QString::number(item.value, 'f', 3);
            QString resultText = (item.result != 1) ? "" : ""; // 保持原逻辑
            QString text = QString("%1: %2 %3").arg(item.label, formattedValue, resultText);

            // 检查 NG 项并设置颜色
            if (item.result != 1) {
                painter.setPen(Qt::red);
            }
            else {
                painter.setPen(defaultColor);
            }

            // 计算当前行的垂直位置 (y 轴)
            int y = yMarginOnCanvas + currentRow * totalRowHeight;

            // 绘制文本
            // 注意：drawText 的 y 坐标通常是基线，所以加上 fontSizeOnCanvas
            painter.drawText(currentX, y + fontSizeOnCanvas, text);

            // 移动到下一行
            currentRow++;
        }
    }
}
void ImagePaint::drawPaintDataEx_flower(QPixmap& pixmap,
    const AllUnifyParams unifyParams,
    QSize displaySize)
{
    // ... (1-7 步：初始化和设置字体，保持不变) ...

    // 1. 安全检查
    if (pixmap.isNull()) {
        qWarning() << "传入的 pixmap 为空，无法绘制。";
        return;
    }

    // 2. 直接在源 Pixmap 上创建 QPainter 进行绘制
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    // 3. 如果传入的UI控件尺寸无效，则将其视为与原图等大（此时无缩放）。
    if (displaySize.isEmpty() || !displaySize.isValid()) {
        displaySize = pixmap.size();
    }

    // 4. 计算缩放比率和画布尺寸
    QSize actualOnScreenSize = pixmap.size().scaled(displaySize, Qt::KeepAspectRatio);
    const QSize canvasSize = pixmap.size();

    // 5. 定义视觉设计参数
    constexpr int TARGET_VISUAL_FONT_SIZE = 16;
    constexpr double ROW_SPACING_VISUAL_PX = 4.0;
    constexpr double X_MARGIN_RATIO = 0.01;
    constexpr double Y_MARGIN_RATIO = 0.05;

    // **新增/修改参数：每行显示的最大元素个数 (固定为 4 个)**
    constexpr int MAX_ELEMENTS_PER_LINE = 4;

    // 6. 根据“实际”显示尺寸，计算出字体/元素在高清画布上应有的像素大小
    if (actualOnScreenSize.height() == 0) return;

    double fontHeightRatio = static_cast<double>(TARGET_VISUAL_FONT_SIZE) / actualOnScreenSize.height();
    int fontSizeOnCanvas = qRound(canvasSize.height() * fontHeightRatio);
    if (fontSizeOnCanvas < 1) return;

    double rowSpacingRatio = ROW_SPACING_VISUAL_PX / actualOnScreenSize.height();
    int rowSpacingOnCanvas = qRound(canvasSize.height() * rowSpacingRatio);

    int totalRowHeight = fontSizeOnCanvas + rowSpacingOnCanvas;

    // 7. 设置字体
    QFont font;
    font.setWeight(QFont::Medium);
    font.setPixelSize(fontSizeOnCanvas);
    painter.setFont(font);

    // 8. 绘制颜色和边距
    int xMarginOnCanvas = qRound(canvasSize.width() * X_MARGIN_RATIO);
    int yMarginOnCanvas = qRound(canvasSize.height() * Y_MARGIN_RATIO);

    QColor defaultColor = Qt::green;

    // --- 统计需要绘制的项 (修改统计逻辑) ---
    int totalItemsToDraw = 0;
    for (auto it = unifyParams.constBegin(); it != unifyParams.constEnd(); ++it)
    {
        const UnifyParam& item = it.value();
        if ((item.check && item.visible) || item.result != 1)
        {
            if (item.extraData.isValid() && item.extraData.type() == QVariant::List) {
                int listSize = item.extraData.toList().size();
                // 数组项：标签占 1 行 + 数据行 (按 MAX_ELEMENTS_PER_LINE 分组)
                int dataLines = (listSize + MAX_ELEMENTS_PER_LINE - 1) / MAX_ELEMENTS_PER_LINE;
                totalItemsToDraw += (1 + dataLines);
            }
            else {
                totalItemsToDraw++; // 单值项只占 1 行
            }
        }
    }

    // --- 计算布局参数 (单/双列) ---
    int availableHeight = canvasSize.height() - 2 * yMarginOnCanvas;
    int maxRowsInOneColumn = availableHeight / totalRowHeight;
    int maxItemsPerColumn = totalItemsToDraw;
    int numColumns = 1;

    if (totalItemsToDraw > maxRowsInOneColumn && maxRowsInOneColumn > 0) {
        numColumns = 2;
        maxItemsPerColumn = (totalItemsToDraw + 1) / 2;
    }

    int columnWidth = canvasSize.width() / numColumns;
    int secondColumnStartX = columnWidth + xMarginOnCanvas;
    if (numColumns == 2) {
        secondColumnStartX = qMax(secondColumnStartX, canvasSize.width() / 2);
    }

    // 9. 初始化起始绘制位置和行计数器
    int currentRow = 0;
    int currentColumn = 1;
    int currentX = xMarginOnCanvas;

    // 10. **核心查找和绘制逻辑：遍历 Map 绘制所有满足条件的项**
    for (auto it = unifyParams.constBegin(); it != unifyParams.constEnd(); ++it)
    {
        const UnifyParam& item = it.value();

        // 核心条件：只有当 (item.check && item.visible) 或 item.result != 1 时才绘制
        if ((item.check && item.visible) || item.result != 1)
        {
            // 检查 NG 项并设置颜色
            QColor drawColor = (item.result != 1) ? Qt::red : defaultColor;
            painter.setPen(drawColor);

            // -------------------------------------------------------------
            // 【关键修改：值格式化和多行绘制逻辑】
            // -------------------------------------------------------------
            if (item.extraData.isValid() && item.extraData.type() == QVariant::List) {

                // --- 数组类型：标签和每个元素组占一行 ---
                QVariantList list = item.extraData.toList();
                int listSize = list.size();

                // 1. 绘制标签行
                QString labelText = QString("%1:").arg(item.label);

                // 换列逻辑检查 (标签行)
                if (currentRow >= maxItemsPerColumn && numColumns == 2) {
                    currentRow = 0;
                    currentColumn = 2;
                    currentX = secondColumnStartX;
                }
                if (currentColumn > 2) {
                    goto END_OF_DRAWING_LOOP;
                }

                int y_label = yMarginOnCanvas + currentRow * totalRowHeight;
                painter.drawText(currentX, y_label + fontSizeOnCanvas, labelText);
                currentRow++; // 标签占一行

                // 2. 绘制分组元素行
                int currentElementIndex = 0;
                int partNumber = 1;

                while (currentElementIndex < listSize) {
                    // 换列逻辑检查 (数据行)
                    if (currentRow >= maxItemsPerColumn && numColumns == 2) {
                        currentRow = 0;
                        currentColumn = 2;
                        currentX = secondColumnStartX;
                    }
                    if (currentColumn > 2) {
                        goto END_OF_DRAWING_LOOP;
                    }

                    // 获取当前行的元素
                    QStringList strList;
                    int elementsInThisLine = qMin(MAX_ELEMENTS_PER_LINE, listSize - currentElementIndex);

                    for (int i = 0; i < elementsInThisLine; ++i) {
                        // 格式化当前值 (保留三位小数)
                        strList.append(QString::number(list.at(currentElementIndex + i).toDouble(), 'f', 3));
                    }

                    // 文本格式：  花瓣N: 值1, 值2, 值3, 值4
                    QString lineText = QString("  花瓣%1: %2").arg(partNumber).arg(strList.join(", "));

                    int y_data = yMarginOnCanvas + currentRow * totalRowHeight;

                    // 绘制当前行
                    painter.drawText(currentX, y_data + fontSizeOnCanvas, lineText);

                    // 更新计数器
                    currentElementIndex += elementsInThisLine; // 移动到下一个分组的起始点
                    partNumber++;
                    currentRow++;

                    // 检查是否超出图像边界
                    if (y_data + totalRowHeight > canvasSize.height() - yMarginOnCanvas) {
                        if (numColumns == 1 || currentColumn == 2) {
                            goto END_OF_DRAWING_LOOP;
                        }
                    }
                }
            }
            else {
                // --- 单值类型：只占一行 (保持不变) ---
                QString formattedValue = QString::number(item.value, 'f', 2);
                QString resultText = (item.result != 1) ? "" : "";

                // 完整文本格式：标签: 实测值 [结果]
                QString text = QString("%1: %2 %3").arg(item.label, formattedValue, resultText);

                // 换列逻辑检查
                if (currentRow >= maxItemsPerColumn && numColumns == 2) {
                    currentRow = 0;
                    currentColumn = 2;
                    currentX = secondColumnStartX;
                }
                if (currentColumn > 2) {
                    break;
                }

                int y = yMarginOnCanvas + currentRow * totalRowHeight;

                // 绘制文本
                painter.drawText(currentX, y + fontSizeOnCanvas, text);

                // 移动到下一行
                currentRow++;

                // 检查是否超出图像边界
                if (y + totalRowHeight > canvasSize.height() - yMarginOnCanvas) {
                    if (numColumns == 1 || currentColumn == 2) {
                        break;
                    }
                }
            }
        }
    }

END_OF_DRAWING_LOOP:; // 用于跳出多层循环

    // 绘制结束
}


void ImagePaint::drawPaintDataEx_VI(QPixmap& pixmap,
    QVector<PaintDataItem> paintDataList,
    QSize displaySize)
{
    // 1. 安全检查，与原函数相同
    if (pixmap.isNull()) {
        qWarning() << "传入的 pixmap 为空，无法绘制。";
        // 假设 GlobalLog 和 _T 宏已定义
        // LOG_DEBUG(GlobalLog::logger, _T("m_pParaDock ptr null"));
        return;
    }
    if(paintDataList.isEmpty()) {
        qWarning() << "传入的 paintDataList 为空，无法绘制。";
        return;
	}
    // 2. 直接在源 Pixmap 上创建 QPainter 进行绘制，与原函数相同
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    // 3. 尺寸处理，与原函数相同
    if (displaySize.isEmpty() || !displaySize.isValid()) {
        displaySize = pixmap.size();
    }

    // 4. 计算缩放比例，与原函数相同
    QSize actualOnScreenSize = pixmap.size().scaled(displaySize, Qt::KeepAspectRatio);
    const QSize canvasSize = pixmap.size();

    // 5. 定义视觉设计参数，与原函数相同
    constexpr int TARGET_VISUAL_FONT_SIZE = 12;
    constexpr double ROW_SPACING_VISUAL_PX = 4.0;
    constexpr double X_MARGIN_RATIO = 0.01;
    constexpr double Y_MARGIN_RATIO = 0.05;
    // 原函数中 COLUMN_GAP_RATIO 仅在多列时使用，这里保持定义但可能不会用到
    // constexpr double COLUMN_GAP_RATIO = 0.02;

    // 6. 字体尺寸计算，与原函数相同
    if (actualOnScreenSize.height() == 0) return;

    double fontHeightRatio = static_cast<double>(TARGET_VISUAL_FONT_SIZE) / actualOnScreenSize.height();
    int fontSizeOnCanvas = qRound(canvasSize.height() * fontHeightRatio);
    if (fontSizeOnCanvas < 1) return;

    double rowSpacingRatio = ROW_SPACING_VISUAL_PX / actualOnScreenSize.height();
    int rowSpacingOnCanvas = qRound(canvasSize.height() * rowSpacingRatio);

    // 7. 设置字体，与原函数相同
    QFont font;
    font.setWeight(QFont::DemiBold);
    font.setPixelSize(fontSizeOnCanvas);
    painter.setFont(font);

    // 8. **核心改动：预计算需要绘制的总数（只筛选“引线总长度”）**
    int itemsToDraw = 0;
    for (const auto& item : paintDataList) {
        if (!item.check) {
            continue;
        }

        // 新的过滤逻辑：只绘制包含“引线总长度”字符的项目
        if (item.label.contains("引线总长度")) {
            itemsToDraw++;
        }
    }

    // 注意：通常引线总长度只有一个，但为了兼容性，我们仍然使用循环计数。
    if (itemsToDraw == 0) {
        return;
    }

    // 9. 动态计算布局参数，简化为单列布局
    QFontMetrics fm(font);
    int lineHeightOnCanvas = fm.height() + rowSpacingOnCanvas;
    if (lineHeightOnCanvas <= 0) return;

    int xMarginOnCanvas = qRound(canvasSize.width() * X_MARGIN_RATIO);
    int yMarginOnCanvas = qRound(canvasSize.height() * Y_MARGIN_RATIO);

    // 因为只绘制一个（或少数几个）项目，我们总是使用单列布局
    int currentColumn = 0; // 总是第0列
    int currentRow = 0;    // 总是第0行（对于第一个找到的项目）

    // 单列宽度计算
    int columnWidth = canvasSize.width() - 2 * xMarginOnCanvas;
    int columnGapOnCanvas = 0; // 单列时没有列间距

    if (columnWidth <= 0) return;

    // 10. **核心改动：循环绘制并修改显示的标签**
    int drawIndex = 0; // 实际上只会绘制一个
    for (const auto& item : paintDataList) {
        if (!item.check) {
            continue;
        }

        // 过滤逻辑 (必须与计数循环中的逻辑保持一致)
        if (!item.label.contains("引线总长度")) {
            continue;
        }

        // 颜色判断，与原函数中颜色逻辑相同，假设 result = 1 表示合格（绿色）
#ifdef USE_MAIN_WINDOW_CAPACITY
        QColor color = (item.result == 1) ? Qt::green : Qt::blue;
#else
        QColor color = (item.result == 1) ? Qt::green : Qt::red;
#endif
        painter.setPen(color);

        // 新的标签显示逻辑：固定显示为“引线总长度: [值]”
        QString text = QString("引线总长度: %1").arg(item.value);

        // 布局计算：因为只绘制一个，所以 drawIndex 总是 0
        int x = xMarginOnCanvas + currentColumn * (columnWidth + columnGapOnCanvas);
        int y = yMarginOnCanvas + currentRow * lineHeightOnCanvas;

        painter.drawText(QRect(x, y, columnWidth, lineHeightOnCanvas), Qt::AlignLeft | Qt::AlignVCenter, text);

        ++drawIndex;
        // 如果我们只需要绘制找到的第一个“引线总长度”，可以在这里 break
        // break; 
    }
}

void ImagePaint::drawDetectionResultExQt(QPixmap& pixmap, const DetectInfo& info)
{
    if (pixmap.isNull()) return;


    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    int imgW = pixmap.width();
    int imgH = pixmap.height();


    QFont timeFont;
    timeFont.setPixelSize(static_cast<int>(imgH * 0.05));

    QFont resultFont;
    resultFont.setPixelSize(static_cast<int>(imgH * 0.07));
    resultFont.setWeight(QFont::Bold); // 结果文本使用粗体以突出显示

    const int margin = 10; // 统一的边缘距离
    const int gap = 15;    // 文本之间的最小间隙
    const int vertical_gap = 5; // 堆叠时的垂直间隙

    // 准备文本和计算尺寸 
    // 左下角文本
    QString timeText = "Cost Time: " + QString::fromStdString(info.timeStr);
    QFontMetrics timeFm(timeFont);
    QRect timeRect = timeFm.boundingRect(timeText); // 等同于 cv::getTextSize

    // 右下角文本
    QString resultText = (info.ret == 0) ? "OK" : "NG";
    QColor resultColor = (info.ret == 0) ? Qt::green : Qt::red; 
    QFontMetrics resultFm(resultFont);
    QRect resultRect = resultFm.boundingRect(resultText);

    // 重叠判断与绘制 
    // 计算默认情况下的位置 
    QPoint timeOrg_default(margin, imgH - margin);
    QPoint resultOrg_default(imgW - resultRect.width() - margin, imgH - margin);

    // 判断左边文本的右边缘是否会与右边文本的左边缘重叠
    if ((timeOrg_default.x() + timeRect.width() + gap) > resultOrg_default.x()) {
        // 会重叠，启用堆叠模式 (全部靠右绘制)

        // "OK/NG" 在下
        QPoint resultOrg_stacked(imgW - resultRect.width() - margin, imgH - margin);
        painter.setFont(resultFont);
        painter.setPen(resultColor);
        painter.drawText(resultOrg_stacked, resultText);

        // "Cost Time" 在上
        // Y坐标 = "OK/NG"的基线Y坐标 - "OK/NG"文本的高度 - 垂直间隙
        int timeY_stacked = resultOrg_stacked.y() - resultRect.height() - vertical_gap;
        QPoint timeOrg_stacked(imgW - timeRect.width() - margin, timeY_stacked);
        painter.setFont(timeFont);
        painter.setPen(Qt::green);
        painter.drawText(timeOrg_stacked, timeText);

    }
    else {
        // 不会重叠，使用默认的左右角模式
        painter.setFont(timeFont);
        painter.setPen(Qt::green);
        painter.drawText(timeOrg_default, timeText);

        painter.setFont(resultFont);
        painter.setPen(resultColor);
        painter.drawText(resultOrg_default, resultText);
    }
}

void ImagePaint::drawDetectionResultExQImage(QImage& image, const DetectInfo& info)
{
    if (image.isNull()) return;

    prepareImageForDrawing(image);

    QPainter painter(&image);
    if (!painter.isActive()) return; // 防呆：确保 Painter 可用

    painter.setRenderHint(QPainter::Antialiasing);

    int imgW = image.width();
    int imgH = image.height();

    // 防呆：确保字体大小至少为 10 像素，避免极小图像导致计算错误
    int timePixelSize = std::max(10, static_cast<int>(imgH * 0.05));
    int resultPixelSize = std::max(12, static_cast<int>(imgH * 0.07));

    QFont timeFont;
    timeFont.setPixelSize(timePixelSize);

    QFont resultFont;
    resultFont.setPixelSize(resultPixelSize);
    resultFont.setWeight(QFont::Bold);

    const int margin = 10;
    const int gap = 15;
    const int vertical_gap = 5;

    QString timeText = "Cost Time: " + QString::fromStdString(info.timeStr);
    QFontMetrics timeFm(timeFont);
    QRect timeRect = timeFm.boundingRect(timeText).adjusted(0, 0, 1, 1);

    QString resultText = (info.ret == 0) ? "OK" : "NG";
    QColor resultColor = (info.ret == 0) ? Qt::green : Qt::red;
    QFontMetrics resultFm(resultFont);
    QRect resultRect = resultFm.boundingRect(resultText).adjusted(0, 0, 1, 1);

    // 默认位置：基线位置坐标
    QPoint timeOrg_default(margin, imgH - margin);
    QPoint resultOrg_default(imgW - resultRect.width() - margin, imgH - margin);

    // 判断逻辑：如果左侧文本右缘 + 间隙 超过了 右侧文本左缘
    if ((timeOrg_default.x() + timeRect.width() + gap) > resultOrg_default.x()) {

        // 堆叠模式：全部靠右对齐
        QPoint resultOrg_stacked(imgW - resultRect.width() - margin, imgH - margin);
        painter.setFont(resultFont);
        painter.setPen(resultColor);
        painter.drawText(resultOrg_stacked, resultText);

        int timeY_stacked = resultOrg_stacked.y() - resultRect.height() - vertical_gap;
        QPoint timeOrg_stacked(imgW - timeRect.width() - margin, timeY_stacked);

        painter.setFont(timeFont);
        painter.setPen(Qt::green);
        painter.drawText(timeOrg_stacked, timeText);
    }
    else {
        // 左右分布模式
        painter.setFont(timeFont);
        painter.setPen(Qt::green);
        painter.drawText(timeOrg_default, timeText);

        painter.setFont(resultFont);
        painter.setPen(resultColor);
        painter.drawText(resultOrg_default, resultText);
    }
}


void ImagePaint::drawPaintDataEx_VI_QImage(QImage& image, QVector<PaintDataItem> paintDataList, QSize displaySize)
{
    if (image.isNull() || paintDataList.isEmpty()) return;

    prepareImageForDrawing(image);

    QPainter painter(&image);
    if (!painter.isActive()) return;

    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);

    if (displaySize.isEmpty() || !displaySize.isValid()) {
        displaySize = image.size();
    }

    // 计算缩放比例与字体
    QSize actualOnScreenSize = image.size().scaled(displaySize, Qt::KeepAspectRatio);
    if (actualOnScreenSize.height() <= 0) return;

    const QSize canvasSize = image.size();
    constexpr int TARGET_VISUAL_FONT_SIZE = 12;

    double fontHeightRatio = static_cast<double>(TARGET_VISUAL_FONT_SIZE) / actualOnScreenSize.height();
    int fontSizeOnCanvas = std::max(1, qRound(canvasSize.height() * fontHeightRatio));

    QFont font;
    font.setWeight(QFont::Medium);
    font.setPixelSize(fontSizeOnCanvas);
    painter.setFont(font);

    // 预定义需要匹配的关键字列表
    static const QStringList keywords = {
        "引线总长度", "底座宽度", "底座高度", "左引脚超底座长度", "右引脚超底座长度",
        "左引脚高度", "右引脚高度", "左引脚宽度", "右引脚宽度", "座板上斜边长度",
        "左引脚弯脚", "正极宽度", "电容角度", "负极宽度", "正极瑕疵", "负极瑕疵",
        "引脚到底座距离", "引脚高度","负极标识"
    };

    // 布局参数
    QFontMetrics fm(font);
    int rowSpacingOnCanvas = qRound(canvasSize.height() * (1.2 / actualOnScreenSize.height()));
    int lineHeightOnCanvas = fm.height() + rowSpacingOnCanvas;

    int xMarginOnCanvas = qRound(canvasSize.width() * 0.01);
    int yMarginOnCanvas = qRound(canvasSize.height() * 0.05);
    int columnWidth = canvasSize.width() - 2 * xMarginOnCanvas;

    if (columnWidth <= 0 || lineHeightOnCanvas <= 0) return;

    int currentRow = 0;

    for (const auto& item : paintDataList) {
        if (!item.check) continue;

        // 查找匹配的关键字
        QString prefix;
        for (const QString& key : keywords) {
            if (item.label.contains(key)) {
                prefix = key;
                break; // 找到第一个匹配即跳出
            }
        }

        if (prefix.isEmpty()) continue;

        // 颜色判定
#ifdef USE_MAIN_WINDOW_CAPACITY
        QColor color = (item.result == 1) ? Qt::green : Qt::blue;
#else
        QColor color = (item.result == 1) ? Qt::green : Qt::red;
#endif
        painter.setPen(color);

        QString text = QString("%1: %2").arg(prefix).arg(item.value);
        int y = yMarginOnCanvas + currentRow * lineHeightOnCanvas;

        // 防呆：如果绘制位置已超出图像底部，停止绘制或换列（此处选择停止，防止无效绘制）
        if (y + lineHeightOnCanvas > canvasSize.height()) {
            break;
        }

        painter.drawText(QRect(xMarginOnCanvas, y, columnWidth, lineHeightOnCanvas),
            Qt::AlignLeft | Qt::AlignVCenter, text);
        currentRow++;
    }
}


void ImagePaint::drawPaintDataOnImage(QImage& canvas,
    const QVector<PaintDataItem>& paintDataList)
{
    // 1. 参数校验
    if (canvas.isNull()) {
        qWarning() << "传入的 canvas (QImage) 为空，无法绘制。";
        return;
    }

    // 2. 直接在传入的 canvas 上创建 QPainter
    QPainter painter(&canvas);
    painter.setRenderHint(QPainter::Antialiasing);

    // 3. 使用 canvas 的尺寸作为所有动态计算的基准
    const QSize fixedSize = canvas.size();

    // 核心布局与绘图逻辑，完全不变，只是基准尺寸现在是 canvas.size() 

    constexpr double TEXT_HEIGHT_RATIO = 0.04;
    constexpr double ROW_SPACING_RATIO = 0.01;
    constexpr double X_MARGIN_RATIO = 0.03;
    constexpr double Y_MARGIN_RATIO = 0.05;
    constexpr double FIRST_COLUMN_WIDTH_RATIO = 0.66;
    constexpr double SECOND_COLUMN_X_START_RATIO = 0.69;
    constexpr double FIRST_COL_HEIGHT_RATIO = 0.66;

    int textPixelSize = qMax(12, static_cast<int>(fixedSize.height() * TEXT_HEIGHT_RATIO));
    int xMargin = static_cast<int>(fixedSize.width() * X_MARGIN_RATIO);
    int yMargin = static_cast<int>(fixedSize.height() * Y_MARGIN_RATIO);
    int rowSpacingExtra = static_cast<int>(fixedSize.height() * ROW_SPACING_RATIO);

    QFont font;
    font.setWeight(QFont::Medium);
    font.setPixelSize(textPixelSize);
    painter.setFont(font);

    QFontMetrics fm(font);
    int lineHeight = fm.height() + rowSpacingExtra;

    int maxRowsFirstCol = static_cast<int>((fixedSize.height() * FIRST_COL_HEIGHT_RATIO - yMargin) / lineHeight);
    if (maxRowsFirstCol <= 0) maxRowsFirstCol = 1;

    int maxRowsSecondCol = (fixedSize.height() - yMargin) / lineHeight;
    if (maxRowsSecondCol <= 0) maxRowsSecondCol = 1;

    int drawIndex = 0;
    for (const auto& item : paintDataList) {
        if (!item.check)
            continue;

        QColor color = (item.result == 1) ? Qt::green : Qt::blue;
        painter.setPen(color);

        int x, y;
        if (drawIndex < maxRowsFirstCol) {
            // 第一列
            x = xMargin;
            y = yMargin + drawIndex * lineHeight + fm.ascent();
        }
        else {
            // 第二列
            int secondColIndex = drawIndex - maxRowsFirstCol;
            if (secondColIndex >= maxRowsSecondCol) {
                break;
            }
            x = static_cast<int>(fixedSize.width() * SECOND_COLUMN_X_START_RATIO);
            y = yMargin + secondColIndex * lineHeight + fm.ascent();
        }

        QString text = QString("%1: %2").arg(item.label, item.value);
        painter.drawText(x, y, text);
        ++drawIndex;
    }
    // painter 在函数结束时自动销毁，对 canvas 的所有修改完成。
}


void ImagePaint::drawPaintDataEx_I(QPixmap& pixmap,
    QVector<PaintDataItem> paintDataList,
    QSize displaySize)
{
    // 1. 安全检查
    if (pixmap.isNull()) {
        qWarning() << "传入的 pixmap 为空，无法绘制。";
        return;
    }

    // 2. 直接在源 Pixmap 上创建 QPainter 进行绘制
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    // 3. 尺寸处理
    if (displaySize.isEmpty() || !displaySize.isValid()) {
        displaySize = pixmap.size();
    }

    // 4. 计算缩放比例
    QSize actualOnScreenSize = pixmap.size().scaled(displaySize, Qt::KeepAspectRatio);
    const QSize canvasSize = pixmap.size();

    // 5. 定义视觉设计参数
    constexpr int TARGET_VISUAL_FONT_SIZE = 20;
    constexpr double X_MARGIN_RATIO = 0.01;
    constexpr double Y_MARGIN_RATIO = 0.05;

    // 6. 字体尺寸计算
    if (actualOnScreenSize.height() == 0) return;

    double fontHeightRatio = static_cast<double>(TARGET_VISUAL_FONT_SIZE) / actualOnScreenSize.height();
    int fontSizeOnCanvas = qRound(canvasSize.height() * fontHeightRatio);
    if (fontSizeOnCanvas < 1) return;

    // 7. 设置字体
    QFont font;
    font.setWeight(QFont::DemiBold);
    font.setPixelSize(fontSizeOnCanvas);
    painter.setFont(font);

    // 8. 只查找并绘制第一个算法NG
    for (const auto& item : paintDataList) {
        if (!item.check) continue;
        if (item.result == 1) continue; // 只找NG

#ifdef USE_MAIN_WINDOW_CAPACITY
        QColor color = Qt::blue;
#else
        QColor color = Qt::red;
#endif
        painter.setPen(color);

        QString text = QStringLiteral("算法NG");

        int x = qRound(canvasSize.width() * X_MARGIN_RATIO);
        int y = qRound(canvasSize.height() * Y_MARGIN_RATIO);

        painter.drawText(QRect(x, y, canvasSize.width() - 2 * x, canvasSize.height() - 2 * y),
            Qt::AlignLeft | Qt::AlignTop, text);
        return; // 只画第一个
    }
    // 没有NG则不画任何内容
}

void ImagePaint::drawPaintDataEx_II(QPixmap& pixmap,
    QVector<PaintDataItem> paintDataList,
    QSize displaySize)
{
    // 1. 安全检查
    if (pixmap.isNull()) {
        qWarning() << "传入的 pixmap 为空，无法绘制。";
        return;
    }

    // 2. 直接在源 Pixmap 上创建 QPainter 进行绘制
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    // 3. 尺寸处理
    if (displaySize.isEmpty() || !displaySize.isValid()) {
        displaySize = pixmap.size();
    }

    // 4. 计算缩放比例
    QSize actualOnScreenSize = pixmap.size().scaled(displaySize, Qt::KeepAspectRatio);
    const QSize canvasSize = pixmap.size();

    // 5. 定义视觉设计参数
    constexpr int TARGET_VISUAL_FONT_SIZE = 20;
    constexpr double X_MARGIN_RATIO = 0.01;
    constexpr double Y_MARGIN_RATIO = 0.05;

    // 6. 字体尺寸计算
    if (actualOnScreenSize.height() == 0) return;

    double fontHeightRatio = static_cast<double>(TARGET_VISUAL_FONT_SIZE) / actualOnScreenSize.height();
    int fontSizeOnCanvas = qRound(canvasSize.height() * fontHeightRatio);
    if (fontSizeOnCanvas < 1) return;

    // 7. 设置字体
    QFont font;
    font.setWeight(QFont::DemiBold);
    font.setPixelSize(fontSizeOnCanvas);
    painter.setFont(font);

    // 8. 只查找并绘制第一个算法NG
    for (const auto& item : paintDataList) {
        if (!item.check) continue;
        if (item.result == 1) continue; // 只找NG

#ifdef USE_MAIN_WINDOW_CAPACITY
        QColor color = Qt::blue;
#else
        QColor color = Qt::red;
#endif
        painter.setPen(color);

        QString text = QStringLiteral("打孔未穿");

        int x = qRound(canvasSize.width() * X_MARGIN_RATIO);
        int y = qRound(canvasSize.height() * Y_MARGIN_RATIO);

        painter.drawText(QRect(x, y, canvasSize.width() - 2 * x, canvasSize.height() - 2 * y),
            Qt::AlignLeft | Qt::AlignTop, text);
        return; // 只画第一个
    }
    // 没有NG则不画任何内容
}

void ImagePaint::drawPaintDataEx_III(QPixmap& pixmap,
    QVector<PaintDataItem> paintDataList,
    QSize displaySize)
{
    // 1. 安全检查
    if (pixmap.isNull()) {
        qWarning() << "传入的 pixmap 为空，无法绘制。";
        return;
    }

    // 2. 直接在源 Pixmap 上创建 QPainter 进行绘制
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    // 3. 尺寸处理
    if (displaySize.isEmpty() || !displaySize.isValid()) {
        displaySize = pixmap.size();
    }

    // 4. 计算缩放比例
    QSize actualOnScreenSize = pixmap.size().scaled(displaySize, Qt::KeepAspectRatio);
    const QSize canvasSize = pixmap.size();

    // 5. 定义视觉设计参数
    constexpr int TARGET_VISUAL_FONT_SIZE = 20;
    constexpr double X_MARGIN_RATIO = 0.01;
    constexpr double Y_MARGIN_RATIO = 0.05;

    // 6. 字体尺寸计算
    if (actualOnScreenSize.height() == 0) return;

    double fontHeightRatio = static_cast<double>(TARGET_VISUAL_FONT_SIZE) / actualOnScreenSize.height();
    int fontSizeOnCanvas = qRound(canvasSize.height() * fontHeightRatio);
    if (fontSizeOnCanvas < 1) return;

    // 7. 设置字体
    QFont font;
    font.setWeight(QFont::DemiBold);
    font.setPixelSize(fontSizeOnCanvas);
    painter.setFont(font);

    // 8. 只查找并绘制第一个算法NG
    for (const auto& item : paintDataList) {
        if (!item.check) continue;
        if (item.result == 1) continue; // 只找NG

#ifdef USE_MAIN_WINDOW_CAPACITY
        QColor color = Qt::blue;
#else
        QColor color = Qt::red;
#endif
        painter.setPen(color);

        QString text = QStringLiteral("顶部破损");

        int x = qRound(canvasSize.width() * X_MARGIN_RATIO);
        int y = qRound(canvasSize.height() * Y_MARGIN_RATIO);

        painter.drawText(QRect(x, y, canvasSize.width() - 2 * x, canvasSize.height() - 2 * y),
            Qt::AlignLeft | Qt::AlignTop, text);
        return; // 只画第一个
    }
    // 没有NG则不画任何内容
}

void ImagePaint::drawPaintDataEx_PixmapUltra(QPixmap& pixmap,
    QString text, // 新增参数：要绘制的文本
    QSize displaySize)
{
    // 1. 安全检查
    if (pixmap.isNull() || text.isEmpty()) {
        if (pixmap.isNull()) {
            qWarning() << "传入的 pixmap 为空，无法绘制。";
        }
        // 如果 text 为空，则不绘制任何内容，直接返回
        return;
    }

    // 2. 直接在源 Pixmap 上创建 QPainter 进行绘制
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    // 3. 尺寸处理
    if (displaySize.isEmpty() || !displaySize.isValid()) {
        displaySize = pixmap.size();
    }

    // 4. 计算缩放比例
    QSize actualOnScreenSize = pixmap.size().scaled(displaySize, Qt::KeepAspectRatio);
    const QSize canvasSize = pixmap.size();

    // 5. 定义视觉设计参数
    constexpr int TARGET_VISUAL_FONT_SIZE = 20;
    constexpr double X_MARGIN_RATIO = 0.01;
    constexpr double Y_MARGIN_RATIO = 0.05;

    // 6. 字体尺寸计算
    if (actualOnScreenSize.height() == 0) return;

    double fontHeightRatio = static_cast<double>(TARGET_VISUAL_FONT_SIZE) / actualOnScreenSize.height();
    int fontSizeOnCanvas = qRound(canvasSize.height() * fontHeightRatio);
    if (fontSizeOnCanvas < 1) return;

    // 7. 设置字体
    QFont font;
    font.setWeight(QFont::DemiBold);
    font.setPixelSize(fontSizeOnCanvas);
    painter.setFont(font);

    // 8. 设置颜色（保留原宏定义逻辑）
#ifdef USE_MAIN_WINDOW_CAPACITY
    QColor color = Qt::blue;
#else
    QColor color = Qt::red;
#endif
    painter.setPen(color);

    // 9. 使用传入的 text 参数进行绘制
    int x = qRound(canvasSize.width() * X_MARGIN_RATIO);
    int y = qRound(canvasSize.height() * Y_MARGIN_RATIO);

    painter.drawText(QRect(x, y, canvasSize.width() - 2 * x, canvasSize.height() - 2 * y),
        Qt::AlignLeft | Qt::AlignTop, text);

}

void ImagePaint::drawPaintDataEx_Ultra(QPixmap& pixmap,
    QSize displaySize,
    const QStringList& str)
{
    // 1. 安全检查
    if (pixmap.isNull()) {
        qWarning() << "传入的 pixmap 为空，无法绘制。";
        return;
    }

    // 如果传入的字符串列表为空，直接返回
    if (str.isEmpty()) return;

    // 2. 直接在源 Pixmap 上创建 QPainter 进行绘制
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    // 3. 尺寸处理
    if (displaySize.isEmpty() || !displaySize.isValid()) {
        displaySize = pixmap.size();
    }

    // 4. 计算缩放比例
    QSize actualOnScreenSize = pixmap.size().scaled(displaySize, Qt::KeepAspectRatio);
    const QSize canvasSize = pixmap.size();

    // 5. 定义视觉设计参数
    constexpr int TARGET_VISUAL_FONT_SIZE = 20;
    constexpr double X_MARGIN_RATIO = 0.01;
    constexpr double Y_MARGIN_RATIO = 0.05;

    // 6. 字体尺寸计算
    if (actualOnScreenSize.height() == 0) return;

    double fontHeightRatio = static_cast<double>(TARGET_VISUAL_FONT_SIZE) / actualOnScreenSize.height();
    int fontSizeOnCanvas = qRound(canvasSize.height() * fontHeightRatio);
    if (fontSizeOnCanvas < 1) fontSizeOnCanvas = 1;

    // 7. 设置字体
    QFont font;
    font.setWeight(QFont::DemiBold);
    font.setPixelSize(fontSizeOnCanvas);
    painter.setFont(font);

    // 8. 绘制逻辑修改
    painter.setPen(Qt::red); // 颜色固定红色

    int x = qRound(canvasSize.width() * X_MARGIN_RATIO);
    int y = qRound(canvasSize.height() * Y_MARGIN_RATIO);

    // --- 核心修改开始 ---
    // 将 QStringList 转换为一个字符串，中间用换行符连接
    // 这样 drawText 会自动把它画成多行
    QString textToDraw = str.join("\n");
    // --- 核心修改结束 ---

    painter.drawText(QRect(x, y, canvasSize.width() - 2 * x, canvasSize.height() - 2 * y),
        Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap, // 建议加上 TextWordWrap 防止单行过长
        textToDraw);
}