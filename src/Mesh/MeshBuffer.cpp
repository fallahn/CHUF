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

///source for mesh buffer class///
#include <Mesh/MeshBuffer.h>
#include <Mesh/MeshHelpers.h>

#include <map>

using namespace ml;

//ctor / dtor
MeshBuffer::MeshBuffer()
: m_vertexBufferId	(0),
m_normalBufferId	(0),
m_tangentBufferId	(0),
m_bitangentBufferId	(0),
m_colourBufferId	(0),
m_textureBufferId	(0),
m_indexBufferId		(0)
{

}

MeshBuffer::~MeshBuffer()
{
	//make sure to delete any gl buffers
	glDeleteBuffers(1, &m_vertexBufferId);
	glDeleteBuffers(1, &m_normalBufferId);
	glDeleteBuffers(1, &m_tangentBufferId);
	glDeleteBuffers(1, &m_bitangentBufferId);
	glDeleteBuffers(1, &m_colourBufferId);
	glDeleteBuffers(1, &m_textureBufferId);
	glDeleteBuffers(1, &m_indexBufferId);	
}


//public
void MeshBuffer::SetVertices(const Vertices& vertices)
{
	m_vertices = vertices;
}

MeshBuffer::Faces& MeshBuffer::GetFaces()
{
	return m_faces;
}

const MeshBuffer::Faces& MeshBuffer::GetFaces() const
{
	return m_faces;
}

MeshBuffer::Vertices& MeshBuffer::GetVertices()
{
	return m_vertices;
}

const MeshBuffer::Vertices& MeshBuffer::GetVertices() const
{
	return m_vertices;
}

MeshBuffer::Normals& MeshBuffer::GetNormals()
{
	return m_normals;
}

const MeshBuffer::Normals& MeshBuffer::GetNormals() const
{
	return m_normals;
}

MeshBuffer::Weights& MeshBuffer::GetWeights()
{
	return m_weights;
}

const MeshBuffer::Weights& MeshBuffer::GetWeights() const
{
	return m_weights;
}

void MeshBuffer::SetJoints(JointPtr joints)
{
	m_joints = joints;
}

MeshBuffer::JointPtr MeshBuffer::GetJoints() const
{
	assert(m_joints != nullptr);
	return m_joints;
}

void MeshBuffer::SetTextureName(const std::string& name)
{
	m_textureName = name;
}

const std::string& MeshBuffer::GetTextureName() const
{
	return m_textureName;
}

void MeshBuffer::Allocate(bool calcNormals)
{
    if(calcNormals) CalcNormals(); 

	if (m_vertexBufferId)
		glDeleteBuffers(1, &m_vertexBufferId);
	if (m_normalBufferId)
		glDeleteBuffers(1, &m_normalBufferId);
	if (m_tangentBufferId)
		glDeleteBuffers(1, &m_tangentBufferId);
	if (m_bitangentBufferId)
		glDeleteBuffers(1, &m_bitangentBufferId);
	if (m_colourBufferId)
		glDeleteBuffers(1, &m_colourBufferId);
	if (m_textureBufferId)
		glDeleteBuffers(1, &m_textureBufferId);
	if (m_indexBufferId)
		glDeleteBuffers(1, &m_indexBufferId);

	glGenBuffers(1, &m_vertexBufferId);
	glGenBuffers(1, &m_normalBufferId);
	glGenBuffers(1, &m_tangentBufferId);
	glGenBuffers(1, &m_bitangentBufferId);
	glGenBuffers(1, &m_colourBufferId);
	glGenBuffers(1, &m_textureBufferId);
	glGenBuffers(1, &m_indexBufferId);

	//allocates the member data to the gl buffer objects
	sf::Uint32 vertexSize = m_vertices.size(), indexSize = m_faces.size() * 3;

    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId);
    glBufferData(GL_ARRAY_BUFFER, vertexSize * 3 * sizeof(float), 0, GL_STREAM_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_normalBufferId);
    glBufferData(GL_ARRAY_BUFFER, vertexSize * 3 * sizeof(float), 0, GL_STREAM_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, m_tangentBufferId);
	glBufferData(GL_ARRAY_BUFFER, vertexSize * 3 * sizeof(float), 0, GL_STREAM_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, m_bitangentBufferId);
	glBufferData(GL_ARRAY_BUFFER, vertexSize * 3 * sizeof(float), 0, GL_STREAM_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_colourBufferId);
    glBufferData(GL_ARRAY_BUFFER, vertexSize * 4 * sizeof(sf::Uint8), 0, GL_STREAM_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_textureBufferId);
    glBufferData(GL_ARRAY_BUFFER, vertexSize * 2 * sizeof(float), 0, GL_STREAM_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBufferId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize * sizeof(sf::Uint32), 0, GL_STREAM_DRAW);

    Update();
}

