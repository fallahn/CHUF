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

///header for scroll sprite UI component//
#ifndef UI_SCROLL_SPRITE_H_
#define UI_SCROLL_SPRITE_H_

#include <Game/UIButton.h>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Clock.hpp>

namespace UI
{
	class UIScrollSprite final : public UIComponent
	{
	public:
		typedef std::shared_ptr<UIScrollSprite> Ptr;

		UIScrollSprite(const sf::Texture& front, const sf::Texture& back);

		void Update(float dt) override; //updates the animations

		void AddItem(const sf::Texture& texture, const std::string& title, const sf::Font& font);
		void ClearItems();

		sf::Uint16 SelectedIndex() const;

		//implements base functions
		bool Selectable() const override;
		void Select() override;
		void Deselect() override;

		void Activate() override;

		void HandleEvent(const sf::Event& e) override;

		bool Contains(const sf::Vector2f& point) const override;

		void SetIndexEnabled(sf::Uint16 index, bool state);
		void SelectIndex(sf::Uint16 index);
		//offset from component centre
		void SetItemOffset(const sf::Vector2f& offset);
		void SetTextOffset(const sf::Vector2f& offset);
		void SetTextSize(sf::Uint16 size);
	private:

		enum State
		{
			ScrollNormal = 0u,
			ScrollSelected = 1u
		};

		sf::Uint16 m_selectedIndex;
		sf::Uint16 m_destinationIndex;

		struct Item
		{
			Item(const sf::Texture& texture, const std::string& title, const sf::Font& font, sf::Uint16 textSize);
			sf::Sprite sprite;
			sf::Text text;
			bool visible;
			bool enabled;
		};
		std::vector<Item> m_items;
		sf::Vector2f m_itemOffset;
		sf::Uint16 m_textSize;
		sf::Vector2f m_textOffset;

		const sf::Texture& m_frontTexture;
		const sf::Texture& m_backTexture;
		std::array<sf::IntRect, 2u> m_subRects;
		sf::Sprite m_frontSprite;
		sf::Sprite m_backSprite;

		sf::Shader m_scanShader;
		sf::Shader m_ghostShader;
		sf::Clock m_scanClock;
		sf::Clock m_ghostClock;

		void draw(sf::RenderTarget& rt, sf::RenderStates state) const;
	};
}

#endif