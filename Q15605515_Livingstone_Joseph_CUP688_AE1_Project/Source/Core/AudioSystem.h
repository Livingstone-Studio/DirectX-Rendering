#pragma once

#include <Audio.h>
#include <unordered_map>
#include <string>

using namespace DirectX;

class AudioSystem
{
public:
	static AudioSystem* Instance;

public:
	AudioSystem();
	~AudioSystem();

public:
	void Update();

	void PlayMusic(std::wstring music_file, bool state = true);
	void PlaySoundEffect(std::wstring effect_file);

private:
	std::unique_ptr<AudioEngine> m_audio_engine = nullptr;

	std::unordered_map<std::wstring, std::unique_ptr<SoundEffect>> m_active_effects;
	std::unordered_map<std::wstring, std::unique_ptr<SoundEffectInstance>> m_active_instances;
};

