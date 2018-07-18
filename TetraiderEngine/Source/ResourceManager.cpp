/* Start Header -------------------------------------------------------
Copyright (C) 2018 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior
written consent of DigiPen Institute of Technology is prohibited.
Author: <Holden Profit>
- End Header --------------------------------------------------------*/

#include <Stdafx.h>

namespace fs = std::experimental::filesystem;

ResourceManager::ResourceManager(){}

ResourceManager::~ResourceManager() 
{
	UnloadAll();
}

#pragma region Private Methods

void ResourceManager::_RenderFont(FontInfo & fontInfo)
{
	if (!fontInfo.IsGlyphProvided())
		return;

	SDL_Color fg = { 0,0,0,255 };

	for (int i = 0; i<128; i++)
	{
		/* cache rendered surface */
		fontInfo.m_text[i] = TTF_RenderGlyph_Blended(fontInfo.m_font, i + fontInfo.m_startGlyph, fg);
		if (!fontInfo.m_text[i])
			std::cout << "TTF_RenderGlyph_Shaded: " << TTF_GetError() << std::endl;

		/* cache metrics */
		TTF_GlyphMetrics(
			fontInfo.m_font, i + fontInfo.m_startGlyph,
			&fontInfo.m_metrics[i].minX, &fontInfo.m_metrics[i].maxX,
			&fontInfo.m_metrics[i].minY, &fontInfo.m_metrics[i].maxY,
			&fontInfo.m_metrics[i].advance
		);
		int mode = fontInfo.m_text[i]->format->BytesPerPixel == 4 ? GL_RGBA : GL_RGB;
		fontInfo.m_texBuffers[i] = _CreateTextureBuffer(fontInfo.m_text[i], mode);
	}
}

FontInfo* ResourceManager::_LoadFont(const Font_Size & font_size)
{
	TTF_Font* font = TTF_OpenFont((TETRA_GAME_CONFIG.FontsDir() + font_size.name).c_str(), font_size.size);
	if (!font) {
		std::cout << "TTF_OpenFont could not open font: " << font_size.name << " size: " << font_size.size << " :: Error: " << TTF_GetError() << std::endl;
		return nullptr;
	}

	FontInfo* pFontInfo = new FontInfo(font, 0, font_size.size);
	_RenderFont(*pFontInfo);

	m_fonts[font_size] = pFontInfo;
	return pFontInfo;
}

GLuint ResourceManager::_CreateTextureBuffer(const SDL_Surface * const sdlSurface, int alphaMode)
{
	GLuint textureBufferID;
	glGenTextures(1, &textureBufferID);
	glBindTexture(GL_TEXTURE_2D, textureBufferID);
	//glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	//glTexStorage2D(GL_TEXTURE_2D, 8, alphaMode, sdlSurface->w, sdlSurface->h);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, alphaMode,
		sdlSurface->w, sdlSurface->h, 0,
		alphaMode,
		GL_UNSIGNED_BYTE, sdlSurface->pixels);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, repeatOrClamp);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, repeatOrClamp);

	return textureBufferID;
}

#pragma endregion

#pragma region Sound

void ResourceManager::Load(Sound_Category type, const std::string & path)
{
	if (m_Sounds[type].find(path) != m_Sounds[type].end())
		return;

	FMOD::Sound* sound;
	TETRA_AUDIO.ErrorCheck(TETRA_AUDIO.getSystem()->createSound(path.c_str(), TETRA_AUDIO.getMode()[type], 0, &sound));
	m_Sounds[type][path] = sound;
}

void ResourceManager::LoadSFX(const std::string & path)
{
	Load(SFX, TETRA_GAME_CONFIG.SFXDir() + path);
}

void ResourceManager::LoadSong(const std::string & path)
{
	Load(SONG, TETRA_GAME_CONFIG.SFXDir() + path);
}

FMOD::Sound* ResourceManager::GetSFX(const std::string& path, Sound_Category type)
{
	FMOD::Sound* sound = m_Sounds[type][path];
	if (sound)
		return sound;

	std::cout << path  <<" Invalid sound name." << std::endl;
	return 0;
}

#pragma endregion

