#pragma once
#ifndef NPCCONTROLLER_H
#define NPCCONTROLLER_H

#include "Agent.h"
#include <unordered_map>
#include "AI_State.h"
#include "AI_StateTypes.h"
#include "AI_StateFactory.h"
#include "GameObjectTags.h"

class Weapon;

class NPCController : public Agent {
public:
	NPCController();
	~NPCController();
	static Component* CreateInstance() { return new NPCController(); }
	virtual void Deactivate();
	virtual void Update(float dt);
	virtual void LateUpdate(float dt);
	virtual void LateInitialize();
	virtual void Serialize(const json& j);
	virtual void HandleEvent(Event* pEvent);

	void ChangeState(NPC_CONTROLLER_AI);
	void MoveToPlayer();
	void GoToPositionAroundPlayer();
	void GoToStartingPoint();
	bool UseAttack(int attack);
	bool IsArrivedAtDestination();
	bool IsPlayerInSight();
	bool IsPlayerOutOfSight();
	bool IsInAttackRange();
	bool IsTooFarFromStartingPoint();
	void PlayAnimation(int animation);
	void SetDestinationToRandomPointInZone();
	void SetVelocityToZero();
	void StopMoving();
	void SetTargetDestination(const Vector3D& pos);
	void SetPositionBehindPlayer(float distance);
	void LookInDirectionOfMovement();
	void LookAtPlayer();
	void LookAtPlayer(float offsetAngle);
	void SetSpeedMultiplier(float speedMultiplier) { m_speedMultiplier = speedMultiplier; }
	bool RollDie(float probability);
protected:
	Weapon* m_pWeapon;
	float GetSquareDistanceToPlayer();

private:
	AIStateFactory AIStateFactory;
	AI_State* m_AIStates[NPC_NUM_BEHAVIOR];
	NPC_CONTROLLER_AI m_currentState;
	NPC_CONTROLLER_AI m_previousState;
	float m_detectionRadius;
	float m_outOfSightRadius;
	float m_zoneWidth;
	float m_zoneHeight;
	float m_attackRange;
	float m_speedMultiplier;
	Vector3D m_startingPoint;
	Vector3D m_targetDestination;
	bool m_arrivedAtDestination;
	bool m_isPlayerDead;
	const Transform* m_pPlayerTransform;
	GameObjectTag m_tagsToIgnore[3];
};

#endif

