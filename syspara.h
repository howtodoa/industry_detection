#ifndef SYSPARA_H
#define SYSPARA_H

#include <QWidget>
#include "public.h"
#include <QFuture>
class SysPara : public QWidget
{
    Q_OBJECT
public:
    explicit SysPara(QWidget *parent = nullptr);
    QFuture<bool> updateSystemDirsToJsonAsync(const QString& filePath);

    static QFuture<bool> updateSystemParamToJsonAsync(
        const QString& filePath,
        const QString& key,
        const QString& newValue
    );
private slots:
    void onModifyClicked(); // 修改按钮槽
    void onCancelClicked(); // 取消按钮槽

private:
    QLineEdit *dataDirEdit; // data_dir 的编辑框
    QLineEdit *logDirEdit;  // log_dir 的编辑框
};

#endif // SYSPARA_H
