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

///implimentation of view manager class

#include <Game/ViewCamera.h>
#include <Game/SceneNode.h>
#include <Helpers.h>

namespace
{
	//radius of target in which we stop moving
	const float minDistance = 10.f;
}

using namespace Game;

//ctors
ViewCamera::ViewCamera()
: m_currentSpeed	(1400.f)
{


}
ViewCamera::ViewCamera(sf::View view)
: m_currentSpeed	(1400.f)
{
	SetView(view);
}

//public
void ViewCamera::SetTarget(sf::Vector2f point)
{
	m_targetPoint = point;
}

void ViewCamera::SetSpeed(float speed)
{
	m_currentSpeed = speed;
}

void ViewCamera::SetBounds(sf::FloatRect bounds)
{
	//sf::Vector2f viewSize = m_view.getSize();
	//bounds.left += viewSize.x / 2.f;
	//bounds.top += viewSize.y / 2.f;
	//bounds.width -= viewSize.x;
	//bounds.height -= viewSize.y;
	m_bounds = bounds;
}

void ViewCamera::SetView(sf::View view)
{
	m_view = view;
	m_targetPoint = view.getCenter();
	//set up some default bounds if there are none
	if (m_bounds.width < 1.f && m_bounds.height < 1.f)
		m_bounds = sf::FloatRect(sf::Vector2f(), view.getSize());
}



const sf::View& ViewCamera::GetView() const
{
	return m_view;
}

void ViewCamera::UpdateParent(float dt)
{
	SetTarget(m_Parent()->GetWorldPosition());
}

Component::Type ViewCamera::GetType() const
{
	return Component::Type::Camera;
}

void ViewCamera::Delete()
{
	m_SetDeleted();
}
//private
void ViewCamera::m_Update(float dt)
{
	//if (!m_bounds.contains(m_targetPoint)) return;
	sf::Vector2f direction = m_targetPoint - m_view.getCenter();

	const float distance = Helpers::Vectors::GetLength(direction);
	if (distance < minDistance) return;
	const float maxDist = m_view.getSize().y / 4.f;
	const float step = m_currentSpeed / maxDist;
	float speed = step * distance;
	direction = Helpers::Vectors::Normalize(direction);
	m_view.move(direction * speed * dt);

	//clip to bounds
	sf::Vector2f centre = m_view.getCenter();
	
	if (centre.x < m_bounds.left)
	{
		centre.x = m_bounds.left;
		m_view.setCenter(centre);
	}
	else if (centre.x > m_bounds.left + m_bounds.width)
	{
		centre.x = m_bounds.left + m_bounds.width;
		m_view.setCenter(centre);
	}

	if (centre.y < m_bounds.top)
	{
		centre.y = m_bounds.top;
		m_view.setCenter(centre);
	}
	else if (centre.y > m_bounds.top + m_bounds.height)
	{
		centre.y = m_bounds.top + m_bounds.height;
		m_view.setCenter(centre);
	}

}