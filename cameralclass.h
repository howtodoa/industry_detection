#ifndef CAMERALCLASS_H
#define CAMERALCLASS_H

#include <QObject>
#include <QString>
#include <QDebug>
#include  "fileoperator.h"


class CameralClass : public QObject
{
    Q_OBJECT
public:
    explicit CameralClass(QObject *parent = nullptr);
    explicit CameralClass(QString cameralpath, QObject *parent = nullptr);

    bool loadParams();
    bool saveParams();

    const Parameters& getParameters() const { return m_parameters; }

    const ParamDetail* getParamDetail(const QString& projectKey, const QString& paramName) const;

    void updateParamValue(const QString& projectKey, const QString& paramName, const QVariant& newValue);

    void updateParamCheck(const QString& projectKey, const QString& paramName, bool checkValue);

private:
    QString m_cameralPath;
    Parameters m_parameters;
};

#endif // CAMERALCLASS_H
