#include "directorymonitorworker.h"
#include <QTimer>
#include <QDir>
#include <QDirIterator>
#include <QStorageInfo>
#include <QSet>
#include <QDebug>
#include <QThread>
#include <QCoreApplication>
#include "public.h"

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

    doCheckAndCleanup();
}

void DirectoryMonitorWorker::doCheckAndCleanup()
{
    qDebug() << "Performing scheduled check...";

    // 检查当前EXE所在磁盘的使用率 ---
    QString appPath = m_pathsToWatch[0];

    bool needsDeletion = IsDiskUsageOverThreshold(appPath, 80.0);

    if (!needsDeletion)
    {
        qDebug() << "Disk space is sufficient. No cleanup needed.";
        return;
    }


    // 如果需要，则启动删除循环 ---
    qDebug() << "Cleanup triggered. Starting deletion loop.";

    
    // 暂时不加锁使用
    //创建队列
    QQueue<QString> m_pathsToClean =
        collectDirsByLevel_BFS(SystemPara::DATA_DIR, 3);
    while (m_saveQueue.queue.empty() && IsDiskUsageOverThreshold(appPath, 70.0))
    {
        if (m_pathsToClean.isEmpty())
        {
            qDebug() << "No more directories to clean. Pausing cleanup.";
            break;
        }

        QString targetDir = m_pathsToClean.dequeue();
        qDebug() << "Cleaning directory:" << targetDir;

        bool deleted = CleanDir(targetDir);

        if (deleted)
        {
            QThread::msleep(200);
        }
        else
        {
            QThread::msleep(200);
            qDebug() << "No image deleted in:" << targetDir;
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

bool CleanDir(const QString& dirPath)
{
    if (dirPath.isEmpty())
        return true;

    QFileInfo info(dirPath);
    if (!info.exists() || !info.isDir())
        return true;

    QDir dir(dirPath);

    bool ok = dir.removeRecursively();
    if (!ok)
    {
        qWarning() << "[CleanDir] Remove failed:" << dirPath;
    }

    return ok;
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