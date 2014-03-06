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

///source for 3d scene node base class///
#include <Mesh/Node.h>
#include <Mesh/MeshScene.h>
#include <Game/SceneNode.h>

#include <glm/gtc/matrix_transform.hpp>

#include <cassert>
#include <algorithm>

using namespace ml;

namespace
{
	const int MAX_LIGHTS = 8;
}

//ctor
Node::Node()
	: m_parent	(nullptr),
	m_scale		(glm::vec3(1.f, 1.f, 1.f)),
	m_parentScene(nullptr)
{

}

Node::~Node()
{

}

//public
void Node::SetPosition(const glm::vec3& position)
{
	m_position = position;
}

void Node::SetPosition(float x, float y, float z)
{
	m_position.x = x;
	m_position.y = y;
	m_position.z = z;
}

const glm::vec3& Node::GetPosition() const
{
	return m_position;
}

void Node::Move(const glm::vec3& amount)
{
	m_position += amount;
}

void Node::Move(float x, float y, float z)
{
	m_position.x += x;
	m_position.y += y;
	m_position.z += z;
}

void Node::SetRotation(const glm::vec3& rotation)
{
	m_rotation  = rotation;
}

void Node::SetRotation(float x, float y, float z)
{
	m_rotation.x = x;
	m_rotation.y = y;
	m_rotation.z = z;
	m_ClampRotation();
}

const glm::vec3& Node::GetRotation() const
{
	return m_rotation;
}

void Node::Rotate(const glm::vec3& amount)
{
	m_rotation += amount;
	m_ClampRotation();
}

void Node::Rotate(float x, float y, float z)
{
	m_rotation.x += x;
	m_rotation.y += y;
	m_rotation.z += z;
	m_ClampRotation();
}

void Node::SetScale(const glm::vec3& scale)
{
	m_scale = scale;
}

const glm::vec3& Node::GetScale() const
{
	return m_scale;
}

void Node::Scale(const glm::vec3& amount)
{
	m_scale += amount;
}

void Node::AddChild(NodePtr node)
{
	node->m_parent = this;
	m_children.push_back(node);
}

NodePtr Node::RemoveChild(const Node& child)
{
	auto result = std::find_if(m_children.begin(),
								m_children.end(),
								[&] (NodePtr& p)->bool {return p.get() == &child;});
	
	//make sure we actually found something
	assert(result != m_children.end());

	NodePtr retVal = std::move(*result); //save pointer to any child we may have found so we can return it
	retVal->m_parent = nullptr;
	m_children.erase(result);
	return retVal;
}

void Node::SetMaterial(const Material& material)
{
	m_material = material;
}

void Node::ApplyLighting(Lights& lights)
{
    // Get nearest lights
    for(auto&& l : lights)
		l->CalcBrightness(m_position);

    std::sort(lights.begin(), lights.end(),
		[](LightPtr& l1, LightPtr& l2)->bool { return l1->GetBrightness() > l2->GetBrightness(); }
	);

	//clamp max number of lights
    int lightCount = lights.size();
    sf::Uint32 maxLights = std::min(MAX_LIGHTS, lightCount);
	//create gl buffers for each light
    for (sf::Uint8 i = 0; i < maxLights; ++i)
    {
        Light& l = *lights[i];
		const float brightness = 0.8f + (0.2f / (l.GetBrightness() + 0.001f));//prevent div by zero

        glEnable(GL_LIGHT0 + i);

        const sf::Color& ambientColour = l.GetAmbientColour();
        GLfloat ambient[] = { static_cast<GLfloat>(ambientColour.r) / 255.f,
                                static_cast<GLfloat>(ambientColour.g) / 255.f,
                                static_cast<GLfloat>(ambientColour.b) / 255.f,
                                static_cast<GLfloat>(ambientColour.a) / 255.f };

        const sf::Color& diffuseColour = l.GetDiffuseColour();
		GLfloat diffuse[] = { static_cast<GLfloat>(diffuseColour.r) / 255.f * brightness,
								static_cast<GLfloat>(diffuseColour.g) / 255.f * brightness,
								static_cast<GLfloat>(diffuseColour.b) / 255.f * brightness,
								static_cast<GLfloat>(diffuseColour.a) / 255.f };

        const sf::Color& specularColour = l.GetSpecularColour();
        GLfloat specular[] = { static_cast<GLfloat>(specularColour.r) / 255.f,
                                static_cast<GLfloat>(specularColour.g) / 255.f,
                                static_cast<GLfloat>(specularColour.b) / 255.f,
                                static_cast<GLfloat>(specularColour.a) / 255.f };

        const glm::vec3& pos = l.GetPosition();
        GLfloat position[] = { pos.x, pos.y, pos.z, 0.f };

        glLightfv(GL_LIGHT0 + i, GL_AMBIENT, ambient);
        glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, diffuse);
        glLightfv(GL_LIGHT0 + i, GL_SPECULAR, specular);
        glLightfv(GL_LIGHT0 + i, GL_POSITION, position);

		//draws a line to the light
        /*glBegin(GL_LINES);
            glColor3ub(0, 0, 0);
            glVertex3f(pos.x, pos.y, pos.z);
            glVertex3f(0, 0, 0);
        glEnd();*/
    }
}

