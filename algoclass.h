#ifndef ALGOCLASS_H
#define ALGOCLASS_H

#include <QObject>
#include "fileoperator.h"
#include "CapacitanceProgram.h"


class AlgoClass : public QObject
{
    Q_OBJECT
public:

    explicit AlgoClass(QObject *parent = nullptr);

    explicit AlgoClass(QString algopath, QObject *parent = nullptr);

    QPixmap convertMatToPixmap(const cv::Mat& mat);

    virtual QWidget* createLeftPanel(QWidget* parent);
    virtual void saveParamAsync();
    cv::Mat image;
    QString m_cameralPath;
private:
    

signals:
    void TransMat(cv::Mat mat);
};


#endif // ALGOCLASS_H


