#include <filesystem>
#include <fstream>
#include <future>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stringstuff.h>
#include <structs.h>
#include <vector>
#include <libzippp/src/libzippp.h>
#include <json.cpp>
#include <http.cpp>
#include <stdio.h>

inline void pause()
{
    printf("PAUSED %i\n", __LINE__);
    char tmp[4];
    std::cin >> tmp;
}

namespace zippp = libzippp;
namespace fs = std::filesystem;

int main (int argc, char *argv[])
{

    if (argc < 3)
    {
        printf("USAGE: curlse [pack manifest zip] [multimc instance folder]\n\n");
        printf(" Example: curlse ~/Downloads/Above+and+Beyond-1.3.zip ~/bin/MultiMC.d/instances\n");
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
    if (JsonEntry.isNull())
    {
        printf("Not a pack manifest zip, Aborting.");
        zf->close();
        return EXIT_FAILURE;
    }
    
    std::string json = (char*)zf->readEntry(JsonEntry, true);

    ModPack Modpack;
    bool success = ParseManifest(json, Modpack);
    if (!success)
    {
        printf("Json read error, corrupted manifest?");
        zf->close();
        return EXIT_FAILURE;
    }
    printf("Manifest found, downloading.\n");

    path += "/" + Modpack.Name + "-" + Modpack.Version + "-" + Modpack.MCVersion;

    if (!fs::exists(path))
    {
        fs::create_directories(path);
    }

    ProgressPercent progress(Modpack.Modlist.size(), 2.5f, "Getting Jar names.");
    for (Mod &mod : Modpack.Modlist)
    {
        if (mod.DownloadUrl.empty())
           {
            std::string name(GetJarName(mod));
            if (name.empty())
            {
                remove("Curlsetemp.txt");
                return EXIT_FAILURE;
            }
            if(fs::exists(fs::path( path.string() + "/.minecraft/mods/" + name)))
                mod.DontDL = true;
            mod.JarName = name;
        }
        progress.Update();
    }
    remove("Curlsetemp.txt"); 

    ProgressPercent progress2( Modpack.Modlist.size(), 2.5f, "Downloading mods.");
    for (Mod& mod : Modpack.Modlist)
    {
        if (mod.DontDL)
        {
            progress2.Update();
            continue;
        }
        int error = DownloadMod(mod, path);
        if(error)
        {
            printf("Mod \"%s\" Failed to download with error code %i! Aborting.\n", mod.DownloadUrl.empty() ? mod.JarName.c_str() : mod.DownloadUrl.c_str(), error);
            return EXIT_FAILURE;
        };
        progress2.Update();
    }

    printf("Extracting Pack configs and resources.\n");
    for (zippp::ZipEntry z: zf->getEntries())
    {
        if (z.getName() == "manifest.json" || z.getName() == "modlist.html")
            continue;
        if (z.isDirectory())
        {
            std::string ending = RemoveXLeadingFolders(1, z.getName()).string();
            if (ending.empty())
                continue;
            fs::create_directories(path.string() + "/.minecraft/" + ending);
            continue;
        }

        std::ofstream overridefile(path.string() + "/.minecraft/" + RemoveXLeadingFolders(1, z.getName()).string());


        if (GetExtention((z.getName())) == "png")
            overridefile << z.readAsBinary();
        else
            overridefile << z.readAsText();
    }
    printf("Creating MultiMc Profile...\n");
    pause();

    if (Modpack.MCVersion.empty() || Modpack.Name.empty() || Modpack.ModLoader.empty() || Modpack.ModLoaderVersion.empty() || Modpack.Version.empty())
    {
        if (Modpack.MCVersion.empty())
            printf("Missing Minecraft version info.\n");
        if (Modpack.Name.empty())
            printf("Missing modpack name info.\n");
        if (Modpack.ModLoader.empty())
            printf("Missing modloader info.\n");
        if (Modpack.ModLoaderVersion.empty())
            printf("Missing modloader version info.\n");
        if (Modpack.Version.empty())
            printf("Missing pack version info.\n");
        printf("Missing vital pack infomation! Cannot continue.\n");
    }



    std::ofstream mmcInstance(path.string() + "/instance.cfg");
    pause();
    mmcInstance << ("InstanceType=OneSix \nname=" + Modpack.Name + " " + Modpack.Version);

    std::ofstream mmcJsonFile(path.string() + "/mmc-pack.json");
    char *mmcPackJson;
    sprintf(mmcPackJson, "\
    {\n \
    \"components\": [\n \
        {\n\
            \"important\": true,\n\
            \"uid\": \"net.minecraft\",\n\
            \"version\": \"%s\"\n\
        },\n\
        {\n\
            \"important\": true,\n\
            \"uid\": \"%s\",\n\
            \"version\": \"%s\"\n\
        }\n\
    ],\n\
    \"formatVersion\": 1\n\
}\
", Modpack.MCVersion.c_str(), Modpack.ModLoader.c_str(), Modpack.ModLoaderVersion.c_str() );

    mmcJsonFile << mmcPackJson;

    zf->close();
    printf("Done!\n");
    return EXIT_SUCCESS;
}
