//#pragma once
#include <iostream>
#include <fstream>
#include <filesystem>
#include <json/json.h>
#include <string>
#include <vector>
#include <stringstuff.h>

#include <structs.h>

#define API_KEY "$2a$10$QbCxI6f4KxEs50QKwE2piu1t6oOA8ayOw27H9N/eaH3Sdp5NTWwvO"

namespace fs = std::filesystem;

//alright, so the modlist.html and manifest.json list the mods in the same order
//meaning we can just peice together a url and grab the download link and throw it in a vector for later


static std::string GetJarName(Mod &mod)
{
    std::string cmd = "curl -o Curlsetemp.txt -sLX GET \"api.curseforge.com/v1/mods/";

    cmd += std::to_string(mod.ModId);
    cmd += "/files/";
    cmd += std::to_string(mod.FileId);
    cmd += "\"";

    cmd += " -H \'Accept: application/json\' -H \'x-api-key: "; 
    cmd += API_KEY;
    cmd += "\'";
#ifdef Windows
    cmd = ReplaceChar(cmd, "\'", "\"");
#endif

    int error = system(cmd.c_str());
    if (error)
    {
        printf("Curl returned error code %i, Aborting.\n", error);
        return NULL;
    }

    std::ifstream html("Curlsetemp.txt");
    html.seekg(0, std::ios::end);
    std::streamsize size = html.tellg();
    html.seekg(0, std::ios::beg);
    std::vector<char> buffer(size);

    if (!html.read(buffer.data(), size)) 
    {
        printf("File read error!\n");
        return NULL;
    }

    Json::Reader reader;
    Json::Value Json;
    reader.parse(buffer.data(), Json);

    if (Json.isNull())
    {
        printf("Got no response from CurseForge servers.\n");
        return NULL;
    }
    Json::Value File = Json["data"]["fileName"];

    if (File.isNull())
    {
        printf("Unexpected response from CurseForge servers, Aborting.\n");
        return NULL;
    }
    return File.asString();
}

static int DownloadMod(Mod mod, fs::path path)
{
    std::string url = "https://edge.forgecdn.net/files/";

    std::string fileid(std::to_string(mod.FileId));
    std::string idSlug = GetFirstXChars(4, fileid) + "/" + GetLastXChars(4, fileid).c_str();//converting to cstring and back fixes not being able to append more after??
    url += idSlug + "/";
    url += mod.JarName;

    url = ReplaceChar(url, ' ', "%20");

    std::string cmd = "curl --create-dirs -sOJL --output-dir ";
    cmd += "\"" + path.string();
    cmd += "/.minecraft/mods/\" ";
    cmd += "--url \"" + url + "\"";

    cmd += " -H \'Accept: application/json\' -H \'x-api-key: ";
    cmd += API_KEY;
    cmd += "\'";

    //cmd += "?api-key=267C6CA3\" ";
    //cmd += "\" -H \"User - Agent: Mozilla / 5.0 (Windows NT 10.0; Win64; x64; rv:106.0) Gecko / 20100101 Firefox / 106.0\" -H \"Accept : text / html, application / xhtml + xml, application / xml; q = 0.9, image / avif, image / webp, */*;q=0.8\" -H \"Accept-Language: en-US,en;q=0.5\" -H \"Accept-Encoding: gzip, deflate, br\" -H \"Referer: https://www.curseforge.com/\" -H \"DNT: 1\" -H \"Connection: keep-alive\" -H \"Upgrade-Insecure-Requests: 1\" -H \"Sec-Fetch-Dest: document\" -H \"Sec-Fetch-Mode: navigate\" -H \"Sec-Fetch-Site: cross-site\" -H \"Sec-GPC: 1\" -H \"TE: trailers\"";
    //printf(cmd.c_str());
    //return true;
    int err = system(cmd.c_str());
    if (err)
        printf("%s\n",cmd.c_str());
    return err;
}
