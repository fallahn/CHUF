///source for energy fence///
#include <Game/EnergyFence.h>

#include <SFML/Graphics/RenderTarget.hpp>

namespace
{
	//number of iterations during generation. actual count is this ^ 2
	const sf::Uint8 generations = 5u;
	const sf::Uint8 boltCount = 2u;
	const float maxOffset = 70.f;
	const float boltThickness = 3.f;
	const float spawnTime = 0.08f;
}

using namespace Game;

///---fence class---///
EnergyFence::EnergyFence(const sf::Vector2f& start, const sf::Vector2f& end, sf::Texture& texture)
	: m_start		(start),
	m_end			(end),
	m_texture		(texture)
{


}

void EnergyFence::Update(float dt)
{
	//update each bolt, remove dead ones
	for(auto&& b : m_bolts)
		b->Update(dt);

	m_bolts.remove_if([](const BoltPtr& b)
		{
			return b->CurrentState == Bolt::Dead;
		});

	//use timer to add new bolts
	if(m_spawnClock.getElapsedTime().asSeconds() > spawnTime
		&& m_bolts.size() <= boltCount)
	{
		sf::Vector2f texSize = static_cast<sf::Vector2f>(m_texture.getSize());
		m_bolts.push_back(Bolt::Create(m_start, m_end, texSize));
		m_spawnClock.restart();
	}
}

void EnergyFence::draw(sf::RenderTarget& rt, sf::RenderStates states) const
{
	//draw each bolt array with texture
	for(auto&& b : m_bolts)
		rt.draw(*b, &m_texture);
}

///---bolt class---///
EnergyFence::BoltPtr EnergyFence::Bolt::Create(const sf::Vector2f& start, const sf::Vector2f& end, const sf::Vector2f& texSize)
{
	//calc segment positions
	std::vector<Segment> segs;
	segs.push_back(Segment(start, end));
	float offset = maxOffset;

	//for this many generations
	for(auto i = 0u; i < generations; ++i)
	{
		std::vector<Segment> newSegs;
		for(auto& s : segs)
		{
			sf::Vector2f midPoint = (s.end - s.start) / 2.f;
			midPoint += s.start;

			//offset by some amount
			midPoint = Helpers::Vectors::GetPerpendicular(midPoint, Helpers::Random::Float(-offset, offset));
			newSegs.push_back(Segment(s.start, midPoint));
			newSegs.push_back(Segment(midPoint, s.end));
		}
		segs = newSegs;


		//halve the offset each generation
		offset /= 2.f;
	}

	//create a bolt and populate vertex array based on segment data
	BoltPtr b(new Bolt);
	for(auto& s : segs)
	{
		float halfWidth = boltThickness / 2.f;		
		sf::Vector2f segDir = s.Direction();
		sf::Vector2f unitDir = Helpers::Vectors::Normalize(segDir);
		sf::Vector2f perp = Helpers::Vectors::GetPerpendicular(unitDir, halfWidth);

		//create quad
		b->m_vertices.append(sf::Vertex(sf::Vector2f(s.start + perp)));
		b->m_vertices.append(sf::Vertex(sf::Vector2f(s.end + perp), sf::Vector2f(texSize.x, 0.f)));
		b->m_vertices.append(sf::Vertex(sf::Vector2f(s.end - perp), texSize));
		b->m_vertices.append(sf::Vertex(sf::Vector2f(s.start - perp), sf::Vector2f(0.f, texSize.y)));
	}

	return std::move(b);
}


void EnergyFence::Bolt::Update(float dt)
{
	m_transparency -= 750.f * dt; //TODO magic number
	if(m_transparency < 0.f) CurrentState = Dead;//m_transparency = 255.f;

	//set vert colour
	for(auto i = 0u; i < m_vertices.getVertexCount(); ++i)
		m_vertices[i].color.a = static_cast<sf::Uint8>(m_transparency);
}

void EnergyFence::Bolt::draw(sf::RenderTarget& rt, sf::RenderStates states) const
{
	rt.draw(m_vertices, states);
}