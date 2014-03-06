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

///source for base article class and particle system ///

#include <Game/BaseParticle.h>
#include <Game/SceneNode.h>

#include <Helpers.h>
#include <cassert>

using namespace Game;

//------BaseParticle---///

//ctor
BaseParticle::BaseParticle(QuadVerts& verts)
	: m_state			(State::Dead),
	m_rotation			(0.f),
	m_size				(1.f, 1.f),
	m_scale				(1.f, 1.f),
	m_vertices			(verts),
	m_applyTransform	(false)
{

}

//dtor
BaseParticle::~BaseParticle(){}

//public
void BaseParticle::Spawn(const sf::Vector2f& position, const sf::Vector2f& velocity, float force, float rotation)
{
	m_velocity = velocity;
	m_force = force;
	m_state = State::Alive;
	m_SetPosition(position);
	m_SetRotation(rotation);
}

void BaseParticle::SetTexture(const sf::Texture& texture)
{
	//set up tex coords - actual texture is stored by particle system
	assert(m_vertices[0]);

	m_vertices[1]->texCoords.x = static_cast<float>(texture.getSize().x);
	m_vertices[2]->texCoords = static_cast<sf::Vector2f>(texture.getSize());
	m_vertices[3]->texCoords.y = static_cast<float>(texture.getSize().y);

	m_SetSize(static_cast<sf::Vector2f>(texture.getSize()));
}

void BaseParticle::ApplyTransform()
{
	if(m_applyTransform)
	{
		//set position / size
		sf::Vector2f halfSize = m_size / 2.f;
		m_vertices[0]->position = m_position - halfSize;
		m_vertices[1]->position.x = m_position.x + halfSize.x;
		m_vertices[1]->position.y = m_position.y - halfSize.y;
		m_vertices[2]->position = m_position + halfSize;
		m_vertices[3]->position.x = m_position.x - halfSize.x;
		m_vertices[3]->position.y = m_position.y + halfSize.y;

		//set rotation and scale
		sf::Transform t;
		t.rotate(m_rotation, m_position);
		t.scale(m_scale, m_position);

		for(auto& v : m_vertices)
			v->position = t.transformPoint(v->position);

		m_applyTransform = false;
	}
}

//protected

void BaseParticle::m_SetColour(const sf::Color& colour)
{
	assert(m_vertices[0]&& m_vertices[3]);
	for(auto& v : m_vertices)
		v->color = colour;
}

void BaseParticle::m_Move(const sf::Vector2f& distance)
{
	m_position += distance;
	m_applyTransform = true;
}

void BaseParticle::m_SetPosition(const sf::Vector2f& position)
{
	m_position = position;
	m_applyTransform = true;
}

void BaseParticle::m_Rotate(float angle)
{
	m_rotation += angle;
	m_applyTransform = true;
}

void BaseParticle::m_SetRotation(float angle)
{
	m_rotation = angle;
	m_applyTransform  = true;
}

void BaseParticle::m_SetSize(const sf::Vector2f& size)
{
	m_size = size;
	m_applyTransform = true;
}

void BaseParticle::m_Scale(float x, float y)
{
	m_scale.x *= x;
	m_scale.y *= y;
	m_applyTransform = true;
}

void BaseParticle::m_SetScale(float x, float y)
{
	m_scale.x = x;
	m_scale.y = y;
	m_applyTransform = true;
}

//private


//------ParticleSystem----//

//ctor
ParticleSystem::ParticleSystem(float emissionRate, sf::Uint16 count, BaseParticle::Ptr (*Create)(QuadVerts& verts))
	: m_emissionRate	(1.f / emissionRate), //particles per second
	m_particleCount		(count),
	m_currentIndex		(0u),
	m_force				(0.f),
	m_rotation			(0.f),
	m_enabled			(false),
	m_randomVelocity	(false),
	m_vertices			(sf::Quads, count * 4u),
	m_texture			(nullptr),
	m_blendMode			(sf::BlendAlpha)
{
	m_particles.reserve(count);

	sf::Uint16 i, j;
	for(i = 0u, j = 0u; i < count; i++, j += 4u)
	{
		std::array<sf::Vertex*, 4u> verts = {&m_vertices[j], &m_vertices[j + 1u], &m_vertices[j + 2u], &m_vertices[j + 3u]};
		m_particles.push_back(Create(verts));
	}
}

//dtor


//public

void ParticleSystem::UpdateParent(float dt)
{
	SetPosition(m_Parent()->GetWorldTransform() * m_origin);
}

Component::Type ParticleSystem::GetType() const
{
	return Component::Type::Particle;
}

void ParticleSystem::Delete()
{
	m_SetDeleted();
}

void ParticleSystem::Update(float dt)
{							
	//if enabled spawn new particle
	if(m_enabled)
	{
		if(m_emitClock.getElapsedTime().asSeconds() > m_emissionRate)
		{		
			m_currentIndex++;
			if(m_currentIndex == m_particleCount)
				m_currentIndex = 0u;
			//don't reset living particles
			if (m_particles[m_currentIndex]->GetState() != BaseParticle::State::Alive)
			{
				sf::Vector2f velocity = m_velocity;
				if (m_randomVelocity)
				{
					velocity.x += Helpers::Random::Float(-m_velocityBounds.x, m_velocityBounds.x);
					velocity.y += Helpers::Random::Float(-m_velocityBounds.y, m_velocityBounds.y);
				}
				
				m_particles[m_currentIndex]->Spawn(m_position, velocity, m_force, m_rotation);
				m_emitClock.restart();
			}
		}
		//update emission duration
		if(m_duration)
		{
			if(m_duration < m_durationClock.getElapsedTime().asSeconds())
				m_enabled = false;
		}
	}

	//then update all particles
	for(auto&& p : m_particles)
		p->Update(dt);
}

void ParticleSystem::draw(sf::RenderTarget& rt, sf::RenderStates states) const
{
	for(auto&& p : m_particles)
		p->ApplyTransform();
	
	states.texture = m_texture;
	states.blendMode = m_blendMode;

	rt.draw(m_vertices, states);
}

void ParticleSystem::SetRandomVelocity(bool b)
{
	m_randomVelocity = b;
}

void ParticleSystem::SetRandomVelocity(const sf::Vector2f& bounds)
{
	m_velocityBounds = bounds;
	m_velocityBounds.x += 0.000001f;
	m_velocityBounds.y += 0.000001f;
	m_randomVelocity = true;
}

void ParticleSystem::Kill()
{
	for(auto&& p : m_particles)
		p->Kill();
}

void ParticleSystem::SetTexture(const sf::Texture& texture)
{
	for(auto&& p : m_particles)
		p->SetTexture(texture); //updates the texture coords for each particle
	
	m_texture = &texture;
}

void ParticleSystem::SetBlendMode(sf::BlendMode mode)
{
	m_blendMode = mode;
}

void ParticleSystem::SetOrigin(const sf::Vector2f& origin)
{
	m_origin = origin;
}

//private