bool ResourceManager::Init()
{
	std::shared_ptr<Mesh> quad = LoadInternalMesh(QUAD_MESH);

	quad->AddVertex(-0.5f, -0.5f, 0.0f, TexCoords(0.f, 0.f));
	quad->AddVertex( 0.5f, -0.5f, 0.0f, TexCoords(1.f, 0.f));
	quad->AddVertex( 0.5f,  0.5f, 0.0f, TexCoords(1.f, 1.f));
	quad->AddVertex(-0.5f,  0.5f, 0.0f, TexCoords(0.f, 1.f));

	quad->AddFace(0, 1, 3);
	quad->AddFace(1, 2, 3);

	quad->FinishMesh();

	std::shared_ptr<Mesh> screenQuad = LoadInternalMesh(SCREEN_QUAD_MESH);

	screenQuad->AddVertex(-1.f, -1.f, 0.0f, TexCoords(0.f, 0.f));
	screenQuad->AddVertex( 1.f, -1.f, 0.0f, TexCoords(1.f, 0.f));
	screenQuad->AddVertex( 1.f,  1.f, 0.0f, TexCoords(1.f, 1.f));
	screenQuad->AddVertex(-1.f,  1.f, 0.0f, TexCoords(0.f, 1.f));

	screenQuad->AddFace(0, 1, 3);
	screenQuad->AddFace(1, 2, 3);

	screenQuad->FinishMesh();

	m_meshes[PLANE_MESH_DEFAULT] = std::make_shared<Mesh>(Plane(PLANE_MESH_DEFAULT_SIZE));
	m_scenes[PLANE_MESH_DEFAULT] = std::make_shared<Scene>(Scene(1));
	(*m_scenes[PLANE_MESH_DEFAULT].get())[0] = m_meshes[PLANE_MESH_DEFAULT];

	m_pDebugLineMesh = new DebugLineMesh(.5f, .0f, .0f, -.5f, .0f, .0f);
	LoadPrefabFiles();

	return true;
}

#pragma region Mesh

std::shared_ptr<Mesh> ResourceManager::_CreateMesh(const std::string & meshName)
{
	std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(Mesh());
	m_meshes[meshName] = mesh;
	return mesh;
}

std::shared_ptr<Mesh> ResourceManager::_CreateMesh(const std::string& meshName, const aiMesh* aiMeshPtr)
{
	std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(Mesh(aiMeshPtr));
	m_meshes[meshName] = mesh;
	return mesh;
}

std::shared_ptr<Scene> ResourceManager::_CreateScene(const std::string & sceneName, unsigned short meshCount)
{
	std::shared_ptr<Scene> scene = std::make_shared<Scene>(Scene(meshCount));
	m_scenes[sceneName] = scene;
	return scene;
}

DebugLineMesh * ResourceManager::GetDebugLineMesh()
{
	return m_pDebugLineMesh;
}

std::shared_ptr<Mesh> ResourceManager::LoadInternalMesh(const std::string & meshName)
{
	std::shared_ptr<Mesh> mesh = m_meshes[meshName];

	if (mesh)
		return mesh;

	mesh = std::shared_ptr<Mesh>(new Mesh());
	if (mesh)
		m_meshes[meshName] = mesh;

	return mesh;
}

std::shared_ptr<Mesh> ResourceManager::LoadMesh(const std::string& meshName)
{
	std::shared_ptr<Mesh> mesh (new Mesh());

	const aiScene* scene = m_importer.ReadFile(TETRA_GAME_CONFIG.MeshesDir() + meshName, 
		aiProcess_CalcTangentSpace | 
		aiProcess_Triangulate | 
		aiProcess_JoinIdenticalVertices | 
		aiProcess_SortByPType 
	);

	if (scene->HasMeshes()) {
		for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
			const aiMesh* aiMeshPtr = scene->mMeshes[i];
			//m_meshes[mesh->mName.C_Str()] = std::shared_ptr<Mesh>(new Mesh(mesh));
			m_meshes[meshName] = std::shared_ptr<Mesh>(new Mesh(aiMeshPtr));

			// SHOULD BE WORKED BETTER HERE
			mesh = m_meshes[meshName];
		}
	}

	return mesh;
}

std::shared_ptr<Mesh> ResourceManager::GetInternalMesh(const std::string & meshName)
{
	std::shared_ptr<Mesh> mesh = m_meshes[meshName];

	if (mesh)
		return mesh;
	else {
		return LoadInternalMesh(meshName);
	}
}

std::shared_ptr<Mesh> ResourceManager::GetMesh(const std::string& meshName)
{
	std::shared_ptr<Mesh> mesh = m_meshes[meshName];

	if (mesh)
		return mesh;
	else {
		return LoadMesh(meshName);
	}
}

