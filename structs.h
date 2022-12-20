#pragma once
#include <iostream>
#include <string>
#include <stringstuff.h>
#include <vector>

struct Mod
{
    int FileId;
    int ModId;
    std::string JarName;
    bool DontDL = false;
};

struct ModPack
{
    std::vector<Mod> Modlist;
    std::string Name;
    std::string ModLoader;
    std::string ModLoaderVersion;
    std::string Version;
    std::string MCVersion;
};


class ProgressPercent
{
private:
    int TasksTotal;
    float TasksDone = 0;
    float MinToUpdate;
    float LastUpdate = 0;

public:
    void Update(int tasks = 1)
    {
        TasksDone += tasks;
        float doneness = (TasksDone / TasksTotal) * 100;

        if ((doneness - MinToUpdate) >= LastUpdate)
        {
            LastUpdate = doneness;
            printf("%.2f%%\n", doneness);
        }
        else if (TasksDone == TasksTotal)
            printf("100%%\n");
        else
            printf(".");
    }
    ProgressPercent(int tasks = 100, float mintoupdate = 1.0f, std::string message = "")
    {
        TasksTotal = tasks;
        MinToUpdate = mintoupdate;
        printf("%s\n0%%\n", message.c_str());
    }
};