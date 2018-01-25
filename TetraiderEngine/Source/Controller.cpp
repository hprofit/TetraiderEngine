#include "Controller.h"
#include "Transform.h"
#include "Body.h"
#include "InputManager.h"
#include "PhysicsManager.h"
#include "DebugManager.h"
#include "../Source/Math/LineSegment2D.h"

#include <iostream>

Controller::Controller() :
	Component(ComponentType::Controller),
	m_speed (0.0f)
{
}

Controller::~Controller() {}

void Controller::Update(float dt) {
	InputManager& inputMngr = InputManager::GetInstance();

	Vector3D posOffset;

	if (inputMngr.IsKeyPressed(SDL_SCANCODE_D))
		posOffset.x += m_speed;
	if (inputMngr.IsKeyPressed(SDL_SCANCODE_A))
		posOffset.x -= m_speed;
	if (inputMngr.IsKeyPressed(SDL_SCANCODE_W))
		posOffset.y += m_speed;
	if (inputMngr.IsKeyPressed(SDL_SCANCODE_S))
		posOffset.y -= m_speed;

	m_pBody->SetVelocity(posOffset);
}

void Controller::Serialize(json j) {
	m_speed = ParseFloat(j, "speed");
}

void Controller::HandleEvent(Event* pEvent) {
	if (pEvent->Type() == EventType::EVENT_OnCollide) {
		OnCollide* pOnCollide = static_cast<OnCollide*>(pEvent);
		//printf("Penetration %f: \n", pOnCollide->mtv.penetration);
	}
}

void Controller::LateInitialize() {
	if (!m_pTransform) {
		if (pGO)
			m_pTransform = static_cast<Transform*>(pGO->GetComponent(ComponentType::Transform));
		else {
			printf("No Game Object found. Controller component failed to operate.\n");
			return;
		}

		if (!m_pTransform) {
			printf("No Transform component found. Controller component failed to operate.\n");
			return;
		}
	}

	if (!m_pBody) {
		if (pGO)
			m_pBody = static_cast<Body*>(pGO->GetComponent(ComponentType::Body));
		else {
			printf("No Game Object found. Controller component failed to operate.\n");
			return;
		}

		if (!m_pBody) {
			printf("No Body component found. Controller component failed to operate.\n");
			return;
		}
	}
}