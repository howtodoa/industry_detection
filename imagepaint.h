#ifndef IMAGEPAINT_H
#define IMAGEPAINT_H

#include <QPixmap>
#include "typdef.h"
class ImagePaint
{
public:

    static void drawResultOnPixmap(QPixmap& sourcePixmap, int result);  
    static void drawResultOnPixmap(QPixmap& sourcePixmap, int result, const QVector<PaintDataItem>& paintDataList, QSize displaySize);
    static void drawDetectionResult(cv::Mat& image, const DetectInfo& info);
    static void drawPaintDataEx_V(QPixmap& pixmap,
        QVector<PaintDataItem> paintDataList,
        QSize displaySize);
    static void drawPaintDataEx_VI(QPixmap& pixmap,
        QVector<PaintDataItem> paintDataList,
        QSize displaySize);
    static void drawDetectionResultEx(cv::Mat& image, const DetectInfo& info);
    static void drawPaintDataOnImage(QImage& canvas,const QVector<PaintDataItem>& paintDataList);
    static void drawDetectionResultExQt(QPixmap& pixmap, const DetectInfo& info);
    static  void drawPaintData(QPixmap& pixmap,
         QVector<PaintDataItem>& paintDataList,
         QSize displaySize);
    static  void drawPaintDataEx(QPixmap& pixmap,
        QVector<PaintDataItem> paintDataList,
        QSize displaySize);
    static void drawPaintDataEx_I(QPixmap& pixmap,
        QVector<PaintDataItem> paintDataList,
        QSize displaySize);
    static void drawPaintDataEx_II(QPixmap& pixmap,
        QVector<PaintDataItem> paintDataList,
        QSize displaySize);
    static void drawPaintDataEx(QPixmap& pixmap,
        const AllUnifyParams unifyParams, 
        QSize displaySize);
private:

    ImagePaint() = delete;
};

#endif // IMAGEPAINT_H
