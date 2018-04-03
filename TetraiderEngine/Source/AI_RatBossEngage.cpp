
#include <Stdafx.h>

AI_RatBossEngage::AI_RatBossEngage()
	: AI_State(NPC_State_RatBossEngage) {

}

AI_RatBossEngage::~AI_RatBossEngage() {
}

void AI_RatBossEngage::OnEnter() {
	switch (pAgent->GetCurrentPhase()) {
	case PHASE1: // NORMAL MELEE ENEMY BEHAVIOR
		pAgent->LookAtPlayer();
		pAgent->GoToPositionAroundPlayer();
		pAgent->SetSpeedMultiplier(1.3f);
		tryTimeDuration = RandomFloat(1.0f, 2.2f);
		minimumEngage = 2;
		engageTimer = RandomFloat(minimumEngage, 4);
		triedMovingSoFar = 0.0f;
		sinceEngage = 0.0f;
		break;
	case PHASE2:
		std::cout << "PHASE2\n";
		pAgent->LookAtPlayer();
		pAgent->GoToPositionAroundPlayer();
		pAgent->SetSpeedMultiplier(1.6f);
		tryTimeDuration = RandomFloat(1.0f, 2.2f);
		minimumEngage = 2;
		engageTimer = RandomFloat(minimumEngage, 4);
		triedMovingSoFar = 0.0f;
		sinceEngage = 0.0f;
		break;
	case PHASE3:
		std::cout << "PHASE3\n";
		pAgent->LookAtPlayer();
		pAgent->GoToPositionAroundPlayer();
		pAgent->SetSpeedMultiplier(2.1f);
		tryTimeDuration = RandomFloat(1.0f, 2.2f);
		minimumEngage = 2;
		engageTimer = RandomFloat(minimumEngage, 4);
		triedMovingSoFar = 0.0f;
		sinceEngage = 0.0f;
		break;
	}
}

void AI_RatBossEngage::OnUpdate(float dt) {
	switch (pAgent->GetCurrentPhase()) {
	case PHASE1: // NORMAL MELEE ENEMY BEHAVIOR
		// always face player on engage
		pAgent->LookAtPlayer();
		// if this engage has gone too long, just go attack!
		if (sinceEngage > engageTimer) {
			pAgent->ChangeState(NPC_ATTACK);
			return;
		}
		// if this move has taken too much, change destination
		if (triedMovingSoFar > tryTimeDuration) {
			pAgent->GoToPositionAroundPlayer();
			triedMovingSoFar = 0.0f;
			return;
		}
		// if player is in attack range, attack!
		if (pAgent->IsInAttackRange() && minimumEngage < sinceEngage) {
			pAgent->ChangeState(NPC_ATTACK);
			return;
		}
		// if next destination reached, pick another destination
		if (pAgent->IsArrivedAtDestination()) {
			pAgent->GoToPositionAroundPlayer();
			triedMovingSoFar = 0.0f;
			return;
		}
		triedMovingSoFar += dt;
		sinceEngage += dt;
		break;
	case PHASE2:
		// always face player on engage
		pAgent->LookAtPlayer();
		// if this engage has gone too long, just go attack!
		if (sinceEngage > engageTimer) {
			pAgent->ChangeState(NPC_REACTION);
			return;
		}
		// if this move has taken too much, change destination
		if (triedMovingSoFar > tryTimeDuration) {
			pAgent->GoToPositionAroundPlayer();
			triedMovingSoFar = 0.0f;
			return;
		}
		// if next destination reached, pick another destination
		if (pAgent->IsArrivedAtDestination()) {
			pAgent->GoToPositionAroundPlayer();
			triedMovingSoFar = 0.0f;
			return;
		}
		triedMovingSoFar += dt;
		sinceEngage += dt;
		break;
	case PHASE3:
		// always face player on engage
		pAgent->LookAtPlayer();
		// if this engage has gone too long, just go attack!
		if (sinceEngage > engageTimer) {
			pAgent->ChangeState(NPC_REACTION);
			return;
		}
		// if this move has taken too much, change destination
		if (triedMovingSoFar > tryTimeDuration) {
			pAgent->GoToPositionAroundPlayer();
			triedMovingSoFar = 0.0f;
			return;
		}
		// if next destination reached, pick another destination
		if (pAgent->IsArrivedAtDestination()) {
			pAgent->GoToPositionAroundPlayer();
			triedMovingSoFar = 0.0f;
			return;
		}
		triedMovingSoFar += dt;
		sinceEngage += dt;
		break;
	}
}

void AI_RatBossEngage::OnExit(){
	switch (pAgent->GetCurrentPhase()) {
	case PHASE1: // NORMAL MELEE ENEMY BEHAVIOR
		break;
	case PHASE2:
		break;
	case PHASE3:
		break;
	}
}


void AI_RatBossEngage::HandleEvent(Event* pEvent) {

}

void AI_RatBossEngage::Serialize(const json& j) {
}