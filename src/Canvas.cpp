///source for canvas class///
#include <Game/Canvas.h>

#include <SFML/Window/Mouse.hpp>

#include <iostream>
#include <Helpers.h>
#include <assert.h>

using namespace Game;

const char* shader =
	"#version 120\n"
	"uniform sampler2D tex;"

	"void main()"
	"{"
		"vec3 colour = texture2D(tex, gl_TexCoord[0].xy).rgb;"
		"float avg = (colour.r + colour.g + colour.b) / 3.0;"
		"if(avg < 1.0) colour = vec3(0.1, 0.0, 0.3);"
		"gl_FragColor = vec4(clamp(colour + 0.5, 0.0, 1.0), 0.5);"
	"}";

Canvas::Canvas()
	: m_hideCursor	(false)
{
	m_renderTexture.create(480u, 640u);
	m_renderTexture.clear(sf::Color::White);

	m_sprite.setTexture(m_renderTexture.getTexture());

	m_brush.setFillColor(sf::Color::Black);
	m_brush.setRadius(6.f);
	m_brush.setOrigin(6.f, 6.f);

	m_shader.loadFromMemory(shader, sf::Shader::Fragment);

}

///public///
void Canvas::Update(float dt, const sf::Vector2f& mousePos)
{
	// convert to local coords
	sf::Vector2f currentPos = mousePos - m_sprite.getPosition();

	if(sf::Mouse::isButtonPressed(sf::Mouse::Left))
	{
		//draw shit
		float stepSize = 3.f; //brush size / 4
		sf::Vector2f path = currentPos - m_lastPoint;
		float stepCount = Helpers::Vectors::GetLength(path) / stepSize;
		sf::Vector2f step = Helpers::Vectors::Normalize(path) * stepSize;

		for(auto i = 0.f; i < stepCount; i++)
		{
			m_brush.move(step);
			m_renderTexture.draw(m_brush);
		}
		m_brush.setPosition(currentPos);
		m_renderTexture.draw(m_brush);
		m_renderTexture.display();
		m_lastPoint = currentPos;
	}
	else
	{
		//move brush with mouse
		m_brush.setPosition(mousePos);
	}

	m_hideCursor = m_sprite.getGlobalBounds().contains(mousePos);
}

void Canvas::PenDown(const sf::Vector2f& mousePos)
{
	m_lastPoint = mousePos - m_sprite.getPosition();
}

float Canvas::Compare(const sf::Image& image)
{
	///**** this function could be slow so we might want to run some progress in a thread***///

	sf::Image canvasImage = m_renderTexture.getTexture().copyToImage();
	assert(canvasImage.getSize() == image.getSize());

	const sf::Uint8* a = canvasImage.getPixelsPtr();
	const sf::Uint8* b = image.getPixelsPtr();

	std::vector<float> results;

	unsigned size = canvasImage.getSize().x * canvasImage.getSize().y * 4u;
	float pc = 100.f / 255.f;
	for(auto i = 0u; i < size; i += 4u)
	{
		float percent = 100.f - std::abs(static_cast<float>(a[i] - b[i]) * pc);
		percent += 100.f - std::abs(static_cast<float>(a[i + 1] - b[i + 1]) * pc);
		percent += 100.f - std::abs(static_cast<float>(a[i + 2] - b[i + 2]) * pc);
		results.push_back(percent / 3.f);
	}

	float total = 0.f;
	for(auto result : results)
		total += result;

	return total / static_cast<float>(results.size());
}

void Canvas::SetCarbon(const sf::Image& image)
{
	m_carbonTexture.loadFromImage(image);
	m_carbonSprite.setTexture(m_carbonTexture);
}

void Canvas::Clear()
{
	m_renderTexture.clear(sf::Color::White);
}

///private///
void Canvas::draw(sf::RenderTarget& rt, sf::RenderStates state) const
{
	rt.draw(m_sprite);
	rt.draw(m_carbonSprite, &m_shader);
	rt.draw(m_brush); //so we see brush following mouse
}
