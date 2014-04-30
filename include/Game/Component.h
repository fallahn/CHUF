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

#ifndef BCOMPONENT_H_
#define BCOMPONENT_H_

#include <Game/Deletable.h>

#include <memory>

namespace Game
{
	class SceneNode;
	class Component : public Deletable
	{
	public:
		typedef std::shared_ptr<Component> Ptr;
		enum class Type
		{
			Physics,
			Mesh,
			Particle,
			Health,
			Behavior,
			Camera
		};

		Component();
		virtual ~Component(){};

		void SetParent(SceneNode& node);
		virtual void UpdateParent(float dt) = 0;

		virtual Component::Type GetType() const = 0;

		virtual void Delete() = 0;

		//HandleMessage(msg);

		bool Attached() const;

	protected:
		void m_SetDeleted();
		SceneNode* m_Parent() const;

	private:
		SceneNode* m_parent;

	};
}

#endif //COMPONENT_H_