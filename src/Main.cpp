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

#include <Mesh/GL/glew.h>
#include <Game/StartScreen.h>
#include <Game/GameScreen.h>
#include <fstream>

#include <Game/TestScreen.h>

int main()
{	
	//init glew for 3d
	glewInit();	

	//load settings - TODO make exception safe
	VideoSettings settings("config.cfg");
	sf::ContextSettings contextSettings;
	contextSettings.antialiasingLevel = settings.getMultiSamples();
	contextSettings.depthBits = 24u;

	if(!settings.getResolution().isValid())
	{
		std::cout << "Invalid video mode, try deleting config file and restarting the application." << std::endl;
		return 1;
	}
	//TODO make this exception safe
	//we can't just swallow any exceptions here as it contains the entire program

	//create window from settings
	sf::RenderWindow renderWindow;
	renderWindow.create(settings.getResolution(), "SFML Window", settings.getFullcreen() ? sf::Style::Fullscreen : sf::Style::Close, contextSettings);
	renderWindow.setVerticalSyncEnabled(settings.getVsync());

	//create object to hold data shared between screens
	Game::SharedData sharedData;
	sharedData.AudioManager.SetGlobalEffectsVolume(settings.getSfxVolume());
	sharedData.AudioManager.SetGlobalMusicVolume(settings.getMusicVolume());

	//create game screens and enter main loop
	std::vector< std::unique_ptr<Game::BaseScreen> > screens;
	int screen = 0;

	screens.push_back(std::unique_ptr<Game::BaseScreen>(new Game::StartScreen(renderWindow, sharedData)));
	screens.push_back(std::unique_ptr<Game::BaseScreen>(new Game::GameScreen(renderWindow, sharedData)));

	while(screen >= 0)
	{
		screen = screens[screen]->Run();
	}

	return 0;
}