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

#include <Game/GameScreen.h>

using namespace Game;

GameScreen::GameScreen(sf::RenderWindow& renderWindow, SharedData& sharedData)
	: BaseScreen	(renderWindow, sharedData),
	m_rootNode		(sharedData.audioManager)
{

}

GameScreen::~GameScreen()
{

}

void GameScreen::m_Update(float dt)
{
	//update physics world

	//update component factories

	//update scene graph
	while (!m_commandQueue.empty())
		m_rootNode.OnCommand(m_commandQueue.pop(), dt);
	m_rootNode.Update(dt);

	//update mesh scene graph

}

void GameScreen::m_Render()
{
	m_RenderWindow().clear(sf::Color(100u, 149u, 237u));
	//draw game items here
	m_RenderWindow().draw(m_SharedData().console);
	m_DrawFps();
	m_RenderWindow().display();

}

void GameScreen::m_HandleCustomEvent()
{
	if (m_SharedData().console.HandleEvent(m_Event())) return;

	if(m_Event().type == sf::Event::KeyPressed
		&& m_HasFocus())
	{
		switch(m_Event().key.code)
		{

		default: break;
		}
	}
}

void GameScreen::m_HandleRealtimeEvent()
{
	if (m_SharedData().console.Visible()) return;
	m_SharedData().console.HandleRealtimeEvent();
}

void GameScreen::m_OnStart()
{
	m_LoadConsoleCommands();
}

void GameScreen::m_OnFinish()
{
	m_UnloadConsoleCommands();
}

void GameScreen::m_LoadConsoleCommands()
{
	Console& console = m_SharedData().console;
	Console::CommandData cd;
	std::string commandName = "quit";
	cd.action = [this](Console::CommandList l)->std::string
	{
		m_SetReturnValue(ScreenId::Quit);
		m_SetRunning(false);
		return "Quitting...";
	};
	cd.help = "quit the game";
	cd.commandFlags = Console::None;
	console.AddItem(commandName, cd);
	m_commandList.push_back(commandName);

	commandName = "main_menu";
	cd.action = [this](Console::CommandList l)->std::string
	{
		m_SetReturnValue(ScreenId::MainMenu);
		m_SetRunning(false);
		return "";
	};
	cd.help = "return to main menu";
	cd.commandFlags = Console::None;
	console.AddItem(commandName, cd);
	m_commandList.push_back(commandName);


	m_SharedData().console.Exec("bind escape main_menu");
	m_SharedData().console.Exec("bind backspace quit");
}
