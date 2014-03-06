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

#ifndef PARTICLE_FACTORY_H_
#define PARTICLE_FACTORY_H_

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/System/NonCopyable.hpp>

#include <Game/BaseParticle.h>

#include <vector>

namespace Game
{
	class ParticleFactory final : public sf::Drawable, private sf::NonCopyable
	{
	public:
		ParticleSystem::Ptr Create(float emitRate, sf::Uint16 particleCount, ParticleSystem::Type type);
		void Update(float dt);
		void Reset();

	private:
		std::vector<ParticleSystem::Ptr> m_systems;
		void draw(sf::RenderTarget& rt, sf::RenderStates states) const;
	};
}

#endif //PARTICLE_FACTORY_H_