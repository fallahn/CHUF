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

///***defines a struct which is used to share data between different screen classes***///
#ifndef SHARED_DATA
#define SHARED_DATA

#include <SFML/Graphics/View.hpp>
#include <Game/ResourceManager.h>
#include <Game/AudioManager.h>
#include <Game/ShaderManager.h>
#include <Game/Console.h>

namespace Game
{
	struct SharedData
	{
		TextureResource Textures;
		FontResource Fonts;
		AudioManager AudioManager;
		ShaderResource Shaders;
		Console Console;

		struct GameData
		{
			//add game items here such as controller config
			//or player scores
		} Game;

		struct ScreenData
		{
			ScreenData() 
				: WindowScale	(1.f)
			{};

			sf::Vector2f ViewSize;
			sf::View DefaultView;
			float WindowScale; //scale of view to window size for shaders which use frag coords
		} Screen;

		SharedData()
			: Fonts(),
			Console(Fonts.Get("assets/fonts/VeraMono.ttf"))
		{

		}
	};
};

#endif //SHARED_DATA