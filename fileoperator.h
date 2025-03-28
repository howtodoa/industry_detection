#ifndef FILEOPERATOR_H
#define FILEOPERATOR_H

#include <QString>
#include <QFile>
#include <QTextStream>
#include "public.h"

class FileOperator
{
public:
    // 分别写入三种类型的参数
    static bool writeRangePara(const QString& filePath, const RangePara& range);
    static bool writeCameralPara(const QString& filePath, const CameralPara& cam);
    static bool writeAlgorithmPara(const QString& filePath, const AlgorithmPara& algo);

    // 分别读取三种类型的参数
    static RangePara readRangePara(const QString& filePath);
    static CameralPara readCameralPara(const QString& filePath);
    static AlgorithmPara readAlgorithmPara(const QString& filePath);
};

#endif // FILEOPERATOR_H
