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

///source for static mesh node class///
#include <Mesh/MeshNode.h>
#include <Mesh/Shaders/Normal.h>
#include <Mesh/Shaders/Phong.h>

#include <glm/gtc/type_ptr.hpp>

using namespace ml;

MorphAnimation::MorphAnimation()
: start	(0u),
end		(0),
fps		(12.f)
{}

MorphAnimation::MorphAnimation(sf::Uint16 first, sf::Uint16 last, float frameRate)
: start	(first),
end		(last),
fps		(frameRate)
{

}

//ctor
MeshNode::MeshNode()
	:	m_meshBuffer	(nullptr),
		m_diffuseTexture(nullptr),
		m_normalTexture	(nullptr),
		m_maskTexture	(nullptr),
		m_shader		(nullptr),
		m_shadowShader	(nullptr),
		m_sendMVMat		(false),
		m_castShadows	(false),
		m_drawVertNormals(false),
		m_receiveShadows(true)
{

}

//public
void MeshNode::SetBufferCollection(BufferCollection& meshBuffers)
{

}

void MeshNode::SetDiffuseTexture(sf::Texture& texture)
{
	m_diffuseTexture = &texture;
	m_diffuseTexture->setRepeated(true);
}

void MeshNode::SetNormalTexture(sf::Texture& texture)
{
	m_normalTexture = &texture;
	m_normalTexture->setRepeated(true);
}

void MeshNode::SetMaskTexture(sf::Texture& texture)
{
	m_maskTexture = &texture;
	m_maskTexture->setRepeated(true);
}

void MeshNode::SetShader(ShaderProgram& shader, MeshShader shaderType)
{
	if (shaderType == MeshShader::Shadow)
	{
		m_shadowShader = &shader;
		m_receiveShadows = true;
	}
	else
	{
		m_shader = &shader;
		m_currentShaderType = shaderType;
		m_sendMVMat = (shaderType == MeshShader::Normal);
	}
}

void MeshNode::SetCastShadows(bool b)
{
	m_castShadows = b;
}

void MeshNode::SetRecieveShadows(bool b)
{
	m_receiveShadows = b;
}

void MeshNode::DrawVertexNormals(bool b)
{
	m_drawVertNormals = b;
}

bool MeshNode::VisibleVertexNormals() const
{
	return m_drawVertNormals;
}

//protected
void MeshNode::m_DrawDebug(){}

void MeshNode::m_UpdateSelf(float dt)
{

}

