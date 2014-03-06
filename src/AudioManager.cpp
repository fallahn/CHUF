/*********************************************************************
Matt Marchant 2013 - 2014
http://trederia.blogspot.com

The zlib license has been used to make this software fully compatible
with SFML. See http://www.sfml-dev.org/license.php

This software is provided 'as-is', without any express or
implied warranty. In no event will the authors be held
liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute
it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented;
you must not claim that you wrote the original software.
If you use this software in a product, an acknowledgment
in the product documentation would be appreciated but
is not required.

2. Altered source versions must be plainly marked as such,
and must not be misrepresented as being the original software.

3. This notice may not be removed or altered from any
source distribution.
*********************************************************************/

///source for audio manager///
#include <Game/AudioManager.h>
#include <SFML/Audio/Listener.hpp>

using namespace Game;

namespace
{
	const float ListenerZ = 300.f;
	const float Attenuation = 8.f;
	const float MinDistance2D = 200.f;
	const float MinDistance3D = std::sqrt(MinDistance2D * MinDistance2D + ListenerZ * ListenerZ);
}


//ctor
AudioManager::AudioManager()
	: m_globalEffectsVolume (100.f),
	m_globalMusicVolume		(100.f)
{
	//map enums to files
	//m_buffers[Audio::Impact] = m_soundBufferResource.Get("assets/sound/fx/vehicles/collide.wav");

	//m_musicPaths[Audio::Title] = "assets/sound/music/menu.ogg";
}


//public
void AudioManager::PlayEffect(Audio::Effect effect)
{
	PlayEffect(effect, GetListenerPosition());
}

void AudioManager::PlayEffect(Audio::Effect effect, const sf::Vector2f& position)
{
	m_sounds.push_back(sf::Sound());
	sf::Sound& sound = m_sounds.back();

	sound.setBuffer(m_buffers[effect]);
	sound.setPosition(position.x, -position.y, 0.f); //OpenAL y coords are inverse to sfml
	sound.setAttenuation(Attenuation);
	sound.setMinDistance(MinDistance3D);
	sound.setVolume(m_globalEffectsVolume);
	sound.play();
}

void AudioManager::RemoveStoppedSounds()
{
	m_sounds.remove_if([](const sf::Sound& s)
	{
		return s.getStatus() == sf::Sound::Stopped;
	});
}

void AudioManager::SetListenerPosition(const sf::Vector2f& position)
{
	sf::Listener::setPosition(position.x, -position.y, ListenerZ);
}

sf::Vector2f AudioManager::GetListenerPosition()
{
	sf::Vector3f position = sf::Listener::getPosition();
	return sf::Vector2f(position.x, -position.y);
}

void AudioManager::SetGlobalEffectsVolume(float volume)
{
	m_globalEffectsVolume = volume;
	for(auto& s : m_sounds)
		s.setVolume(m_globalEffectsVolume);
}

float AudioManager::GetGlobalEffectsVolume() const
{
	return m_globalEffectsVolume;
}

sf::SoundBuffer& AudioManager::GetEffectBuffer(std::string path)
{
	return m_soundBufferResource.Get(path);
}

void AudioManager::PlayMusic(Audio::Music music)
{
	if(m_music.openFromFile(m_musicPaths[music]))
	{
		m_music.setVolume(m_globalMusicVolume);
		m_music.setLoop(true);
		m_music.play();
	}
}

void AudioManager::PauseMusic(bool paused)
{
	if(paused) m_music.pause();
	else m_music.play();
}

void AudioManager::StopMusic()
{
	m_music.stop();
}

void AudioManager::SetGlobalMusicVolume(float volume)
{
	m_globalMusicVolume = volume;
	m_music.setVolume(m_globalMusicVolume);
}

float AudioManager::GetGlobalMusicVolume() const
{
	return m_globalMusicVolume;
}