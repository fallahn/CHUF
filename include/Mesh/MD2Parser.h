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

///parses a quake MD2 file into a series of mesh buffers for each frame of animation///
//MD2 file format reference at http://tfc.duke.free.fr/old/models/md2.htm
#ifndef MD2_PARSE_H_
#define MD2_PARSE_H_

#include <SFML/System/NonCopyable.hpp>

#include <Mesh/MeshBuffer.h>

#include <memory>
#include <array>

namespace ml
{
	class MD2Parser final : private sf::NonCopyable
	{
	public:
		static bool LoadFromFile(const std::string& path, BufferCollection& destination);

	private:

		typedef std::array<float, 3u> Vector3;

		struct MD2Header
		{
			sf::Uint32 ident, version;
			sf::Uint32 texWidth, texHeight;
			sf::Uint32 frameSize;
			//count of data to expect in model
			sf::Uint32 numSkins, numVerts, numTexCoords, numTriangles, numGlCmd, numFrames;
			//offset in bytes to data
			sf::Uint32 offSkins, offTexCoords, offTriangles, offFrames, offGlCmd, offEnd;
		};

		struct MD2Vertex
		{
			std::array<sf::Uint8, 3u> vPos;
			sf::Uint8 normalIndex;
		};

		struct MD2TexCoord
		{
			sf::Uint16 u, v;
		};

		struct MD2Triangle
		{
			std::array<sf::Uint16, 3u> vertex;
			std::array<sf::Uint16, 3u> texCoord;
		};

		struct MD2Frame
		{
			Vector3 scale, translation;
			std::array<char, 16u> name;
			std::unique_ptr<MD2Vertex[]> vertices;
		};

		struct MD2Skin
		{
			std::array<char, 68> name;
		};
	};
}

#endif //MD2_PARSE_H_