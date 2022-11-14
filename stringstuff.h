#pragma once
#include <iostream>
#include <string>

static std::string GetLastXChars(int chars, std::string stringin)
{
    std::string out;
    while (chars-->0)
    {
        out += stringin[stringin.length() - chars];
    }
    return out;
}

static std::string GetFirstXChars(int chars, std::string stringin)
{
    std::string out;
    int x = 0;
    while (x++ < chars)
    {
        out += stringin[x-1];
    }
    return out;
};

static std::string ReplaceChar(std::string stringin, char what, char with)
{
    for (char &x : stringin)
    {
        if (x == what)
            x = with;
    }
    return stringin;
}


