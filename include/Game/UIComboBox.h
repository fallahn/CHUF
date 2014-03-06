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

///implements a combobox UI component///
#ifndef UI_COMBO_H_
#define UI_COMBO_H_

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Rect.hpp>

#include <Game/UIComponent.h>
#include <vector>

namespace UI
{
	class UIComboBox final : public UIComponent
	{
	public:
		typedef std::shared_ptr<UIComboBox> Ptr;

		UIComboBox(const sf::Font& font, const sf::Texture& texture);

		bool Selectable() const;
		void Select();
		void Deselect();

		void Activate();
		void Deactivate();

		void HandleEvent(const sf::Event& e);

		bool Contains(const sf::Vector2f& point) const;
		
		//--------//

		void AddItem(const std::string& text, sf::Int32 val);
		const std::string& GetSelectedText() const;
		sf::Int32 GetSelectedValue() const;

		void SetBackgroundColour(const sf::Color& colour);
		void SetHighlightColour(const sf::Color& colour);
		void SetSelectedIndex(sf::Uint16 index);
		sf::Uint32 Size() const;
		void SelectItem(const std::string& s);
		void SelectItem(sf::Uint16 val);

	private:
		struct Item
		{
			typedef std::unique_ptr<Item> Ptr;
			std::string Text;
			sf::Int32 Value;

			sf::FloatRect Bounds;
			sf::Text ItemText;

			Item(const std::string& s, sf::Int32 v, const sf::Font& font);
		};
		
		std::vector<Item::Ptr> m_items;

		bool m_showItems;
		sf::RectangleShape m_mainShape;
		sf::RectangleShape m_dropdownShape;
		mutable sf::RectangleShape m_highlightShape;

		sf::Uint16 m_selectedIndex;
		sf::Text m_selectedText;

		mutable sf::Vector2f m_mousePoint;

		const sf::Font& m_font;

		void draw(sf::RenderTarget& rt, sf::RenderStates states) const;

	};
}

#endif