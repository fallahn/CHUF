///dummy car for testing
#ifndef DUMMYCAR
#define DUMMYCAR

#include <Game/LineSegment.h>

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

#include <array>
#include <memory>

namespace Game
{
	class DummyCar : public sf::Drawable, private sf::NonCopyable
	{
	public:
		DummyCar() : m_rotation (0.f)
		{
			m_collisionPoints[0] = sf::Vector2f(-200.f, -100.f);
			m_collisionPoints[1] = sf::Vector2f(200.f, -100.f);
			m_collisionPoints[2] = sf::Vector2f(200.f, 100.f);
			m_collisionPoints[3] = sf::Vector2f(-200.f, 100.f);
		}
		void Update(float dt)
		{
			//parse input into transform
			sf::Vector2f movement;
			const float movespeed = 500.f;
			if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
				movement.x = -movespeed *dt;
			else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
				movement.x = movespeed * dt;

			if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
				movement.y = -movespeed * dt;
			else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
				movement.y = movespeed * dt;

			m_position += movement;

			float rotation = 0.f;
			if(sf::Keyboard::isKeyPressed(sf::Keyboard::A))
				rotation = -45.f * dt;
			else if(sf::Keyboard::isKeyPressed(sf::Keyboard::D))
				rotation = 45.f * dt;

			m_rotation += rotation;
			if(m_rotation > 360.f) m_rotation -= 360.f;
			else if (m_rotation < 0.f) m_rotation += 350.f;


			//update position
			sf::Transform t;
			t.rotate(rotation, m_position).translate(movement);
			for(auto& p : m_collisionPoints)
				 p = t.transformPoint(p);

			m_lineSegs[0] = std::unique_ptr<LineSegment>(new LineSegment(m_collisionPoints[0], m_collisionPoints[2]));
			m_lineSegs[1] = std::unique_ptr<LineSegment>(new LineSegment(m_collisionPoints[1], m_collisionPoints[3]));
		}
		sf::Vector2f GetPosition() const
		{
			return m_position;	
		}
		std::array<sf::Vector2f, 4>& GetCollisionPoints()
		{
			return m_collisionPoints;
		}
		float GetRotation() const { return m_rotation;};
	private:
		float m_rotation;
		sf::Vector2f m_position; // centre point
		std::array<sf::Vector2f, 4> m_collisionPoints;
		std::array<std::unique_ptr<LineSegment>, 2> m_lineSegs;
		void draw(sf::RenderTarget& rt, sf::RenderStates states) const
		{
			rt.draw(*m_lineSegs[0]);
			rt.draw(*m_lineSegs[1]);
		}
	};
}

#endif