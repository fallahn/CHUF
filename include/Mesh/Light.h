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

///creates a light in 3D space///
#ifndef LIGHT_H_
#define LIGHT_H_

#include <SFML/Graphics/Color.hpp>

#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <memory>

namespace ml
{
    class Light
    {
        public:
			//class representing a light source
            Light();
			//set the light position in 3d space
			void SetPosition(const glm::vec3& position);
			void SetPosition(float x, float y, float z);
			//get the light's current position in 3d space
			const glm::vec3& GetPosition() const;
			//move the light. Equivalent of adding move parameter to current position
			void Move(const glm::vec3& amount);
			void Move(float x, float y, float z);
			//sets the light's ambient colour - ie the shadow colour
            void SetAmbientColour(const sf::Color& colour);
			//returns the light's current ambient colour value
            const sf::Color& GetAmbientColour() const;
			//sets the light's diffuse colour - ie the colour which it casts on illuminated objects
            void SetDiffuseColour(const sf::Color& colour);
			//returns the light's current diffuse colour
            const sf::Color& GetDiffuseColour() const;
			//sets the light's specular colour - ie the colour which affects highlights of glossy materials
            void SetSpecularColour(const sf::Color& colour);
			//returns the light's current specular colour
            const sf::Color& GetSpecularColour() const;
			//calculates the brightness of the light from the distance of a given position
			void CalcBrightness(const glm::vec3& position);
			//returns the light's brightness
            float GetBrightness() const;
			//sets the target if light is directional
			void SetTarget(const glm::vec3& target);
			void SetTarget(float targetX, float targetY, float targetZ);
			//gets the target position the light faces
			const glm::vec3& GetTarget() const;
			//returns the near clipping plane when using like to project shadow map
			float GetNearClip() const;
			//returns the far clip
			float GetFarClip() const;

			//returns lights projection matrix for shadow mapping
			const glm::mat4& GetProjectionMatrix() const;
			//returns the lights view matrix based on position and target
			const glm::mat4& GetViewMatrix() const;

        private:
			glm::vec3 m_position, m_target;
            float m_brightness, m_nearClip, m_farClip;
            sf::Color m_ambientColour, m_diffuseColour, m_specularColour;

			glm::mat4 m_projectionMatrix;
			glm::mat4 m_viewMatrix;
			void m_UpdateProjectionMatrix();
			void m_UpdateViewMatrix();
    };

	typedef std::shared_ptr<Light> LightPtr;
	typedef std::vector<LightPtr> Lights;
}

#endif //LIGHT_H_