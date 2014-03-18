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

#include <SFML/Graphics/VertexArray.hpp>

#include <Game/PostEffect.h>

using namespace Game;

//ctor
PostEffect::PostEffect(ShaderResource& sr)
: m_shaderResource(sr){}

//dtor
PostEffect::~PostEffect(){}

//public
bool PostEffect::Supported()
{
	return sf::Shader::isAvailable();
}

//protected
void PostEffect::m_ApplyShader(const sf::Shader& shader, sf::RenderTarget& rt)
{
	sf::Vector2f outputSize = static_cast<sf::Vector2f>(rt.getSize());

	sf::VertexArray verts(sf::TrianglesStrip, 4u);
	verts[0] = sf::Vertex(sf::Vector2f(), sf::Vector2f(0.f, 1.f));
	verts[1] = sf::Vertex(sf::Vector2f(outputSize.x, 0.f), sf::Vector2f(1.f, 1.f));
	verts[2] = sf::Vertex(sf::Vector2f(0.f, outputSize.y), sf::Vector2f());
	verts[3] = sf::Vertex(outputSize, sf::Vector2f(1.f, 0.f));

	sf::RenderStates states;
	states.shader = &shader;
	states.blendMode = sf::BlendNone;

	rt.draw(verts, states);
}

ShaderResource& PostEffect::m_ShaderResource()
{
	return m_shaderResource;
}
