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

///base class for 3D scene nodes. Analogous to the SceneNode class but dedicated
//to 3D scenes. These nodes can be tied to SceneNodes so that they may update each other.

#ifndef NODE_H_
#define NODE_H_

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

#include <Mesh/GL/glew.h>
#include <Mesh/Light.h>
#include <Mesh/Material.h>
#include <Mesh/MeshHelpers.h>
#include <Mesh/ShaderProgram.h>

#include <glm/matrix.hpp>
#include <glm/vec3.hpp>

#include <Game/Component.h>

#include <vector>
#include <memory>
#include <array>

namespace ml
{
	class Node;
	class MeshScene;
	typedef std::shared_ptr<Node> NodePtr;
	typedef std::vector<NodePtr> Nodes;

	class Node : private sf::NonCopyable, public Game::Component
	{
	public:
		enum class RenderPass
		{
			Depth,
			Colour,
			Shadow
		};

		Node();
		virtual ~Node();

		//sets 3d world position
		void SetPosition(const glm::vec3& position);
		void SetPosition(float x, float y, float z);
		//returns position in 3d world
		const glm::vec3& GetPosition() const;
		//moves node - equivalent of position + move parameter
		void Move(const glm::vec3& move);
		void Move(float x, float y, float z);
		//sets the node rotation
		void SetRotation(const glm::vec3& rotation);
		void SetRotation(float x, float y, float z);
		//returns the nodes current rotation
		const glm::vec3& GetRotation() const;
		//rotates the node - equivalent of current rotation + rotate parameter
		void Rotate(const glm::vec3& rotate);
		void Rotate(float x, float y, float z);
		//sets the nodes scale
		void SetScale(const glm::vec3& scale);
		//returns the nodes current scale
		const glm::vec3& GetScale() const;
		//scales the node - equvalent of current scale + scale parameter
		void Scale(const glm::vec3& scale);
		//adds a child node
		void AddChild(NodePtr node);
		//removes a child node
		NodePtr RemoveChild(const Node& node);
		//applies a material to a node
		void SetMaterial(const Material& material);
		//gets the name of the current material
		const Material& GetMaterial() const;
		//applies lighting to node data
		void ApplyLighting(Lights& lights);
		//returns the model matrix
		glm::mat4 GetTransform() const;
		//return 2D AABB in world space, used for frustum culling
		sf::FloatRect GetGlobalBounds() const;
		//delete this node (and all children)
		void Delete();

		//impliment component
		void UpdateParent(float dt);
		Game::Component::Type GetType() const;

		void Update(float dt);
		void Draw(RenderPass pass);

		void SetParentScene(const MeshScene* scene);
		const MeshScene& GetParentScene() const;
    protected:

        void m_ApplyMaterial();

		virtual void m_UpdateSelf(float dt);
		virtual void m_DrawSelf(RenderPass pass);

		std::array<glm::vec4, 8u>& m_GetBoundingBox();

		Material& m_Material(){return m_material;}
		Nodes& m_GetChildren(){return m_children;}

	private:
        //world coordinates
		glm::vec3 m_position, m_rotation, m_scale;

        //Parent / children
        Nodes m_children;
        Node* m_parent;
	
		const MeshScene* m_parentScene;

        //Materials
        Material m_material;

		//vert positions for bounding box
		std::array<glm::vec4, 8u> m_boundingBox;

		void m_ClampRotation();
	};
}

#endif //NODE_H_