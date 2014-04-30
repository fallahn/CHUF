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

//class to allow easy updating of views which can follow scene nodes or sent to a specific point//

#ifndef VIEW_CAMERA_H_
#define VIEW_CAMERA_H_

#include <SFML/Graphics/View.hpp>
#include <SFML/System/NonCopyable.hpp>

#include <Game/Component.h>

#include <vector>
#include <memory>

namespace Game
{
	class SceneNode;
	class CameraFactory;

	class ViewCamera final : private sf::NonCopyable, public Component
	{		
	public:
		friend class CameraFactory;
		typedef std::shared_ptr<ViewCamera> Ptr;
		typedef std::vector<ViewCamera::Ptr> Cameras;

		ViewCamera();
		//constructs the manager from an existing view
		ViewCamera(sf::View view);
		//sets world point to target
		void SetTarget(sf::Vector2f worldPoint);
		//sets the speed at which the view travels to target
		void SetSpeed(float speed);
		//sets the bounds which limit the overall movement of the view
		void SetBounds(sf::FloatRect bounds);
		//sets a new internal view. Use this if you need to resize the view
		void SetView(sf::View view);
		//returns the current view
		const sf::View& GetView() const;
		//implement component
		void UpdateParent(float dt) override;
		Component::Type GetType() const override;
		void Delete() override;

	private:

		sf::Vector2f m_targetPoint;
		float m_currentSpeed;
		sf::FloatRect m_bounds;
		sf::View m_view;

		void m_Update(float dt);
	};

	class CameraFactory final : private sf::NonCopyable
	{
	public:
		CameraFactory();

		void Update(float dt);
		ViewCamera::Ptr Create();
		ViewCamera::Ptr Create(const sf::View& view);
		void Reset();

		const ViewCamera::Cameras& GetCameras() const;
	private:
		ViewCamera::Cameras m_cameras;

	};
}

#endif