void MeshNode::m_DrawSelf(Node::RenderPass pass)
{
	if (!m_MeshBuffer()) return;

	glPushMatrix();
	glMultMatrixf(glm::value_ptr(GetTransform()));

	m_ApplyMaterial();

	//bind texture and shader if they exist
	switch (pass)
	{
	case RenderPass::Colour:
		if (!m_shader && m_DiffuseTexture())
		{
			glEnable(GL_TEXTURE_2D);
			sf::Texture::bind(m_diffuseTexture);
		}

		if (m_shader)
		{
			switch (m_currentShaderType)
			{
			case MeshShader::Normal:
				if (m_diffuseTexture)
					m_shader->setParameter("colorMap", *m_diffuseTexture);
				if (m_normalTexture)
					m_shader->setParameter("normalMap", *m_normalTexture);
				if (m_maskTexture)
					m_shader->setParameter("maskMap", *m_maskTexture);
				break;
			case MeshShader::Phong:

				break;
			default:break;
			}
			ShaderProgram::bind(m_shader);
		}
		break;
	case RenderPass::Depth:
		ShaderProgram::bind(0);
		if (!m_castShadows)
		{
			//skip drawing as an a occluder
			glPopMatrix();
			sf::Texture::bind(0);
			return;
		}
		break;
	case RenderPass::Shadow:
		if (m_receiveShadows && m_shadowShader)
		{
			ShaderProgram::bind(m_shadowShader);
		}
		else
		{
			glPopMatrix();
			sf::Texture::bind(0);
			return;
		}
		break;
	}

	//apply shader matrix for cube mapping
	if (m_sendMVMat)
	{
		std::array<float, 16> modelView;
		glGetFloatv(GL_MODELVIEW_MATRIX, &modelView[0]);
		m_shader->setParameter("modelView4x4", &modelView[0]);
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	m_meshBuffer->Use(MeshBuffer::VERTEX_BUFFER);
	glVertexPointer(3, GL_FLOAT, 0, 0);

	glEnableClientState(GL_NORMAL_ARRAY);
	m_meshBuffer->Use(MeshBuffer::NORMAL_BUFFER);
	glNormalPointer(GL_FLOAT, 0, 0);

	if (pass == RenderPass::Colour && m_currentShaderType == MeshShader::Normal)
	{
		//send tangents / bitangents to shader
		m_meshBuffer->Use(MeshBuffer::TANGENT_BUFFER);
		m_shader->setVertexAttribute("tangent", 3, 0);
		m_meshBuffer->Use(MeshBuffer::BITANGENT_BUFFER);
		m_shader->setVertexAttribute("bitangent", 3, 0);
	}

	glEnableClientState(GL_COLOR_ARRAY);
	m_meshBuffer->Use(MeshBuffer::COLOUR_BUFFER);
	glColorPointer(4, GL_UNSIGNED_BYTE, 0, 0);

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	m_meshBuffer->Use(MeshBuffer::TEXTURE_BUFFER);
	glTexCoordPointer(2, GL_FLOAT, 0, 0);

	m_MeshBuffer()->Use(MeshBuffer::INDEX_BUFFER);
	glDrawElements(GL_TRIANGLES, m_meshBuffer->GetFaces().size() * 3, GL_UNSIGNED_INT, 0);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	ShaderProgram::bind(0);
	sf::Texture::bind(0);
	glDisable(GL_TEXTURE_2D);

	if (m_drawVertNormals && pass == RenderPass::Colour)
	{
		m_DrawNormals();
		m_DrawDebug();
	}

	glPopMatrix();
}

void MeshNode::m_SetMeshBuffer(MeshBuffer* buffer)
{
	m_meshBuffer = buffer;
}

void MeshNode::m_SetBufferCollection(BufferCollection* bc)
{
	m_bufferCollection = bc;
}

BufferCollection* MeshNode::m_BufferCollection() const
{
	return m_bufferCollection;
}

MeshBuffer* MeshNode::m_MeshBuffer() const
{
	return m_meshBuffer;
}

const sf::Texture* MeshNode::m_DiffuseTexture() const
{
	return m_diffuseTexture;
}

const sf::Texture* MeshNode::m_NormalTexture() const
{
	return m_normalTexture;
}

const sf::Texture* MeshNode::m_MaskTexture() const
{
	return m_maskTexture;
}

ShaderProgram* MeshNode::m_Shader() const
{
	return m_shader;
}

const ShaderProgram* MeshNode::m_ShadowShader() const
{
	return m_shadowShader;
}

MeshShader MeshNode::m_CurrentShaderType() const
{
	return m_currentShaderType;
}

bool MeshNode::m_SendMVMat() const
{
	return m_sendMVMat;
}

bool MeshNode::m_CastShadows() const
{
	return m_castShadows;
}

bool MeshNode::m_ReceiveShadows() const
{
	return m_receiveShadows;
}

void MeshNode::m_DrawNormals()
{
	if (m_meshBuffer)
	{
		glPushAttrib(GL_ENABLE_BIT);
		glDisable(GL_LIGHTING);

		MeshBuffer::Vertices& verts = m_meshBuffer->GetVertices();

		//normal
		glColor3f(0.f, 0.f, 1.f);
		glBegin(GL_LINES);
		for (const auto& v : verts)
		{
			const std::array<float, 3u> start = { v.position.x, v.position.y, v.position.z };
			const glm::vec3 ep = v.position + (v.normal * 2.f); //make them a bit longer
			const std::array<float, 3u> end = { ep.x, ep.y, ep.z };

			glVertex3fv(&start[0]);
			glVertex3fv(&end[0]);
		}
		glEnd();

		//tangent
		glColor3f(1.f, 0.f, 0.f);
		glBegin(GL_LINES);
		for (const auto& v : verts)
		{
			const std::array<float, 3u> start = { v.position.x, v.position.y, v.position.z };
			const glm::vec3 ep = v.position + (v.tangent * 2.f);
			const std::array<float, 3u> end = { ep.x, ep.y, ep.z };

			glVertex3fv(&start[0]);
			glVertex3fv(&end[0]);
		}
		glEnd();

		//bitangent
		glColor3f(0.f, 1.f, 0.f);
		glBegin(GL_LINES);
		for (const auto& v : verts)
		{
			const std::array<float, 3u> start = { v.position.x, v.position.y, v.position.z };
			const glm::vec3 ep = v.position + (v.bitangent * 2.f);
			const std::array<float, 3u> end = { ep.x, ep.y, ep.z };

			glVertex3fv(&start[0]);
			glVertex3fv(&end[0]);
		}
		glEnd();


		glPopAttrib();
	}
}
