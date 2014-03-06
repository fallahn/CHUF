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

//base class for attachable components//

#include <Game/Component.h>
#include <cassert>

using namespace Game;

Component::Component()
: m_parent(nullptr){}

//public
void Component::SetParent(SceneNode& node)
{
	m_parent = &node;
}

bool Component::Attached() const
{
	return (m_parent != nullptr);
}

//protected
void Component::m_SetDeleted()
{
	Deletable::m_SetDeleted();
}

SceneNode* Component::m_Parent() const
{
	assert(m_parent);
	return m_parent;
}

//private