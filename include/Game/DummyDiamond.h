///dummy diamond shape for collision testing///
#ifndef DUMMY_D
#define DUMMY_D_

#include <Game/LineSegment.h>
#include <Game/DummyCar.h>

#include <array>

namespace Game
{
	class DummyDiamond : public sf::Drawable
	{
	public:
		DummyDiamond()
		{
			m_polyPoints[0] = sf::Vector2f(500.f, 500.f);
			m_polyPoints[1] = sf::Vector2f(750.f, 750.f);
			m_polyPoints[2] = sf::Vector2f(500.f, 1000.f);
			m_polyPoints[3] = sf::Vector2f(250.f, 750.f);


			m_segments[0] = std::unique_ptr<LineSegment>(new LineSegment(m_polyPoints[3], m_polyPoints[0]));
			m_segments[1] = std::unique_ptr<LineSegment>(new LineSegment(m_polyPoints[0], m_polyPoints[1]));
			m_segments[2] = std::unique_ptr<LineSegment>(new LineSegment(m_polyPoints[1], m_polyPoints[2]));
			m_segments[3] = std::unique_ptr<LineSegment>(new LineSegment(m_polyPoints[2], m_polyPoints[3]));
		}

		void Update(float dt, DummyCar& car)
		{
			for(auto&& l : m_segments)
			{
				l->setColour(sf::Color::White);
			}

			std::array<sf::Vector2f, 4>& collisionPoints = car.GetCollisionPoints();
			for(auto i = 0u; i < collisionPoints.size(); i++)
			{
				if(m_Contains(collisionPoints[i]))
				{
					sf::Uint8 id = i;

					id += std::floor((car.GetRotation() -45.f) / 90.f);
					if(id > 3) id -= 4;

					m_segments[id]->setColour(sf::Color::Red);
					break;
				}
			}
		}

	private:
		std::array<sf::Vector2f, 4> m_polyPoints;
		std::array< std::unique_ptr<LineSegment>, 4 > m_segments;

		bool m_Contains(const sf::Vector2f& point)
		{
			unsigned int i, j;
			bool result = false;
			for (i = 0, j = m_polyPoints.size() - 1; i < m_polyPoints.size(); j = i++)
			{
				if (((m_polyPoints[i].y > point.y) != (m_polyPoints[j].y > point.y)) &&
				(point.x < (m_polyPoints[j].x - m_polyPoints[i].x) * (point.y - m_polyPoints[i].y)
					/ (m_polyPoints[j].y - m_polyPoints[i].y) + m_polyPoints[i].x))
						result = !result;
			}
			return result;
		}

		void draw(sf::RenderTarget& rt, sf::RenderStates state) const
		{
			for(auto&& l : m_segments)
			{
				rt.draw(*l);
			}
		}
	};
}
#endif