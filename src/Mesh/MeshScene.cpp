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

//implements drawable root node of a mesh scene graph///
#include <Mesh/MeshScene.h>

#ifndef GLM_FORCE_RADIANS
//#define GLM_FORCE_RADIANS
#endif //forces all angles in glm to radians

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <cassert>

using namespace ml;

namespace
{
	//config file directories here
	const std::string meshPath = "assets/meshes/"; //remember trailing /
	const std::string mapPath = "assets/maps/";
	const std::string texturePath = "assets/textures/materials/";
	const std::string materialPath = "assets/materials/";
	const std::string reflectMapPath = "assets/textures/environment/nebula_env.jpg";

	const char debugShader[] =
		"#version 120\n"
		"uniform sampler2D tex;"
		"void main()"
		"{"
		"gl_FragColor = texture2D(tex, gl_TexCoord[0].xy);"
		"}";

	const float directionalLightNear = 1.5f;
	const float directionalLightFar = -4.f;
	const glm::vec2 lightPosOffset(-2.5f, 10.5f);
	const glm::vec2 lightTargetOffset(0.f, -0.5f);
}

//ctor
MeshScene::MeshScene(const sf::RenderWindow& rw, Game::ShaderResource& shaderResource)
	:	m_camera		(std::make_unique<Camera>()),
	m_directionalLight	(std::make_shared<Light>()),
	m_useDirectionalLight(true),
	m_shaderResource	(shaderResource),
	m_renderWindow		(rw),
	m_cameraZ			(0.f),
	m_sceneScale		(0.01f),
	m_biasMatrix		(0.5, 0.0, 0.0, 0.0,
						0.0, 0.5, 0.0, 0.0,
						0.0, 0.0, 0.5, 0.0,
						0.5, 0.5, 0.5, 1.0),
	m_shadowShader		(shaderResource.Get(ml::MeshShader::Shadow)),
	m_blendShader		(shaderResource.Get(Game::SfmlShader::ShadowBlend)),
	m_depthTextureId	(0),
	m_depthBufferId		(0)
{
	Resize();

	//set light direction
	m_lights.push_back(m_directionalLight);
}

//public
LightPtr MeshScene::AddLight()
{
	LightPtr l(new Light);
	if (m_useDirectionalLight)
	{
		m_useDirectionalLight = false;
		m_lights.pop_back();
	}
	m_lights.push_back(l);

	return l;
}

StaticMeshNode::Ptr MeshScene::AddStaticMesh(BufferCollection& meshBuffers)
{
	StaticMeshNode::Ptr node = std::make_shared<StaticMeshNode>(meshBuffers);
	node->SetParentScene(this);
	AddChild(static_cast<NodePtr>(node));
	return node;
}

MorphMeshNode::Ptr MeshScene::AddMorphMesh(BufferCollection& meshBuffers)
{
	MorphMeshNode::Ptr node = std::make_shared<MorphMeshNode>(meshBuffers);
	AddChild(static_cast<NodePtr>(node));
	node->SetParentScene(this);
	return node;
}

SkeletalMeshNode::Ptr MeshScene::AddSkeletalMesh(BufferCollection& bc)
{
	SkeletalMeshNode::Ptr node = std::make_shared<SkeletalMeshNode>(bc);
	AddChild(static_cast<NodePtr>(node));
	node->SetParentScene(this);
	return node;
}

