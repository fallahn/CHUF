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

#include <Game/Behaviors.h>

#include <cassert>

using namespace Game;

////behaviour ABC/////
Behavior::Behavior()
{}

Component::Type Behavior::GetType() const
{
	return Component::Type::Behavior;
}

void Behavior::Delete()
{
	m_SetDeleted();
}

/////behavior factory////
Component::Ptr BehaviorFactory::Create(BehaviorType type)
{
	switch (type)
	{
	/*case Game::BehaviorType::Coin:
	{
		m_behaviors.push_back(Behavior::Ptr(new BehaviorCoin()));
		return m_behaviors.back();
	}
	case BehaviorType::Mole:
	{
		m_behaviors.push_back(Behavior::Ptr(new BehaviorMole()));
		return m_behaviors.back();
	}*/
	default:
		break;
	}
	return nullptr;
}

void BehaviorFactory::Update()
{
	//remove any deleted behaviors
	m_behaviors.remove_if([](const Behavior::Ptr& b) -> bool
	{
		return (b->Deleted() || !b->Attached());
	});
}

void BehaviorFactory::Reset()
{
	m_behaviors.clear();
}