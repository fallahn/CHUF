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

//wrapper class for vertex/colour/normal buffers which make up a single mesh//
//if you write a particular mesh format loader its result should be stored//
//in a mesh buffer object so that it can be drawn by the node graph//
#ifndef MESH_BUFFER_H_
#define MESH_BUFFER_H_

#include <SFML/System/NonCopyable.hpp>
#include <SFML/Graphics/Color.hpp>

#include <vector>
#include <memory>
#include <array>

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

#include <Mesh/GL/glew.h>

namespace ml
{
	class MeshBuffer : private sf::NonCopyable
	{
	public:
		//struct to represent a single vertex
		struct Vertex
		{
			Vertex() : startWeight(0), weightCount(0), positionId(-1){}
			glm::vec3 position;
			glm::vec3 normal, tangent, bitangent;
			glm::vec2 texCoord;
			sf::Color colour;
			sf::Uint32 startWeight;
			sf::Uint32 weightCount;
			sf::Int32 positionId; //if verts have same position then they have the same Id
		};
		//struct which represents a single triangle / face made of 3 vertices
		struct Face
		{
			std::array<sf::Uint32, 3> vertIndex; //indices of vertices in vertex array
			std::array<sf::Uint32, 3> normalIndex; //indices of vertex normals in normal array
			glm::vec3 normal; //face normal
		};
		//enum for identifying data in a particular array
		enum BufferType
		{
			NONE,
			VERTEX_BUFFER,
			NORMAL_BUFFER,
			TANGENT_BUFFER,
			BITANGENT_BUFFER,
			COLOUR_BUFFER,
			TEXTURE_BUFFER,
			INDEX_BUFFER
		};
		//weights and joints for meshes with skeletal animation
		struct Weight
		{
			Weight() : jointId(-1), bias(0.f){}
			sf::Int32 jointId;
			float bias;
			glm::vec3 position;
		};

		struct Joint
		{
			Joint() : parentId(-1){}
			std::string name;
			sf::Int32 parentId;
			glm::vec3 position;
			glm::quat rotation;
		};

		//typedefs for creating buffers
		typedef std::vector<Vertex> Vertices;
		typedef std::vector<Face> Faces;
		typedef std::vector<glm::vec3> Normals;
		typedef std::vector<Weight> Weights;
		typedef std::vector<Joint> Joints;
		typedef std::shared_ptr<Joints> JointPtr;

		//members
		MeshBuffer();
		~MeshBuffer();

		//sets the vertex array
		void SetVertices(const Vertices& vertices);
		//gets the vertex buffer array
		Vertices& GetVertices();
		const Vertices& GetVertices() const;
		//gets the face buffer array
		Faces& GetFaces();
		const Faces& GetFaces() const;
		//gets the normal data array
		Normals& GetNormals();
		const Normals& GetNormals() const;
		//array of bone weights if skeletal information exists
		Weights& GetWeights();
		const Weights& GetWeights() const;
		//pointer to joint information for this mesh
		void SetJoints(JointPtr joints);
		JointPtr GetJoints() const;
		//texture name *without* file extension
		void SetTextureName(const std::string& name);
		const std::string& GetTextureName() const;
		//allocates the gl buffers for mesh data
		//only call this on buffers which will actually be drawn
		//as opposed to buffers used to store mesh data when parsing
		//a particular file format.
		void Allocate(bool calcNormals = true);
		void Update();
		void Use(BufferType type);
		//calculates face and vertex normals
		void CalcNormals();
	private:
		Vertices m_vertices; //vertex data array
		Normals m_normals; //normal data array
		Faces m_faces; //face data array
		Weights m_weights; //vertex weights in skeletal meshes
		JointPtr m_joints;//pointer as multiple meshes can share the same set of joints

		std::string m_textureName;

		//IDs of gl buffer objects
		GLuint m_vertexBufferId;
		GLuint m_normalBufferId;
		GLuint m_tangentBufferId;
		GLuint m_bitangentBufferId;
		GLuint m_colourBufferId;
		GLuint m_textureBufferId;
		GLuint m_indexBufferId;



	};

	typedef std::shared_ptr<MeshBuffer> BufferPtr;
	typedef std::vector<BufferPtr> BufferCollection;
}

#endif //MESH_BUFFER_H_