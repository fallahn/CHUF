///dummy box for testing collision testing :)///
#ifndef DUMMYBOX
#define DUMMYBOX

#include <Game/LineSegment.h>
#include <Game/DummyCar.h>

#include <SFML/System/NonCopyable.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include <array>
#include <memory>
#include <iostream>

namespace Game
{
	class DummyBox : public sf::Drawable, private sf::NonCopyable
	{
	public:
		enum Side
		{
			Top,
			Right,
			Bottom,
			Left
		};
		DummyBox()
		{
			m_rectangle.top = 500.f;
			m_rectangle.left = 700.f;
			m_rectangle.width = 400.f;
			m_rectangle.height = 50.f;

			//create line segs starting at top and winding clockwise
			m_lineSegs[Top] = std::unique_ptr<LineSegment>(
				new LineSegment(sf::Vector2f(m_rectangle.left, m_rectangle.top),
								sf::Vector2f(m_rectangle.left + m_rectangle.width, m_rectangle.top)));
			m_lineSegs[Right] = std::unique_ptr<LineSegment>(
				new LineSegment(sf::Vector2f(m_rectangle.left + m_rectangle.width, m_rectangle.top),
								sf::Vector2f(m_rectangle.left + m_rectangle.width, m_rectangle.top + m_rectangle.height)));
			m_lineSegs[Bottom] = std::unique_ptr<LineSegment>(
				new LineSegment(sf::Vector2f(m_rectangle.left + m_rectangle.width, m_rectangle.top + m_rectangle.height),
								sf::Vector2f(m_rectangle.left, m_rectangle.top + m_rectangle.height)));
			m_lineSegs[Left] = std::unique_ptr<LineSegment>(
				new LineSegment(sf::Vector2f(m_rectangle.left, m_rectangle.top + m_rectangle.height),
								sf::Vector2f(m_rectangle.left, m_rectangle.top)));

			m_position.x = m_rectangle.left + m_rectangle.width / 2.f;
			m_position.y = m_rectangle.top + m_rectangle.height / 2.f;
		}
		void Update(float dt, DummyCar& car)
		{
			for(auto&& l : m_lineSegs)
				l->setColour(sf::Color::White);
			
			std::array<sf::Vector2f, 4>& collisionPoints = car.GetCollisionPoints();
			for(auto& p : collisionPoints)
			{
				if(m_rectangle.contains(p))
				{
					m_lineSegs[m_getSide(p)]->setColour(sf::Color::Red);
					return;
				}
			}
		}

		sf::Vector2f GetCentre() const
		{
			return m_position;
		}

	private:
		sf::FloatRect m_rectangle;
		sf::Vector2f m_position;
		std::array< std::unique_ptr<LineSegment>, 4> m_lineSegs;

		void draw(sf::RenderTarget& rt, sf::RenderStates states) const
		{
			for(auto&& l : m_lineSegs)
				rt.draw(*l);
		}

		Side m_getSide(const sf::Vector2f& carPos)
		{
			sf::Vector2f boxCorner(m_rectangle.width / 2.f, m_rectangle.height / 2.f);
			sf::Vector2f impactPoint = carPos - m_position;


			float boxAngle = std::atan2(boxCorner.x, boxCorner.y);
			float pointAngle = std::atan2(impactPoint.x, impactPoint.y);

			std::cout << pointAngle << std::endl;

			if(pointAngle < 0)
			{
				//top half
				if(pointAngle > -boxAngle)
				{
					return Bottom;
				}
				else if(pointAngle > boxAngle - 3.1415f)
				{
					return Left;
				}
				else
				{
					return Top;
				}
			}
			else
			{
				//bottom half
				if(pointAngle < boxAngle)
				{
					return Bottom;
				}
				else if(pointAngle < 3.1415f - boxAngle)
				{
					return Right;
				}
				else
				{
					return Top;
				}
			}
		}
	};
}

#endif