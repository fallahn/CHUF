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

///header file for game screen class which runs main gameplay
#ifndef GAME_SCREEN_H_
#define GAME_SCREEN_H_

#include <Game/BaseScreen.h>
#include <Game/SceneNode.h>

#include <Mesh/MeshLib.h>
#include <tmx/MapLoader.h>
#include <Box2D/Box2D.h>


namespace Game
{
	class GameScreen final : public BaseScreen
	{
	public:
		GameScreen(sf::RenderWindow& renderWindow, SharedData& sharedData);
		~GameScreen();
	private:

		CommandQueue m_commandQueue;
		SceneNode m_rootNode;

		void m_Update(float dt) override;
		void m_Render() override;
		void m_HandleCustomEvent() override;
		void m_HandleRealtimeEvent() override;

		void m_OnStart() override;
		void m_OnFinish() override;
		void m_OnContextCreate(const sf::VideoMode& v) override{};

		void m_LoadConsoleCommands() override;
	};
}

#endif //GAME_SCREEN_H_