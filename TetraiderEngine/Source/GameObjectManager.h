#ifndef GAME_OBJECT_MANAGER_H
#define GAME_OBJECT_MANAGER_H

#include <string>
#include <vector>
#include "ComponentFactory.h"
#include "Subscriber.h"

// Forward declarations
class GameObject;
class Component;
class Event;
enum class GameObjectTag;

class GameObjectManager: public Subscriber
{
private:
	unsigned int m_currentId;
	ComponentFactory componentFactory;
	std::vector<GameObject*> m_GameObjectsQueue;
	GameObject* m_pCamera;
	std::vector<GameObject*> mGameObjects;
	
	void SetGameObjectTag(std::string tag, GameObject* pGO);
	void AddGameObject(GameObject* pGO);
	void AddGameObjectToQueue(GameObject* pGO);
	void AddGameObjectsFromQueueToMainVector();
	void DestroyAllGameObjects();
	void DestroyGameObjects();
	void HandleEvent(Event *pEvent);

public:
	GameObjectManager();
	~GameObjectManager();
	GameObjectManager(const GameObjectManager &) = delete;
	void operator=(const GameObjectManager &) = delete;

	void Update(float dt);
	void LateUpdate(float dt);
	void RenderGameObjects();

	GameObject* CreateGameObject(std::string name);
	GameObjectTag GameObjectManager::FindTagWithString(std::string tag);
	void UpdateStatus();
	// Component* AddComponentToGameObject(GameObject* pGO, json j);
	GameObject* FindObjectWithTag(GameObjectTag tag);
	
	GameObject* GetActiveCamera();
};

#endif