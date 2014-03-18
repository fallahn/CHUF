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

#include <Game/BloomEffect.h>

#include <cassert>

using namespace Game;

//ctor
BloomEffect::BloomEffect(ShaderResource& sr)
: PostEffect		(sr),
m_bloomSaturation	(1.4f),
m_baseSaturation	(1.1f),
m_bloomIntensity	(1.4f),
m_baseIntensity		(1.2f),
m_threshold			(0.5f)
{
	//doing this here makes sure the shaders are precached
	m_ShaderResource().Get(SfmlShader::BloomExtract);
	m_ShaderResource().Get(SfmlShader::BloomBlend);
	m_ShaderResource().Get(SfmlShader::Downsample);
	m_ShaderResource().Get(SfmlShader::GaussianBlur);
}

//public
void BloomEffect::Apply(const sf::RenderTexture& input, sf::RenderTarget& output)
{
	m_PrepareTexture(input.getSize());
	m_Extract(input, m_extractTexture);

	m_DownSample(m_extractTexture, m_firstPassTextures[0]);
	m_BlurMultipass(m_firstPassTextures);

	m_DownSample(m_firstPassTextures[0], m_secondPassTextures[0]);
	m_BlurMultipass(m_secondPassTextures);

	m_Blend(m_firstPassTextures[0], m_secondPassTextures[0], m_firstPassTextures[1]);
	m_firstPassTextures[1].display();
	m_Blend(input, m_firstPassTextures[1], output);
	//output.draw(sf::Sprite(m_extractTexture.getTexture()));
}

void BloomEffect::SetBloomSaturation(float value)
{
	assert(value > 0.f);
	m_bloomSaturation = value;
}

void BloomEffect::SetBaseSaturation(float value)
{
	assert(value > 0.f);
	m_baseSaturation = value;
}

void BloomEffect::SetBloomIntesinty(float value)
{
	assert(value > 0.f);
	m_bloomIntensity = value;
}

void BloomEffect::SetBaseIntensity(float value)
{
	assert(value > 0.f);
	m_baseIntensity = value;
}

void BloomEffect::SetThreshold(float value)
{
	assert(value >= 0.f && value <= 1.f);
	m_threshold = value;
}

//private
void BloomEffect::m_PrepareTexture(sf::Vector2u size)
{
	if (m_extractTexture.getSize() != size)
	{
		m_extractTexture.create(size.x, size.y);
		m_extractTexture.setSmooth(true);

		m_firstPassTextures[0].create(size.x / 2u, size.y / 2u);
		m_firstPassTextures[0].setSmooth(true);
		m_firstPassTextures[1].create(size.x / 2u, size.y / 2u);
		m_firstPassTextures[1].setSmooth(true);

		m_secondPassTextures[0].create(size.x / 4u, size.y / 4u);
		m_secondPassTextures[0].setSmooth(true);
		m_secondPassTextures[1].create(size.x / 4u, size.y / 4u);
		m_secondPassTextures[1].setSmooth(true);
	}
}

void BloomEffect::m_Extract(const sf::RenderTexture& input, sf::RenderTexture& output)
{
	sf::Shader& extract = m_ShaderResource().Get(SfmlShader::BloomExtract);
	extract.setParameter("texture", input.getTexture());
	extract.setParameter("threshold", m_threshold);
	m_ApplyShader(extract, output);
	output.display();
}

void BloomEffect::m_BlurMultipass(RenderTextures& textures)
{
	auto texSize = textures[0].getSize();
	for (auto i = 0u; i < 2u; ++i)
	{
		m_Blur(textures[0], textures[1], sf::Vector2f(0.f, 1.f / texSize.y));
		m_Blur(textures[1], textures[0], sf::Vector2f(1.f / texSize.x, 0.f));
	}
}

void BloomEffect::m_Blur(const sf::RenderTexture& input, sf::RenderTexture& output, sf::Vector2f offset)
{
	sf::Shader& blur = m_ShaderResource().Get(SfmlShader::GaussianBlur);
	blur.setParameter("source", input.getTexture());
	blur.setParameter("offsetFactor", offset);
	m_ApplyShader(blur, output);

	output.display();
}

void BloomEffect::m_DownSample(const sf::RenderTexture& input, sf::RenderTexture& output)
{
	sf::Shader& downsample = m_ShaderResource().Get(SfmlShader::Downsample);
	downsample.setParameter("source", input.getTexture());
	downsample.setParameter("sourceSize", sf::Vector2f(input.getSize()));
	m_ApplyShader(downsample, output);

	output.display();
}

void BloomEffect::m_Blend(const sf::RenderTexture& base, const sf::RenderTexture& bloom, sf::RenderTarget& output)
{
	sf::View currentView = output.getView();
	output.setView(output.getDefaultView());
	
	sf::Shader& blend = m_ShaderResource().Get(SfmlShader::BloomBlend);
	blend.setParameter("baseTexture", base.getTexture());
	blend.setParameter("glowTexture", bloom.getTexture());
	blend.setParameter("bloomSaturation", m_bloomSaturation);
	blend.setParameter("baseSaturation", m_baseSaturation);
	blend.setParameter("bloomIntensity", m_bloomIntensity);
	blend.setParameter("baseIntensity", m_baseIntensity);

	m_ApplyShader(blend, output);
	output.setView(currentView);
}