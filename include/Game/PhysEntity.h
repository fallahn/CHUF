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

///ABC for creating complex physics entities with box 2D which can be attached to a scene node///

#ifndef PHYS_ENT_H_
#define PHYS_ENT_H_

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/System/NonCopyable.hpp>
#include <SFML/System/Vector2.hpp>

#include <Game/Component.h>

#include <Box2D/Dynamics/b2Body.h>

#include <memory>

namespace Game
{
	class PhysEntity : public Component, public sf::Drawable, private sf::NonCopyable
	{
	public:
		enum class FixtureType
		{
			Solid,
			FootSensor,
			ArmSensor
		};

		typedef std::shared_ptr<PhysEntity> Ptr;

		PhysEntity(){};
		virtual ~PhysEntity(){};

		//use this to return the body used as the attachment point to the scene node
		virtual b2Body* GetPhysBody() const = 0;
		//use this to create a custom entity
		virtual void Create(b2World& physWorld, sf::Vector2f position){};
		//use this to delete the entity from the phys simulation
		virtual void Delete() = 0;
		//implement component class
		virtual void UpdateParent(float dt);
		Component::Type GetType() const;
	private:
		//this can be used to draw shapes for debugging for example
		virtual void draw(sf::RenderTarget& rt, sf::RenderStates states) const{};
	};
}

#endif //PHYS_ENT_H_