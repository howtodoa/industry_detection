#ifndef ALGOCLASS_H
#define ALGOCLASS_H

#include <QObject>
#include "fileoperator.h"


class AlgoClass : public QObject
{
    Q_OBJECT
public:

    explicit AlgoClass(QObject *parent = nullptr);


    explicit AlgoClass(QString algopath, QObject *parent = nullptr);


    bool loadParams();


    bool saveParams();


    const Parameters& getParameters() const { return m_parameters; }


    const ParamDetail* getParamDetail(const QString& projectKey, const QString& paramName) const;


    void updateParamValue(const QString& projectKey, const QString& paramName, const QVariant& newValue);

    void updateParamCheck(const QString& projectKey, const QString& paramName, bool checkValue);

private:
    QString m_cameralPath;    // 内部保存算法参数文件的路径
    Parameters m_parameters;  // 存储所有解析后的结构化算法参数数据


};


#endif // ALGOCLASS_H


