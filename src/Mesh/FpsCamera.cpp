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

///source for fps camera class
#include <Mesh/FpsCamera.h>
#include <Mesh/MeshHelpers.h>

using namespace ml;
  
//ctor
FpsCamera::FpsCamera(sf::RenderWindow& renderWindow)
    :   Camera		(),
	m_renderWindow	(renderWindow),
	m_mouseSpeed	(0.3f),
	m_inputEnabled	(true),
	//--default key bindings--//
	m_keyForward	(sf::Keyboard::W),
	m_keyBackward	(sf::Keyboard::S),
	m_strafeLeft	(sf::Keyboard::A),
	m_strafeRight	(sf::Keyboard::D),
	m_pitchUp		(sf::Keyboard::Up),
	m_pitchDown		(sf::Keyboard::Down),
	m_yawLeft		(sf::Keyboard::Left),
	m_yawRight		(sf::Keyboard::Right)
{
	m_screenCentre = static_cast<sf::Vector2f>(m_renderWindow.getSize() / 2u);
}


//public
void FpsCamera::StrafeLeft()
{
    float yRad = GetRotation().y / 180.f * Helpers::pi;

	glm::vec3 pos = GetPosition();
    pos.x += std::cos(yRad);
    pos.z += std::sin(yRad);
    SetPosition(pos);
}

void FpsCamera::StrafeRight()
{
    float yRad = GetRotation().y / 180.f * Helpers::pi;
	glm::vec3 pos = GetPosition();
    pos.x -= std::cos(yRad);
    pos.z -= std::sin(yRad);
    SetPosition(pos);
}

void FpsCamera::MoveForward()
{
    float yRad = GetRotation().y / 180.f * Helpers::pi;
    float xRad = GetRotation().x / 180.f * Helpers::pi;
	glm::vec3 pos = GetPosition();
    pos.x -= std::sin(yRad);
    pos.z += std::cos(yRad);
    pos.y += std::sin(xRad);
    SetPosition(pos);
}

void FpsCamera::MoveBackward()
{
    float yRad = GetRotation().y / 180.f * Helpers::pi;
    float xRad = GetRotation().x / 180.f * Helpers::pi;
	glm::vec3 pos = GetPosition();
    pos.x += std::sin(yRad);
    pos.z -= std::cos(yRad);
    pos.y -= std::sin(xRad);
    SetPosition(pos);
}



void FpsCamera::Update(float dt)
{
	const float speed = 100.f;
		
	if(sf::Keyboard::isKeyPressed(m_pitchUp))
		Pitch(-speed * dt);

	if(sf::Keyboard::isKeyPressed(m_pitchDown))
		Pitch(speed * dt);

	if(sf::Keyboard::isKeyPressed(m_yawLeft))
        Yaw(-speed * dt);

	if(sf::Keyboard::isKeyPressed(m_yawRight))
        Yaw(speed * dt);

	if(sf::Keyboard::isKeyPressed(m_keyForward))
        MoveForward();

	if(sf::Keyboard::isKeyPressed(m_keyBackward))
        MoveBackward();

	if(sf::Keyboard::isKeyPressed(m_strafeRight))
        StrafeRight();

    if(sf::Keyboard::isKeyPressed(m_strafeLeft))
        StrafeLeft();

	//update mouse look
	if(m_inputEnabled)
	{
		sf::Vector2f mousePos = static_cast<sf::Vector2f>(sf::Mouse::getPosition(m_renderWindow)) - m_screenCentre;		
		mousePos *= m_mouseSpeed;
		Rotate(glm::vec3(mousePos.y, mousePos.x, 0.f));
		sf::Mouse::setPosition(sf::Vector2i(m_screenCentre), m_renderWindow);
	}

}

void FpsCamera::Pitch(float amount)
{
	Rotate(amount, 0.f, 0.f);
}

void FpsCamera::Yaw(float amount)
{
	Rotate(0.f, amount, 0.f);
}

void FpsCamera::Roll(float amount)
{
	Rotate(0.f, 0.f, amount);
}

void FpsCamera::SetKey(Input input, sf::Keyboard::Key key)
{
	switch(input)
	{
	case FORWARD:
		m_keyForward = key;
		break;
	case BACKWARD:
		m_keyBackward = key;
		break;
	case STRAFE_LEFT:
		m_strafeLeft = key;
		break;
	case STRAFE_RIGHT:
		m_strafeRight = key;
		break;
	case PITCH_UP:
		m_pitchUp = key;
		break;
	case PITCH_DOWN:
		m_pitchDown = key;
		break;
	case YAW_LEFT:
		m_yawLeft = key;
		break;
	case YAW_RIGHT:
		m_yawRight = key;
		break;
	}
}

void FpsCamera::SetInputEnabled(bool b)
{
	m_inputEnabled = b;
	m_renderWindow.setMouseCursorVisible(!b);
}
