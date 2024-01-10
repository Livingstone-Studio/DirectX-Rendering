#include "AudioSystem.h"

AudioSystem* AudioSystem::Instance = nullptr;

AudioSystem::AudioSystem()
{
	if (!Instance)
		Instance = this;
	else
		delete this;

	HRESULT result = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	if (FAILED(result))
	{
		OutputDebugString(L"Error initializing audio system.");
		return;
	}

	AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;
	m_audio_engine = std::make_unique<AudioEngine>(eflags);

	m_audio_engine->SetMasterVolume(0.3f);
}

AudioSystem::~AudioSystem()
{
	m_audio_engine->Suspend();
	m_audio_engine.release();
}

void AudioSystem::PlayMusic(std::wstring music_file, bool state)
{
	std::unordered_map<std::wstring, std::unique_ptr<SoundEffectInstance>>::iterator it;
	it = m_active_instances.find(music_file);
	if (it != m_active_instances.end())
	{
		m_active_instances[music_file]->Play(state);
		return;
	}

	std::unique_ptr<SoundEffect> sound_effect;
	sound_effect = std::make_unique<SoundEffect>(m_audio_engine.get(), music_file.c_str());
	auto effect = sound_effect->CreateInstance();
	effect->Play(state);
	effect->SetVolume(0.2f);

	m_active_effects.insert(std::make_pair(music_file, move(sound_effect)));
	m_active_instances.insert(std::make_pair(music_file, move(effect)));
}

void AudioSystem::PlaySoundEffect(std::wstring effect_file)
{
	std::unordered_map<std::wstring, std::unique_ptr<SoundEffect>>::iterator it;
	it = m_active_effects.find(effect_file);
	if (it != m_active_effects.end())
	{
		m_active_effects[effect_file]->Play();
		return;
	}

	std::unique_ptr<SoundEffect> sound_effect;
	sound_effect = std::make_unique<SoundEffect>(m_audio_engine.get(), effect_file.c_str());
	auto effect = sound_effect->CreateInstance();
	effect->Play(false);
	m_active_effects.insert(std::make_pair(effect_file, move(sound_effect)));
	m_active_instances.insert(std::make_pair(effect_file, move(effect)));
}

void AudioSystem::Update()
{
	if (!m_audio_engine->Update())
	{
		if (m_audio_engine->IsCriticalError())
		{
			OutputDebugString(L"Audio engine encountered critical error.");
			return;
		}
	}
}
