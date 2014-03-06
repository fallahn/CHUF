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

///helper functions for 3D vectors///
#ifndef MESH_HELPERS_H_
#define MESH_HELPERS_H_

#include <SFML/System/Vector3.hpp>
#include <Mesh/GL/glew.h>
#include <glm/vec3.hpp>
#include <glm/geometric.hpp>
#include <cmath>


namespace Helpers
{
	const float pi = 3.141592654f;

	namespace Vectors
	{
        static sf::Vector3f Cross(const sf::Vector3f& v1, const sf::Vector3f& v2)
        {
            sf::Vector3f vect;
            vect.x = v1.y * v2.z - v1.z * v2.y;
            vect.y = v1.z * v2.x - v1.x * v2.z;
            vect.z = v1.x * v2.y - v1.y * v2.x;
            return vect;
        }

		static glm::vec3 Cross(const glm::vec3& v1, const glm::vec3& v2)
		{
			glm::vec3 vect;
			vect.x = v1.y * v2.z - v1.z * v2.y;
			vect.y = v1.z * v2.x - v1.x * v2.z;
			vect.z = v1.x * v2.y - v1.y * v2.x;
			return vect;
		}

        static float Dot(const sf::Vector3f& v1, const sf::Vector3f& v2)
        {
            return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
        }

        static float GetLength(const sf::Vector3f& vect)
        {
            return std::sqrt(Dot(vect, vect));
        }

        static float GetLengthSquared(const sf::Vector3f& vect)
        {
            return Dot(vect, vect);
        }

        static sf::Vector3f Normalize(const sf::Vector3f& vect)
        {
            float length = GetLength(vect);
            sf::Vector3f v;
            v.x = vect.x / length;
            v.y = vect.y / length;
            v.z = vect.z / length;
            return v;
        }

		static glm::vec3 Normalize(const glm::vec3& v)
		{
			const float length = std::sqrt(glm::dot(v, v));
			glm::vec3 rv;
			rv.x = v.x / length;
			rv.y = v.y / length;
			rv.z = v.z / length;
			return rv;
		}

		static sf::Vector3f Lerp(const sf::Vector3f& start, const sf::Vector3f& end, float percent)
		{
			return start + ((end - start) * percent);
		}

		static glm::vec3 Lerp(const glm::vec3& start, const glm::vec3& end, float percent)
		{
			return start + ((end - start) * percent);
		}

		static sf::Vector3f Slerp(const sf::Vector3f& start, sf::Vector3f& end, float percent)
		{
			float dot = Dot(start, end);
			float theta = std::acos(dot) * percent;
			sf::Vector3f relativeVec = Normalize(end - start * dot);

			return ((start * std::cos(theta)) + (relativeVec * std::sin(theta)));
		}
	}
}


#endif //MESH_HELPERS_H_