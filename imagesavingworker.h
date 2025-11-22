#ifndef IMAGESAVINGWORKER_H
#define IMAGESAVINGWORKER_H

#include <QObject>
#include "public.h"

class ImageSaverWorker : public QObject //主线程来管理Qthread
{
    Q_OBJECT

public:

    explicit ImageSaverWorker(SaveQueue& saveQueue, QObject *parent = nullptr);
    ~ImageSaverWorker();

    void saveLoop();
    void saveLoop_QImage();


private:

    SaveQueue& m_saveQueue;
    std::atomic<bool> thread_stopFlag{false};

};
#endif // IMAGESAVINGWORKER_H
