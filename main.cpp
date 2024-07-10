#include <filesystem>
#include <fstream>
#include <future>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stringstuff.h>
#include <structs.h>
#include <vector>
#include <zip.h>
#include <curl/curl.h>
#include <json.cpp>
#include <http.cpp>
#include <stdio.h>

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

    int err;
    zip_t *zf = zip_open(file.lexically_normal().string().c_str(), 0, &err);

    if (!zf)
    {
        zip_error_t error;
        zip_error_init_with_code(&error, err);
        printf("Zip read error: %i / %s\n", error, zip_error_strerror(&error));
        zip_error_fini(&error);
        return EXIT_FAILURE;
    }

    //zippp::ZipEntry JsonEntry = zf->getEntry("manifest.json", false, false);
    zip_file_t *JsonEntry = zip_fopen(zf, "manifest.json", 0);
    if (!JsonEntry)
    {
        printf("Not a pack manifest zip, Aborting.");
        zip_close(zf);
        return EXIT_FAILURE;
    }
    
    zip_stat_t manifestStat;
    zip_stat(zf, "manifest.json", 0, &manifestStat);
    void *JsonBuf = malloc(manifestStat.size + 1);
    zip_fread(JsonEntry, JsonBuf, manifestStat.size);

    //std::string json = (char*)zf->readEntry(JsonEntry, true);
    std::string json((const char*)JsonBuf, manifestStat.size);
    free(JsonBuf);

    ModPack Modpack;
    if (!ParseManifest(json, Modpack))
    {
        printf("Json read error, corrupted manifest?");
        zip_close(zf);
        return EXIT_FAILURE;
    }
    printf("Manifest found, downloading.\n");

    CURL *curl = curl_easy_init();
    if (!curl)
    {
        printf("Unable to Init CurlLib.\n");
        return EXIT_FAILURE;
    }
    if (path.string().back() != '/')
        path += "/";
    path += Modpack.Name + "-" + Modpack.Version + "-" + Modpack.MCVersion;

    if(!fs::exists(path / ".minecraft/mods"))
        fs::create_directories(path / ".minecraft/mods");

    ProgressPercent progress(Modpack.Modlist.size(), 2.5f, "Getting Jar names.");
    for (Mod &mod : Modpack.Modlist)
    {
        if (mod.DownloadUrl.empty())
           {
            std::string name(GetJarName(mod, curl));
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
        int error = DownloadMod(mod, path, curl);
        if(error)
        {
            printf("Mod \"%s\" Failed to download with error code %i! Aborting.\n", mod.DownloadUrl.empty() ? mod.JarName.c_str() : mod.DownloadUrl.c_str(), error);
            return EXIT_FAILURE;
        };
        progress2.Update();
    }

    printf("Extracting Pack configs and resources.\n");
    zip_uint64_t num_entries = zip_get_num_entries(zf, 0);
    for (zip_uint64_t i = 0; i < num_entries; i++)
    {
        std::string name = zip_get_name(zf, i, 0);
        if (name == "manifest.json" || name == "modlist.html")
            continue;
        if (name.back() == '/')//dirrectory
        {
            std::string ending = RemoveXLeadingFolders(1, name);
            if (ending.empty())
                continue;
            fs::create_directories(path.string() + "/.minecraft/" + ending);
            continue;
        }

        std::string overridePath = path.string() + "/.minecraft/" + RemoveXLeadingFolders(1, name).string();
        fs::path overrideFolder = fs::path(RemoveXTrailingFolders(1, fs::path(overridePath)));
        if (!fs::exists( overrideFolder))
            fs::create_directories(overrideFolder);

        std::ofstream overrideFile(overridePath);


        zip_file_t *zippedFile = zip_fopen(zf, name.c_str(), 0);
        zip_stat_t entryStat; zip_stat(zf, name.c_str(), 0, &entryStat);
        void *buf = malloc(entryStat.size + 1);

        zip_fread(zippedFile, buf, entryStat.size);
        overrideFile.write((const char*)buf, entryStat.size);
        printf("%s\n", entryStat.name);
        free(buf);
    }
    printf("Creating MultiMc Profile...\n");

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
    mmcInstance << ("InstanceType=OneSix \nname=" + Modpack.Name + " " + Modpack.Version);

    std::ofstream mmcJsonFile(path.string() + "/mmc-pack.json");
    char mmcPackJson[2048];
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
}\0", Modpack.MCVersion.c_str(), Modpack.ModLoader.c_str(), Modpack.ModLoaderVersion.c_str() );
    mmcJsonFile << mmcPackJson;

    zip_close(zf);
    printf("Done!\n");
    return EXIT_SUCCESS;
}
