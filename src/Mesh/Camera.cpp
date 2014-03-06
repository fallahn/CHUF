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

///source for camera class
#include <Mesh/Camera.h>
#include <Mesh/GL/glew.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cassert>

using namespace ml;

//ctor / dtor
Camera::Camera()
	:	m_FOV		(22.5f),
	m_nearValue		(1.f),
	m_farValue		(40.f),
	m_aspectRatio	(16.f / 9.f)
{
	m_UpdateBoundsAtZero(true);
}

Camera::~Camera()
{

}

//public
void Camera::SetPosition(const glm::vec3& position)
{
	m_position = position;
	m_UpdateBoundsAtZero();
}

void Camera::SetPosition(float x, float y, float z)
{
	m_position.x = x;
	m_position.y = y;
	m_position.z = z;
	m_UpdateBoundsAtZero();
}

const glm::vec3& Camera::GetPosition() const
{
	return m_position;
}

void Camera::Move(const glm::vec3& amount)
{
	m_position += amount;
	m_UpdateBoundsAtZero();
}

void Camera::Move(float x, float y, float z)
{
	m_position.x += x;
	m_position.y += y;
	m_position.z += z;
	m_UpdateBoundsAtZero();
}

void Camera::SetRotation(const glm::vec3& rotation)
{
	m_rotation = rotation;
}

void Camera::SetRotation(float x, float y, float z)
{
	m_rotation.x = x;
	m_rotation.y = y;
	m_rotation.z = z;
}

const glm::vec3& Camera::GetRotation() const
{
	return m_rotation;
}

void Camera::Rotate(const glm::vec3& amount)
{
	m_rotation += amount;
	m_ClampRotation();
}

void Camera::Rotate(float x, float y, float z)
{
	m_rotation.x += x;
	m_rotation.y += y;
	m_rotation.z += z;
	m_ClampRotation();
}

void Camera::SetFOV(float angle)
{
	assert(angle < 180.f && angle > 0.f);
	m_FOV = angle;
	m_UpdateBoundsAtZero(true);
}

float Camera::GetFOV() const
{
	return m_FOV;
}

void Camera::SetAspectRatio(float ratio)
{
	assert(ratio > 0.f);
	m_aspectRatio = ratio;
	m_UpdateBoundsAtZero(true);
}

float Camera::GetAspectRatio() const
{
	return m_aspectRatio;
}

void Camera::SetNearValue(float distance)
{
	assert(distance < m_farValue && distance > 0.f);
	m_nearValue = distance;
	//m_UpdateBoundsAtZero();
}

float Camera::GetNearValue() const
{
	return m_nearValue;
}

void Camera::SetFarValue(float distance)
{
	assert(distance > m_nearValue);
	m_farValue = distance;
	m_UpdateBoundsAtZero(true);
}

glm::mat4 Camera::GetTransform() const
{
	glm::mat4 m = glm::translate(glm::mat4(), glm::vec3(m_position.x, m_position.y, m_position.z));
	m = glm::rotate(m, m_rotation.y, glm::vec3(0.f, 1.f, 0.f));
	m = glm::rotate(m, m_rotation.z, glm::vec3(0.f, 0.f, 1.f));
	m = glm::rotate(m, m_rotation.x, glm::vec3(1.f, 0.f, 0.f));
	return m;
}

float Camera::GetFarValue() const
{
	return m_farValue;
}

void Camera::Update(float dt)
{
	//override this in derived classes
}

void Camera::Draw()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glm::mat4 perspective = glm::perspective(m_FOV, m_aspectRatio, m_nearValue, m_farValue);
	glLoadMatrixf(glm::value_ptr(perspective));

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
	glLoadMatrixf(glm::value_ptr(GetTransform()));

}

bool Camera::ViewContains(const glm::vec3& point) const
{
	if (point.z + m_position.z > 0.f) //behind camera
		return false;
	if (m_farValue + point.z < 0.f) //beyond clip
		return false;

	return m_GetScaledBounds(point.z).contains(sf::Vector2f(point.x, -point.y));
}

bool Camera::ViewIntersects(const sf::FloatRect& bounds, float depth) const
{
	if (depth + m_position.z > 0.f) //behind camera
		return false;
	if (m_farValue + depth < 0.f) //beyond clip
		return false;

	return m_GetScaledBounds(depth).intersects(bounds);
}

///private
void Camera::m_ClampRotation()
{
	if(m_rotation.x > 360.f) m_rotation.x -= 360.f;
	else if (m_rotation.x < -360.f) m_rotation.x += 360.f;

	if(m_rotation.y > 360.f) m_rotation.y -= 360.f;
	else if (m_rotation.y < -360.f) m_rotation.y += 360.f;

	if(m_rotation.z > 360.f) m_rotation.z -= 360.f;
	else if (m_rotation.z < -360.f) m_rotation.z += 360.f;
}

void Camera::m_UpdateBoundsAtZero(bool resize)
{
	if (resize)
	{
		const float angle = std::tan(m_FOV / 2.f * 0.0174532925f);
		m_boundsAtZero.height = (m_farValue * angle) * 2.f;
		m_boundsAtZero.width = m_boundsAtZero.height * m_aspectRatio;
	}
	m_boundsAtZero.left = -(m_position.x + (m_boundsAtZero.width / 2.f));
	m_boundsAtZero.top = m_position.y - (m_boundsAtZero.height) / 2.f;
}

sf::FloatRect Camera::m_GetScaledBounds(float depth) const
{
	sf::FloatRect bounds = m_boundsAtZero;
	const float scale = (m_farValue - depth) / m_farValue;
	bounds.width *= scale;
	bounds.height *= scale;
	bounds.left += (m_boundsAtZero.width - bounds.width) / 2.f;
	bounds.top += (m_boundsAtZero.height - bounds.height) / 2.f;
	return bounds;
}