void MeshScene::SetView(const sf::View& view)
{
	//we only want to do this if the aspect ratio has changed
	//to avoid unnecessary calls to tan
	const float aspect = view.getSize().x / view.getSize().y;
	if (m_camera->GetAspectRatio() != aspect)//potentially rendered moot by float comparison...
	{
		m_camera->SetAspectRatio(aspect);
		const float angle = std::tan(m_camera->GetFOV() / 2.f * 0.0174532925f);
		m_cameraZ = (static_cast<float>(view.getSize().y) / 2.f) / angle;
		m_cameraZ *= -m_sceneScale;
	}
	//set position
	m_camera->SetPosition(-view.getCenter().x * m_sceneScale,
						view.getCenter().y * m_sceneScale,
						m_cameraZ);

	//update viewport
	sf::Vector2u winSize = m_renderWindow.getSize();
	GLuint x = static_cast<GLuint>(view.getViewport().left * static_cast<float>(winSize.x));
	GLuint y = static_cast<GLuint>((1.f - view.getViewport().top) * static_cast<float>(winSize.y));
	GLuint w = static_cast<GLuint>(view.getViewport().width * static_cast<float>(winSize.x));
	GLuint h = static_cast<GLuint>(view.getViewport().height * static_cast<float>(winSize.y));

	//invert position
	y -= h;
	glViewport(x, y, w, h);

	//update directional light
	if (m_useDirectionalLight)
	{
		glm::vec3 pos = m_camera->GetPosition();
		m_directionalLight->SetPosition(-pos.x + lightPosOffset.x, -pos.y + lightPosOffset.y, directionalLightNear);
		m_directionalLight->SetTarget(-pos.x + lightTargetOffset.x, -pos.y + lightTargetOffset.y, directionalLightFar);
	}
}

