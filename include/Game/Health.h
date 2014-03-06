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

///health component which deletes parent node when health reaches zero///
#ifndef HEALTH_H_
#define HEALTH_H_

#include <SFML/System/NonCopyable.hpp>

#include <Game/Component.h>

#include <memory>

namespace Game
{
	class HealthComponent final : public Component, private sf::NonCopyable
	{

	public:
		typedef std::shared_ptr<HealthComponent> Ptr;
		explicit HealthComponent(float maxHealth);
	
		void Increase(float amount = 1.f);
		void Decrease(float amount = 1.f);

		float GetValue() const;

		void UpdateParent(float dt);
		Component::Type GetType() const;
		void Delete();

	private:

		float m_health;
		float m_maxHealth;
	};
}

#endif