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

#ifndef MESH_NODE_H_
#define MESH_NODE_H_

#include <SFML/Graphics/Texture.hpp>

#include <Mesh/Node.h>
#include <Mesh/MeshBuffer.h>
#include <Mesh/ShaderProgram.h>

#include <Game/ShaderManager.h>

#include <memory>

namespace ml
{
	//defines animation ranges used in deformable mesh nodes
	struct MorphAnimation
	{
		MorphAnimation();
		MorphAnimation(sf::Uint16 first, sf::Uint16 last, float frameRate);
		sf::Uint16 start;
		sf::Uint16 end;
		float fps;
	};
	
	//base class for mesh nodes such as static or deformable models
	//which are usually expected to be rendered with normal mapping
	//and / or shadow casting.
	class MeshNode : public Node
	{
	public:
		MeshNode();
		virtual ~MeshNode(){};

		virtual void SetBufferCollection(BufferCollection& meshBuffers);
		//sets currently active diffuse texture
		void SetDiffuseTexture(sf::Texture& texture);
		//sets currently active normal texture
		void SetNormalTexture(sf::Texture& texture);
		//sets currently active mask texture
		void SetMaskTexture(sf::Texture& texture);
		void SetShader(ShaderProgram& shader, MeshShader shaderType);
		void SetCastShadows(bool b);
		void SetRecieveShadows(bool b);
		void DrawVertexNormals(bool b);
		bool VisibleVertexNormals() const;

	protected:
		virtual void m_UpdateSelf(float dt);
		virtual void m_DrawSelf(Node::RenderPass pass);
		virtual void m_DrawDebug();


		//accessors for private members
		void m_SetBufferCollection(BufferCollection* bc);
		BufferCollection* m_BufferCollection() const;
		void m_SetMeshBuffer(MeshBuffer* buffer);
		MeshBuffer* m_MeshBuffer() const;
		const sf::Texture* m_DiffuseTexture() const;
		const sf::Texture* m_NormalTexture() const;
		const sf::Texture* m_MaskTexture() const;
		ShaderProgram* m_Shader() const;
		const ShaderProgram* m_ShadowShader() const;
		MeshShader m_CurrentShaderType() const;
		bool m_SendMVMat() const;
		bool m_CastShadows() const;
		bool m_ReceiveShadows() const;
		
	private:
		//pointer to collection of buffers which
		//make up node. Several meshes can either
		//make one model, or represent multiple
		//key frames in the case of morph animations
		BufferCollection* m_bufferCollection;
		//pointer to buffer to draw
		MeshBuffer* m_meshBuffer;
		sf::Texture* m_diffuseTexture;
		sf::Texture* m_normalTexture;
		sf::Texture* m_maskTexture;
		ShaderProgram* m_shader; //currently active shader for colour pass
		ShaderProgram* m_shadowShader; //used if mesh receives shadows
		MeshShader m_currentShaderType;
		bool m_sendMVMat; //enable sending ModelView matrix to shaders which need it
		bool m_castShadows, m_receiveShadows;
		bool m_drawVertNormals;
		//utility for drawing vertex normals
		void m_DrawNormals();
	};
}

#endif //STATIC_MESH_H_