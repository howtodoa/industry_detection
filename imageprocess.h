#ifndef IMAGEPROCESS_H
#define IMAGEPROCESS_H

#include <QObject>
#include "Cameral.h"
#include <QThread>
#include "typdef.h"

class ImageProcess : public QThread //继承 QThread
{
    Q_OBJECT

public:
    // 构造函数：初始化 ImageProcess 对象。在创建它的线程中运行。
    explicit ImageProcess(Cameral* cam, SaveQueue* m_saveQueue,QObject* parent = nullptr);


    ~ImageProcess() override;


signals:
    void StopDevice();
    void PaintSend(QVector<PaintDataItem> paintData);
    void BuildUIFromUnifyParameters(const AllUnifyParams& params);
	void UpdateRealtimeData(const AllUnifyParams& params);
    void imageProcessed(std::shared_ptr<cv::Mat> processedImagePtr, DetectInfo info);
	void imageProcessed_Brader(std::shared_ptr<cv::Mat> processedImagePtr, DetectInfo info);
    void ImageLoaded(std::shared_ptr<cv::Mat> image);
    void Learn();
    void SetButtonBackground(const QString& color);
    void StartGetIn();
public:
    // 此方法将在独立的 QThread 中执行（即 ImageProcess 实例本身所代表的线程）。
    void run() override; // 重写 QThread 的 run 方法
	void ChangeDir(Cameral& cam);
    Cameral* cam_instance = nullptr;

    ImageQueuePack* m_inputQueue = nullptr;

    SaveData dataToSave;
    SaveQueue* saveToQueue;
    DetectInfo info;

    // 用于在 run() 方法中检查是否需要停止的原子标志。
    std::atomic<bool> thread_stopFlag{false};

    // 不再需要 QThread* m_processThread 成员变量，因为 ImageProcess 实例本身就是 QThread。
    // 不再需要 startProcessing() 方法，因为继承 QThread 后可以直接调用 QThread::start()。

};

#endif // IMAGEPROCESS_H


