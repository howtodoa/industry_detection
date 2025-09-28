#include "localfiledetector.h"

LocalFileDetector::LocalFileDetector(ImageQueuePack* imageQueuePack, const QString& monitorPath, QObject *parent)
    : QThread(parent),
    m_imageQueuePack(imageQueuePack),
    m_monitorPath(monitorPath) {
    qDebug() << "LocalFileDetector created, monitoring:" << m_monitorPath;
}

LocalFileDetector::~LocalFileDetector() {
    stop();
    quit();
    wait();
    qDebug() << "LocalFileDetector destroyed.";
}

void LocalFileDetector::stop() {
    m_stopRequested.store(true);
    qDebug() << "LocalFileDetector stop requested.";
}

void LocalFileDetector::run() {
    qDebug() << "LocalFileDetector thread started, monitoring path:" << m_monitorPath;

    QDir monitorDir(m_monitorPath);

    QStringList nameFilters;
    nameFilters << "*.jpg" << "*.jpeg";

    // 在线程启动时，只读取一次文件列表
    QFileInfoList initialFileList = monitorDir.entryInfoList(nameFilters, QDir::Files | QDir::NoDotAndDotDot, QDir::Name);
    qDebug() << "LocalFileDetector: Initial scan found" << initialFileList.size() << "JPG/JPEG files.";

    int currentFileIndex = 0; // 当前要尝试读取的文件索引

    // 无限循环，直到收到停止请求
    while (!m_stopRequested.load()) {
        // 1. 检查当前索引是否超出范围，如果是则从头开始
        if (currentFileIndex >= initialFileList.size()) {
            currentFileIndex = 0; // 从第一个文件开始重新循环
            qDebug() << "LocalFileDetector: Reached end of initial file list. Restarting from first file.";
            // 这里可以加一个短暂停，避免在文件列表非常短时循环过快
            QThread::msleep(200);
            continue; // 继续外层循环
        }

        // 2. 获取当前文件信息并尝试处理
        const QFileInfo& fileInfo = initialFileList.at(currentFileIndex);
        QString filePath = fileInfo.absoluteFilePath();

        // 每次处理前检查停止标志
        if (m_stopRequested.load()) {
            qDebug() << "LocalFileDetector: Stop requested, exiting run loop.";
            break; // 退出 while 循环
        }

//文件不存在的时候
        if (!fileInfo.exists()) {
            qWarning() << "LocalFileDetector: File at index" << currentFileIndex << " (" << filePath << ") no longer exists. Skipping.";
            currentFileIndex++; // 移动到下一个文件
            QThread::msleep(50); // 短暂暂停，避免快速空转
            continue; // 跳过此文件，处理下一个
        }

        qDebug() << "LocalFileDetector: Processing file at index" << currentFileIndex << ":" << filePath;

        std::shared_ptr<cv::Mat> imagePtr = loadImageFromFile(filePath);

        if (imagePtr) {
            if (m_imageQueuePack) {
                std::unique_lock<std::mutex> lock(m_imageQueuePack->mutex);
                m_imageQueuePack->queue.push_back(imagePtr);
                qDebug() << "LocalFileDetector: Pushed image to queue. Current queue size:" << m_imageQueuePack->queue.size();
                lock.unlock();
                m_imageQueuePack->cond.notify_all();
            } else {
                qWarning() << "LocalFileDetector: Image queue pack is null, cannot push image.";
            }
        } else {
            // 如果文件存在但加载失败（如损坏），也跳过
            qWarning() << "LocalFileDetector: Failed to load image from:" << filePath << ". File might be corrupted. Skipping.";
        }
        imagePtr.reset(); // 释放 shared_ptr 持有的内存

        currentFileIndex++; // 移动到下一个文件

        QThread::msleep(500); // 暂停
    }

    qDebug() << "LocalFileDetector thread finished processing files (stop requested).";
}
