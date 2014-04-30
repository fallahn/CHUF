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

///base class for all UI components (buttons etc)///
#ifndef UI_COMPONENT_H_
#define UI_COMPONENT_H_

#include <SFML/System/NonCopyable.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>

#include <memory>

namespace sf
{
	class Event;
}

namespace UI
{
	class UIComponent : public sf::Drawable, public sf::Transformable, private sf::NonCopyable
	{
	public:
		typedef std::shared_ptr<UIComponent> Ptr;

		UIComponent();
		virtual ~UIComponent();

		virtual void Update(float dt);

		virtual bool Selectable() const = 0;
		bool Selected() const;
		virtual void Select();
		virtual void Deselect();

		virtual bool Active() const;
		virtual void Activate();
		virtual void Deactivate();

		virtual void HandleEvent(const sf::Event& e) = 0;

		virtual bool Contains(const sf::Vector2f& point) const;

		void SetEnabled(bool b = true);
		bool Enabled() const;

	private:
		bool m_selected;
		bool m_active;
		bool m_enabled;
	};
}

#endif