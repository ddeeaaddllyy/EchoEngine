//
// Created by ddeeaaddllyy on 08.01.2026.
//

#ifndef ECHOENGINE_STRING_UTILS_H
#define ECHOENGINE_STRING_UTILS_H
#include <string>

using namespace std;

class StringUtils {
public:
    static string utf16_to_utf8(const wstring& wstr);

    static void enableAnsiColors();
};

#endif //ECHOENGINE_STRING_UTILS_H