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

///resource managers///
#ifndef RESOURCES_H_
#define RESOURCES_H_

#include <SFML/System/NonCopyable.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Graphics/Font.hpp>

#include <memory>
#include <array>

namespace Game
{
	template <class T>
	class BaseResource : private sf::NonCopyable
	{
	public:
		BaseResource()
		{
		}

		virtual ~BaseResource(){};

		T& Get(const std::string& path = "default")
		{
			//if we have a valid path check current resources and return if found
			if(!path.empty())
			{
				auto r = m_resources.find(path);
				if(r != m_resources.end())
				{
					return *r->second;
				}
			}

			//else attempt to load from file
			std::unique_ptr<T> r = std::unique_ptr<T>(new T());
			if(path.empty() || !r->loadFromFile(path))
			{
				m_resources[path] = m_errorHandle(); //error handle should return message endl
			}
			else
			{
				m_resources[path] = std::move(r);
			}

			//TODO - we could check for map size here and flush if over certain size

			return *m_resources[path];
		}

	protected:
		virtual std::unique_ptr<T> m_errorHandle() = 0;

	private:
		std::map<std::string, std::unique_ptr<T> > m_resources;
	};


	class TextureResource final : public BaseResource<sf::Texture>
	{
	private:
		std::unique_ptr<sf::Texture> m_errorHandle()
		{
			std::unique_ptr<sf::Texture> t(new sf::Texture());
			sf::Image i;
			i.create(20u, 20u, sf::Color::Magenta);
			t->loadFromImage(i);
			return std::move(t);
		}
	};

	class SoundBufferResource final : public BaseResource<sf::SoundBuffer>
	{
	public:
		SoundBufferResource()
		{
			//create a single cycle square wave @ 441Hz
			int i = 0;
			for(i; i < 50; i++)
				m_byteArray[i] = 16383;

			for(i; i < 100; i++)
				m_byteArray[i] = -16384;
		}
	private:
		std::array<sf::Int16, 100> m_byteArray;
		std::unique_ptr<sf::SoundBuffer> m_errorHandle()
		{
			//load the byte array into a new buffer
			std::unique_ptr<sf::SoundBuffer> b(new sf::SoundBuffer);
			b->loadFromSamples(&m_byteArray[0], m_byteArray.size(), 1u, 44100);
			return std::move(b);
		}
	};

	class FontResource final : public BaseResource<sf::Font>
	{
	public:
		FontResource();
	private:
		sf::Font m_font;
		std::unique_ptr<sf::Font> m_errorHandle();
	};

	//TODO image class
};


#endif