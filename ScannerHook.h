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

    // 开始扫码
    // timeoutMs < 0  表示无限等待（推荐你现在用）
    bool start(int timeoutMs = -1);

    // 停止扫码（可安全重复调用）
    void stop();

signals:
    // 扫码完成，原样返回字符串
    void scanFinished(const QString& code);

private:
    static LRESULT CALLBACK keyboardProc(int nCode, WPARAM wParam, LPARAM lParam);

    void finish();   // 内部统一结束（emit + stop）

private:
    QString m_buffer;        // 扫码内容
    int     m_timeoutMs;     // 超时（-1 = 无限）
    HHOOK   m_hook = nullptr;
};
