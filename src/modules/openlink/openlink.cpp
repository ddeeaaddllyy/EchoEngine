//
// Created by user on 09.01.2026.
//

#include "../../../include/EchoEngine/modules/openlink/openlink.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>
#include <format>
#include <iostream>


void OpenLink::open_link(const std::string& inputLink) {
    ShellExecuteW(
        nullptr,
        L"open",
        std::wstring(inputLink.begin(), inputLink.end()).c_str(),
        nullptr,
        nullptr,
        SW_MAXIMIZE
    );

    std::wcout << L"[+] - Link successfully opened";
}
