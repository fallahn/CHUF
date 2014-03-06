/////header for canvas class, allows drawing with a mouse/////
#ifndef CANVAS_H_
#define CANVAS_H_

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <SFML/Graphics/Sprite.hpp>

namespace Game 
{
	class Canvas : public sf::Drawable
	{
	public:
		Canvas();

		void Update(float dt, const sf::Vector2f& mousePos);
		void PenDown(const sf::Vector2f& mousePos);
		void PenUp();
		bool ShowCursor() const{return !m_hideCursor;};
		void Clear();
		//returns a percentage based on similarlity between canvas and given image
		float Compare(const sf::Image& image);
		//sets image to use as carbon / outline
		void SetCarbon(const sf::Image& image);

	private:
		sf::CircleShape m_brush;
		sf::Vector2f m_lastPoint;

		sf::RenderTexture m_renderTexture;
		sf::Sprite m_sprite;

		sf::Texture m_carbonTexture;
		sf::Sprite m_carbonSprite;
		sf::Shader m_shader;

		bool m_hideCursor;

		void draw(sf::RenderTarget& rt, sf::RenderStates states) const;
	};
}


#endif