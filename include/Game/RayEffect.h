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

//godray post process effect//

#ifndef GODRAY_H_
#define GODRAY_H_

#include <Game/PostEffect.h>

namespace Game
{
	class RayEffect : public PostEffect
	{
	public:
		RayEffect(ShaderResource& sr);

		void Apply(const sf::RenderTexture& input, sf::RenderTarget& output) override;
		//normalised coordinates
		void SetSourcePosition(sf::Vector2f screenCoords);
		void SetThreshold(float value);
		void SetIntensity(float value);
		void SetDecay(float value);
		void SetDensity(float value);
		void SetWeight(float value);

	private:
		sf::RenderTexture m_extractTexture;
		RenderTextures m_blurBuffers;
		sf::Vector2f m_rayCentre;
		float m_threshold;

		float m_intensity;
		float m_decay;
		float m_density;
		float m_weight;

		void m_PrepareTextures(sf::Vector2u size);
		void m_Extract(const sf::RenderTexture& input, sf::RenderTexture& output);
		void m_DownSample(const sf::RenderTexture& input, sf::RenderTexture& output);
		void m_RayBlur(RenderTextures& buffers);
		void m_DoBlur(const sf::RenderTexture& input, sf::RenderTexture& output);
		void m_Blend(const sf::RenderTexture& base, const sf::RenderTexture& additive, sf::RenderTarget& output);
	};
}

#endif //GODRAY_H_