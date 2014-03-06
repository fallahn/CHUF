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

///there are two types of shader program available - the default sfml shader and a 
///specialisation for the 3D MeshLib. Shader resources are seperated out from other resources as 
///they are loaded from memory streams rather than disk.

#ifndef SHADER_MANAGER_H_
#define SHADER_MANAGER_H_

#include <SFML/System/NonCopyable.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <Mesh/ShaderProgram.h>

#include <memory>
#include <map>

//sfml shaders
#include <Game/Shaders/Convolution.h>
#include <Game/Shaders/Normal.h>
#include <Game/Shaders/ScanLines.h>

//mesh shaders
#include <Mesh/Shaders/Normal.h>
#include <Mesh/Shaders/Phong.h>
#include <Mesh/Shaders/DepthMap.h>
#include <Mesh/Shaders/Shadow.h>

namespace ml
{
	enum class MeshShader
	{
		Normal,
		Phong,
		Depth,
		Shadow
	};
}

namespace Game
{
	typedef std::unique_ptr<sf::Shader> sfPtr;
	typedef std::unique_ptr<ml::ShaderProgram> mlPtr;
	
	enum class SfmlShader
	{
	//include the header for the relevant shader file then add an entry here
		Convolution,
		NormalMap,
		ScanLines,
		Ghost,
		ShadowBlend
	};

	class ShaderResource : private sf::NonCopyable
	{
	public:
		sf::Shader& Get(SfmlShader shaderType);
		ml::ShaderProgram& Get(ml::MeshShader shaderType);
	private:
		std::map<SfmlShader, sfPtr> m_sfmlShaders;
		std::map<ml::MeshShader, mlPtr> m_meshShaders;
	};
}

#endif //SHADER_MANAGER_H_