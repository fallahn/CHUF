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

//non animated meshes, such as scenery. Meshes can still be transformed / scaled
//for basic animations, but mesh data is not deformable like MorphMeshes
#ifndef STATIC_NODE_H_
#define STATIC_NODE_H_

#include <Mesh/MeshNode.h>

namespace ml
{
	class StaticMeshNode : public MeshNode
	{
	public:
		typedef std::shared_ptr<StaticMeshNode> Ptr;

		StaticMeshNode();
		explicit StaticMeshNode(BufferCollection& meshBuffers);
		virtual ~StaticMeshNode(){};

		virtual void SetBufferCollection(BufferCollection& meshBuffers);

		void AddDiffuseTexture(sf::Texture& t);
		void AddNormalTexture(sf::Texture& t);
		void AddMaskTexture(sf::Texture& t);

	protected:


	private:
		std::vector<sf::Texture*> m_diffuseTextures;
		std::vector<sf::Texture*> m_normalTextures;
		std::vector<sf::Texture*> m_maskTextures;
		
		void m_DrawSelf(Node::RenderPass pass);

	};
}

#endif //STATIC_NODE_H_