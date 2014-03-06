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

#ifndef CONSOLE_H_
#define CONSOLE_H_

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/System/NonCopyable.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>

#include <string>
#include <functional>
#include <vector>
#include <list>
#include <map>

namespace Game
{
	class Console final : public sf::Drawable, private sf::NonCopyable
	{
	public:
		enum CommandFlag
		{
			None   = 0,
			Cheat  = 1 << 0,
			Config = 1 << 1, //result of this command should be saved in .con file
			ControllerAnalogue = 1 << 2 //command cannot be mapped to controller axis without this
		};

		typedef std::vector<std::string> CommandList;
		typedef std::function<std::string(CommandList& commands)> Command;

		struct CommandData
		{
			CommandData() : commandFlags(0){}
			Command action;
			std::string help;
			sf::Uint32 commandFlags;
		};

		explicit Console(const sf::Font& font);

		void AddItem(const std::string& name, CommandData cd);
		void RemoveItem(const std::string& name);
		void Print(const std::string& text);

		bool HandleEvent(const sf::Event& e);
		void HandleRealtimeEvent();
		void Exec(const std::string& cmd);

		void Show(bool b = true);
		bool Visible() const;

		//TODO enable logging

	private:

		std::string m_commandBuffer; //console input
		CommandList m_commandHistory;
		sf::Uint8 m_historyIndex;
		std::list<std::string> m_textBuffer; //console output
		std::map<std::string, CommandData> m_items;

		typedef std::map<sf::Keyboard::Key, std::string> CommandKeyMap;
		typedef std::map<std::string, sf::Keyboard::Key> KeyMap;
		typedef std::map<sf::Mouse::Button, std::string> CommandMouseMap;
		typedef std::map<std::string, sf::Mouse::Button> MouseMap;
		//(button ID << 8 | controller ID), command
		typedef std::map<sf::Uint16, std::string> CommandJoyMap;
		typedef std::map<std::string, sf::Uint16> JoyMap;
		//(joyAxis << 8 | controller ID), command
		typedef std::map<sf::Uint16, std::string> CommandAxisMap;
		typedef std::map<std::string, sf::Joystick::Axis> AxisMap;

		CommandKeyMap m_keyCommands;
		CommandKeyMap m_realtimeKeyCommands;
		KeyMap m_keyMap;

		CommandMouseMap m_mouseCommands;
		CommandMouseMap m_realtimeMouseCommands;
		MouseMap m_mouseMap;

		CommandJoyMap m_joyCommands;
		CommandJoyMap m_realtimeJoyCommands;
		JoyMap m_joyMap;
		CommandAxisMap m_axisCommands;
		AxisMap m_axisMap;

		std::string m_BindInput(CommandList& l, bool realtime);

		CommandList m_configList; //list of commands to store in autoexec.con
		void m_AddToCfg(const std::string& cmd);
		void m_RemoveFromCfg(const std::string& cmd);

		bool m_show;
		bool m_cheatsEnabled;
		sf::RectangleShape m_backgroundRect;
		sf::Text m_outputText;
		sf::Text m_bufferText;
		sf::View m_view;

		void m_ParseCommand();
		void m_UpdateText();
		void draw(sf::RenderTarget& rt, sf::RenderStates states) const;
		void m_MapStrToInput();
	};
}

#endif //CONSOLE_H_