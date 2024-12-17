#include "log.h"

#include <iostream>
using std::cout;
using std::endl;

void Log::Info(const std::string &text)
{
    ResetColor();
    cout << text << endl;
}

void Log::Warning(const std::string &text)
{
    SetColor(33);
    cout << text << endl;
    ResetColor();
}

void Log::Error(const std::string &text)
{
    SetColor(31);
    cout << text << endl;
    ResetColor();
}

void Log::System(const std::string &text)
{
    SetColor(94);
    cout << text << endl;
    ResetColor();
}

void Log::SetColor(int textColor)
{
    cout << "\033[" << textColor << "m";
}

void Log::ResetColor()
{
    cout << "\033[0m";
}