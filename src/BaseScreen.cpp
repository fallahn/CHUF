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

#include <Game/BaseScreen.h>
#include <algorithm>

using namespace Game;

BaseScreen::BaseScreen(sf::RenderWindow& renderWindow, SharedData& sharedData)
	: m_timeScale			(1.f),
	m_renderWindow			(renderWindow),
	m_sharedData			(sharedData),
	m_hasFocus				(true),
	m_threadRunning			(false),
	m_loadingThread			(&BaseScreen::m_UpdateLoadingWindow, this)
{
	m_font = m_sharedData.fonts.Get("");

	//TODO load an image into thread texture
}

//***public functions***//
ScreenId BaseScreen::Run()
{
	m_renderWindow.setView(m_sharedData.Screen.DefaultView);

	//show loading screen while any screens load assets here
	m_LaunchLoadingWindow();
	m_OnStart();
	m_StopLoadingWindow();

	m_running = true;
	const float fixedStep = 1.f / 30.f;

	while(m_running)
	{
		while(m_renderWindow.pollEvent(m_event))
		{
			//check if window has focus
			switch(m_event.type)
			{
			case sf::Event::GainedFocus:
				m_hasFocus = true;
				std::cout << "gained focus" << std::endl;
				break;
			case sf::Event::LostFocus:
				m_hasFocus = false;
				std::cout << "lost focus" << std::endl;
				break;
			case sf::Event::Closed:
				return ScreenId::Quit;
			case sf::Event::KeyPressed:
				if(!m_hasFocus) break;
				switch(m_event.key.code)
				{
				case sf::Keyboard::F5:
					m_SaveScreenShot();
					break;
				default: break;
				}
			default: break;
			}

			//execute any custom events in derived screens
			m_HandleCustomEvent();
		}

		//perform logic updates
		float frameTime = m_dtClock.restart().asSeconds() * m_timeScale;
		sf::Uint8 maxSteps = 4; //get out clause to prevent "spiral of death"
		while(frameTime > 0.f && maxSteps > 0)
		{
			m_HandleRealtimeEvent();

			float dt = std::min(frameTime, fixedStep);
			m_Update(dt);
			frameTime -= dt;
			maxSteps--;
		}

		//draw everything
		m_Render();
	}
	m_OnFinish();

	return m_return;
}

//***protected functions***//
sf::RenderWindow& BaseScreen::m_RenderWindow()
{
	return m_renderWindow;
}

sf::Clock& BaseScreen::m_FpsClock()
{
	return m_fpsClock;
}

sf::Clock& BaseScreen::m_DtClock()
{
	return m_dtClock;
}

SharedData& BaseScreen::m_SharedData()
{
	return m_sharedData;
}

float BaseScreen::m_GetTimeScale() const
{
	return m_timeScale;
}

void BaseScreen::m_SetTimeScale(float scale)
{
	assert(scale > 0.f);
	m_timeScale = scale;
}

sf::Text& BaseScreen::m_Text()
{
	return m_text;
}

sf::Font& BaseScreen::m_Font()
{
	return m_font;
}

sf::Event& BaseScreen::m_Event()
{
	return m_event;
}

bool BaseScreen::m_Running() const
{
	return m_running;
}

void BaseScreen::m_SetRunning(bool b)
{
	m_running = b;
}

bool BaseScreen::m_HasFocus() const
{
	return m_hasFocus;
}

void BaseScreen::m_SetFocused(bool b)
{
	m_hasFocus = b;
}

ScreenId BaseScreen::m_ReturnValue() const
{
	return m_return;
}

void BaseScreen::m_SetReturnValue(ScreenId value)
{
	m_return = value;
}

void BaseScreen::m_DrawFps()
{
	float frameRate = 1.f / m_fpsClock.getElapsedTime().asSeconds();
	m_fpsClock.restart();

	m_renderWindow.setTitle("FPS: " + std::to_string(frameRate));
}

void BaseScreen::m_SaveScreenShot()
{
		std::time_t time = std::time(NULL);
		struct tm* timeInfo;

		timeInfo = std::localtime(&time);

		char buffer[40];
		std::string fileName;

		strftime(buffer, 40, "screenshot%d_%m_%y_%H_%M_%S.png", timeInfo);

		fileName.assign(buffer);

		sf::Image screenCap = m_renderWindow.capture();
		if(screenCap.saveToFile(fileName))
			std::cout << "Saved " << fileName << " to disk." << std::endl;

}

void BaseScreen::m_UnloadConsoleCommands()
{
	for (auto& s : m_commandList)
		m_sharedData.console.RemoveItem(s);

	m_commandList.clear();
}

///------threading------///
void BaseScreen::m_LaunchLoadingWindow()
{
	std::cerr << "Launching new thread" << std::endl;
	m_threadRunning = true;
	m_renderWindow.setActive(false);
	m_loadingThread.launch();
}

void BaseScreen::m_UpdateLoadingWindow()
{
	while(m_threadRunning)
	{
		m_loadingSprite.rotate(1440.f * m_dtClock.restart().asSeconds());
		m_loadingSprite.setPosition(m_renderWindow.getView().getCenter());
		m_renderWindow.clear();
		m_renderWindow.draw(m_loadingSprite);
		m_renderWindow.display();
	}
	m_renderWindow.setActive(false);
}

void BaseScreen::m_StopLoadingWindow()
{
	m_threadRunning = false;
	m_loadingThread.wait();
	std::cerr << "Successfully quit thread" << std::endl;
}
