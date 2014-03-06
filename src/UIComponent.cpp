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

///surce for UIComponent base class///
#include <Game/UIComponent.h>

using namespace UI;

//ctor/dtor
UIComponent::UIComponent()
	: m_selected	(false),
	m_active		(false),
	m_enabled		(true)
{

}

UIComponent::~UIComponent()
{

}

//public
void UIComponent::Update(float dt)
{

}

bool UIComponent::Selected() const
{
	return m_selected;
}

void UIComponent::Select()
{
	m_selected = true;
}

void UIComponent::Deselect()
{
	m_selected = false;
}

bool UIComponent::Active() const
{
	return m_active;
}

void UIComponent::Activate()
{
	m_active = true;
}

void UIComponent::Deactivate()
{
	m_active = false;
}

bool UIComponent::Contains(const sf::Vector2f& point) const
{
	return false;
}

void UIComponent::SetEnabled(bool b)
{
	m_enabled = b;
}

bool UIComponent::Enabled() const
{
	return m_enabled;
}