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

#include <Game/ParticleFactory.h>
#include <Game/SmokeParticle.h>

using namespace Game;

ParticleSystem::Ptr ParticleFactory::Create(float emitRate, sf::Uint16 particleCount, ParticleSystem::Type type)
{
	ParticleSystem::Ptr p;
	switch (type)
	{
	case ParticleSystem::Type::Smoke:
		p = std::make_shared<ParticleSystem>(emitRate, particleCount, SmokeParticle::Create);
		break;
	default: break;
	}
	m_systems.push_back(p);
	return p;
}

void ParticleFactory::Update(float dt)
{
	m_systems.erase(std::remove_if(m_systems.begin(),
		m_systems.end(),
		[](const ParticleSystem::Ptr& p)->bool
		{
			return (p->Deleted() || !p->Attached());
		}), m_systems.end());

	for (auto& p : m_systems)
		p->Update(dt);
}

void ParticleFactory::Reset()
{
	m_systems.clear();
}

void ParticleFactory::draw(sf::RenderTarget& rt, sf::RenderStates states) const
{
	//TODO bounds checking to draw only on screen systems
	
	for (auto& p : m_systems)
		rt.draw(*p, states);
}