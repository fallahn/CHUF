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

/*parses the mesh info of the Doom 3 md5 mesh format*/
/*Reference: http://3dgep.com/?p=1053 */
/*and http://tfc.duke.free.fr/coding/md5-specs-en.html */

#include <Mesh/MD5Parser.h>
#include <Mesh/MeshHelpers.h>

#include <Helpers.h>

#include <fstream>
#include <iostream>
#include <cassert>
#include <string>

using namespace ml;

bool MD5Parser::LoadFromFile(const std::string& path, BufferCollection& destination)
{
	std::fstream file(path.c_str());
	if (file.fail())
	{
		std::cerr << "Unable to open MD5 mesh file: " << path << std::endl;
		return false;
	}

	file.seekg(0, file.end);
	sf::Int32 fileLength = static_cast<sf::Int32>(file.tellg());
	assert(fileLength > 0);
	file.seekg(0, file.beg);

	std::string param, junk;
	MeshHeader header;
	MeshBuffer::JointPtr joints = std::make_shared<MeshBuffer::Joints>();

	file >> param;
	while (!file.eof())
	{
		if (param == "MD5Version")
		{
			file >> header.version;
			assert(header.version == 10);
		}
		else if (param == "commandline")
		{
			//ignore doom engine commands
			file.ignore(fileLength, '\n');
		}
		else if (param == "numJoints")
		{
			file >> header.numJoints;
			joints->reserve(header.numJoints);
		}
		else if (param == "numMeshes")
		{
			file >> header.numMeshes;
		}
		else if (param == "joints")
		{
			MeshBuffer::Joint joint;
			file >> junk; //reads away the '{' chars

			for (auto i = 0u; i < header.numJoints; ++i)
			{
				file >> joint.name >> joint.parentId >> junk
					>> joint.position.x >> joint.position.y >> joint.position.z >> junk >> junk
					>> joint.rotation.x >> joint.rotation.y >> joint.rotation.z >> junk;

				Helpers::String::RemoveChar(joint.name, '\"');

				m_ComputeQuatW(joint.rotation);

				joints->push_back(joint);
				file.ignore(fileLength, '\n');
			}
			file >> junk; //closing '}'
		}
		else if (param == "mesh")
		{
			BufferPtr mesh = std::make_shared<MeshBuffer>();
			sf::Uint32 numVerts, numFaces, numWeights;

			file >> junk; //'{'
			file >> param;

			while (param != "}")
			{
				if (param == "shader")
				{
					std::string shaderName;
					file >> shaderName;
					Helpers::String::RemoveChar(shaderName, '\"');
					//remove file extension
					sf::Int32 pos = shaderName.find_last_of(".");
					if (pos != std::string::npos)
						shaderName = shaderName.substr(0, pos);
					//remove any file path
					pos = shaderName.find_last_of("/");
					if (pos != std::string::npos)
						shaderName = shaderName.substr(pos + 1);

					mesh->SetTextureName(shaderName);

					file.ignore(fileLength, '\n');
				}
				else if (param == "numverts")
				{
					file >> numVerts;
					file.ignore(fileLength, '\n');

					MeshBuffer::Vertices& verts = mesh->GetVertices();
					verts.reserve(numVerts);

					for (auto i = 0u; i < numVerts; ++i)
					{
						MeshBuffer::Vertex v;
						file >> junk >> junk >> junk
							>> v.texCoord.x >> v.texCoord.y >> junk
							>> v.startWeight >> v.weightCount;

						file.ignore(fileLength, '\n');
						verts.push_back(v);
					}
				}
				else if (param == "numtris")
				{
					file >> numFaces;
					MeshBuffer::Faces& faces = mesh->GetFaces();
					faces.reserve(numFaces);

					file.ignore(fileLength, '\n');

					for (auto i = 0u; i < numFaces; ++i)
					{
						MeshBuffer::Face face;
						file >> junk >> junk >> face.vertIndex[2] >> face.vertIndex[1] >> face.vertIndex[0];
						file.ignore(fileLength, '\n');
						faces.push_back(face);
					}
				}
				else if (param == "numweights")
				{
					file >> numWeights;
					MeshBuffer::Weights& weights = mesh->GetWeights();
					weights.reserve(numWeights);

					file.ignore(fileLength, '\n');

					for (auto i = 0u; i < numWeights; ++i)
					{
						MeshBuffer::Weight w;
						file >> junk >> junk >> w.jointId >> w.bias >> junk
							>> w.position.x >> w.position.y >> w.position.z >> junk;

						file.ignore(fileLength, '\n');
						weights.push_back(w);
					}
				}
				else
				{
					file.ignore(fileLength, '\n');
				}
				file >> param;
			}

			//set mesh pointer to joints
			mesh->SetJoints(joints);

			m_ApplyBoneWeights(*mesh);

			m_CalcNormals(*mesh);

			destination.push_back(mesh);
		}
		file >> param;
	}

	assert(joints->size() == header.numJoints);
	assert(destination.size() == header.numMeshes);
	return true;
}

