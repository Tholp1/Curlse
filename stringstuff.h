#pragma once
#include <iostream>
#include <filesystem>
#include <string>
namespace fs = std::filesystem;

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
    for (char &c : stringin)
    {
        if (c == what)
            c = with;
    }
    return stringin;
}


static fs::path RemoveXLeadingFolders(int x , fs::path path)
{
    if (x < 0)
        return path;
    std::string pathstring = path;
    std::string newstring;
    int i = 0;
    for (char c : pathstring)
    {   
        if (x == 0)
            newstring += c;
        if (c == char("/") || c == char("\\"))
        {
            x--;
        }
        i++;
    }
    return newstring;
}