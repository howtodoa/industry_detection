#include "ScannerHook.h"
#include <QTimer>
#include <QCoreApplication>

ScannerHook* g_scannerInstance = nullptr; // 用于回调识别实例

ScannerHook::ScannerHook(QObject* parent)
    : QObject(parent)
{
    g_scannerInstance = this;
}

ScannerHook::~ScannerHook()
{
    if (m_hook)
    {
        UnhookWindowsHookEx(m_hook);
        m_hook = nullptr;
    }
    g_scannerInstance = nullptr;
}

bool ScannerHook::start(int timeoutMs)
{
    if (m_hook)
        return false; // 已经在监听

    m_buffer.clear();
    m_timeoutMs = timeoutMs;

    HINSTANCE hInstance = GetModuleHandle(nullptr);
    // WH_KEYBOARD_LL = 13，低级键盘钩子，全局
    m_hook = SetWindowsHookEx(WH_KEYBOARD_LL, keyboardProc, hInstance, 0);
    if (!m_hook)
        return false;

    // 超时处理
    if (m_timeoutMs > 0)
    {
        QTimer::singleShot(m_timeoutMs, this, [this]() {
            finish();
            });
    }

    return true;
}

void ScannerHook::finish()
{
    if (!m_buffer.isEmpty())
    {
        emit scanFinished(m_buffer);
        m_buffer.clear();
    }

    // 卸载钩子
    if (m_hook)
    {
        UnhookWindowsHookEx(m_hook);
        m_hook = nullptr;
    }
}

// 键盘钩子回调
LRESULT CALLBACK ScannerHook::keyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HC_ACTION && (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN))
    {
        KBDLLHOOKSTRUCT* key = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
        DWORD vkCode = key->vkCode;

        if (!g_scannerInstance)
            return CallNextHookEx(nullptr, nCode, wParam, lParam);

        // Enter / Return 结束
        if (vkCode == VK_RETURN)
        {
            g_scannerInstance->finish();
        }
        else
        {
            // 直接把字符转换成 Qt 字符
            // 注意：这里只是简单示例，只能处理 ASCII 可打印字符
            if (vkCode >= 32 && vkCode <= 126)
            {
                g_scannerInstance->m_buffer += QChar(static_cast<ushort>(vkCode));
            }
        }
    }

    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}
