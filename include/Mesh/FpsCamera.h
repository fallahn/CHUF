/*********************************************************************
MeshLib - Library for easy integration of 3D meshes into a 2D scene built
on SFML.

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

///a camera with realtime input which creates an first person style view///
// controllable with configurable mouse / keyboard
#ifndef FPS_CAMERA_H_
#define FPS_CAMERA_H_

#include <Mesh/Camera.h>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window.hpp>

namespace ml
{
	class FpsCamera final : public Camera
	{
	public:
		enum Input
		{
			FORWARD,
			BACKWARD,
			STRAFE_LEFT,
			STRAFE_RIGHT,
			PITCH_UP,
			PITCH_DOWN,
			YAW_LEFT,
			YAW_RIGHT
		};

		//represents a first person camera by adding keyboard input to camera class
        FpsCamera(sf::RenderWindow& renderWindow);

        void    StrafeLeft();
        void    StrafeRight();

		//moves into the scene
        void    MoveForward();
		//moves out of the scene
        void    MoveBackward();

		void Pitch(float amount = 1.f);
		void Yaw(float amount = 1.f);
		void Roll(float amount = 1.f);

		//updates controller input
		void Update(float dt);
		//sets mouse speed multiplier
		void SetMouseSpeed(float);

		//allows assigning keys for control
		void SetKey(FpsCamera::Input input, sf::Keyboard::Key key);

		//enable / disable mouse input so we can override using a ui or something
		void SetInputEnabled(bool b);
		bool GetInputEnabled() const{return m_inputEnabled;};

    private:
		sf::RenderWindow& m_renderWindow;
		sf::Vector2f m_screenCentre;
		float m_mouseSpeed;

		sf::Keyboard::Key m_keyForward, m_keyBackward;
		sf::Keyboard::Key m_strafeLeft, m_strafeRight;
		sf::Keyboard::Key m_pitchUp, m_pitchDown, m_yawLeft, m_yawRight;

		bool m_inputEnabled;
	};
}

#endif