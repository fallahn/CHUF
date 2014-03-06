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

////parses mesh data from obj files into a mesh buffer collection////
#ifndef OBJ_FILE_H_
#define OBJ_FILE_H_

#include <SFML/System/NonCopyable.hpp>
#include <Mesh/MeshBuffer.h>
#include <array>

namespace ml
{
	class ObjParser final : private sf::NonCopyable
	{
	public:
		static bool LoadFromFile(const std::string& path, BufferCollection& destination);

	private:
		struct Face
		{
			Face()
			{
				for(auto i = 0u; i < 3u; i++)
				{
					vIndex[i] = -1;
					fIndex[i] = -1;
					nIndex[i] = -1;
				}
			}
			std::array<sf::Int16, 3> vIndex, fIndex, nIndex;
		};
		typedef std::vector<Face> Faces;
		//gets vertex position
        static glm::vec3 m_GetVectorFromLine(const std::string& line);
		//gets texCoord
		static glm::vec2 m_GetTexCoordFromLine(const std::string& line);
		//gets face
        static Faces m_GetFacesFromLine(const std::string& line);
	};
}

#endif //OBJ_FILE_H_