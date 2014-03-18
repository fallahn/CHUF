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

//bloom post process effect//

#ifndef BLOOM_POST_H_
#define BLOOM_POST_H_

#include <Game/PostEffect.h>

namespace Game
{
	class BloomEffect : public PostEffect
	{
	public:
		BloomEffect(ShaderResource& sr);

		void Apply(const sf::RenderTexture& input, sf::RenderTarget& output) override;

		void SetBloomSaturation(float value);
		void SetBaseSaturation(float value);
		void SetBloomIntesinty(float value);
		void SetBaseIntensity(float value);
		//normalised value
		void SetThreshold(float value);

	private:

		sf::RenderTexture m_extractTexture;
		RenderTextures m_firstPassTextures;
		RenderTextures m_secondPassTextures;

		float m_bloomSaturation;
		float m_baseSaturation;
		float m_bloomIntensity;
		float m_baseIntensity;
		float m_threshold;

		void m_PrepareTexture(sf::Vector2u size);
		void m_Extract(const sf::RenderTexture& input, sf::RenderTexture& output);
		void m_BlurMultipass(RenderTextures& textures);
		void m_Blur(const sf::RenderTexture& input, sf::RenderTexture& output, sf::Vector2f offset);
		void m_DownSample(const sf::RenderTexture& input, sf::RenderTexture& output);
		void m_Blend(const sf::RenderTexture& base, const sf::RenderTexture& bloom, sf::RenderTarget& output);
	};
}

#endif