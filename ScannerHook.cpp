#include "ScannerHook.h"
#include <QTimer>
#include <QGuiApplication>
#include <windows.h>

static ScannerHook* g_scannerInstance = nullptr;

ScannerHook::ScannerHook(QObject* parent)
    : QObject(parent)
{
    g_scannerInstance = this;
}

ScannerHook::~ScannerHook()
{
    stop();
    g_scannerInstance = nullptr;
}

bool ScannerHook::start(int timeoutMs)
{
    if (m_hook)
        return false;

    m_buffer.clear();
    m_timeoutMs = timeoutMs;

    m_hook = SetWindowsHookEx(
        WH_KEYBOARD_LL,
        keyboardProc,
        GetModuleHandle(nullptr),
        0);

    if (!m_hook)
        return false;

    // 超时（-1 = 无限）
    if (m_timeoutMs > 0)
    {
        QTimer::singleShot(m_timeoutMs, this, [this]() {
            finish();
            });
    }

    return true;
}

void ScannerHook::stop()
{
    if (m_hook)
    {
        UnhookWindowsHookEx(m_hook);
        m_hook = nullptr;
    }
}

void ScannerHook::finish()
{
    stop();

    if (!m_buffer.isEmpty())
    {
        emit scanFinished(m_buffer);
        m_buffer.clear();
    }
}

LRESULT CALLBACK ScannerHook::keyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HC_ACTION && wParam == WM_KEYDOWN)
    {
        auto* key = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);

        if (!g_scannerInstance)
            return CallNextHookEx(nullptr, nCode, wParam, lParam);

        // Enter 结束
        if (key->vkCode == VK_RETURN)
        {
            g_scannerInstance->finish();
            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        }

        // ---- vk -> Unicode ----
        BYTE keyboardState[256];
        GetKeyboardState(keyboardState);

        WCHAR buf[4] = { 0 };
        HKL layout = GetKeyboardLayout(0);

        int ret = ToUnicodeEx(
            key->vkCode,
            key->scanCode,
            keyboardState,
            buf,
            4,
            0,
            layout
        );

        if (ret > 0)
        {
            g_scannerInstance->m_buffer.append(QString::fromWCharArray(buf, ret));
        }
    }

    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}
