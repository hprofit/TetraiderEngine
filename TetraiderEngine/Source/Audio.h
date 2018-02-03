#pragma once
#ifndef AUDIO_H
#define AUDIO_H

#include "Component.h"
#include "TetraiderAPI.h"
#include <vector>

class Audio:public Component
{
public:
	Audio();
	~Audio();

	virtual void Update(float dt);
	virtual void Serialize(json j);
	virtual void LateInitialize();
	virtual void LateUpdate(float dt) {}
	virtual void HandleEvent(Event* pEvent) {}

	bool IsPlaying();
	void Play();
	void Stop();
	void Pause();
	void Resume();
	void SetVolume(float volume) { m_volume = volume; }
	void Loop(bool active) { m_isLoop = active; }
	void Mute(bool active) { m_Ismute = active; }
private:
	std::string m_audioClip;
	float m_volume;
	bool m_Ismute;
	bool m_isLoop;
	bool m_isPlayOnAwake;
};

#endif
