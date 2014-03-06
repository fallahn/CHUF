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

#include <Game/ShaderManager.h>

using namespace Game;

sf::Shader& ShaderResource::Get(SfmlShader shaderType)
{
	auto s = m_sfmlShaders.find(shaderType);
	if (s != m_sfmlShaders.end())
		return *s->second;

	sfPtr shaderPtr = std::make_unique<sf::Shader>();
	switch (shaderType)
	{
	case SfmlShader::Convolution:
		shaderPtr->loadFromMemory(convolution, sf::Shader::Fragment);
		break;
	case SfmlShader::Ghost:
		shaderPtr->loadFromMemory(ghost, sf::Shader::Fragment);
		break;
	case SfmlShader::NormalMap:
		shaderPtr->loadFromMemory(NormalMap, sf::Shader::Fragment);
		break;
	case SfmlShader::ScanLines:
		shaderPtr->loadFromMemory(scanlines, sf::Shader::Fragment);
		break;
	case SfmlShader::ShadowBlend:
		shaderPtr->loadFromMemory(shadowBlend, sf::Shader::Fragment);
		break;
	default: break;
	}
	m_sfmlShaders[shaderType] = std::move(shaderPtr);
	return *m_sfmlShaders[shaderType];
}

ml::ShaderProgram& ShaderResource::Get(ml::MeshShader shaderType)
{
	auto s = m_meshShaders.find(shaderType);
	if (s != m_meshShaders.end())
		return *s->second;

	mlPtr shaderPtr = std::make_unique<ml::ShaderProgram>();
	switch (shaderType)
	{
	case ml::MeshShader::Normal:
		shaderPtr->loadFromMemory(normalVert, normalFrag);
		break;
	case ml::MeshShader::Phong:
		shaderPtr->loadFromMemory(phongVert, phongFrag);
		break;
	case ml::MeshShader::Depth:
		shaderPtr->loadFromMemory(depthVert, depthFrag);
		break;
	case ml::MeshShader::Shadow:
		shaderPtr->loadFromMemory(shadowVert, shadowFrag);
		break;
	default: break;
	}
	m_meshShaders[shaderType] = std::move(shaderPtr);
	return *m_meshShaders[shaderType];
}