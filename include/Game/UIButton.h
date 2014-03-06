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

///header file UIButton component///
#ifndef UI_BUTTON_H_
#define UI_BUTTON_H_

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

#include <Game/UIComponent.h>
#include <functional>
#include <array>

namespace UI
{
	class UIButton : public UIComponent
	{
	public:
		typedef std::function<void()> Callback;
		typedef std::shared_ptr<UIButton> Ptr;

		UIButton(const sf::Font& font, const sf::Texture& texture);

		void SetCallback(Callback callback);
		void SetText(const std::string& text);
		void SetToggle(bool b);

		virtual bool Selectable() const;
		virtual void Select();
		virtual void Deselect();

		virtual void Activate();
		virtual void Deactivate();

		virtual void HandleEvent(const sf::Event& e);

		virtual bool Contains(const sf::Vector2f& point) const;

		sf::Vector2f GetSize() const;
	private:
		enum State
		{
			ButtonNormal   = 0u,
			ButtonSelected = 1u,
			ButtonActive   = 2u
		};

		Callback m_callback;

		const sf::Texture& m_texture;
		sf::Sprite m_sprite;
		sf::Text m_text;
		std::array<sf::IntRect, 3> m_subrects;

		bool m_toggle;

		virtual void draw(sf::RenderTarget& rt, sf::RenderStates states) const;
	};
}

#endif