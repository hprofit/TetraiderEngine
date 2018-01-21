#ifndef GAME_CONFIG_H
#define GAME_CONFIG_H

#include <string>
#include "JsonReader.h"

using json = nlohmann::json;

class GameConfig
{
private:
	GameConfig();
	~GameConfig();

public:
	GameConfig(const GameConfig &) = delete;
	void operator=(const GameConfig &) = delete;

	static GameConfig& GetInstance()
	{
		static GameConfig instance;
		return instance;
	}

	void LoadConfig(std::string);

	std::string m_texturesDir;
	std::string m_levelFilesDir;
	std::string m_prefabsDir;
	std::string m_sfxDir;
	std::string m_shadersDir;
};

#endif