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

#include <Game/Health.h>
#include <Game/SceneNode.h>

#include <cassert>

using namespace Game;

//ctor
HealthComponent::HealthComponent(float maxHealth)
: m_maxHealth	(maxHealth),
m_health		(maxHealth)
{}

//public
void HealthComponent::Increase(float amount)
{
	m_health += amount;
	if (m_health > m_maxHealth)
		m_health = m_maxHealth;
}

void HealthComponent::Decrease(float amount)
{
	m_health -= amount;
	if (m_health < 0.f)
	{
		m_Parent()->Delete();
		m_health = 0.f;
	}
}

float HealthComponent::GetValue() const
{
	return m_health;
}

void HealthComponent::UpdateParent(float dt)
{

}

Component::Type HealthComponent::GetType() const
{
	return Component::Type::Health;
}

void HealthComponent::Delete()
{
	m_SetDeleted();
}