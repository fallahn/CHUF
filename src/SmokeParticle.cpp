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

///source for smoke particle class///

#include <Game/SmokeParticle.h>
#include <Helpers.h>

using namespace Game;

namespace
{
	const float colour = 255.f;
	const float radius = 5.f;
	const float transparencyReduction = 200.f;
	const float colourReduction = transparencyReduction * 0.9f;
	const float radiusIncrease = 90.f;
}

//ctor
SmokeParticle::SmokeParticle(QuadVerts& verts)
	: BaseParticle	(verts),
	m_colour		(colour),
	m_radius		(radius),
	m_transparency	(colour),
	m_rotationAmount(Helpers::Random::Float(180.f, 250.f))
{
	m_SetSize(sf::Vector2f(m_radius, m_radius) * 2.f);
}


//public
void SmokeParticle::Update(float dt)
{
	if(m_state == State::Alive)
	{
		m_colour -= colourReduction * dt;
		m_transparency -= transparencyReduction * dt;
		if(m_transparency < 0.f)
		{
			m_state = State::Dead;
			return;
		}

		m_radius += radiusIncrease * dt;
		m_SetSize(sf::Vector2f(m_radius, m_radius) * 2.f);

		sf::Uint8 c = static_cast<sf::Uint8>(m_colour);
		sf::Uint8 t = static_cast<sf::Uint8>(m_transparency);
		m_SetColour(sf::Color(c, c, c, t));
		m_Move(m_velocity * m_force * dt);
		m_Rotate(m_rotationAmount * dt);
	}
	else
	{
		//reset properties
		m_radius = radius;
		m_colour = m_transparency = colour;
		m_rotationAmount = Helpers::Random::Float(180.f, 250.f);
	}
}
