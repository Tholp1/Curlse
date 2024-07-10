#pragma once
#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
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

static std::string ReplaceChar(std::string stringin, char what, const char* with)
{
    std::string stringout = "";
    for (char c : stringin)
    {
        if (c == what)
            stringout += with;
        else
            stringout += c;
    }
    return stringout;
}


static fs::path RemoveXLeadingFolders(int x , fs::path path)
{
    if (x < 0)
        return path;
    std::string pathstring = path;
    std::string newstring;
    for (char c : pathstring)
    {   
        if (x == 0)
            newstring += c;
        else if (c == '/' || c == '\\')
            x--;
    }
    return newstring;
}

static fs::path RemoveXTrailingFolders(int x, fs::path path)
{
    if (x <= 0)
        return path;

    for (int i = path.string().length(); i > 0; i--)
    {
        if (path.string()[i] == '/')
            x--;
        if (x == 0)
            return GetFirstXChars(i , path.string());
    }
    return path.string();
}

static std::vector<std::string> Split(std::string stringin, char splitter, int num = 0)//0 for all instances
{
    std::string concat;
    std::vector<std::string> vec;
    int times = 0;
    for (char c : stringin)
    {
        if (c == splitter && (times != num || num <= 0))
        {
            vec.push_back(concat);
            concat = "";
            times++;
            continue;
        }
        concat += c;
    }
    vec.push_back(concat);
    return vec;
}

static std::string GetExtention(std::string stringin)
{
    std::string extention;

    for (auto it = stringin.rbegin(); it != stringin.rend(); it++ )
    {
        if (*it == '.')
            break;
        extention += *it;
    }
    return extention;

}
