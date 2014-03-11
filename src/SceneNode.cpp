/*********************************************************************
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

///source for SceneNode class///

#include <Game/SceneNode.h>
#include <Box2D/Dynamics/b2Body.h>
#include <Box2D/Dynamics/b2Fixture.h>
#include <Mesh/MeshScene.h>

#include <cassert>

using namespace Game;

///ctor
SceneNode::SceneNode(AudioManager& am)
	: m_parent		(nullptr),
	m_category		(NodeCommand::Scene),
	m_drawPhysEnt	(false),
	m_audioManager	(am),
	m_inputBits		(0u),
	m_xAxisInput	(0.f),
	m_yAxisInput	(0.f),
	m_zAxisInput	(0.f)
{
}

//dtor
SceneNode::~SceneNode()
{
	//make sure we tidy up any attached components
	for (auto& c : m_components)
		c->Delete();
}

///public
void SceneNode::AddChild(NodePtr& child)
{
	child->m_parent = this;
	//set world position relative to parent
	child->setPosition(child->getPosition() - GetWorldPosition());
	m_children.push_back(std::move(child));
}

SceneNode::NodePtr SceneNode::RemoveChild(const SceneNode& child)
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

void SceneNode::RemoveChildren()
{
	m_children.clear();
}

void SceneNode::Update(float dt)
{
	updateSelf(dt);

	m_children.erase(std::remove_if(
		m_children.begin(),
		m_children.end(),
		[](const NodePtr& p)->bool
	{
		return p->Deleted();
	}), m_children.end());

	for(auto&& c : m_children)
		c->Update(dt);
}

sf::Uint16 SceneNode::GetCategory() const
{
	return m_category;
}

void SceneNode::SetCategory(sf::Uint16 category)
{
	m_category = category;
	m_SetPhysCategory();
}

void SceneNode::OnCommand(const NodeCommand& command, float dt)
{
	if (command.category & m_category) //command is intended for this node
	{
		if (command.action)
			command.action(*this, dt);
		//execute any component commands
		for (auto& c : command.componentCommands)
		{
			auto component = m_FindComponent(c.targetType);
			if (component != m_components.end())
			{
				c.action(*component->get(), dt);
			}
		}
	}

	//pass command down to children
	for(auto&& c : m_children)
		c->OnCommand(command, dt);
}

void SceneNode::SetSprite(const sf::Sprite& sprite)
{
	m_sprite = sprite;
}

sf::Sprite& SceneNode::GetSprite()
{
	return m_sprite;
}

sf::Transform SceneNode::GetWorldTransform() const
{
	sf::Transform t;
	for (const SceneNode* node = this; node != nullptr; node = node->m_parent)
		t = node->getTransform() * t;

	return t;
}

sf::Vector2f SceneNode::GetWorldPosition() const
{
	return GetWorldTransform() * sf::Vector2f();
}

void SceneNode::SetWorldPosition(sf::Vector2f position)
{
	if (m_parent)
		position -= m_parent->GetWorldPosition();

	setPosition(position);
}

void SceneNode::SetPosition3D(const sf::Vector3f& position)
{
	m_3dPosition = position;
}

void SceneNode::SetPosition3D(float x, float y, float z)
{
	m_3dPosition.x = x;
	m_3dPosition.y = y;
	m_3dPosition.z = z;
}

void SceneNode::Move3D(const sf::Vector3f& amount)
{
	m_3dPosition += amount;
}

void SceneNode::Move3D(float x, float y, float z)
{
	m_3dPosition.x += x;
	m_3dPosition.y += y;
	m_3dPosition.z += z;
}

const sf::Vector3f& SceneNode::GetPosition3D() const
{
	return m_3dPosition;
}

void SceneNode::SetRotation3D(const sf::Vector3f& rotation)
{
	m_3dRotation = rotation;
}

void SceneNode::SetRotation3D(float x, float y, float z)
{
	m_3dRotation.x = x;
	m_3dRotation.y = y;
	m_3dRotation.z = z;
}

void SceneNode::Rotate3D(const sf::Vector3f& amount)
{
	m_3dRotation += amount;
}

void SceneNode::Rotate3D(float x, float y, float z)
{
	m_3dRotation.x += x;
	m_3dRotation.y += y;
	m_3dRotation.z += z;
}

const sf::Vector3f& SceneNode::GetRotation3D() const
{
	return m_3dRotation;
}

void SceneNode::Delete()
{
	m_SetDeleted();
}

void SceneNode::AttachComponent(Component::Ptr& component)
{
	assert(!component->Attached());

	component->SetParent(*this);
	Component::Type type = component->GetType();

	//remove existing components first
	if (type != Component::Type::Particle)
	{
		auto c = m_FindComponent(type);
		if (c != m_components.end())
			c->get()->Delete();
	}

	switch (type)
	{
	case Component::Type::Physics:
	{
		auto p = dynamic_cast<PhysEntity*>(component.get());
		auto b = p->GetPhysBody();
		b->SetUserData(this);
		m_SetPhysCategory();
	}
		break;

	default: break;
	}

	m_components.push_back(component);
}

void SceneNode::DrawPhysEnt(bool b)
{
	m_drawPhysEnt = b;
}

void SceneNode::PlaySound(Audio::Effect effect)
{
	m_audioManager.PlayEffect(effect, GetWorldPosition());
}

void SceneNode::SetInput(SceneNode::InputBit bit, bool b)
{
	(b) ? m_inputBits |= bit : m_inputBits &= ~bit;
}

void SceneNode::SetInput(SceneNode::InputAxis axis, float value)
{
	assert(value > -100.f && value < 100.f);
	switch (axis)
	{
	case X:
		m_xAxisInput = value;
		break;
	case Y:
		m_yAxisInput = value;
		break;
	case Z:
		m_zAxisInput = value;
		break;
	default: break;
	}
}

sf::Uint16 SceneNode::GetInputBits() const
{
	return m_inputBits;
}

sf::Vector3f SceneNode::GetInputAxis() const
{
	return sf::Vector3f(m_xAxisInput, m_yAxisInput, m_zAxisInput);
}

void SceneNode::ResetInput()
{
	m_inputBits = 0u;
	m_xAxisInput = 0.f;
	m_yAxisInput = 0.f;
	m_zAxisInput = 0.f;
}

///private
void SceneNode::draw(sf::RenderTarget& rt, sf:: RenderStates states) const
{
	states.transform *= getTransform(); //add in our own transform before passing states to our children

	drawSelf(rt, states);

	//draw all our children
	for(auto&& c : m_children)
		c->draw(rt, states);
}

void SceneNode::drawSelf(sf::RenderTarget& rt, sf::RenderStates states) const
{
	rt.draw(m_sprite, states);

	if (m_drawPhysEnt)
	{
		auto c = m_FindComponent(Component::Type::Physics);
		if (c != m_components.end())
		{
			auto pb = dynamic_cast<PhysEntity*>(c->get());
			rt.draw(*pb, states);
		}
	}
}

void SceneNode::m_SetPhysCategory()
{
	auto c = m_FindComponent(Component::Type::Physics);
	if (c != m_components.end())
	{
		auto pb = dynamic_cast<PhysEntity*>(c->get());
		b2Filter filter;
		filter.categoryBits = m_category;

		auto body = pb->GetPhysBody();
		for (auto f = body->GetFixtureList(); f; f = f->GetNext())
		{
			f->SetFilterData(filter);
		}
	}
}

std::vector<Component::Ptr>::const_iterator SceneNode::m_FindComponent(Component::Type type) const
{
	return std::find_if(m_components.begin(),
					m_components.end(),
					[&type](const Component::Ptr& c)->bool
					{
						return c->GetType() == type;
					});
}

void SceneNode::updateSelf(float dt)
{
	//remove component if marked as deleted
	m_components.erase(std::remove_if(m_components.begin(),
										m_components.end(),
										[](const Component::Ptr& p)->bool
										{
											return p->Deleted();
										}), m_components.end());

	//update node via remaining components
	for (auto& p : m_components)
		p->UpdateParent(dt);

}
