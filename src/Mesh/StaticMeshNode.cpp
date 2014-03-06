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

#include <Mesh/StaticMeshNode.h>

using namespace ml;

//ctors
StaticMeshNode::StaticMeshNode()
{}

StaticMeshNode::StaticMeshNode(BufferCollection& meshBuffers)
{
	SetBufferCollection(meshBuffers);
}

//public
void StaticMeshNode::SetBufferCollection(BufferCollection& meshBuffers)
{
	assert(meshBuffers.size());
	for (auto& mb : meshBuffers)
		mb->Allocate();
	m_SetBufferCollection(&meshBuffers);
}

void StaticMeshNode::AddDiffuseTexture(sf::Texture& t)
{
	m_diffuseTextures.push_back(&t);
}

void StaticMeshNode::AddNormalTexture(sf::Texture& t)
{
	m_normalTextures.push_back(&t);
}

void StaticMeshNode::AddMaskTexture(sf::Texture& t)
{
	m_maskTextures.push_back(&t);
}

//protected
void StaticMeshNode::m_DrawSelf(Node::RenderPass pass)
{
	BufferCollection& bc = *m_BufferCollection();
	sf::Uint16 i = 0u;
	for (auto& mb : bc)
	{
		if (i < m_diffuseTextures.size())
			SetDiffuseTexture(*m_diffuseTextures[i]);

		if (i < m_normalTextures.size())
			SetNormalTexture(*m_normalTextures[i]);

		if (i < m_maskTextures.size())
			SetMaskTexture(*m_maskTextures[i]);

		m_SetMeshBuffer(mb.get());
		MeshNode::m_DrawSelf(pass);

		++i;
	}
}