#ifndef DIRECTORYMONITORWORKER_H
#define DIRECTORYMONITORWORKER_H

#include <QObject>
#include <QStringList>
#include "typdef.h"
#include <QFileInfo>
class DirectoryMonitorWorker : public QObject
{
    Q_OBJECT
public:
    explicit DirectoryMonitorWorker(const QStringList& pathsToWatch, SaveQueue& saveQueue, QObject* parent = nullptr);
    ~DirectoryMonitorWorker();
public slots:

    void startMonitoring();

private slots:

    void doCheckAndCleanup();

private:

    bool findFirstJpgFile(const QString& basePath, QFileInfo& firstFile);

    // --- 成员变量 ---
    QStringList m_pathsToWatch; // 存储从删除文件的路径列表
    SaveQueue& m_saveQueue;     // 存储对存图队列的引用，以检查其状态
    QTimer* m_timer;            // 用于实现1分钟定时任务的定时器
};

#endif // DIRECTORYMONITORWORKER_H
