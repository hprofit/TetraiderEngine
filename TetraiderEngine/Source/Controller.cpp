#include "GameObject.h"
#include "Controller.h"
#include "Weapon.h"
#include "Health.h"
#include "Transform.h"
#include "TetraiderAPI.h"
#include "Camera.h"
#include <iostream>

Controller::Controller() :
	Agent(ComponentType::C_Controller)
{
}

Controller::~Controller() {}

void Controller::Update(float dt) {
	Vector3D moveDir;

	if (TETRA_INPUT.IsKeyPressed(SDL_SCANCODE_D) || TETRA_INPUT.IsKeyPressed(XBOX_DPAD_RIGHT))
		moveDir.x += 1;
	if (TETRA_INPUT.IsKeyPressed(SDL_SCANCODE_A) || TETRA_INPUT.IsKeyPressed(XBOX_DPAD_LEFT))
		moveDir.x -= 1;
	if (TETRA_INPUT.IsKeyPressed(SDL_SCANCODE_W) || TETRA_INPUT.IsKeyPressed(XBOX_DPAD_UP))
		moveDir.y += 1;
	if (TETRA_INPUT.IsKeyPressed(SDL_SCANCODE_S) || TETRA_INPUT.IsKeyPressed(XBOX_DPAD_DOWN))
		moveDir.y -= 1;


	if (TETRA_INPUT.IsMouseButtonPressed(MOUSEBTN::MOUSE_BTN_RIGHT) || TETRA_INPUT.IsKeyPressed(XBOX_BTN_B)) {
		m_pWeapon->UseAttack(0, m_lookDirection);
	}

	if (TETRA_INPUT.IsMouseButtonPressed(MOUSEBTN::MOUSE_BTN_LEFT) || TETRA_INPUT.IsKeyPressed(XBOX_BTN_A)) {
		m_pWeapon->UseAttack(1, m_lookDirection);
	}

	if (TETRA_INPUT.IsKeyTriggered(SDL_SCANCODE_P) || TETRA_INPUT.IsKeyTriggered(XBOX_BTN_START))
		TETRA_AUDIO.TogglePause();

	moveDir.Normalize();
	m_targetVelocity = moveDir * m_speed;
	m_lookDirection = GetDirectionToMouse();
	Agent::Update(dt);

}
void Controller::Serialize(const json& j) {
	Agent::Serialize(j["AgentData"]);
}

void Controller::HandleEvent(Event* pEvent) {
	Agent::HandleEvent(pEvent);
}

void Controller::LateInitialize() {
	Agent::LateInitialize();

	if(!m_pWeapon) {
		if (pGO)
			m_pWeapon = pGO->GetComponent<Weapon>(ComponentType::C_Weapon);
		else {
			printf("No Game Object found. Controller component failed to operate.\n");
			return;
		}

		if (!m_pWeapon) {
			printf("No Weapon component found. Controller component failed to operate.\n");
			return;
		}
	}
}

Vector3D Controller::GetDirectionToMouse() {
	Vector3D mousePos = Vector3D((float)TETRA_INPUT.MousePosX(), (float)TETRA_INPUT.MousePosY(), 0);
	GameObject* mainCam = TETRA_GAME_OBJECTS.GetCamera(1);
	Camera* camComponent = mainCam->GetComponent<Camera>(ComponentType::C_Camera);
	Vector3D screenSpace = camComponent->TransformPointToScreenSpace(m_pTransform->GetPosition());
	Vector3D dirToMousePos = mousePos - screenSpace;
	dirToMousePos.y *= -1;
	dirToMousePos.Normalize();
	return dirToMousePos;
}