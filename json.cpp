//#pragma once
#include <iostream>
#include <vector>
#include <structs.h>
#include <string>
#include <stringstuff.h>
#include <json/json.h>

static bool ParseManifest(std::string &JsonString, std::vector<Mod> &List)
{
	Json::Reader reader;
	Json::Value Json;
	reader.parse(JsonString.c_str(), Json);
	if (Json == NULL)
	{
		printf("manifest.json unable to be loaded.\n");
		return false;
	}
	Json::Value Files = Json["files"];
	if (Files == NULL)
	{
		printf("Invalidly formated manifest.json.\n");
		return false;
	}
	for (int i = 0; i < Files.size(); ++i)
	{
		Mod mod;
		mod.ModId = Files[i]["projectID"].asInt();
		mod.FileId = Files[i]["fileID"].asInt();
		List.push_back(mod);
	}

	return true;
}