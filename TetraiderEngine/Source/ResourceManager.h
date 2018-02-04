/* Start Header -------------------------------------------------------
Copyright (C) 2018 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior
written consent of DigiPen Institute of Technology is prohibited.
File Name: ResourceManager.h
Purpose: Loads Meshes and other commonly used game resources into
memory and manages their instances as well as returns references to them.
Language: C++
Project: GAM541
Author: Holden Profit
Creation date: 1/17/18
- End Header --------------------------------------------------------*/

#pragma once

#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include "Mesh.h"
#include "DebugLineMesh.h"
#include "STBSurface.h"
#include <unordered_map>
#include <utility>
#include "fmod_errors.h"
#include "fmod_studio.hpp"
#include "fmod.hpp"
#include "JsonReader.h"

using json = nlohmann::json;
using namespace JsonReader;

enum Sound_Category { SFX, SONG, CATEGORY_COUNT };
typedef std::unordered_map<std::string, FMOD::Sound*> SoundMap;
class ResourceManager
{
private:
	struct TextureInfo {
		std::string filename;
		bool hasAlpha;
	};
	
	DebugLineMesh * m_pDebugLineMesh;
	std::unordered_map<std::string, Mesh*> m_meshes;
	std::unordered_map<std::string, SurfaceTextureBuffer * > m_textures;
	std::map<std::string, json*> m_prefabs;

	SoundMap m_Sounds[CATEGORY_COUNT];
	
	GLuint _CreateTextureBuffer(const STB_Surface * const stbSurface);
	TextureInfo _LoadTextureInfoFile(std::string textureInfoFilePath, std::string texturesDir, bool hasAlpha);
	SurfaceTextureBuffer * _LoadTexture(std::string textureName, bool hasAlpha);

public:
	ResourceManager();
	~ResourceManager();
	ResourceManager(const ResourceManager &) = delete;
	void operator=(const ResourceManager &) = delete;

	bool Init();
	DebugLineMesh* GetDebugLineMesh();
	Mesh * LoadMesh(const std::string& meshName);
	Mesh * GetMesh(const std::string& meshName);
	void UnloadMesh(const std::string& meshName);

	SurfaceTextureBuffer * GetTexture(const std::string& textureName, bool hasAlpha);
	void UnloadTexture(const std::string& textureName);

	void UnloadAll();
	void LoadPrefabFiles();
	void Load(Sound_Category type, const std::string& path);
	void LoadSFX(const std::string& path);
	void LoadSong(const std::string& path);
	json* GetPrefabFile(const std::string& path);
	SoundMap* GetSoundMap();
};

#endif