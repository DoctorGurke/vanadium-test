#pragma once

#include <string>

class Log
{
public:
    static void Info(const std::string &text);
    static void Warning(const std::string &text);
    static void Error(const std::string &text);
    static void System(const std::string &text);
    static void SetColor(int textColor);
    static void ResetColor();
};