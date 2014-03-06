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

///source code for 3D light class///
#include <Mesh/Light.h>
#include <Mesh/MeshHelpers.h>

#include <glm/glm.hpp>

using namespace ml;

namespace
{
	const float halfProjectionSize = 8.f;
}

//ctor
Light::Light()
    : m_ambientColour	(sf::Color(100u, 100u, 100u)),
      m_diffuseColour	(sf::Color(200u, 200u, 200u)),
      m_specularColour	(sf::Color(225u, 225u, 225u)),
	  m_brightness		(0.f),
	  m_nearClip		(0.5f), //used when rendering shadow maps
	  m_farClip			(10.f),
	  m_target			(1.f, 0.f, 1.f)
{
	m_UpdateProjectionMatrix();
	m_UpdateViewMatrix();
}

//public
void Light::SetPosition(const glm::vec3& position)
{
	glm::vec3 diff = position - m_position;	
	m_position = position;

	m_UpdateViewMatrix();
}

void Light::SetPosition(float x, float y, float z)
{
	glm::vec3 diff = m_position - glm::vec3(x, y, z);
	m_position.x = x;
	m_position.y = y;
	m_position.z = z;

	m_UpdateViewMatrix();
}

const glm::vec3& Light::GetPosition() const
{
	return m_position;
}

void Light::Move(const glm::vec3& amount)
{
	m_position += amount;
	m_UpdateViewMatrix();
}

void Light::Move(float x, float y, float z)
{
	m_position.x += x;
	m_position.y += y;
	m_position.z += z;

	m_UpdateViewMatrix();
}

void Light::SetAmbientColour(const sf::Color& colour)
{
	m_ambientColour = colour;
}

const sf::Color& Light::GetAmbientColour() const
{
	return m_ambientColour;
}

void Light::SetDiffuseColour(const sf::Color& colour)
{
	m_diffuseColour = colour;
}

const sf::Color& Light::GetDiffuseColour() const
{
	return m_diffuseColour;
}

void Light::SetSpecularColour(const sf::Color& colour)
{
	m_specularColour = colour;
}

const sf::Color& Light::GetSpecularColour() const
{
	return m_specularColour;
}

void Light::CalcBrightness(const glm::vec3& nodePosition)
{
	glm::vec3 distance = m_position - nodePosition;
	m_brightness = glm::length(distance);
}

float Light::GetBrightness() const
{
	return m_brightness;
}

void Light::SetTarget(const glm::vec3& target)
{
	m_target = target;
	m_nearClip = glm::length(m_target - m_position) -28.f;
	m_farClip = m_nearClip + 50.f;
	m_UpdateProjectionMatrix();
	m_UpdateViewMatrix();
}

void Light::SetTarget(float targetX, float targetY, float targetZ)
{
	SetTarget(glm::vec3(targetX, targetY, targetZ));
}

const glm::vec3& Light::GetTarget() const
{
	return m_target;
}

float Light::GetNearClip() const
{
	return m_nearClip;
}

float Light::GetFarClip() const
{
	return m_farClip;
}

const glm::mat4& Light::GetProjectionMatrix() const
{
	return m_projectionMatrix;
}

const glm::mat4& Light::GetViewMatrix() const
{
	return m_viewMatrix;
}

//private

void Light::m_UpdateProjectionMatrix()
{
	m_projectionMatrix = glm::ortho<float>(-halfProjectionSize, halfProjectionSize, -halfProjectionSize, halfProjectionSize, m_nearClip, m_farClip);

	//m_projectionMatrix = glm::perspective(m_camera->GetFOV(),
	//										m_camera->GetAspectRatio(),
	//										m_nearClip,
	//										m_farClip);
}

void Light::m_UpdateViewMatrix()
{
	m_viewMatrix = glm::lookAt(m_position, m_target,glm::vec3(0.f, 1.f, 0.f));
}