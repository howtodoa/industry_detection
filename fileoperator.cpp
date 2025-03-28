#include "FileOperator.h"

bool FileOperator::writeRangePara(const QString& filePath, const RangePara& range) {
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }
    QTextStream out(&file);
    out << range.toString();
    file.close();
    return true;
}

bool FileOperator::writeCameralPara(const QString& filePath, const CameralPara& cam) {
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }
    QTextStream out(&file);
    out << cam.toString();
    file.close();
    return true;
}

bool FileOperator::writeAlgorithmPara(const QString& filePath, const AlgorithmPara& algo) {
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }
    QTextStream out(&file);
    out << algo.toString();
    file.close();
    return true;
}

RangePara FileOperator::readRangePara(const QString& filePath) {
    QFile file(filePath);
    RangePara param;
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return param;
    }
    QTextStream in(&file);
    QString content = in.readAll();
    file.close();

    QStringList sections = content.split("\n\n", Qt::SkipEmptyParts);
    for (const QString& section : sections) {
        if (section.startsWith("范围参数：")) {
            param.fromString(section);
            break;
        }
    }
    return param;
}

CameralPara FileOperator::readCameralPara(const QString& filePath) {
    QFile file(filePath);
    CameralPara param;
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return param;
    }
    QTextStream in(&file);
    QString content = in.readAll();
    file.close();

    QStringList sections = content.split("\n\n", Qt::SkipEmptyParts);
    for (const QString& section : sections) {
        if (section.startsWith("相机参数：")) {
            param.fromString(section);
            break;
        }
    }
    return param;
}

AlgorithmPara FileOperator::readAlgorithmPara(const QString& filePath) {
    QFile file(filePath);
    AlgorithmPara param;
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return param;
    }
    QTextStream in(&file);
    QString content = in.readAll();
    file.close();

    QStringList sections = content.split("\n\n", Qt::SkipEmptyParts);
    for (const QString& section : sections) {
        if (section.startsWith("算法参数：")) {
            param.fromString(section);
            break;
        }
    }
    return param;
}
