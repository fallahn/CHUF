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

//abstract base class for full scene shader based post effects

#ifndef POST_EFFECTS_H_
#define POST_EFFECTS_H_

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/System/NonCopyable.hpp>

#include <Game/ShaderManager.h>

#include <array>

namespace Game
{
	class PostEffect : private sf::NonCopyable
	{
	public:
		explicit PostEffect(ShaderResource& sr);
		virtual ~PostEffect();
		virtual void Apply(const sf::RenderTexture& input, sf::RenderTarget& target) = 0;
		static bool Supported();

	protected:
		typedef std::array<sf::RenderTexture, 2u> RenderTextures;
		static void m_ApplyShader(const sf::Shader& shader, sf::RenderTarget& output);
		ShaderResource& m_ShaderResource();
	private:
		ShaderResource& m_shaderResource;
	};
}

#endif //POST_EFFECTS_H_