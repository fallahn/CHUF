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


///creates an audio manager for playing effects an music///
#ifndef AUDIO_MAN_H_
#define AUDIO_MAN_H_

#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/Music.hpp>

#include <Game/ResourceManager.h>
#include <list>

namespace Game
{
	namespace Audio
	{
		enum Effect
		{
			// name of effects go here

		};

		enum Music
		{
			//name of music go here

		};
	}
	
	class AudioManager : private sf::NonCopyable
	{
	public:
		AudioManager();

		//----sfx----//

		void PlayEffect(Audio::Effect effect);
		void PlayEffect(Audio::Effect effect, const sf::Vector2f& position);
		void RemoveStoppedSounds();
		void SetListenerPosition(const sf::Vector2f& position);
		sf::Vector2f GetListenerPosition();
		void SetGlobalEffectsVolume(float volume);
		float GetGlobalEffectsVolume() const;

		//if we want to externally manage sounds like engines use the same resource
		sf::SoundBuffer& GetEffectBuffer(std::string path);

		//---music---//

		void PlayMusic(Audio::Music music);
		void PauseMusic(bool paused);
		void StopMusic();
		void SetGlobalMusicVolume(float volume);
		float GetGlobalMusicVolume() const;

	private:
		//----sfx----//

		SoundBufferResource m_soundBufferResource;
		std::list<sf::Sound> m_sounds;
		std::map<Audio::Effect, sf::SoundBuffer> m_buffers;
		float m_globalEffectsVolume;

		//---music---//

		sf::Music m_music; //currently playing music
		std::map<Audio::Music, std::string> m_musicPaths;
		float m_globalMusicVolume;
	};
}


#endif