void MeshScene::UpdateScene(float dt, const Game::ViewCamera::Cameras& viewCams)
{	
	Update(dt); //updates self and all children

	//update depth texture
	m_context.setActive(true); //we need a valid context before we can draw
	glBindFramebuffer(GL_FRAMEBUFFER, m_depthBufferId);
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(2.f, 4.f);

	//TODO check if directional light is false and do multipass lighting

	//need to store the combined view projection matrix for each view
	std::vector<std::pair<const sf::View, glm::mat4>> viewMatrices;

	Nodes& children = m_GetChildren();
	for (const auto& v : viewCams)
	{
		SetView(v->GetView());
		viewMatrices.push_back(std::make_pair(v->GetView(), m_ApplyDepthMatrices()));

		for (auto&& c : children)
		{
			if (m_camera->ViewContains(c->GetPosition()))
			{
				c->Draw(Node::RenderPass::Depth);
			}
		}
	}
	glDisable(GL_POLYGON_OFFSET_FILL);
	glFlush();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	//---------------------------//
	//update shadow texture


	m_shadowTexture.setActive(true);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glClearColor(1.f, 1.f, 1.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	for (const auto& p : viewMatrices)
	{
		SetView(p.first);
		m_camera->Draw();

		m_shadowShader.setParameter("depthV", m_lights[0]->GetViewMatrix());
		m_shadowShader.setParameter("lightPos", m_lights[0]->GetPosition());

		for (auto&& c : children)
		{
			if (m_camera->ViewContains(c->GetPosition()))
			{
				glm::mat4 m = c->GetTransform();
				m_shadowShader.setParameter("depthM", m);
				m = p.second * m;
				m_shadowShader.setParameter("depthMVP", m_biasMatrix * m);
				c->Draw(Node::RenderPass::Shadow);
			}
		}
	}

	m_shadowTexture.display();

	//---------------------------//
	//update colour texture
	m_colourTexture.setActive(true);
	//return params for normal drawing
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glClearColor(0.f, 0.f, 0.f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_FUNC_ADD);
	
	for (const auto& p : viewMatrices)
	{
		SetView(p.first);
		m_camera->Draw();

		for (auto&& c : children)
		{
			glm::vec3 pos = c->GetPosition();
			if (m_camera->ViewContains(pos))
			{
				c->ApplyLighting(m_lights);
				c->Draw(Node::RenderPass::Colour);
			}
		}
	}
	glDisable(GL_BLEND);
	glDisable(GL_LIGHTING);
	m_colourTexture.display();
}

void MeshScene::Resize()
{	
	//clear out any existing buffers before recreating them
	m_DeleteBuffers();
	
	sf::Vector2u winSize = m_renderWindow.getSize();
	m_CreateDepthBuffer(winSize.x, winSize.y);

	m_shadowTexture.create(winSize.x, winSize.y, true);
	m_blendShader.setParameter("shadowMap", m_shadowTexture.getTexture());
	m_shadowSprite.setTexture(m_shadowTexture.getTexture(), true);
	m_shadowTexture.setActive(true);
	glViewport(0, 0, winSize.x, winSize.y);

	m_colourTexture.create(winSize.x, winSize.y, true);
	m_colourSprite.setTexture(m_colourTexture.getTexture(), true);
	m_colourTexture.setActive(true);
	glViewport(0, 0, winSize.x, winSize.y);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	

	//set camera position
	m_camera->SetAspectRatio(static_cast<float>(winSize.x) / winSize.y);
	const float angle = std::tan(m_camera->GetFOV() / 2.f * 0.0174532925f);
	m_cameraZ = (static_cast<float>(m_renderWindow.getView().getSize().y) / 2.f) / angle;	
	m_cameraZ *= -m_sceneScale;
}

const sf::Texture& MeshScene::GetTexture()
{
	return m_colourTexture.getTexture();
}

float MeshScene::GetSceneScale() const
{
	return m_sceneScale;
}

void MeshScene::LoadScene(const std::string& mapName)
{
	//m_GetChildren().clear(); //remove existing scene data

	////attempt to open file
	//std::string path = mapPath + mapName + ".sne";
	//pugi::xml_document sceneDoc;
	//pugi::xml_parse_result result = sceneDoc.load_file(path.c_str());
	//if(!result)
	//{
	//	std::cerr <<"Failed to open scene graph " << path << std::endl;
	//	std::cerr << "Reason: " << result.description() << std::endl;
	//	return;
	//}

	////get root node and iterate over children
	//pugi::xml_node rootNode, meshNode;
	//if(!(rootNode = sceneDoc.child("scene"))
	//	|| !(meshNode = rootNode.child("mesh")))
	//{
	//	std::cerr << "Invalid mesh scene data..." << std::endl;
	//	std::cerr << "Scene graph not created." << std::endl;
	//	return;
	//}

	//while(meshNode)
	//{
	//	std::string meshName = meshNode.attribute("name").as_string();
	//	assert(!meshName.empty());
	//	assert(meshNode.child("position"));
	//	assert(meshNode.child("rotation"));
	//	assert(meshNode.child("scale"));

	//	pugi::xml_node posNode, rotNode, scaleNode;
	//	posNode = meshNode.child("position");
	//	rotNode = meshNode.child("rotation");
	//	scaleNode = meshNode.child("scale");

	//	glm::vec3 position, rotation, scale;
	//	position.x = posNode.attribute("x").as_float();
	//	position.y = posNode.attribute("y").as_float();
	//	position.z = posNode.attribute("z").as_float();

	//	rotation.x = rotNode.attribute("x").as_float();
	//	rotation.y = rotNode.attribute("y").as_float();
	//	rotation.z = rotNode.attribute("z").as_float();

	//	scale.x = scaleNode.attribute("x").as_float();
	//	scale.y = scaleNode.attribute("y").as_float();
	//	scale.z = scaleNode.attribute("z").as_float();

	//	//load mesh
	//	StaticMeshNode* staticMesh = AddStaticMesh(m_meshResource.Get(meshPath + meshName, ml::FileType::Obj));
	//	staticMesh->SetPosition(position);
	//	staticMesh->SetRotation(rotation);
	//	staticMesh->SetScale(scale);

	//	//load material data if found
	//	pugi::xml_node matNode;
	//	if(matNode = meshNode.child("material"))
	//	{
	//		std::string matName = materialPath + mapName + "/" + matNode.text().as_string();
	//		pugi::xml_document matDoc;
	//		result = matDoc.load_file(matName.c_str());
	//		if(!result)
	//		{
	//			std::cerr << "Failed to load material file " << matName << std::endl;
	//			std::cerr << result.description() << std::endl;
	//		}
	//		else
	//		{
	//			assert(matDoc.child("matdef"));
	//			pugi::xml_node defNode = matDoc.child("matdef");
	//			assert(defNode.child("shader"));
	//			assert(defNode.child("map"));

	//			//get texture info
	//			pugi::xml_node texNode = defNode.child("map");
	//			if(texNode.attribute("diffuse")) 
	//				staticMesh->SetDiffuseTexture(m_textureResource.Get(texturePath + mapName + "/" + texNode.attribute("diffuse").as_string()));

	//			//parse shader types - TODO set up shadow casting
	//			std::string shaderType = defNode.child("shader").attribute("type").as_string();
	//			if(shaderType == "phong")
	//			{
	//				staticMesh->SetShader(m_shaderResource.Get(MeshShader::Phong), MeshShader::Phong);
	//			}
	//			else if(shaderType == "normal")
	//			{
	//				assert(texNode.attribute("normal"));
	//				assert(texNode.attribute("mask"));
	//				
	//				ShaderProgram& shader = m_shaderResource.Get(MeshShader::Normal);
	//				shader.setParameter("reflectMap", m_textureResource.Get(reflectMapPath));
	//				staticMesh->SetShader(shader, MeshShader::Normal);
	//				staticMesh->SetNormalTexture(m_textureResource.Get(texturePath + texNode.attribute("normal").as_string()));
	//				staticMesh->SetMaskTexture(m_textureResource.Get(texturePath + texNode.attribute("mask").as_string()));
	//			}

	//			if(pugi::xml_node matDataNode = defNode.child("material"))
	//			{
	//				//create opengl material
	//				assert(matDataNode.child("ambient"));
	//				assert(matDataNode.child("diffuse"));
	//				assert(matDataNode.child("specular"));
	//				assert(matDataNode.child("emissive"));
	//				assert(matDataNode.child("shininess"));

	//				sf::Color ambient(matDataNode.child("ambient").attribute("r").as_uint(),
	//								matDataNode.child("ambient").attribute("g").as_uint(),
	//								matDataNode.child("ambient").attribute("b").as_uint());

	//				sf::Color diffuse(matDataNode.child("diffuse").attribute("r").as_uint(),
	//								matDataNode.child("diffuse").attribute("g").as_uint(),
	//								matDataNode.child("diffuse").attribute("b").as_uint());

	//				sf::Color specular(matDataNode.child("specular").attribute("r").as_uint(),
	//								matDataNode.child("specular").attribute("g").as_uint(),
	//								matDataNode.child("specular").attribute("b").as_uint());

	//				sf::Color emissive(matDataNode.child("emissive").attribute("r").as_uint(),
	//								matDataNode.child("emissive").attribute("g").as_uint(),
	//								matDataNode.child("emissive").attribute("b").as_uint());

	//				float shininess = matDataNode.child("shininess").attribute("value").as_float();

	//				Material meshMaterial(ambient, diffuse, specular, emissive);
	//				meshMaterial.SetShininess(shininess);
	//				staticMesh->SetMaterial(meshMaterial);
	//			}
	//		}

	//	}
	//	else
	//	{
	//		std::cerr << "Material not found for " << meshName << std::endl;
	//	}
	//	meshNode = meshNode.next_sibling("mesh"); //move to next mesh
	//}
	////parse any light positions and add lights to the scene
	//if(pugi::xml_node lightNode = rootNode.child("light"))
	//{
	//	while(lightNode)
	//	{
	//		assert(lightNode.attribute("x"));
	//		assert(lightNode.attribute("y"));
	//		assert(lightNode.attribute("z"));
	//		
	//		glm::vec3 lightPos(lightNode.attribute("x").as_float(),
	//							lightNode.attribute("y").as_float(),
	//							lightNode.attribute("z").as_float());

	//		LightPtr l = AddLight();
	//		l->SetPosition(lightPos);

	//		lightNode = lightNode.next_sibling("light");
	//	}
	//}
	//else
	//{
	//	std::cerr << "Warning: no lighting data found in scene" << std::endl;
	//}
}



//private
void MeshScene::m_CreateDepthBuffer(sf::Uint16 width, sf::Uint16 height)
{
	m_context.setActive(true); //make sure we have a valid context

	//create FBO
	glGenFramebuffers(1, &m_depthBufferId);
	glBindFramebuffer(GL_FRAMEBUFFER, m_depthBufferId);

	glViewport(0, 0, width, height);

	//create depth only texture and attach to fbo
	glGenTextures(1, &m_depthTextureId);
	glBindTexture(GL_TEXTURE_2D, m_depthTextureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	//these 2 lines allow using 2dShadow sampler in shader, but need to be commented out when rendering
	//the depth texture to screen.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTextureId, 0);

	//prevent drawing colour to currently bound frame buffer
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cerr << "Error creating depth fbo" << std::endl;
	}
	else
	{
		const float w = static_cast<float>(width);
		const float h = static_cast<float>(height);
		m_shadowShader.setParameter("shadowMap", m_depthTextureId);
		m_shadowShader.setParameter("mapScale", sf::Vector2f(1.f / w, 1.f / h));
		m_blendShader.setParameter("width", w);
		m_blendShader.setParameter("height", h);
	}
}

void MeshScene::m_DeleteBuffers()
{
	if (m_depthBufferId)
		glDeleteFramebuffers(1, &m_depthBufferId);
	if (m_depthTextureId)
		glDeleteTextures(1, &m_depthTextureId);
}

void MeshScene::draw(sf::RenderTarget& rt, sf::RenderStates states) const
{
	sf::View v = rt.getView();
	rt.setView(m_colourTexture.getView());
	states.shader = &m_blendShader;
	rt.draw(m_colourSprite, states);

	//debug
	//rt.draw(m_shadowSprite);
	
	/*rt.pushGLStates();
	drawShadowMap(static_cast<sf::Vector2f>(rt.getSize()));
	rt.popGLStates();*/

	rt.setView(v);
}

void MeshScene::drawShadowMap(const sf::Vector2f& size) const
{
	const float scale = 0.334f;
	const float left = -size.x / 2.f;
	const float right = size.x / 2.f;
	const float bottom = -size.y / 2.f;
	const float top = size.y / 2.f;

	glUseProgramObjectARB(0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glm::mat4 projection = glm::ortho<float>(left, right, bottom, top, 1.f, 20.f);
	glLoadMatrixf(glm::value_ptr(projection));

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glColor4f(1, 1, 1, 1);
	glActiveTextureARB(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_depthTextureId);
	//sf::Texture::bind(&m_depthTexture.getTexture());
	glEnable(GL_TEXTURE_2D);
	glTranslated(0, 0, -1);
	glBegin(GL_QUADS);

	glTexCoord2d(0, 0); glVertex3f(left, top * scale, 0);
	glTexCoord2d(1, 0); glVertex3f(left * scale, top * scale, 0);
	glTexCoord2d(1, 1); glVertex3f(left * scale, top, 0);
	glTexCoord2d(0, 1); glVertex3f(left, top, 0);
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

glm::mat4 MeshScene::m_ApplyDepthMatrices()
{
	glm::mat4 depthProjectionMatrix = m_lights[0]->GetProjectionMatrix();
	glm::mat4 depthViewMatrix = m_lights[0]->GetViewMatrix();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glLoadMatrixf(glm::value_ptr(depthProjectionMatrix));

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glLoadMatrixf(glm::value_ptr(depthViewMatrix));

	return depthProjectionMatrix * depthViewMatrix;
}