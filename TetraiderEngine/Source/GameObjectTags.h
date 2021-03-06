/* Start Header -------------------------------------------------------
Copyright (C) 2018 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior
written consent of DigiPen Institute of Technology is prohibited.
Author: <Moodie Ghaddar>
- End Header --------------------------------------------------------*/

#pragma once

#ifndef GAME_OBJECT_TAGS_H
#define GAME_OBJECT_TAGS_H

// Macro trick to make Tags enums from the file TagNames.h
#define REGISTER_TAG_NAME(x) x,
typedef enum
{
#include "TagNames.h"

	T_None
} GameObjectTag;
#undef REGISTER_TAG_NAME

#define REGISTER_TAG_NAME(x) #x,
static const char* TagNameText[] =
{
#include "TagNames.h"
	"Invalid"
};
#undef REGISTER_TAG_NAME

static std::unordered_map<std::string, GameObjectTag> TAG_NAMES = {
	{ "Player", T_Player },
	{ "Camera", T_Camera },
	{ "Enemy", T_Enemy },
	{ "Projectile", T_Projectile },
	{ "UI", T_UI}, 
	{ "Obstacle", T_Obstacle },
	{ "Hazard", T_Hazard },
	{ "AcidHazard", T_AcidHazard},
	{ "Particle", T_Particle },
	{ "Emitter", T_Emitter },
	{ "Pause",T_Pause },
	{ "PowerUpText",T_PowerUpText },
	{ "Cursor",T_Cursor },
	{ "SliderBar",T_SliderBar },
	{ "Text",T_Text },
	{ "DeadEnemey", T_DeadEnemy},
	{ "TriggerBox", T_TriggerBox },
	{ "TriggerBoxBoss", T_TriggerBoxBoss},
	{ "PlayerUI", T_PlayerUI },
	{ "BoundaryObstacle", T_BoundaryObstacle },
	{ "Background", T_Background },
	{ "NONE", T_None },
	{ "None", T_None }

};

#endif