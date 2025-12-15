#ifndef PUBLIC_H
#define PUBLIC_H

#include <QList>
#include <QDebug>
#include <QLabel>
#include <QQueue>
#include <QFont>
#include <QGridLayout>
#include <QPushButton>
#include <QScreen>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QStatusBar>
#include <QTimer>
#include <QDir>
#include <QFileDialog>
#include <QDesktopServices>
#include <QStackedLayout>
#include <QLineEdit>
#include <QVector>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QHBoxLayout>
#include <windows.h>
#include "typdef.h"
#include <dxgi1_4.h> 
#include <Pdh.h> 
#include <psapi.h>
#include <winternl.h>
#include <QtConcurrent/QtConcurrentRun>
#include <QFuture>
#include <QFutureWatcher>
#include "fileoperator.h"
#include <QStorageInfo>

void renameJpgFiles(const QString& directoryPath);

std::shared_ptr<cv::Mat> loadImageFromFile(const QString& filePath);

bool containsOnlyAscii(const QString& str);

cv::Mat loadMatFromFile(const QString& filePath);

void MyImageCallback(cv::Mat& image, void* pUser);

void MyImageCallback_Flower(cv::Mat& image, void* pUser);

bool areMatFormatsIdentical(const cv::Mat& mat1, const cv::Mat& mat2);

int callWithTimeout_cpp11(std::function<int()> func, int timeoutMs, int defaultValue);

bool generateDir(const std::string& dirPath);

bool isPointerSafe(void* ptr, size_t requiredSize = 1);

bool isMatSafe(const cv::Mat& mat, size_t requiredSize = 0);

cv::Mat QPixmapToMat(const QPixmap& pixmap);

QString FirstFailedParamLabel(const AllUnifyParams& allParams);

cv::Mat convertQImageToMat(const QImage& image);

std::string generateStamp();

QQueue<QString> collectDirsByLevel_BFS(const QString& rootPath, int level);

bool CleanDir(const QString& dirPath);

bool IsDiskUsageOverThreshold(const QString& path, double thresholdPercent);

void printOutStampResParam(const OutStampResParam& param, const std::string& structName = "OutStampResParam");

QPixmap convertMatToPixmap(const cv::Mat& mat);

QImage convertMatToQImage(const cv::Mat& mat);

qint64 getAvailableSystemMemoryMB();

qint64 getAvailableVRAM_MB();

int callWithTimeout(std::function<int()> func, int timeoutMs, int defaultValue = -1);

int CheckRAM();

int CheckPixmap(const QPixmap& pixmap);

bool SaveParamsJson(const QString& absPath, const AllUnifyParams& params);

#endif // PUBLIC_H
