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

///ui container component//
#ifndef UI_CONTAINER_H_
#define UI_CONTAINER_H_

#include <SFML/Graphics/RenderWindow.hpp>
#include <Game/UIComponent.h>
#include <vector>


namespace UI
{
	class UIContainer : public UIComponent
	{
	public:
		typedef std::shared_ptr<UIContainer> Ptr;

		explicit UIContainer(const sf::RenderWindow& rw);

		virtual void Update(float dt);
		void AddComponent(UIComponent::Ptr component);

		virtual bool Selectable() const;
		virtual void HandleEvent(const sf::Event& e);

		void SetVisible(bool b = true);

	private:
		std::vector<UIComponent::Ptr> m_children;
		sf::Int16 m_selectedChild;
		bool m_visible;

		bool m_HasSelection() const;
		void m_Select(sf::Int16 index);
		void m_SelectNext();
		void m_SelectPrevious();

		const sf::RenderWindow& m_renderWindow;

		virtual void draw(sf::RenderTarget& rt, sf::RenderStates states) const;
	};
}

#endif