void MeshBuffer::Update()
{
    sf::Uint32 vertexSize = m_vertices.size(), indexSize = m_faces.size() * 3;
    sf::Uint32 v = 0, n = 0, t = 0, c = 0, i = 0;

    std::unique_ptr<float[]> vertices(new float[vertexSize * 3]);
	std::unique_ptr<float[]> normals(new float[vertexSize * 3]);
	std::unique_ptr<float[]> tangents(new float[vertexSize * 3]);
	std::unique_ptr<float[]> bitangents(new float[vertexSize * 3]);
    std::unique_ptr<float[]> texCoords(new float[vertexSize * 2]);
	std::unique_ptr<sf::Uint8[]> colors(new sf::Uint8[vertexSize * 4]);
    std::unique_ptr<sf::Uint32[]> indices(new sf::Uint32[indexSize]);

    for (auto vertex : m_vertices)
    {
        vertices[v + 0] = vertex.position.x;
        vertices[v + 1] = vertex.position.y;
        vertices[v + 2] = vertex.position.z;
        v += 3;

        normals[n + 0] = vertex.normal.x;
        normals[n + 1] = vertex.normal.y;
        normals[n + 2] = vertex.normal.z;

		tangents[n + 0] = vertex.tangent.x;
		tangents[n + 1] = vertex.tangent.y;
		tangents[n + 2] = vertex.tangent.z;

		bitangents[n + 0] = vertex.tangent.x;
		bitangents[n + 1] = vertex.tangent.y;
		bitangents[n + 2] = vertex.tangent.z;
        n += 3;

        colors[c + 0] = vertex.colour.r;
        colors[c + 1] = vertex.colour.g;
        colors[c + 2] = vertex.colour.b;
        colors[c + 3] = vertex.colour.a;
        c += 4;

        texCoords[t + 0] = vertex.texCoord.x;
        texCoords[t + 1] = vertex.texCoord.y;
        t += 2;
    }

    for (auto face : m_faces)
    {
        indices[i + 0] = face.vertIndex[0];
        indices[i + 1] = face.vertIndex[1];
        indices[i + 2] = face.vertIndex[2];

        i += 3;
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertexSize * 3 * sizeof(float), vertices.get());

    glBindBuffer(GL_ARRAY_BUFFER, m_normalBufferId);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertexSize * 3 * sizeof(float), normals.get());

	glBindBuffer(GL_ARRAY_BUFFER, m_tangentBufferId);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertexSize * 3 * sizeof(float), tangents.get());

	glBindBuffer(GL_ARRAY_BUFFER, m_bitangentBufferId);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertexSize * 3 * sizeof(float), bitangents.get());

    glBindBuffer(GL_ARRAY_BUFFER, m_colourBufferId);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertexSize * 4 * sizeof(sf::Uint8), colors.get());

    glBindBuffer(GL_ARRAY_BUFFER, m_textureBufferId);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertexSize * 2 * sizeof(float), texCoords.get());

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBufferId);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indexSize * sizeof(sf::Uint32), indices.get());
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void MeshBuffer::Use(MeshBuffer::BufferType type)
{
    switch(type)
	{
	case VERTEX_BUFFER:
        glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId);
		return;
	case COLOUR_BUFFER:
        glBindBuffer(GL_ARRAY_BUFFER, m_colourBufferId);
		return;
	case NORMAL_BUFFER:
        glBindBuffer(GL_ARRAY_BUFFER, m_normalBufferId);
		return;
	case TANGENT_BUFFER:
		glBindBuffer(GL_ARRAY_BUFFER, m_tangentBufferId);
		return;
	case BITANGENT_BUFFER:
		glBindBuffer(GL_ARRAY_BUFFER, m_bitangentBufferId);
		return;
	case TEXTURE_BUFFER:
        glBindBuffer(GL_ARRAY_BUFFER, m_textureBufferId);
		return;
	case INDEX_BUFFER:
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBufferId);
		return;
	default:
        glBindBuffer(GL_ARRAY_BUFFER, 0);
		return;
	}
}

void MeshBuffer::CalcNormals()
{
	for(auto& f : m_faces)
    {
        const std::array<glm::vec3, 3u> p = { m_vertices[f.vertIndex[0]].position, m_vertices[f.vertIndex[1]].position, m_vertices[f.vertIndex[2]].position };
		glm::vec3 deltaPos1 = p[1] - p[0];
		glm::vec3 deltaPos2 = p[2] - p[0];		
		f.normal = glm::cross(deltaPos1, deltaPos2);

		const std::array<glm::vec2, 3u> uv = { m_vertices[f.vertIndex[0]].texCoord, m_vertices[f.vertIndex[1]].texCoord, m_vertices[f.vertIndex[2]].texCoord };
		glm::vec2 deltaUV1 = uv[1] - uv[0];
		glm::vec2 deltaUV2 = uv[2] - uv[0];

		float r = 1.f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
		glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
		glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

        for (sf::Uint8 i = 0; i < 3; ++i)
        {
            glm::vec3 a = p[(i + 1) % 3] - p[i];
            glm::vec3 b = p[(i + 2) % 3] - p[i];
			float weight = std::acos(glm::dot(a, b) / (glm::length(a) * glm::length(b)));
			auto& v = m_vertices[f.vertIndex[i]];
			v.normal += weight * f.normal;
			v.tangent += tangent;
			v.bitangent += bitangent;
        }
    }

	//sum weighted normals of shared vertices
	std::map<sf::Int32, Vertex> accumulator;
	for (auto& v : m_vertices)
	{
		auto f = accumulator.find(v.positionId);
		if (f == accumulator.end())
			accumulator.insert(std::make_pair(v.positionId, v));
		else
		{
			f->second.normal += v.normal;
			f->second.tangent += v.tangent;
			f->second.bitangent += v.bitangent;
		}
	}

	//set normalised normal
	for(auto& v : m_vertices)
    {
		v.normal = Helpers::Vectors::Normalize(accumulator[v.positionId].normal);
		v.tangent = Helpers::Vectors::Normalize(accumulator[v.positionId].tangent);
		v.bitangent = Helpers::Vectors::Normalize(accumulator[v.positionId].bitangent);
    }
}

//private