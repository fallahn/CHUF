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

///creates customisable behaviors which can be attached to scene nodes///
#ifndef BEHAVIORS_H_
#define BEHAVIORS_H_

#include <SFML/System/NonCopyable.hpp>

#include <Game/Component.h>

#include <memory>
#include <list>

namespace Game
{
	class SceneNode;
	class Behavior : private sf::NonCopyable, public Component
	{
	public:
		typedef std::shared_ptr<Behavior> Ptr;

		Behavior();
		virtual ~Behavior(){};

		//implement component
		virtual void UpdateParent(float dt) = 0;
		Component::Type GetType() const;
		void Delete();
	};

	enum class BehaviorType
	{
		Coin,
		Mole
		//dwarf
		//etc
	};

	class BehaviorCoin;
	class BehaviorFactory final : private sf::NonCopyable
	{
	public:

		Component::Ptr Create(BehaviorType type);
		void Update();
		void Reset();

	private:
		std::list<Behavior::Ptr> m_behaviors;
	};
}


#endif //BEHAVIORS_H_