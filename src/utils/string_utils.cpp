//
// Created by ddeeaaddllyy on 08.01.2026.
//
#define WIN32_LEAN_AND_MEAN

#include "../../include/EchoEngine/utils/string_utils.h"

#include <handleapi.h>
#include <windows.h>
#include <winnls.h>
#include <winbase.h>
#include <wincon.h>
#include <winnls.h>

using namespace std;

string StringUtils::utf16_to_utf8(const wstring& wstr) {
    if (wstr.empty()) return {};

    const int size = WideCharToMultiByte(
        CP_UTF8,
        0,
        wstr .data(),
        static_cast<int>(wstr.size()),
        nullptr,
        0,
        nullptr,
        nullptr
    );

    std::string result(size, 0);

    WideCharToMultiByte(
        CP_UTF8,
        0,
        wstr.data(),
        static_cast<int>(wstr.size()),
        result.data(),
        size,
        nullptr,
        nullptr
    );

    return result;
}

void StringUtils::enableAnsiColors() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return;

    DWORD mode = 0;
    if (!GetConsoleMode(hOut, &mode)) return;

    mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, mode);
}