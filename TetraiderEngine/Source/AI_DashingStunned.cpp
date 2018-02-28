#include "Agent.h"
#include "AI_DashingStunned.h"
#include "TetraiderAPI.h"

AI_DashingStunned::AI_DashingStunned()
: AI_State(NPC_State_DashingStunned) {

}

AI_DashingStunned::~AI_DashingStunned(){
}

void AI_DashingStunned::OnEnter(){	
	triedSofar = 0.0f;
	tryTime = 2.0f;
}

void AI_DashingStunned::OnUpdate(float dt){
	if (triedSofar > tryTime) {
		pAgent->ChangeState(NPC_IDLE);
	}
	else {
		triedSofar += dt;
	}
}

void AI_DashingStunned::OnExit(){
	
}

void AI_DashingStunned::HandleEvent(Event* pEvent) {

}