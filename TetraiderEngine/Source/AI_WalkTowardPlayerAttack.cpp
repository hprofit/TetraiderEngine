#include "Agent.h"
#include "AI_WalkTowardPlayerAttack.h"
#include "TetraiderAPI.h"

AI_WalkTowardPlayerAttack::AI_WalkTowardPlayerAttack()
: AI_State(NPC_State_WalkTowardPlayerAttack) {

}

AI_WalkTowardPlayerAttack::~AI_WalkTowardPlayerAttack(){
}

void AI_WalkTowardPlayerAttack::OnEnter(){
	idleDuration = RandomFloat(0.5f, 0.9f); 
	idledSoFar = 0.0f;
	pAgent->LookAtPlayer();
	
}

void AI_WalkTowardPlayerAttack::OnUpdate(float dt){
	if (idledSoFar > idleDuration) {
		pAgent->LookAtPlayer();
		pAgent->MoveToPlayer();
	}
	else {
		idledSoFar += dt;
	}
	if (pAgent->IsPlayerOutOfSight()) {
		pAgent->ChangeState(NPC_STUNNED);
	}
}

void AI_WalkTowardPlayerAttack::OnExit(){
}

void AI_WalkTowardPlayerAttack::HandleEvent(Event* pEvent) {

}