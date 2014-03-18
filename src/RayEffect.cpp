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

#include <Game/RayEffect.h>

#include <SFML/Graphics/Sprite.hpp>

#include <iostream>
#include <cassert>

using namespace Game;

//ctor
RayEffect::RayEffect(ShaderResource& sr)
: PostEffect(sr),
m_rayCentre	(0.5f, 0.6f),
m_threshold	(0.55f),
m_intensity	(0.5f),
m_decay		(0.78f),
m_density	(0.95f),
m_weight	(0.5f)
{
	//load these here to precache shaders
	sr.Get(SfmlShader::GodRay);
	sr.Get(SfmlShader::BloomExtract);
	sr.Get(SfmlShader::Downsample);
	sr.Get(SfmlShader::BlendAdd);
}

//dtor

//public
void RayEffect::Apply(const sf::RenderTexture& input, sf::RenderTarget& output)
{
	m_PrepareTextures(input.getSize());
	m_Extract(input, m_extractTexture);
	m_DownSample(m_extractTexture, m_blurBuffers[0]);
	m_RayBlur(m_blurBuffers);
	m_Blend(input, m_blurBuffers[0], output); //make sure we always use the correct buffer with result
	//output.draw(sf::Sprite(m_extractTexture.getTexture()));
}

void RayEffect::SetSourcePosition(sf::Vector2f position)
{
	m_rayCentre = position;
	const float tolerance = 3.f;
	if (position.x > tolerance || position.x < -tolerance)
	{
		std::cerr << "WARNING: Godray X position is very large. Did you mean to use normalised coordinates?" << std::endl;
	}
	else if (position.y > tolerance || position.y < -tolerance)
	{
		std::cerr << "WARNING: Godray Y position is very large. Did you mean to use normalised coordinates?" << std::endl;
	}
}

void RayEffect::SetThreshold(float value)
{
	assert(value >= 0.f && value <= 1.f);
	m_threshold = value;
}

void RayEffect::SetIntensity(float value)
{
	assert(value >= 0.f && value <= 1.f);
	m_intensity = value;
}

void RayEffect::SetDecay(float value)
{
	assert(value >= 0.f && value <= 1.f);
	m_decay = value;
}

void RayEffect::SetDensity(float value)
{
	assert(value >= 0.f && value <= 1.f);
	m_density = value;
}

void RayEffect::SetWeight(float value)
{
	assert(value >= 0.f && value <= 1.f);
	m_weight = value;
}

//private
void RayEffect::m_PrepareTextures(sf::Vector2u size)
{
	if (m_extractTexture.getSize() != size)
	{
		m_extractTexture.create(size.x, size.y);
		m_extractTexture.setSmooth(true);

		const sf::Uint8 reduction = 4u;
		m_blurBuffers[0].create(size.x / 2u, size.y / 2u);
		m_blurBuffers[0].setSmooth(true);
		m_blurBuffers[1].create(size.x / reduction, size.y / reduction);
		m_blurBuffers[1].setSmooth(true);
	}
}

void RayEffect::m_Extract(const sf::RenderTexture& input, sf::RenderTexture& output)
{
	sf::Shader& extract = m_ShaderResource().Get(SfmlShader::BloomExtract);
	extract.setParameter("texture", input.getTexture());
	extract.setParameter("threshold", m_threshold);
	m_ApplyShader(extract, output);
	output.display();
}

void RayEffect::m_DownSample(const sf::RenderTexture& input, sf::RenderTexture& output)
{
	sf::Shader& downsample = m_ShaderResource().Get(SfmlShader::Downsample);
	downsample.setParameter("source", input.getTexture());
	downsample.setParameter("sourceSize", sf::Vector2f(input.getSize()));
	m_ApplyShader(downsample, output);

	output.display();
}

void RayEffect::m_RayBlur(RenderTextures& buffers)
{
	//make sure we end up back on the first buffer
	m_DoBlur(buffers[0], buffers[1]);
	m_DoBlur(buffers[1], buffers[0]);
}

void RayEffect::m_DoBlur(const sf::RenderTexture& input, sf::RenderTexture& output)
{
	sf::Shader& blur = m_ShaderResource().Get(SfmlShader::GodRay);
	blur.setParameter("lightPosition", m_rayCentre);
	blur.setParameter("texture", input.getTexture());
	blur.setParameter("intensity", m_intensity);
	blur.setParameter("decay", m_decay);
	blur.setParameter("density", m_density);
	blur.setParameter("weight", m_weight);
	m_ApplyShader(blur, output);
	output.display();
}

void RayEffect::m_Blend(const sf::RenderTexture& base, const sf::RenderTexture& additive, sf::RenderTarget& output)
{
	sf::View defaultView = output.getView();
	output.setView(output.getDefaultView());
	
	sf::Shader& blend = m_ShaderResource().Get(SfmlShader::BlendAdd);
	blend.setParameter("base", base.getTexture());
	blend.setParameter("additive", additive.getTexture());

	m_ApplyShader(blend, output);
	output.setView(defaultView);
}