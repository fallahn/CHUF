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

//ui component representing a togglable checkbox / bullet//
#ifndef UI_CHECK_H_
#define UI_CHECK_H_

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

#include <Game/UIComponent.h>
#include <array>

namespace UI
{
	class UICheckBox final : public UIComponent
	{
	public:
		typedef std::shared_ptr<UICheckBox> Ptr;
		enum class Alignment
		{
			Left,
			Right
		};

		UICheckBox(const sf::Font& font, const sf::Texture& texture);

		void SetText(const std::string& text);
		void SetTextSize(sf::Uint8 size);
		bool Checked() const;
		void Check(bool b = true);
		void SetAlignment(UICheckBox::Alignment alignment);

		bool Selectable() const override;
		void Select() override;
		void Deselect() override;

		void Activate() override;
		void Deactivate() override;

		void HandleEvent(const sf::Event& e) override;

		bool Contains(const sf::Vector2f& point) const override;

	private:
		enum State
		{
			Normal = 0u,
			Selectected = 1u,
			CheckedNormal = 2u,
			CheckedSelected = 3u
		};
		const sf::Texture& m_texture;
		sf::Sprite m_sprite;
		sf::Text m_text;
		std::array<sf::IntRect, 4u> m_subrects;
		bool m_checked;
		Alignment m_alignment;

		void draw(sf::RenderTarget& rt, sf::RenderStates states) const;
		void m_AlignText();
	};
}

#endif