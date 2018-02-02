#pragma once

#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <vector>
#include "Subscriber.h"
#include "GameObjectTags.h"
#include "ComponentTypes.h"

// Forward declaration
class Event;
class Component;
enum LAYER;

class GameObject : public Subscriber 
{
private:
	Component* mComponents[ComponentType::NUM_COMPONENTS];
	//std::vector<Component*> mComponents;
	unsigned int m_id;
	unsigned short m_layer;
public:
	bool m_isActive;
	bool m_isRender;
	bool m_isCollisionDisabled;
	bool m_isDestroy;
	GameObjectTag m_tag;

	GameObject(unsigned int id);
	~GameObject();

	bool operator==(const GameObject& rhs) const;

	virtual void HandleEvent(Event* pEvent);
	void Update(float dt);
	void LateUpdate(float dt);
	void LateInitialize();
	void Destroy();

	void SetLayer(LAYER layer) { m_layer = layer; }
	unsigned short GetLayer() { return m_layer; }
	unsigned int ID() const { return m_id; }
	void AddComponent(Component* pComponent);
	bool HasComponent(ComponentType type) const;

	template <typename C>
	C* GetComponent(ComponentType type) {
		return mComponents[type];
	}

	template <typename C>
	const C * GetComponent(ComponentType type) const
	{
		return mComponents[type];
	}

	bool HasComponent(ComponentType type) const;
	
	bool m_isActive;
	bool m_isRender;
	bool m_isCollisionDisabled;
	bool m_isDestroy;
	GameObjectTag m_tag;

	virtual void HandleEvent(Event* pEvent);
};

#endif 

