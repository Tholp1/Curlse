//#pragma once
#include <iostream>
#include <vector>
#include <structs.h>
#include <string>
#include <stringstuff.h>
#include <json/json.h>


static bool ParseManifest(std::string &JsonString, ModPack &Pack)
{
	Json::Reader reader;
	Json::Value Json;
	reader.parse(JsonString.c_str(), Json);
	if (Json.isNull())
	{
		printf("manifest.json unable to be loaded.\n");
		return false;
	}
	if (strcmp(Json["manifestType"].asCString(), "minecraftModpack"))
	{
		printf("Not a modpack manifest.\n");
		return false;
	}

	Json::Value Files = Json["files"];
	if (Files.isNull())
	{
		printf("Invalidly formated manifest.json.\n");
		return false;
	}
	for (int i = 0; i < Files.size(); ++i)
	{
		Mod mod;
		mod.ModId = Files[i]["projectID"].asInt();
		mod.FileId = Files[i]["fileID"].asInt();
        if (Files[i]["downloadUrl"].asCString())
            mod.DownloadUrl = Files[i]["downloadUrl"].asCString();
		Pack.Modlist.push_back(mod);
	}

	Pack.Name		= Json["name"].asString();
	Pack.Version	= Json["version"].asString();
	Pack.MCVersion	= Json["minecraft"]["version"].asString();

	std::string modloaderstr	= Json["minecraft"]["modLoaders"][0]["id"].asString();
    std::vector<std::string> vec = Split(modloaderstr, '-');

	if(!strcmp(vec[0].c_str(), "forge"))
	{
		Pack.ModLoader = "net.minecraftforge";
	}
	else if (!strcmp(vec[0].c_str(), "fabric"))
	{
		Pack.ModLoader = "net.fabricmc.fabric-loader";
	}
	else
	{
		printf("Unrecognized modloader, aborting.");
		return false;
	}
	Pack.ModLoaderVersion = vec[1];

	return true;
}
