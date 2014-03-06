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

#include <Mesh/MD2Parser.h>

#include <fstream>
#include <iostream>
#include <map>
#include <string>

namespace
{
	const sf::Uint32 MD2_IDENT = 844121161; //"IDP2"
}

using namespace ml;

bool MD2Parser::LoadFromFile(const std::string& path, BufferCollection& destination)
{
	//open file and parse header
	std::fstream file;
	file.open(path, std::ios::in | std::ios::binary);

	if (file.fail())
	{
		std::cerr << "Failed to open file: " << path << std::endl;
		return false;
	}
	
	MD2Header header;
	file.read(reinterpret_cast<char*>(&header), sizeof(MD2Header));

	if (header.version != 8u || header.ident != MD2_IDENT)
	{
		file.close();
		std::cerr << "MD2 parser: file corrupted ot not MD2 file. Loading failed." << std::endl;
		std::cerr << path << std::endl;
		return false;
	}

	//header checks out so attempt to read the rest of the file
	std::unique_ptr<MD2Skin[]> skins(new MD2Skin[header.numSkins]);
	std::unique_ptr<MD2TexCoord[]> texCoords(new MD2TexCoord[header.numTexCoords]);
	std::unique_ptr<MD2Triangle[]> triangles(new MD2Triangle[header.numTriangles]);
	std::unique_ptr<MD2Frame[]> frames(new MD2Frame[header.numFrames]);

	file.seekg(header.offSkins, std::ios::beg);
	file.read((char*)(skins.get()), sizeof(char) * 68 * header.numSkins);

	file.seekg(header.offTexCoords, std::ios::beg);
	file.read((char*)(texCoords.get()), sizeof(MD2TexCoord)* header.numTexCoords);

	file.seekg(header.offTriangles, std::ios::beg);
	file.read((char*)(triangles.get()), sizeof(MD2Triangle) * header.numTriangles);

	file.seekg(header.offFrames, std::ios::beg);
	for (sf::Uint32 i = 0u; i < header.numFrames; ++i)
	{
		frames[i].vertices = std::unique_ptr<MD2Vertex[]>(new MD2Vertex[header.numVerts]);

		file.read(reinterpret_cast<char*>(&frames[i].scale), sizeof(Vector3));
		file.read(reinterpret_cast<char*>(&frames[i].translation), sizeof(Vector3));
		file.read(reinterpret_cast<char*>(&frames[i].name), sizeof(char)* 16);
		file.read(reinterpret_cast<char*>(frames[i].vertices.get()), sizeof(MD2Vertex)* header.numVerts);
	}
	file.close();


	//create a mesh buffer for each animation frame
	for (sf::Uint32 i = 0u; i < header.numFrames; ++i)
	{
		BufferPtr buffer = std::make_shared<MeshBuffer>();

		if (header.numSkins)
			buffer->SetTextureName(std::string(&skins[0].name[0]));

		const MD2Frame& frame = frames[i];
		MeshBuffer::Vertices& bufferVerts = buffer->GetVertices();

		//parse vert data and translate / scale
		auto j = 0u;
		for (sf::Uint32 v = 0u; v < header.numVerts; ++v)
		{
			MeshBuffer::Vertex bufferVert;
			const MD2Vertex& vert = frame.vertices[v];
			bufferVert.position.x = static_cast<float>(vert.vPos[0]) * frame.scale[0] + frame.translation[0];
			bufferVert.position.y = static_cast<float>(vert.vPos[1]) * frame.scale[1] + frame.translation[1];
			bufferVert.position.z = static_cast<float>(vert.vPos[2]) * frame.scale[2] + frame.translation[2];
			bufferVert.positionId = j;
			j++;

			//swap Z and Y to put into right handed space
			std::swap(bufferVert.position.y, bufferVert.position.z);
			
			bufferVert.colour = sf::Color::White;
			bufferVerts.push_back(bufferVert);
		}

		//parse triangle data
		MeshBuffer::Faces& bufferFaces = buffer->GetFaces();
		std::map<sf::Uint16, MeshBuffer::Vertex> usedIds;

		for (sf::Uint32 f = 0u; f < header.numTriangles; ++f)
		{
			MeshBuffer::Face bufferFace;
			const MD2Triangle& face = triangles[f];

			//check if vert already used and clone with new UV coords if necessary
			for (auto i = 0u; i < 3u; ++i)
			{
				sf::Uint16 id = face.vertex[i];
				if (usedIds.find(id) == usedIds.end())
				{
					//not yet used
					usedIds[id] = bufferVerts[id];
				}
				else
				{
					bufferVerts.push_back(usedIds[id]);
					id = bufferVerts.size() - 1u;
				}
				bufferFace.vertIndex[i] = id;
			}

			//uncompress texture coords by dividing by texture size
			bufferVerts[bufferFace.vertIndex[0]].texCoord.x = static_cast<float>(texCoords[face.texCoord[0]].u) / header.texWidth;
			bufferVerts[bufferFace.vertIndex[0]].texCoord.y = static_cast<float>(texCoords[face.texCoord[0]].v) / header.texHeight;
			bufferVerts[bufferFace.vertIndex[1]].texCoord.x = static_cast<float>(texCoords[face.texCoord[1]].u) / header.texWidth;
			bufferVerts[bufferFace.vertIndex[1]].texCoord.y = static_cast<float>(texCoords[face.texCoord[1]].v) / header.texHeight;
			bufferVerts[bufferFace.vertIndex[2]].texCoord.x = static_cast<float>(texCoords[face.texCoord[2]].u) / header.texWidth;
			bufferVerts[bufferFace.vertIndex[2]].texCoord.y = static_cast<float>(texCoords[face.texCoord[2]].v) / header.texHeight;

			bufferFaces.push_back(bufferFace);
		}
		buffer->CalcNormals();
		destination.push_back(buffer);
	}

	return true;
}