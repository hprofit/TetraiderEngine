#pragma once

#ifndef EVENT_H
#define EVENT_H

#include <string>

// Macro trick to make message names enums
// from the file EventNames.h
#define REGISTER_EVENT_NAME(x) x,
typedef enum
{
	#include "EventNames.h"
	EVENT_OnCollide,
	EVENT_OnLevelInitialized,
	EVENT_OnEnemyHealthZero,
	EVENT_OnPlayerHealthZero,
	EVENT_OnTakeDamage,
	EVENT_OnPlayerHeal,

	EVENT_NUM
} EventType;
#undef REGISTER_EVENT_NAME

#define REGISTER_EVENT_NAME(x) #x,
static const char* EventNameText[] =
{
#include "EventNames.h"
	"Invalid"
};
#undef REGISTER_EVENT_NAME

// Abstract struct, data objects should inherit from this
struct EventData {};

class Event
{
protected:
	double m_time;		// Time left until event is broadcast to subscribers
	EventType m_type;	// Type of event
	EventData* m_data;	// Data for event, if any
public:
	Event(EventType type, double time = 0.0);
	Event(EventType type, EventData* eventData, double time = 0.0);
	virtual ~Event();

	double Time() const { return m_time; }
	EventType Type() const { return m_type; }
	EventData* Data() const { return m_data; }

	void DecrementTime(double amt) { m_time -= amt; }



	//static EventType GetEventTypeFromTitle(std::string eventTitle);
};

#endif