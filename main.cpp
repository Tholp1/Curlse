#include <filesystem>
#include <fstream>
#include <future>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stringstuff.h>
#include <structs.h>
#include <vector>
#include <libzippp/libzippp.h>
#include <json.cpp>
#include <http.cpp>


namespace zippp = libzippp;
namespace fs = std::filesystem;

int ThreadsUsed = 0;
//just to make async a bit easier
bool dlmod(Mod& mod, fs::path path, ProgressPercent* progress)
{
    ThreadsUsed++;
    bool success = DownloadMod(mod, path);
    printf("\na\n");
    ThreadsUsed--;
    return success;
}

int main (int argc, char *argv[])
{

    if (argc < 2)
    {
        printf("Please specify a modpack manifest zip");
        return EXIT_FAILURE;
    }

    if (argc < 3)
    {
        printf("Please specify a folder to install to");
        return EXIT_FAILURE;
    }

    std::string pack = argv[1];
    fs::path path = argv[2];

    std::string zipcheck = GetLastXChars(5, pack);
    bool localmanifest = !strcmp(zipcheck.c_str(), ".zip");//is this a localaly stored manifest?
    if (!localmanifest)//assume its a url
    {
        printf("not a zip file\n");
        return EXIT_FAILURE;
    }

    fs::path file(pack.c_str());
      
    if (!fs::exists(file))
    {
        printf("File not found");
        return EXIT_FAILURE;
    }

    if (!fs::exists(path))
        fs::create_directories(path);


    //this is some bullshit
    //have to create a file stream of the zip and create a libzippp class from that because the standard way doesnt work...

    std::ifstream zip(file.lexically_normal().string(), std::ios::binary);//| std::ios::ate);
    zip.unsetf(std::ios::skipws);
    zip.seekg(0, std::ios::end);
    std::streamsize size = zip.tellg();
    zip.seekg(0, std::ios::beg);
    std::vector<char> buffer(size);

    if (!zip.read(buffer.data(), size))
    {
        printf("File read error!!\n");
        return EXIT_FAILURE;
    }

    zippp::ZipArchive *zf = zippp::ZipArchive::fromBuffer(buffer.data(), size);
    if (!zf)
    {
        printf("Zip read error!!\n");
        return EXIT_FAILURE;
    }

    zippp::ZipEntry JsonEntry = zf->getEntry("manifest.json", false, false);
    zippp::ZipEntry HtmlEntry = zf->getEntry("modlist.html", false, false);
    if (JsonEntry.isNull() || HtmlEntry.isNull())
    {
        printf("Not a pack manifest zip, Aborting.");
        zf->close();
        return EXIT_FAILURE;
    }
    else
        printf("Manifest found, downloading.\n");
    
    std::string json = (char*)zf->readEntry(JsonEntry, true);
    std::string html = (char*)zf->readEntry(HtmlEntry, true);

    std::vector<Mod> Modlist;
    bool success = ParseManifest(json, Modlist);
    if (!success)
    {
        printf("Json read error, corrupted manifest?");
        zf->close();
        return EXIT_FAILURE;
    }

    ProgressPercent progress(Modlist.size(), 2.5f, "Getting Jar names.");
    for (Mod &mod : Modlist)
    {
        std::string name = GetJarName(mod);
        if (name.empty())
        {
            remove("Curlsetemp.txt");
            return EXIT_FAILURE;
        }
        mod.JarName = name;
        progress.Update();
    }
    remove("Curlsetemp.txt");

    ProgressPercent progress2( Modlist.size(), 2.5f, "Downloading mods.");
    //std::vector<std::future<bool>> Waitlist;
    int i = 0;
    for (Mod& mod : Modlist)
    {
        if(DownloadMod(mod, path) > 0)
        {
            printf("Mod \"%s\" Failed to download! Aborting.\n", mod.JarName.c_str());
            return EXIT_FAILURE;
        };
        progress2.Update();
    }
    /*
    i = 0;
    for (auto &x : Waitlist)
    {
        i++;
        if (!x.get())
        {
            printf("Mod %s failed to download", Modlist[i].JarName.c_str());
            return EXIT_FAILURE;
        }
    }
    */

    
    zf->close();
    return EXIT_SUCCESS;
}