glm::mat4 Node::GetTransform() const
{
	glm::mat4 m = glm::translate(glm::mat4(), m_position);
	m = glm::rotate(m, m_rotation.y, glm::vec3(0.f, 1.f, 0.f));
	m = glm::rotate(m, m_rotation.z, glm::vec3(0.f, 0.f, 1.f));
	m = glm::rotate(m, m_rotation.x, glm::vec3(1.f, 0.f, 0.f));
	m = glm::scale(m, m_scale);
	return m;
}

sf::FloatRect Node::GetGlobalBounds() const
{
	glm::mat4 transform = GetTransform();

	sf::FloatRect rect;
	float minX = 0.f;
	float minY = 0.f;
	float maxX = 0.f;
	float maxY = 0.f;

	//put points into world space and build rect
	for (auto p : m_boundingBox)
	{
		p = p * transform;
		if (p.x < minX) minX = p.x;
		else if (p.x > maxX) maxX = p.x;

		if (p.y < minY) minY = p.y;
		else if (p.y > maxY) maxY = p.y;
	}
	const float width = maxX - minX;
	const float height = maxY - minY;
	rect.left = m_position.x - (width / 2.f);
	rect.width = width;
	rect.top = m_position.y;// -(height / 2.f);
	rect.height = height;
	return rect;
}

void Node::Delete()
{
	m_SetDeleted();
}

void Node::UpdateParent(float dt)
{
	const sf::Vector3f rot3D = m_Parent()->GetRotation3D();
	SetRotation(rot3D.x, rot3D.y, rot3D.z - m_Parent()->getRotation());

	const float scale = GetParentScene().GetSceneScale();
	const sf::Vector2f worldPos = m_Parent()->GetWorldPosition() * scale;
	const sf::Vector3f pos3D = m_Parent()->GetPosition3D() * scale;
	SetPosition(worldPos.x + pos3D.x, -worldPos.y + pos3D.y, pos3D.z);
}

Game::Component::Type Node::GetType() const
{
	return Game::Component::Type::Mesh;
}

void Node::Update(float dt)
{
	//remove children if marked for deletion
	m_children.erase(std::remove_if(
		m_children.begin(),
		m_children.end(),
		[](const NodePtr& node)->bool
	{
		return node->Deleted(); 
	}), m_children.end());
	
	m_UpdateSelf(dt);
	for(auto&& c : m_children)
		c->Update(dt);
}

void Node::Draw(RenderPass pass)
{
	m_DrawSelf(pass);
	for(auto&& c : m_children)
		c->Draw(pass);
}

void Node::SetParentScene(const MeshScene* scene)
{
	m_parentScene = scene;
}

const MeshScene& Node::GetParentScene() const
{
	assert(m_parentScene);
	return *m_parentScene;
}

//protected
void Node::m_ApplyMaterial()
{
    //set gl material buffers
	sf::Color c = m_material.GetAmbientColour();
	GLfloat ambient[] = { static_cast<GLfloat>(c.r) / 255.f,
                            static_cast<GLfloat>(c.g) / 255.f,
                            static_cast<GLfloat>(c.b) / 255.f,
                            static_cast<GLfloat>(c.a) / 255.f };

	c = m_material.GetDiffuseColour();
    GLfloat diffuse[] = { static_cast<GLfloat>(c.r) / 255.f,
                            static_cast<GLfloat>(c.g) / 255.f,
                            static_cast<GLfloat>(c.b) / 255.f,
                            static_cast<GLfloat>(c.a) / 255.f };

	c = m_material.GetSpecularColour();
    GLfloat specular[] = { static_cast<GLfloat>(c.r) / 255.f,
                            static_cast<GLfloat>(c.g) / 255.f,
                            static_cast<GLfloat>(c.b) / 255.f,
                            static_cast<GLfloat>(c.a) / 255.f };

	c = m_material.GetEmissiveColour();
    GLfloat emissive[] = { static_cast<GLfloat>(c.r) / 255.f,
                            static_cast<GLfloat>(c.g) / 255.f,
                            static_cast<GLfloat>(c.b) / 255.f,
                            static_cast<GLfloat>(c.a) / 255.f};

	//set shininess
	GLfloat shininess = m_material.GetShininess();

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emissive);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);

    if(m_material.Smooth())
        glShadeModel(GL_SMOOTH);
    else
        glShadeModel(GL_FLAT);
}


void Node::m_UpdateSelf(float dt)
{
	//implement this in derived classes
}

void Node::m_DrawSelf(RenderPass pass)
{
	//implement this in derived classes
}

std::array<glm::vec4, 8u>& Node::m_GetBoundingBox()
{
	return m_boundingBox;
}



//private
void Node::m_ClampRotation()
{
	if(m_rotation.x > 360.f) m_rotation.x -= 360.f;
	else if (m_rotation.x < -360.f) m_rotation.x += 360.f;

	if(m_rotation.y > 360.f) m_rotation.y -= 360.f;
	else if (m_rotation.y < -360.f) m_rotation.y += 360.f;

	if(m_rotation.z > 360.f) m_rotation.z -= 360.f;
	else if (m_rotation.z < -360.f) m_rotation.z += 360.f;
}