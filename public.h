#ifndef PUBLIC_H
#define PUBLIC_H

#include <QList>
#include <QDebug>
#include <QLabel>
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

void renameJpgFiles(const QString& directoryPath);

std::shared_ptr<cv::Mat> loadImageFromFile(const QString& filePath);

bool containsOnlyAscii(const QString& str);

cv::Mat loadMatFromFile(const QString& filePath);

void MyImageCallback(cv::Mat& image, void* pUser);

bool areMatFormatsIdentical(const cv::Mat& mat1, const cv::Mat& mat2);

bool generateDir(const std::string& dirPath);

cv::Mat QPixmapToMat(const QPixmap& pixmap);

cv::Mat convertQImageToMat(const QImage& image);

std::string generateStamp();

void printOutStampResParam(const OutStampResParam& param, const std::string& structName = "OutStampResParam");

QPixmap convertMatToPixmap(const cv::Mat& mat);

QImage convertMatToQImage(const cv::Mat& mat);

#endif // PUBLIC_H
