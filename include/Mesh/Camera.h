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

///base class for 3D camera used by Mesh::Scene///
#ifndef CAMERA_H_
#define CAMERA_H_

#include <SFML/Graphics/Rect.hpp>

#include <glm/matrix.hpp>
#include <glm/vec3.hpp>

namespace ml
{
	class Camera
	{
	public:
		Camera();
		virtual ~Camera();

		//sets the camera's postion
		void SetPosition(const glm::vec3& position);
		void SetPosition(float x, float y, float z);
		//gets the camera's current position
		const glm::vec3& GetPosition() const;
		//moves the camera, equivalent to adding amount to current position
		void Move(const glm::vec3& amount);
		void Move(float x, float y, float z);
		//sets the rotation of the camera
		void SetRotation(const glm::vec3& rotation);
		void SetRotation(float x, float y, float z);
		//gets the camera's current rotation
		const glm::vec3& GetRotation() const;
		//rotate the camera by adding amount to the current rotation
		void Rotate(const glm::vec3& amount);
		void Rotate(float x, float y, float z);
		//sets the camera FOV in degrees
		void SetFOV(float angle);
		//returns the camera's current FOV
		float GetFOV() const;
		//sets the camera's aspect ratio, eg 1.77f for 16:9
		void SetAspectRatio(float ratio);
		//gets the camera's current aspect ratio
		float GetAspectRatio() const;
		//sets the near plane distance of viewing frustum
		void SetNearValue(float distance);
		//gets the distance of the near plane of the viewing frustum
		float GetNearValue() const;
		//sets the far plane distance of the viewing frustum
		void SetFarValue(float distance);
		//gets the distance to the far plane of the viewing frustum
		float GetFarValue() const;
		//returns the current transform
		glm::mat4 GetTransform() const;
		//updates any camera properties, for example via real time input
		virtual void Update(float dt);
		//sets the view matrix for rendering the scene via the camera
		virtual void Draw();
		//checks if a point is within an enlarged view frustum for culling
		bool ViewContains(const glm::vec3& point) const;
		//checks if rectangle intersects enlarged frastrum at specified depth
		bool ViewIntersects(const sf::FloatRect& bounds, float depth = 0.f) const;

	private:
		float m_FOV;
		float m_nearValue, m_farValue;
		float m_aspectRatio;

		glm::vec3 m_position, m_rotation;

		//represents the x/y culling bounds when Z is zero
		sf::FloatRect m_boundsAtZero;

		void m_ClampRotation();
		void m_UpdateBoundsAtZero(bool resize = false);
		sf::FloatRect m_GetScaledBounds(float depth) const;
	};
}

#endif //CAMERA_H_