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

///implements the obj file parser///
#include <Mesh/ObjParser.h>
#include <Mesh/MeshHelpers.h>
#include <Helpers.h>

#include <iostream>
#include <fstream>
#include <cassert>
#include <map>

using namespace ml;

//public
bool ObjParser::LoadFromFile(const std::string& path, BufferCollection& destination)
{
    std::fstream file(path.c_str());
    if (file.fail())
    {
        std::cerr << "Unable to load mesh file \"" << path << "\" " << std::endl;
        return false;
    }

	//TODO exception handling when obj file not valid


	std::vector<glm::vec2> texPos;
	std::vector<glm::vec3> vertPos;
	std::vector<glm::vec3> normals;
	std::map<sf::Uint16, Faces> faces;
	std::vector<std::string> matNames;

	sf::Uint16 mtlCount = 1u; //always at least one material even if not specified
	faces[0] = Faces();

    std::string line; //"v " is position, "vt" is tex coord, "vn" is normal vector
    while (std::getline(file, line))
    {
        //create new buffer if new material is used
		if (line.substr(0, 6) == "usemtl")
		{
			matNames.push_back(line.substr(7));
			if (matNames.size() > mtlCount)
			{
				//create a new set of faces for material
				mtlCount++;
				faces[mtlCount - 1] = Faces();
			}
		}
		
		switch (line[0])
        {
            case 'v' :
            {
                    
				switch (line[1])
                {
                    case ' ': //vertex pos
                        vertPos.push_back(m_GetVectorFromLine(line));
						break;
					case 'n':
						//normal data
						normals.push_back(m_GetVectorFromLine(line));
						break;
					case 't':
						//uv data
						texPos.push_back(m_GetTexCoordFromLine(line));
						break;
                };
            }
            break;

			//even though this is in the same switch block we assume all the vert
			//and texture coords have been parsed out of the file
            case 'f' :
				Faces newFaces = m_GetFacesFromLine(line);
				auto& fc = faces[mtlCount - 1];
				fc.insert(fc.end(), newFaces.begin(), newFaces.end());
				break;
        };
    }

	//parse each set of faces and create a new mesh buffer
	//TODO: fix this so vertices are shared when appropriate
	//and only duplicate for new UV coords - see MD2 parser
	for (auto& fc : faces)
	{
		BufferPtr meshBuffer = std::make_shared<MeshBuffer>();
		MeshBuffer::Vertices& bufferVerts = meshBuffer->GetVertices();
		MeshBuffer::Faces& bufferFaces = meshBuffer->GetFaces();
		
		if (fc.first < matNames.size())
			meshBuffer->SetTextureName(matNames[fc.first]);

		for (auto& f : fc.second)
		{
			MeshBuffer::Face newFace;
			for (auto i = 0u; i < 3u; i++)
			{
				MeshBuffer::Vertex newVert;
				newVert.position = vertPos[f.vIndex[i]];
				newVert.positionId = f.vIndex[i];
				if (f.fIndex[i] > -1) newVert.texCoord = texPos[f.fIndex[i]];
				if (f.nIndex[i] > -1)
				{
					newFace.normalIndex[i] = f.nIndex[i];
					newVert.normal = normals[f.nIndex[i]];
				}
				bufferVerts.push_back(newVert);
				newFace.vertIndex[i] = bufferVerts.size() - 1u;
			}
			bufferFaces.push_back(newFace);
		}
		destination.push_back(std::move(meshBuffer));
	}
	return true;
}

//private
glm::vec3 ObjParser::m_GetVectorFromLine(const std::string& line)
{
	std::vector<std::string> point = Helpers::String::Tokenize(line, ' ');
	assert(point.size() >= 4);

	return glm::vec3(Helpers::String::GetFromString<float>(point[1], 0.f),
						Helpers::String::GetFromString<float>(point[2], 0.f),
						Helpers::String::GetFromString<float>(point[3], 0.f));
}

glm::vec2 ObjParser::m_GetTexCoordFromLine(const std::string& line)
{
	std::vector<std::string> coord = Helpers::String::Tokenize(line, ' ');
	assert(coord.size() >= 3);

	return glm::vec2(Helpers::String::GetFromString<float>(coord[1], 0.f),
						1.f - Helpers::String::GetFromString<float>(coord[2], 0.f));
}

ObjParser::Faces ObjParser::m_GetFacesFromLine(const std::string& line)
{
    //Get all elements as string
	ObjParser::Face face;
	ObjParser::Faces faces;
    sf::Uint8 i = 0u;
	std::vector<std::string> data = Helpers::String::Tokenize(line, ' ');
	assert(data.size() >= 4u);
	for (auto& str : data)
    {
		if (i != 0u)
		{
			std::vector<std::string> values = Helpers::String::Tokenize(str, '/', true);
			while (values.size() < 3u) values.push_back("0");

			if (i < 4u)
			{
				//vertex ID
				face.vIndex[i - 1] = Helpers::String::GetFromString<sf::Uint32>(values[0], 0) - 1u;
				//texCoord ID
				face.fIndex[i - 1] = Helpers::String::GetFromString<sf::Uint32>(values[1], 0) - 1u;
				//normal coord ID
				face.nIndex[i - 1] = Helpers::String::GetFromString<sf::Uint32>(values[2], 0) - 1u;
			}
			else //we have a quad
			{
				//store the current face
				faces.push_back(face);

				//create a new face to finish the quad
				ObjParser::Face quadFace;
				quadFace.vIndex[0] = Helpers::String::GetFromString<sf::Uint32>(values[0], 0) - 1u;
				quadFace.fIndex[0] = Helpers::String::GetFromString<sf::Uint32>(values[1], 0) - 1u;
				quadFace.nIndex[0] = Helpers::String::GetFromString<sf::Uint32>(values[2], 0) - 1u;

				quadFace.vIndex[1] = face.vIndex[0];
				quadFace.fIndex[1] = face.fIndex[0];
				quadFace.nIndex[1] = face.nIndex[0];

				quadFace.vIndex[2] = face.vIndex[2];
				quadFace.fIndex[2] = face.fIndex[2];
				quadFace.nIndex[2] = face.nIndex[2];

				faces.push_back(quadFace);
				return faces;
			}
		}
        i++;
    }
	faces.push_back(face);
    return faces;
}
