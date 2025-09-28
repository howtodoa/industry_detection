#include "directorymonitorworker.h"
#include <QTimer>
#include <QDir>
#include <QDirIterator>
#include <QStorageInfo>
#include <QSet>
#include <QDebug>
#include <QThread>
#include <QCoreApplication>
#include "typdef.h"

DirectoryMonitorWorker::DirectoryMonitorWorker(const QStringList& pathsToWatch, SaveQueue& saveQueue, QObject* parent)
    : QObject(parent),
    m_pathsToWatch(pathsToWatch),
    m_saveQueue(saveQueue),
    m_timer(nullptr)
{
    qDebug() << "DirectoryMonitorWorker created. Paths to watch:" << m_pathsToWatch;
}

DirectoryMonitorWorker::~DirectoryMonitorWorker()
{
    qDebug() << "DirectoryMonitorWorker destructor called.";
}

void DirectoryMonitorWorker::startMonitoring()
{
    qDebug() << "DirectoryMonitorWorker started in thread" << QThread::currentThreadId();

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &DirectoryMonitorWorker::doCheckAndCleanup);

    // 设置半分钟的定时器
    m_timer->start(1 * 30 * 1000);

    qDebug() << "Monitoring started. Check will be performed every 5 minutes.";

    // 立即执行一次检查
    doCheckAndCleanup();
}

void DirectoryMonitorWorker::doCheckAndCleanup()
{
    qDebug() << "Performing scheduled check...";

    // 检查当前EXE所在磁盘的使用率 ---
    bool needsDeletion = false;
    QString appPath = QCoreApplication::applicationDirPath();
    QStorageInfo storage(appPath);

    if (storage.isValid() && storage.isReady()) {
        if (storage.bytesTotal() > 0) {
            double usagePercent = 100.0 * (storage.bytesTotal() - storage.bytesFree()) / storage.bytesTotal();
            qDebug() << "Checking application disk [" << storage.rootPath() << "]. Usage:" << usagePercent << "%";
            if (usagePercent > 90.0) {
                qDebug() << "WARNING: Application disk usage is over 70%. Triggering cleanup.";
                needsDeletion = true;
            }
        }
    }
    else {
        qWarning() << "Could not retrieve storage info for application path:" << appPath;
    }

    if (!needsDeletion) {
        qDebug() << "Disk space is sufficient. No cleanup needed.";
        return;
    }

    // 如果需要，则启动删除循环 ---
    qDebug() << "Cleanup triggered. Starting deletion loop.";

    
    // 暂时不加锁使用
    //
    while (m_saveQueue.queue.empty()) {
        QFileInfo fileToDelete; // 用于存储将要删除的文件信息
        bool found = false;    // 标志位，用于判断是否已找到文件

        // 遍历所有要监视的目录
        for (const QString& path : m_pathsToWatch) {
            // 尝试在当前路径中找到第一个 .jpg 文件
            if (findFirstJpgFile(path, fileToDelete)) {
                // 找到了！立即设置标志位并跳出循环，不再检查其他目录
                found = true;
                break;
            }
        }

        // 检查是否在任何目录中找到了文件
        if (found) {
            // 直接删除找到的第一个文件
            QString filePath = fileToDelete.filePath();
            qDebug() << "Deleting first available file:" << filePath;

            if (QFile::remove(filePath)) {
                // 成功删除后短暂休眠
                QThread::msleep(20);
            }
            else {
                // 如果删除失败，则跳过
                qWarning() << "Failed to delete file:" << filePath << ". Pausing cleanup for this cycle.";
                continue;
            }
        }
        else {
            // 如果遍历完所有目录都找不到文件，则退出本次清理大循环
            qDebug() << "No .jpg files found in any monitored path. Pausing cleanup.";
            break;
        }
    }
    // 循环结束
    if (m_saveQueue.stopFlag.load()) {
        qDebug() << "Deletion loop stopped because save queue flag is now true.";
    }
    else if (!m_saveQueue.queue.empty()) {
        qDebug() << "Deletion loop stopped because save queue is no longer empty.";
    }
}


bool DirectoryMonitorWorker::findFirstJpgFile(const QString& basePath, QFileInfo& firstFile)
{
    qDebug() << "basePath:" << basePath;
    // QDirIterator 默认就会按名称排序进行深度优先遍历
    QDirIterator it(basePath, QStringList() << "*.jpg", QDir::Files, QDirIterator::Subdirectories);

    // 只需要找到第一个匹配项即可
    if (it.hasNext()) {
        firstFile = QFileInfo(it.next()); // 获取第一个文件的信息
        return true;
    }

    return false; // 该目录下没有任何.jpg文件
}