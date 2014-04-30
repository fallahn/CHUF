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

///creates a UIComponent slider with range 0 - X///
#ifndef SLIDER_H_
#define SLIDER_H_

#include <Game/UIComponent.h>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>

#include <array>

namespace UI
{
	class UISlider final : public UIComponent
	{
	public:
		typedef std::shared_ptr<UISlider> Ptr;
		enum class Direction
		{
			Horizontal,
			Vertical
		};

		UISlider(const sf::Font& font, const sf::Texture& t, float length = 250.f, float maxValue = 100.f);

		bool Selectable() const override;
		void Select() override;
		void Deselect() override;

		void Activate() override;
		void Deactivate() override;

		void HandleEvent(const sf::Event& e) override;

		bool Contains(const sf::Vector2f& point) const override;

		void Update(float dt) override;

		//-------------------//

		void SetMaxValue(float value);
		void SetDirection(UISlider::Direction d);
		void SetSize(float length);
		float GetValue() const;

		void SetText(const std::string& s);
		void SetTextSize(sf::Uint16 size);
		void SetValue(float val);

	private:
		float m_maxValue;
		float m_length;
		Direction m_direction;
		sf::FloatRect m_localBounds;

		sf::Sprite m_handleSprite;
		sf::RectangleShape m_slotShape;
		enum State
		{
			Normal = 0u,
			Selected = 1u
		};
		std::array<sf::IntRect, 2u> m_subrects;
		mutable sf::Vector2f m_mousePos;

		const sf::Font& m_font;
		sf::Text m_text;

		void draw(sf::RenderTarget& rt, sf::RenderStates states) const;
		void m_UpdateText();
	};
}

#endif