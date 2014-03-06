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

#include <Game/UIComboBox.h>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>
#include <cassert>
#include <iostream>

using namespace UI;

//ctor
UIComboBox::UIComboBox(const sf::Font& font, const sf::Texture& texture)
	: m_showItems	(false),
	m_mainShape		(static_cast<sf::Vector2f>(texture.getSize())),
	m_highlightShape(m_mainShape.getSize()),
	m_dropdownShape	(m_mainShape.getSize()),
	m_font			(font),
	m_selectedIndex	(0u),
	m_selectedText	("", font, 18u)
{
	m_mainShape.setTexture(&texture);
	m_highlightShape.setFillColor(sf::Color(120u, 120u, 120u));
	m_dropdownShape.setFillColor(sf::Color(90u, 90u, 90u));
	m_dropdownShape.setPosition(0.f, m_mainShape.getSize().y);
}

//public
bool UIComboBox::Selectable() const
{
	return true;
}

void UIComboBox::Select()
{
	UIComponent::Select();

	//show border
	m_mainShape.setOutlineThickness(2.f);
}

void UIComboBox::Deselect()
{
	UIComponent::Deselect();
	
	//hide border
	m_mainShape.setOutlineThickness(0.f);
	//m_showItems = false;
}

void UIComboBox::Activate()
{
	UIComponent::Activate();
	
	//show drop down
	m_showItems = true;
	m_highlightShape.setPosition(m_dropdownShape.getPosition());
}

void UIComboBox::Deactivate()
{
	UIComponent::Deactivate();

	//hide drop down
	m_showItems = false;
}

bool UIComboBox::Contains(const sf::Vector2f& point) const
{
	m_mousePoint = point; //save for handling events
	if(m_showItems)
	{
		sf::Transform t = getTransform();
		for(auto& i : m_items)
		{
			//set highlight position if items contains point
			if(t.transformRect(i->Bounds).contains(point))
			{
				m_highlightShape.setPosition(0.f, i->Bounds.top);
				break;
			}
		}
		return (t.transformRect(m_mainShape.getGlobalBounds()).contains(point)
			|| t.transformRect(m_dropdownShape.getGlobalBounds()).contains(point));
	}
	return getTransform().transformRect(m_mainShape.getGlobalBounds()).contains(point);
}

void UIComboBox::HandleEvent(const sf::Event& e)
{	
	if(e.type == sf::Event::MouseButtonPressed
		&& e.mouseButton.button == sf::Mouse::Left
		&& m_showItems)
	{
		for(auto i = 0u; i < m_items.size(); ++i)
		{
			if(getTransform().transformRect(m_items[i]->Bounds).contains(m_mousePoint))
			{
				SetSelectedIndex(i);
				break;
			}
		}
		m_showItems = false; //hide items even if we clicked outside
	}

	//TODO handle keyboard / controller input
}

void UIComboBox::AddItem(const std::string& text, sf::Int32 val)
{
	Item::Ptr item(new Item(text, val, m_font));
	item->Bounds = m_highlightShape.getLocalBounds();

	const float dropHeight = item->Bounds.height * static_cast<float>(m_items.size() + 1u);
	sf::Vector2f size = m_dropdownShape.getSize();
	size.y = dropHeight;
	m_dropdownShape.setSize(size);

	item->Bounds.top = dropHeight; //offset each item by at least main box height
	item->ItemText.setPosition(2.f, item->Bounds.top); //TODO fix magic number	
	m_items.push_back(std::move(item));

	SetSelectedIndex(m_items.size() - 1u);
}

const std::string& UIComboBox::GetSelectedText() const
{
	assert(m_items.size());
	return m_items[m_selectedIndex]->Text;
}

sf::Int32 UIComboBox::GetSelectedValue() const
{
	assert(m_items.size());
	return m_items[m_selectedIndex]->Value;
}

void UIComboBox::SetBackgroundColour(const sf::Color& colour)
{
	m_dropdownShape.setFillColor(colour);
}

void UIComboBox::SetHighlightColour(const sf::Color& colour)
{
	m_highlightShape.setFillColor(colour);
}

void UIComboBox::SetSelectedIndex(sf::Uint16 index)
{
	m_selectedIndex = index;
	m_selectedText.setString(m_items[index]->Text);
}

sf::Uint32 UIComboBox::Size() const
{
	return m_items.size();
}

void UIComboBox::SelectItem(const std::string& s)
{
	for(auto i =0u; i < m_items.size(); ++i)
	{
		if(m_items[i]->Text == s)
		{
			SetSelectedIndex(i);
			return;
		}
	}
}

void UIComboBox::SelectItem(sf::Uint16 val)
{
	for(auto i = 0u; i < m_items.size(); ++i)
	{
		if(m_items[i]->Value == val)
		{
			SetSelectedIndex(i);
			return;
		}
	}
}

//private
void UIComboBox::draw(sf::RenderTarget& rt, sf::RenderStates states) const
{
	states.transform *= getTransform();
	rt.draw(m_mainShape, states);
	rt.draw(m_selectedText, states);
	if(m_showItems)
	{
		rt.draw(m_dropdownShape, states);
		rt.draw(m_highlightShape, states);
		for(auto& i : m_items)
			rt.draw(i->ItemText, states);
	}
}


//-----struct for items----//
UIComboBox::Item::Item(const std::string& s, sf::Int32 v, const sf::Font& font)
	: Text	(s),
	Value	(v),
	ItemText(s, font, 18u)
{

}