#include "Weapon.h"
#include "RangeAttack.h"
#include "MeleeAttack.h"
#include "Controller.h"
#include "GameObject.h"
#include "Transform.h"
#include "TetraiderAPI.h"

Weapon::Weapon(): Component(C_Weapon) {}

Weapon::~Weapon() {
	for (auto attack : m_Attacks) {
		delete attack;
	}

	m_Attacks.clear();
}

void Weapon::Update(float dt) {
	for (auto attacks : m_Attacks) {
		attacks->Update(dt);

		// Debug
		if (attacks->GetType() == AttackType::Melee) {
			Controller* pController = pGO->GetComponent<Controller>(ComponentType::C_Controller);
			if(pController)
				attacks->Debug(pController->GetLookDirection());
		}
	}
}

void Weapon::Serialize(const json& j) {
	//TODO: Cannot override weapon component at the moment
	if (m_Attacks.size() > 0) 
		return;

	int numberOfAttacks = j["Attacks"].size();

	//TODO: Move to a factory method if gets too complex
	for (int i = 0; i < numberOfAttacks; ++i) {
		if (ParseString(j["Attacks"][i], "type") == "Melee") {
			Attack* attack = new MeleeAttack(
				ParseFloat(j["Attacks"][i], "coolDown"),
				ParseInt(j["Attacks"][i], "baseDamage"),
				AttackType::Melee,
				ParseFloat(j["Attacks"][i], "radius"),
				ParseFloat(j["Attacks"][i], "angle"),
				ParseFloat(j["Attacks"][i], "triggerAttackIn")
			);

			attack->m_pOwner = this;
			m_Attacks.push_back(attack);
		}
		else if (ParseString(j["Attacks"][i], "type") == "Ranged") {
			Attack* attack = new RangeAttack(
				ParseFloat(j["Attacks"][i], "coolDown"),
				ParseInt(j["Attacks"][i], "baseDamage"),
				AttackType::Ranged,
				ParseFloat(j["Attacks"][i], "projectileSpeed"),
				ParseFloat(j["Attacks"][i], "instantiationOffset"),
				ParseFloat(j["Attacks"][i], "lifeTime"),
				ParseString(j["Attacks"][i], "projectilePrefab")
			);

			attack->m_pOwner = this;
			m_Attacks.push_back(attack);
		}
	}
}

void Weapon::LateInitialize() {}
void Weapon::HandleEvent(Event* pEvent) {}

// Assumes direction to be normalized
bool Weapon::UseAttack(int attack, const Vector3D& dirToAttack) {
	return m_Attacks[attack]->Use(dirToAttack);
}