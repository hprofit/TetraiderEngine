#pragma once
#ifndef CAM_FOLLOW_H
#define CAM_FOLLOW_H

#include "Component.h"
class Transform;
class GameObject;

class CamFollow : public Component
{
private:
	Transform* m_pTransform;
	GameObject* m_pTarget;
	float m_followSpeed;
	std::string m_defaultTargetTag;
public:
	CamFollow();
	~CamFollow();

	virtual void LateInitialize();
	virtual void Update(float dt);
	virtual void LateUpdate(float dt);
	virtual void Serialize(json j);
	virtual void HandleEvent(Event* pEvent);
};

#endif
