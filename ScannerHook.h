#pragma once

#include <QObject>
#include <QString>
#include <windows.h>

class ScannerHook : public QObject
{
    Q_OBJECT
public:
    explicit ScannerHook(QObject* parent = nullptr);
    ~ScannerHook();

    // 开始一次扫码（超时自动结束，默认 300ms）
    bool start(int timeoutMs = 300);

signals:
    // 扫码完成，原样返回字符串
    void scanFinished(const QString& code);

private:
    static LRESULT CALLBACK keyboardProc(int nCode, WPARAM wParam, LPARAM lParam);

    void finish();  // 内部统一结束

private:
    QString m_buffer;   // 保存原始字符
    int m_timeoutMs = 300;
    HHOOK m_hook = nullptr; // 键盘钩子句柄
};
