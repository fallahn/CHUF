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

///base class for particle types///

#ifndef BASE_PARTICLE_H_
#define BASE_PARTICLE_H_

#include <SFML/System/Vector2.hpp>
#include <SFML/System/NonCopyable.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/RenderTexture.hpp>

#include <Game/Component.h>

#include <memory>
#include <array>

namespace Game
{
	typedef std::array<sf::Vertex*, 4u> QuadVerts;
	class BaseParticle : private sf::NonCopyable
	{
	public:
		enum class State
		{
			Alive,
			Dying,
			Dead
		};

		explicit BaseParticle(QuadVerts& verts);
		virtual ~BaseParticle();

		virtual void Update(float dt) = 0;
		void Spawn(const sf::Vector2f& position, const sf::Vector2f& m_velocity, float m_force, float rotation);
		State GetState() const{return m_state;};
		void Kill(){m_state = State::Dead;};
		void SetTexture(const sf::Texture& texture);
		void ApplyTransform();

		typedef std::shared_ptr<BaseParticle> Ptr;
	protected:
		float m_force; //force in pixels per second
		sf::Vector2f m_velocity;
		State m_state;

		void m_SetColour(const sf::Color& colour);
		void m_Move(const sf::Vector2f& distance);
		void m_SetPosition(const sf::Vector2f& position);
		void m_Rotate(float angle);
		void m_SetRotation(float angle);
		void m_SetSize(const sf::Vector2f& size);
		void m_Scale(float x, float y);
		void m_SetScale(float x, float y);

	private:
		QuadVerts m_vertices;
		sf::Vector2f m_position;
		sf::Vector2f m_size, m_scale;
		float m_rotation;

		bool m_applyTransform;
	};



	class ParticleSystem : public Component, public sf::Drawable, private sf::NonCopyable
	{
	public:
		enum class Type
		{
			Smoke
		};

		typedef std::shared_ptr<ParticleSystem> Ptr;

		ParticleSystem(float emissionRate, sf::Uint16 count, BaseParticle::Ptr (*Create)(QuadVerts& verts));
		void Update(float dt);
		void Start(float duration = 0.f)
		{
			m_enabled = true;
			m_duration = duration;
			m_durationClock.restart();
		};
		void Stop(){m_enabled = false;};
		void SetPosition(const sf::Vector2f& position){ m_position = position; };
		void SetVelocity(const sf::Vector2f& velocity){m_velocity = velocity;};
		void SetRandomVelocity(bool b);
		void SetRandomVelocity(const sf::Vector2f& bounds);
		void SetStrength(float strength){m_force = strength;};
		void SetRotation(float rotation){m_rotation = rotation;};
		//performs hard reset on all particles
		void Kill();
		void SetTexture(const sf::Texture& texture);
		void SetBlendMode(sf::BlendMode mode);
		void SetOrigin(const sf::Vector2f& origin);
		//implement component
		void UpdateParent(float dt) override;
		Component::Type GetType() const override;
		void Delete() override;

	private:
		sf::VertexArray m_vertices;
		std::vector<BaseParticle::Ptr>m_particles;
		sf::Uint16 m_particleCount, m_currentIndex;
		float m_emissionRate; //particles per second
		sf::Clock m_emitClock, m_durationClock;
		sf::Vector2f m_position, m_velocity, m_origin;
		float m_force, m_rotation, m_duration;
		bool m_enabled, m_randomVelocity;
		sf::Vector2f m_velocityBounds; //a value between this and negative this is added to velocity if enabled
		const sf::Texture* m_texture;
		sf::BlendMode m_blendMode;

		void draw(sf::RenderTarget& rt, sf::RenderStates states) const;
	};
};

#endif //base particle