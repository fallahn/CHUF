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

#include <Game/PhysEntity.h>
#include <Game/SceneNode.h>

#include <tmx/tmx2box2d.h>

using namespace Game;

void PhysEntity::UpdateParent(float dt)
{
	b2Body* b = GetPhysBody();
	if (b)
	{
		m_Parent()->setRotation(tmx::BoxToSfAngle(b->GetAngle()));
		m_Parent()->SetWorldPosition(tmx::BoxToSfVec(b->GetPosition()));
	}
}

Component::Type PhysEntity::GetType() const
{
	return Component::Type::Physics;
}