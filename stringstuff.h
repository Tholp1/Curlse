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
        if (c == '/' || c == '\\')
        {
            x--;
        }
    }
    return newstring;
}

static std::vector<std::string> Split(std::string stringin, char splitter)
{
    std::string concat;
    std::vector<std::string> vec;
    for (char c : stringin)
    {
        if (c == splitter)
        {
            vec.push_back(concat);
            concat = "";
            continue;
        }
        concat += c;
    }
    vec.push_back(concat);
    return vec;
}
