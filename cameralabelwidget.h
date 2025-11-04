#ifndef CAMERALABELWIDGET_H
#define CAMERALABELWIDGET_H

#include "MZ_ClientControl.h"
#include <QWidget>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "cameralmenu.h"
#include "Cameral.h"
#include "imageprocess.h"
#include "imageprocess_brader.h"
#include "imagesavingworker.h"
#include "parawidget.h"
#include "fullscreenwindow.h"
#include "ZoomableLabel.h"
#include "tcp_client.h"
#include "typdef.h"
#include "localfiledetector.h"

class CameraLabelWidget : public QWidget
{
    Q_OBJECT
public:
    // 构造函数：index 用于生成唯一的名称、显示文字，parent 为父控件
    explicit CameraLabelWidget(Cameral * cam, int index, const QString &fixedTextName,SaveQueue *m_saveQueue,QWidget *parent = nullptr);
    ~CameraLabelWidget();
    static void pic_handle(HImages inputImages, HValues inputParams, HImages& outputImages, HValues& outputParams,int& errcode, string& errmsg);
    static void pic_display(HImages inputImages, HValues inputParams, HImages& outputImages, HValues& outputParams,int& errcode, string& errmsg);
    static void add_ui(HImages inputImages, HValues inputParams,  HImages& outputImages, HValues& outputParams,   int& errcode, std::string& errmsg);
    void displayimg(QPixmap &pixmap);
    void displayimg(HImage &himage);
   static HImage convertQPixmapToHImage(const QPixmap &pixmap);
    QPixmap convertHImageToPixmap(const HImage& hImage);
   void startTask(Cameral* cam, int seconds);
    void triggerCameraStart(Cameral *cam);
     void triggerCameraStop(Cameral *cam);
     void handleCameraPush(Cameral* cam);
     void handleCameraLocalRun(Cameral* cam);
     void ChangeDateDir(Cameral &cam);
     void updateDebugValuesAsync(const QString& cameraKey, const DebugInfo* DI, const QString& filePath);
    void stopLocalFileDetectorThread();
   

    QLabel* getImageLabel() const { return imageLabel; }

protected:

public slots:
    void triggerCameraPhoto(Cameral* cam);
    void onSetButtonBackground(const QString& color);
    void onCameraStart();
    void onCameraStop();
    void onCameraPhoto();
    void onParam();
    void onImageProcessed_plate(std::shared_ptr<cv::Mat> processedImagePtr,DetectInfo info);
    void onImageProcessed_flower(std::shared_ptr<cv::Mat> processedImagePtr, DetectInfo info);
    void onImageProcessed(std::shared_ptr<cv::Mat> processedImagePtr, DetectInfo info);
    void onSaveDebugInfo(DebugInfo *DI);
    void onImageLoaded(std::shared_ptr<cv::Mat> image);
    void onLearn();
    void onCheck();
    void onViewImageClicked();
    void onSwitchFullScreen(bool status);
    void onCameraConnect(const QString& color);
    void onStartGetIn();
    void onImageProcessed_Brader(std::shared_ptr<cv::Mat> processedImagePtr, DetectInfo info);
private:
    cv::Mat image;
    QPixmap currentPixmap;
    ZoomableLabel *imageLabel; // 显示图像（支持鼠标滚轮缩放和平移）
    QLabel        *fixedText;  // 固定文本标签
    CameraMenu    *cameraMenu; // 操作菜单
    QPushButton* runButton;
    QPushButton* stopButton;
    QPushButton* captureButton;
    QPushButton* paramButton;
    QPushButton* tenButton;
    QToolButton* textMenuButton;
    QPushButton* shieldButton;
    LocalFileDetector *m_localfilethread=nullptr;
    SaveData dataToSave;
    SaveQueue *saveToQueue;
    std::atomic<int64> *sumcount;
    std::atomic<int64> *ngcount;
    std::atomic<bool> check_flag = false;
    std::shared_ptr<cv::Mat> learnimage =nullptr;
    std::shared_ptr<cv::Mat> checkimage = nullptr;
    std::atomic<bool> FullScreen = false;
    bool thread_exit=false;
    Cameral *m_cam=nullptr;
	std::atomic<bool> ngDisplay = false;
public:
    ImageProcess* m_imageProcessor;
    ImageProcess* m_imageProcessor_Red;
    ImageViewerWindow* imageView = nullptr;
    ParaWidget* parawidget = nullptr;
signals:
    void FullShow(const QPixmap& pixmap);
};

#endif // CAMERALABELWIDGET_H
