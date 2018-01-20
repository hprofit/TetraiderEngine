#ifndef GAME_OBJECT_MANAGER_H
#define GAME_OBJECT_MANAGER_H

#include <string>
#include <vector>
#include "ComponentFactory.h"
#include "RenderManager.h"

// Forward declarations
class GameObject;
class Component;
enum class GameObjectTag;

class GameObjectManager
{
private:
	unsigned int m_currentId;
	ComponentFactory componentFactory;
	std::vector<GameObject*> m_GameObjectsQueue;
	GameObject* m_pCamera;
	RenderManager& renderMngr;

	GameObjectManager();
	~GameObjectManager();
	
	void SetGameObjectTag(std::string tag, GameObject* pGO);
	void AddGameObject(GameObject* pGO);
	void AddGameObjectToQueue(GameObject* pGO);
	void AddGameObjectsFromQueueToMainVector();
	void DestroyAllGameObjects();
	void DestroyGameObjects();

public:
	GameObjectManager(const GameObjectManager &) = delete;
	void operator=(const GameObjectManager &) = delete;

	static GameObjectManager& GetInstance()
	{
		static GameObjectManager instance;
		return instance;
	}

	void Update(float dt);
	void LateUpdate(float dt);
	void RenderGameObjects();

	GameObject* CreateGameObject(std::string name);
	void UpdateStatus();
	// Component* AddComponentToGameObject(GameObject* pGO, json j);
	GameObject* FindObjectWithTag(GameObjectTag tag);
	std::vector<GameObject*> mGameObjects;
};

#endif