std::shared_ptr<Scene> ResourceManager::LoadScene(const std::string& meshSceneName)
{
	const aiScene* scene = m_importer.ReadFile(TETRA_GAME_CONFIG.MeshesDir() + meshSceneName,
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType
	);

	if (scene->HasMeshes()) {
		std::shared_ptr<Scene> meshScene = make_shared<Scene>(Scene(scene->mNumMeshes));
		m_scenes[meshSceneName] = meshScene;
		for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
			const aiMesh* aiMeshPtr = scene->mMeshes[i];
			m_meshes[aiMeshPtr->mName.C_Str()] = std::shared_ptr<Mesh>(new Mesh(aiMeshPtr));
			(*meshScene.get())[i] = m_meshes[aiMeshPtr->mName.C_Str()];
		}
		return meshScene;
	}
	else
		return nullptr;
}

std::shared_ptr<Scene> ResourceManager::GetScene(const std::string& meshSceneName)
{
	std::shared_ptr<Scene> meshScene = m_scenes[meshSceneName];

	if (meshScene)
		return meshScene;
	else
		return LoadScene(meshSceneName);
}

void ResourceManager::UnloadMesh(const std::string& meshName)
{
	if (m_meshes[meshName] && m_meshes[meshName].unique()) {
		m_meshes[meshName].reset();
		m_meshes.erase(meshName);
	}
}

#pragma endregion

FontInfo * ResourceManager::GetFont(const Font_Size & font_size)
{
	FontInfo * pFontInfo = m_fonts[font_size];

	if (pFontInfo)
		return pFontInfo;
	else
		return _LoadFont(font_size);
}

#pragma region Texture

SurfaceTextureBuffer * ResourceManager::_LoadTexture(std::string textureName)
{
	SurfaceTextureBuffer * stbuff = m_textures[textureName];

	if (stbuff)	return stbuff;

	// Load image
	SDL_Surface* loadedSurface = IMG_Load((TETRA_GAME_CONFIG.TexturesDir() + textureName).c_str());
	if (!loadedSurface) {
		std::cout << "Failed to load image \"" << textureName << "\" :: SDL Error : " << SDL_GetError() << std::endl;
		return nullptr;
	}
	else {
		int mode = loadedSurface->format->BytesPerPixel == 4 ? GL_RGBA : GL_RGB;
		stbuff = new SurfaceTextureBuffer(loadedSurface, _CreateTextureBuffer(loadedSurface, mode), mode);
		m_textures[textureName] = stbuff;
		return stbuff;
	}
}

SurfaceTextureBuffer * ResourceManager::GetTexture(const std::string& textureName)
{
	if (textureName == "") return nullptr;

	SurfaceTextureBuffer * stbuff = m_textures[textureName];

	if (stbuff)
		return stbuff;
	else
		return _LoadTexture(textureName);
}

void ResourceManager::UnloadTexture(const std::string& textureName)
{
	if (m_textures[textureName]) {
		SDL_FreeSurface(m_textures[textureName]->surface);
		delete m_textures[textureName]->surface;
		delete m_textures[textureName];

		m_textures.erase(textureName);
	}
}

#pragma endregion

void ResourceManager::UnloadAll()
{
	for (auto comp : m_meshes) {
		if (comp.second && comp.second.unique())
			comp.second.reset();
	}
	m_meshes.clear();

	for (auto comp : m_textures) {
		if (comp.second) {
			SDL_FreeSurface(comp.second->surface);
			delete comp.second;
		}
	}
	m_textures.clear();

	for (auto comp : m_prefabs) {
		if (comp.second) {
			delete comp.second;
		}
	}

	m_prefabs.clear();

	//Release sound in each category 
	// TODO: Double check if there are any memory leaks with this method
	for (auto comp : m_Sounds[SONG]) {
		if (comp.second) {
			TETRA_AUDIO.ErrorCheck(comp.second->release());
		}
	}
	m_Sounds[SONG].clear();

	for (auto comp : m_Sounds[SFX]) {
		if (comp.second) {
			TETRA_AUDIO.ErrorCheck(comp.second->release());
		}
	}
	m_Sounds[SFX].clear();
}

#pragma region Prefabs

void ResourceManager::LoadPrefabFiles() {
	std::string path = TETRA_GAME_CONFIG.PrefabsDir();
	for (auto &p : fs::directory_iterator(path)) {
		json* j = new json();
		*j = OpenJsonFile(p.path().string());
		std::string s = p.path().filename().string();
		m_prefabs[s] = j;
		s.erase(s.end() - 5, s.end());
		m_prefabStrings.push_back(s);
	}
}

json* ResourceManager::GetPrefabFile(const std::string& path) {
	json* file = m_prefabs[path];
	if (file)
		return file;

	//std::cout << "Could not Get prefab json. Invalid prefab name: " << path << std::endl;
	return 0;
}

#pragma endregion
