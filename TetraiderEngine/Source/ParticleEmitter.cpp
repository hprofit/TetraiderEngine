#include "ParticleEmitter.h"
#include "GameObject.h"
#include <iostream>

ParticleEmitter::ParticleEmitter() : Component(ComponentType::C_ParticleEmitter) {}

ParticleEmitter::~ParticleEmitter() {}

void ParticleEmitter::LateInitialize()
{
	if (!m_pTransform) {
		if (pGO)
			m_pTransform = pGO->GetComponent<Transform>(ComponentType::C_Transform);
		else {
			std::cout << "No Game Object found. ParticleEmitter component failed to operate." << std::endl;
			return;
		}

		if (!m_pTransform) {
			std::cout << "No Transform component found. ParticleEmitter component failed to operate." << std::endl;
			assert(m_pTransform);
			return;
		}
	}
}

void ParticleEmitter::Update(float dt)
{
}

void ParticleEmitter::LateUpdate(float dt)
{
}

void ParticleEmitter::Serialize(json j)
{
	m_loopDuration = ParseFloat(j, "loopDuration");
	m_looping = ParseBool(j, "looping");
	m_prewarmed = ParseBool(j, "prewarmed");
	m_startDelay = ParseFloat(j, "startDelay");
	m_lifeTime = ParseFloat(j, "lifeTime");
	m_speed = ParseFloat(j, "speed");
	m_size = ParseFloat(j, "size");
	m_rotation = ParseFloat(j, "rotation");
	m_color = ParseColor(j, "color");
}

void ParticleEmitter::HandleEvent(Event * p_event)
{
}