//private
void MD5Parser::m_ComputeQuatW(glm::quat& quat)
{
	float t = 1.f - (quat.x * quat.x) - (quat.y * quat.y) - (quat.z * quat.z);
	if (t < 0.f)
		quat.w = 0.f;
	else
		quat.w = -std::sqrt(t);
}

void MD5Parser::m_ApplyBoneWeights(MeshBuffer& mesh)
{
	auto& verts = mesh.GetVertices();
	const auto& weights = mesh.GetWeights();
	const auto& joints = *mesh.GetJoints();

	for (auto& v : verts)
	{
		for (auto i = 0u; i < v.weightCount; ++i)
		{
			const auto& w = weights[v.startWeight + i];
			const auto& j = joints[w.jointId];

			const glm::vec3 rotpos = j.rotation * w.position;
			v.position += (j.position + rotpos) * w.bias;
		}
	}
}

void MD5Parser::m_CalcNormals(MeshBuffer& mesh)
{
	auto& faces = mesh.GetFaces();
	auto& verts = mesh.GetVertices();
	
	for (auto& f : faces)
	{
		const std::array<glm::vec3, 3u> p = { verts[f.vertIndex[0]].position, verts[f.vertIndex[1]].position, verts[f.vertIndex[2]].position };
		glm::vec3 deltaPos1 = p[1] - p[0];
		glm::vec3 deltaPos2 = p[2] - p[0];
		f.normal = Helpers::Vectors::Cross(deltaPos1, deltaPos2);


		const std::array<glm::vec2, 3u> uv = { verts[f.vertIndex[0]].texCoord, verts[f.vertIndex[1]].texCoord, verts[f.vertIndex[2]].texCoord };
		glm::vec2 deltaUV1 = uv[1] - uv[0];
		glm::vec2 deltaUV2 = uv[2] - uv[0];

		float r = 1.f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
		glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
		glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

		for (sf::Uint8 i = 0; i < 3; ++i)
		{
			auto& v = verts[f.vertIndex[i]];
			v.normal += f.normal;
			v.tangent += tangent;
			v.bitangent += bitangent;
		}
	}

	const auto& joints = *mesh.GetJoints();
	const auto& weights = mesh.GetWeights();

	//normalize all the normals
	for (auto& v : verts)
	{
		glm::vec3 n = Helpers::Vectors::Normalize(v.normal);
		glm::vec3 t = Helpers::Vectors::Normalize(v.tangent);
		glm::vec3 b = Helpers::Vectors::Normalize(v.bitangent);

		v.normal = glm::vec3();
		v.tangent = glm::vec3();
		v.bitangent = glm::vec3();
		
		//move normals into joint space
		for (auto i = 0u; i < v.weightCount; ++i)
		{
			const auto& w = weights[v.startWeight + i];
			const auto& j = joints[w.jointId];

			v.normal += (n * j.rotation) * w.bias;
			v.tangent += (t * j.rotation) * w.bias;
			v.bitangent += (b * j.rotation) * w.bias;
		}
	}
}