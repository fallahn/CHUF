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

/*Screen classes. Each 'screen' represents a different operating mode of the game, called upon
by the main function. BaseScreen defines the basis for each screen class which provides game loop
timing functions, and screen rendering functions, called when each screen is 'run'. All derived
screen classes must provide their own m_Update() and m_Render() functions as BaseScreen is an ABC.
Run() can also be overridden by derived classes to provide for custom construction and destruction
of objects when screens are switched, although it is recommended to always call BaseScreen::Run()
in each overridden instance. Better explaination of how this works can be found here:
https://github.com/LaurentGomila/SFML/wiki/Tutorial%3A-Manage-different-Screens */

#ifndef _SCREENS_H_
#define _SCREENS_H_

#include <Game/SharedData.h>
#include <Game/SaveSettings.h>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Window/Event.hpp>

#include <atomic>

namespace Game
{	
	enum ScreenId
	{
		Quit = -1,
		MainMenu = 0,
		Game = 1
	};
	
	class BaseScreen : private sf::NonCopyable
	{
	public:
		BaseScreen(sf::RenderWindow& renderWindow, SharedData& sharedData);
		//entry point for all screen classes, called from the main function
		virtual ~BaseScreen(){};
		virtual ScreenId Run();

	protected:

		sf::RenderWindow& m_RenderWindow();
		sf::Clock& m_FpsClock();
		sf::Clock& m_DtClock();
		SharedData& m_SharedData();
		float m_GetTimeScale() const;
		void m_SetTimeScale(float scale);
		sf::Text& m_Text();
		sf::Font& m_Font();
		sf::Event& m_Event();
		bool m_Running() const;
		void m_SetRunning(bool b);
		bool m_HasFocus() const;
		void m_SetFocused(bool b);
		ScreenId m_ReturnValue() const;
		void m_SetReturnValue(ScreenId value);


		//override this in derived classes with calls to object logic updates.
		virtual void m_Update(float dt) = 0;
		//override with in any derived classes with object draw calls. Called as often as the system supports after
		//performing any logic updates
		virtual void m_Render() = 0;
		//utility function for drawing the window title. To use it call it from the class's m_Render function.
		void m_DrawFps();
		//utilty function for saving a time stamped screenshot to the same directory as the game executable
		void m_SaveScreenShot();
		//override this in any screen which needs custom event handling
		virtual void m_HandleCustomEvent(){};
		//overide this for a screen's real time input
		virtual void m_HandleRealtimeEvent(){};
		//reads current config settings and updates options menu
		void m_RefreshOptions();
		//called when screen starts running
		virtual void m_OnStart() = 0;
		//called when screen stops running
		virtual void m_OnFinish() = 0;
		//called when window / context recreated eg when chaning resolution
		virtual void m_OnContextCreate(const sf::VideoMode& v) = 0;
		//implement and call this on startup to load any screen specific console commands
		virtual void m_LoadConsoleCommands() = 0;
		//MUST be called during when exiting a screen if console commands have been loaded
		virtual void m_UnloadConsoleCommands();
		Console::CommandList m_commandList;

	private:
		//loading window//
		sf::Texture m_loadingTexture;
		sf::Sprite m_loadingSprite;
		void m_LaunchLoadingWindow();
		void m_UpdateLoadingWindow();
		void m_StopLoadingWindow();
		std::atomic<bool> m_threadRunning;
		sf::Thread m_loadingThread;


		//-----------------------------//
		sf::RenderWindow& m_renderWindow;
		sf::Clock m_fpsClock, m_dtClock;
		//global data struct
		SharedData& m_sharedData;

		float m_timeScale; //used to speed up and slow down rate of game
		sf::Text m_text;
		sf::Font m_font;
		sf::Event m_event;
		bool m_running, m_hasFocus;
		ScreenId m_return;

	};
};


#endif