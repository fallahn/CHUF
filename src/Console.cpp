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

#include <Game/Console.h>

#include <SFML/Graphics/RenderTarget.hpp>

#include <Helpers.h>
#include <cassert>
#include <ostream>
#include <fstream>

using namespace Game;

namespace
{
	const sf::Uint8 maxBufferLines = 42u;
	const sf::Uint8 maxBufferLength = 100u;
	const sf::Vector2f position(-5000.f, -5000.f);
	const sf::Vector2f size(1152.f, 768.f);
	const float textOffset = 10.f;
	const std::string prompt(">");
	const std::string cursor("_");
	const sf::Uint16 charSize = 15u;
	const sf::Uint8 commandHistorySize = 10u;
	const std::string configExtension(".con");
}

Console::Console(const sf::Font& font)
	: m_show		(false),
	m_cheatsEnabled	(true),
	m_historyIndex	(0u)
{
	//set up text
	m_bufferText.setFont(font);
	m_bufferText.setCharacterSize(charSize);
	m_bufferText.setPosition(position);
	m_bufferText.move(textOffset, size.y - (charSize * 2u));

	m_outputText.setFont(font);
	m_outputText.setCharacterSize(charSize);
	m_outputText.setPosition(position);
	m_outputText.move(textOffset, textOffset);

	//set up background
	m_backgroundRect.setFillColor(sf::Color(0u, 0u, 0u, 120u));
	m_backgroundRect.setSize(size);
	m_backgroundRect.setPosition(position);

	//set up custom view so console always the same size
	m_view.setSize(size);
	m_view.setCenter(position + (size / 2.f));

	m_UpdateText();
	m_MapStrToInput();

	m_commandHistory.reserve(commandHistorySize + 1);

	//create some default commands
	CommandData cd;
	cd.action = [this](CommandList l) -> std::string
	{
		for (const auto& i : m_items)
			Print(i.first + ": " + i.second.help + ((i.second.commandFlags & CommandFlag::Cheat) ? " CHEAT" : ""));

		return std::to_string(m_items.size()) + " commands found";
	};
	cd.help = "Lists all available commands";
	AddItem("list_commands", cd);

	cd.action = [this](CommandList l)->std::string
	{
		if (l.empty())
		{
			Exec("list_commands");
			return "Help usage: type help <command> to display any available information for that command.";
		}

		const auto i = m_items.find(l[0]);
		if (i != m_items.end())
		{
			std::string info = i->second.help;
			if (info.empty()) info = "No help available.";
			return i->first + ": " + info;
		}
		else
		{
			return i->first + ": command not found.";
		}
	};
	cd.help = "displays available help information for given command";
	AddItem("help", cd);

	cd.action = [this](CommandList l) ->std::string
	{
		return m_BindInput(l, false);
	};
	cd.help = "params <input> <command> <controllerId (optional)> bind console commands to non-realtime events";
	AddItem("bind", cd);

	cd.action = [this](CommandList l)->std::string
	{
		return m_BindInput(l, true);
	};
	cd.help = "params <input> <command> <controllerId (optional)> bind console commands to realtime events such as input";
	AddItem("bind_realtime", cd);

	cd.action = [this](CommandList l)->std::string
	{
		if (l.size() < 3)
		{
			Print("missing parameter");
			return "usage: bind_axis <axis> <command> <controllerId>";
		}
		auto axis = m_axisMap.find(l[0]);
		if (axis == m_axisMap.end())
		{
			for (auto& s : m_axisMap)
				Print("Available: " + s.first);

			return l[0] + ": invalid controller axis";
		}

		auto command = m_items.find(l[1]);
		if (command == m_items.end())
			return l[1] + " not found";
		else if ((command->second.commandFlags & CommandFlag::ControllerAnalogue) == 0)
			return l[1] + " cannot be bound to a controller axis";

		sf::Uint8 controllerId = Helpers::String::GetFromString(l[2], 8u);
		if (controllerId >= sf::Joystick::Count)
		{
			return std::to_string(controllerId) + ": invalid controller ID - must be 0 - 7";
		}
		sf::Uint16 uniqueId = (axis->second << 8) | controllerId;
		auto binding = m_axisCommands.find(uniqueId);
		if (binding != m_axisCommands.end())
		{
			binding->second = l[1];
		}
		else
		{
			m_axisCommands.insert(std::make_pair(uniqueId, l[1]));
		}
		return "bound " + l[0] + " on controller " + l[2] + " to " + l[1];
	};
	cd.help = "params <axis> <command> <controllerId> binds console commands to controller axis.";
	cd.commandFlags = CommandFlag::Config;
	AddItem("bind_axis", cd);

	cd.action = [this](CommandList l)->std::string
	{
		Show(!m_show);
		return "";
	};
	cd.help = "";
	cd.commandFlags = 0u;
	AddItem("show_console", cd);

	cd.action = [this](CommandList l)->std::string
	{
		std::ofstream file("commands.txt");

		for (const auto& i : m_items)
			file << i.first + ": " + i.second.help << ((i.second.commandFlags & CommandFlag::Cheat) ? " CHEAT"  : "") << std::endl;

		file << std::endl;
		file << "Inputs available for mapping:" << std::endl;
		file << "---------------------------" << std::endl;
		for (const auto& k : m_keyMap)
			file << "Key: " << k.first << std::endl;

		file << std::endl;
		for (const auto& m : m_mouseMap)
			file << "Mouse: " << m.first << std::endl;

		file << std::endl;
		for (const auto& j : m_joyMap)
			file << "Controller: " << j.first << std::endl;
		file << std::endl;
		for (const auto& j : m_axisMap)
			file << "Constroller axis: " << j.first << std::endl;

		return "Wrote file commands.txt";
	};
	cd.help = "Creates a text file containing all currently registered commands.";
	AddItem("create_cmd_list", cd);

	cd.action = [this](CommandList l)->std::string
	{
		if (l.empty())
		{
			return "No file specified. Usage: <string> filename";
		}

		std::string fileName = l[0];
		if (fileName.substr(fileName.size() - 4u) != configExtension)
			fileName += configExtension;

		std::fstream file(fileName);
		if (file.fail())
		{
			return "Could not open " + fileName;
		}
		else
		{
			std::string line;
			while (std::getline(file, line))
			{
				Exec(line);
			}
			return "Finished executing " + fileName;
		}
	};
	cd.help = "param <string> Executes specified .con file";
	AddItem("exec", cd);

	cd.action = [this](CommandList l)->std::string
	{
		if (l.empty()) return " echo Usage: <string> prints a string to the console";
		Print(l[0]);
		return "";
	};
	cd.help = "param <string> prints a string to the console window";
	AddItem("echo", cd);

	cd.action = [this](CommandList l)->std::string
	{
		//TODO remove duplicate code
		
		if (l.empty())
			return "usage: unbind <input> <controllerID (optional)> - input type to unbind from command";
		
		std::string input = Helpers::String::ToLower(l[0]);
		if (input == "all")
		{
			m_keyCommands.clear();
			m_realtimeKeyCommands.clear();
			m_mouseCommands.clear();
			m_realtimeMouseCommands.clear();
			m_joyCommands.clear();
			m_realtimeJoyCommands.clear();

			m_RemoveFromCfg("bind");
			m_RemoveFromCfg("bind_realtime");

			return "unbound all inputs. Was that wise?";
		}

		std::string removeMsg = "Unbound " + input + " from: ";

		//search key bindings
		const auto k = m_keyMap.find(input);
		if (k != m_keyMap.end())
		{
			auto kc = m_keyCommands.find(k->second);
			if (kc != m_keyCommands.end())
			{
				removeMsg += kc->second;
				m_keyCommands.erase(kc);
				m_RemoveFromCfg("bind " + input);
				return removeMsg;
			}
			kc = m_realtimeKeyCommands.find(k->second);
			if (kc != m_realtimeKeyCommands.end())
			{
				removeMsg += kc->second;
				m_realtimeKeyCommands.erase(kc);
				m_RemoveFromCfg("bind_realtime " + input);
				return removeMsg;
			}
		}

		//search mouse bindings
		const auto m = m_mouseMap.find(input);
		if (m != m_mouseMap.end())
		{
			auto mc = m_mouseCommands.find(m->second);
			if (mc != m_mouseCommands.end())
			{
				removeMsg += mc->second;
				m_mouseCommands.erase(mc);
				m_RemoveFromCfg("bind " + input);
				return removeMsg;
			}
			mc = m_realtimeMouseCommands.find(m->second);
			if (mc != m_realtimeMouseCommands.end())
			{
				removeMsg += mc->second;
				m_realtimeMouseCommands.erase(mc);
				m_RemoveFromCfg("bind_realtime " + input);
				return removeMsg;
			}
		}

		//search controller bindings
		const auto j = m_joyMap.find(input);
		if (j != m_joyMap.end())
		{
			if (l.size() < 2)
				return "unbinding controller input requires controller ID. Type 'help unbind' for more information";

			sf::Int32 cid = Helpers::String::GetFromString(l[1], -1);
			if (cid < 0 || cid > sf::Joystick::Count)
				return "Invalid controller ID.";

			sf::Uint16 uniqueId = (j->second << 8) | cid;
			auto jc = m_joyCommands.find(uniqueId);
			if (jc != m_joyCommands.end())
			{
				removeMsg += jc->second + " on controller " + l[1];
				m_joyCommands.erase(jc);
				m_RemoveFromCfg("bind " + input);
				return removeMsg;
			}
			jc = m_realtimeJoyCommands.find(uniqueId);
			if (jc != m_realtimeJoyCommands.end())
			{
				removeMsg += jc->second + " on controller " + l[1];
				m_realtimeJoyCommands.erase(jc);
				m_RemoveFromCfg("bind_realtime " + input);
				return removeMsg;
			}
		}
		return input + " not bound to any command";
	};
	cd.help = "param <input> <controllerID (optional)> unbinds an input if it is bound";
	AddItem("unbind", cd);

	cd.action = [this](CommandList l)->std::string
	{
		std::string fileName;
		if (!l.empty())
		{
			fileName = l[0];
			if (fileName.substr(fileName.size() - 4) != configExtension)
			{
				fileName += configExtension;
			}
		}
		else
		{
			fileName = "default.con";
		}
		
		std::ofstream cfgFile(fileName);
		for (auto& cmd : m_configList)
			cfgFile << cmd << std::endl;

		return "Wrote " + std::to_string(m_configList.size()) + " lines to " + fileName;
	};
	cd.help = "<path (optional)> Saves the current configuration to configuration file";
	AddItem("export_cfg", cd);

	cd.action = [this](CommandList l)->std::string
	{
		m_cheatsEnabled = !m_cheatsEnabled;
		return "set cheats enabled to " + (m_cheatsEnabled) ? "true" : "false";
	};
	cd.help = "toggles enabling cheat commands";
	AddItem("cheats_enable", cd);
}

//public
void Console::AddItem(const std::string& name, CommandData cd)
{
	assert(m_items.find(name) == m_items.end());
	m_items.insert(std::make_pair(name, cd));
}

void Console::RemoveItem(const std::string& str)
{
	const auto item = m_items.find(str);
	if (item != m_items.end())
		m_items.erase(item);
}

void Console::Print(const std::string& line)
{
	//TODO optional print each line to log file
	m_textBuffer.push_back(line);
	if (m_textBuffer.size() > maxBufferLines)
		m_textBuffer.pop_front();

	if (m_show)
		m_UpdateText();
}

bool Console::HandleEvent(const sf::Event& event)
{
	//update keyboard input - only when visible
	if (m_show)
	{
		if (event.type == sf::Event::KeyPressed)
		{
			switch (event.key.code)
			{
			case sf::Keyboard::Tab:
				m_show = false;
				return true;
			case sf::Keyboard::BackSpace:
				if (!m_commandBuffer.empty())
					m_commandBuffer.pop_back();
				m_UpdateText();
				return true;
			case sf::Keyboard::Return:
					m_ParseCommand();
				return true;
			case sf::Keyboard::Up:
				if (!m_commandHistory.empty()
					&& m_historyIndex > 0)
				{
					m_commandBuffer = m_commandHistory[--m_historyIndex];
					m_UpdateText();
				}
				return true;
			case sf::Keyboard::Down:
				if (!m_commandHistory.empty()
					&& m_historyIndex < m_commandHistory.size())
				{
					m_commandBuffer = m_commandHistory[m_historyIndex++];
					m_UpdateText();
				}
				return true;
			}
		}
		else if (event.type == sf::Event::TextEntered)
		{
			// Handle ASCII characters only, skipping backspace and delete
			if (event.text.unicode > 31
				&& event.text.unicode < 127
				&& m_commandBuffer.size() < maxBufferLength)
			{
				m_commandBuffer += static_cast<char>(event.text.unicode);
				m_UpdateText();
			}
		}
	}
	else
	{
		//handle any keybinds
		if (event.type == sf::Event::KeyPressed)
		{
			auto k = m_keyCommands.find(event.key.code);
			if (k != m_keyCommands.end())
			{
				Exec(k->second);
				return true;
			}
		}
		//mouse bindings
		if (event.type == sf::Event::MouseButtonPressed)
		{
			auto m = m_mouseCommands.find(event.mouseButton.button);
			if (m != m_mouseCommands.end())
			{
				Exec(m->second);
				return true;
			}
		}

		//and controller bindings
		if (event.type == sf::Event::JoystickButtonPressed)
		{
			sf::Uint16 uniqueId = (event.joystickButton.button << 8) | event.joystickButton.joystickId;
			auto j = m_joyCommands.find(uniqueId);
			{
				if (j != m_joyCommands.end())
				{
					Exec(j->second);
					return true;
				}
			}
		}
	}
	return false;
}

void Console::HandleRealtimeEvent()
{
	for (const auto& k : m_realtimeKeyCommands)
	{
		if (sf::Keyboard::isKeyPressed(k.first))
			Exec(k.second);
	}

	for (const auto& m : m_realtimeMouseCommands)
	{
		if (sf::Mouse::isButtonPressed(m.first))
			Exec(m.second);
	}

	for (const auto& j : m_realtimeJoyCommands)
	{
		sf::Uint8 button = (j.first >> 8);
		sf::Uint8 controller = (j.first & 0xff00);
		if (sf::Joystick::isButtonPressed(button, controller))
			Exec(j.second);
	}

	for (const auto& j : m_axisCommands)
	{
		sf::Joystick::Axis axis = static_cast<sf::Joystick::Axis>(j.first >> 8);
		sf::Uint8 controller = (j.first & 0xff00);
		if (sf::Joystick::isConnected(controller))
			Exec(j.second + " " + std::to_string(sf::Joystick::getAxisPosition(controller, axis)));
	}
}

void Console::Exec(const std::string& cmd)
{
	m_commandBuffer = cmd;
	m_ParseCommand();
}

void Console::Show(bool b)
{
	m_show = b;
	if (b) m_UpdateText();
}

bool Console::Visible() const
{
	return m_show;
}

//private
std::string Console::m_BindInput(CommandList& l, bool realtime)
{
	CommandKeyMap& keyCommands = (realtime) ? m_realtimeKeyCommands : m_keyCommands;
	CommandMouseMap& mouseCommands = (realtime) ? m_realtimeMouseCommands : m_mouseCommands;
	CommandJoyMap& joyCommands = (realtime) ? m_realtimeJoyCommands : m_joyCommands;
	
	std::string input = Helpers::String::ToLower(l[0]);
	std::string type = (realtime) ? "bind_realtime" : "bind";
	//if any commands are missing output current binding for input or help
	auto s = l.size();
	switch (s)
	{
	case 0:
	{
		return type + " parameters: <input> <command> <controllerId (optional)>";
	}
	case 1:
	{
		//keyboard
		auto k = m_keyMap.find(input);
		if (k != m_keyMap.end())
		{
			const auto ic = keyCommands.find(k->second);
			if (ic != keyCommands.end())
			{
				return k->first + " currently bound to: " + ic->second;
			}
		}
		//mouse
		auto m = m_mouseMap.find(input);
		if (m != m_mouseMap.end())
		{
			const auto ic = mouseCommands.find(m->second);
			if (ic != mouseCommands.end())
			{
				return m->first + " currently bound to: " + ic->second;
			}
		}
		//controller
		auto j = m_joyMap.find(input);
		if (j != m_joyMap.end())
		{
			for (auto i = 0; i < sf::Joystick::Count; ++i)
			{
				const auto ic = joyCommands.find((j->second << 8) | i);
				if (ic != joyCommands.end())
				{
					Print(j->first + " currently bound to: " + ic->second + " on controller " + std::to_string(i));
				}
			}
			return "";
		}
		return input + " not currently bound to any command.";
	}
	default: break;
	}

	//otherwise go ahead and bind input
	auto command = m_items.find(l[1]);
	if (command == m_items.end())
		return l[1] + ": command not found.";
	else if (command->second.commandFlags & CommandFlag::ControllerAnalogue)
		return l[1] + ": can only be bound to a controller axis. type 'help bind_axis' for more information";

	std::string cfgArgs = type;
	for (auto& str : l) cfgArgs += " " + str;

	auto k = m_keyMap.find(input);
	if (k != m_keyMap.end())
	{
		//update key if already bound
		auto ic = keyCommands.find(k->second);
		if (ic != keyCommands.end())
		{
			ic->second = l[1];
			m_RemoveFromCfg(type + " " + input);
		}
		else
		{
			keyCommands.insert(std::make_pair(k->second, l[1]));
		}

		//check key not bound to alt bind type
		auto& altBinds = (realtime) ? m_keyCommands : m_realtimeKeyCommands;
		const auto bc = altBinds.find(k->second);
		if (bc != altBinds.end())
		{
			altBinds.erase(bc);
			m_RemoveFromCfg((realtime) ? "bind " + input : "bind_realtime " + input);
			Print("Removed existing binding for " + input);
		}
		m_AddToCfg(cfgArgs);
		return "bound " + input + " to: '" + l[1] + "'";
	}
	//-----------------------------
	auto m = m_mouseMap.find(input);
	if (m != m_mouseMap.end())
	{
		const auto ic = mouseCommands.find(m->second);
		if (ic != mouseCommands.end())
		{
			ic->second = l[1];
			m_RemoveFromCfg(type + " " + input);
		}
		else
		{
			mouseCommands.insert(std::make_pair(m->second, l[1]));
		}

		//check not already bound to alt input type and remove if necessary
		auto& altBinds = (realtime) ? m_mouseCommands : m_realtimeMouseCommands;
		const auto bc = altBinds.find(m->second);
		if (bc != altBinds.end())
		{
			altBinds.erase(bc);
			m_RemoveFromCfg((realtime) ? "bind " + input : "bind_realtime " + input);
			Print("Removed existing binding for " + input);
		}
		m_AddToCfg(cfgArgs);
		return "bound " + input + " to: '" + l[1] + "'";
	}
	//---------------------------
	auto j = m_joyMap.find(input);
	if (j != m_joyMap.end())
	{
		//controllers are slightly diffrent as there can be up to eight
		if (s < 3)
		{
			return "No controller ID supplied. type 'help " + type + "' for more information";
		}
		sf::Uint8 cid = Helpers::String::GetFromString(l[2], 0u);
		if (cid >= sf::Joystick::Count) return l[2] + ": Invalid controller ID";

		//create a unique ID from controller ID and button ID
		sf::Uint16 uniqueId = (j->second << 8) | cid;
		const auto ic = joyCommands.find(uniqueId);
		if (ic != joyCommands.end())
		{
			ic->second = l[1];
			m_RemoveFromCfg(type + " " + input);
		}
		else
		{
			joyCommands.insert(std::make_pair(uniqueId, l[1]));
		}

		//check not already bound to alt input type and remove if necessary
		auto& altBinds = (realtime) ? m_joyCommands : m_realtimeJoyCommands;
		const auto bc = altBinds.find(uniqueId);
		if (bc != altBinds.end())
		{
			altBinds.erase(bc);
			m_RemoveFromCfg((realtime) ? "bind " + input : "bind_realtime " + input);
			Print("Removed existing binding for " + input);
		}
		m_AddToCfg(cfgArgs);
		return "bound " + input + " to: '" + l[1] + "' on controller '" + std::to_string(cid) + "'";
	}

	return input + ": invalid input (did you mean to use lower_case?)";
}

void Console::m_AddToCfg(const std::string& cmd)
{
	m_configList.push_back(cmd);
	std::sort(m_configList.begin(), m_configList.end());
	m_configList.erase(std::unique(m_configList.begin(), m_configList.end()), m_configList.end());
}

void Console::m_RemoveFromCfg(const std::string& cmd)
{
	m_configList.erase(std::remove_if(m_configList.begin(), m_configList.end(),
		[&cmd](const std::string& str)->bool
	{
		std::size_t s = cmd.size();
		auto pos = cmd.find(' ');
		return (pos != std::string::npos) && (s > pos) && (str.substr(0, cmd.size()) == cmd);
	}), m_configList.end());
}

void Console::m_ParseCommand()
{	
	if (m_commandBuffer.empty()) return;
	
	//allow commands in quotes as single string
	std::string cmd;
	std::string argString; 
	const auto comment = m_commandBuffer.find_first_of('"');
	if (comment != std::string::npos)
	{
		cmd = m_commandBuffer.substr(0u, comment);
		argString = m_commandBuffer.substr(m_commandBuffer.find_first_of('"'), m_commandBuffer.find_last_of('"'));
	}
	else
	{
		cmd = m_commandBuffer;
	}
	//split into command and arguments
	CommandList commandList = Helpers::String::Tokenize(cmd, ' ');	
	if (!argString.empty())
	{
		//Helpers::String::RemoveChar(argString, '"');
		argString.erase(argString.begin());
		argString.erase(argString.end() - 1);
		commandList.push_back(argString);
	}

	const auto item = m_items.find(commandList[0]);
	if (item != m_items.end())
	{
		//check if result of command should be saved in con file
		if ((item->second.commandFlags & CommandFlag::Config))
		{
			m_RemoveFromCfg(cmd);
			m_AddToCfg(cmd);
		}

		if ((item->second.commandFlags & CommandFlag::Cheat)
			&& !m_cheatsEnabled)
		{
			Print(commandList[0] + " cannot execute without cheats enabled");
		}
		else
		{
			commandList.erase(commandList.begin()); //remove command name before passing args

			std::string result = item->second.action(commandList);
			if (!result.empty())	Print(result);
		}
	}
	else
	{
		//print command not found to console
		Print(commandList[0] + ": unknown command.");
		//Print("Sorry, I don't know how to " + commandList[0]);
	}
	m_commandHistory.push_back(m_commandBuffer);
	if (m_commandHistory.size() > commandHistorySize)
		m_commandHistory.erase(m_commandHistory.begin());
	m_commandBuffer.clear();
	m_historyIndex = m_commandHistory.size();
	m_UpdateText();
}

void Console::m_UpdateText()
{
	m_bufferText.setString(prompt + m_commandBuffer + cursor);

	//update output buffer
	std::string output;
	for (const auto& s : m_textBuffer)
		output += s + "\n";

	m_outputText.setString(output);
}

void Console::draw(sf::RenderTarget& rt, sf::RenderStates states) const
{
	if (m_show)
	{
		sf::View v = rt.getView();
		rt.setView(m_view);

		//do drawing
		rt.draw(m_backgroundRect, states);
		rt.draw(m_outputText, states);
		rt.draw(m_bufferText, states);

		//restore view
		rt.setView(v);
	}
}

void Console::m_MapStrToInput()
{
	m_keyMap["a"] = sf::Keyboard::A;
	m_keyMap["b"] = sf::Keyboard::B;
	m_keyMap["c"] = sf::Keyboard::C;
	m_keyMap["d"] = sf::Keyboard::D;
	m_keyMap["e"] = sf::Keyboard::E;
	m_keyMap["f"] = sf::Keyboard::F;
	m_keyMap["g"] = sf::Keyboard::G;
	m_keyMap["h"] = sf::Keyboard::H;
	m_keyMap["i"] = sf::Keyboard::I;
	m_keyMap["j"] = sf::Keyboard::J;
	m_keyMap["k"] = sf::Keyboard::K;
	m_keyMap["l"] = sf::Keyboard::L;
	m_keyMap["m"] = sf::Keyboard::M;
	m_keyMap["n"] = sf::Keyboard::N;
	m_keyMap["o"] = sf::Keyboard::O;
	m_keyMap["p"] = sf::Keyboard::P;
	m_keyMap["q"] = sf::Keyboard::Q;
	m_keyMap["r"] = sf::Keyboard::R;
	m_keyMap["s"] = sf::Keyboard::S;
	m_keyMap["t"] = sf::Keyboard::T;
	m_keyMap["u"] = sf::Keyboard::U;
	m_keyMap["v"] = sf::Keyboard::V;
	m_keyMap["w"] = sf::Keyboard::W;
	m_keyMap["x"] = sf::Keyboard::X;
	m_keyMap["y"] = sf::Keyboard::Y;
	m_keyMap["z"] = sf::Keyboard::Z;

	m_keyMap["num0"]          = sf::Keyboard::Num0;
	m_keyMap["num1"]          = sf::Keyboard::Num1;
	m_keyMap["num2"]          = sf::Keyboard::Num2;
	m_keyMap["num3"]          = sf::Keyboard::Num3;
	m_keyMap["num4"]          = sf::Keyboard::Num4;
	m_keyMap["num5"]          = sf::Keyboard::Num5;
	m_keyMap["num6"]          = sf::Keyboard::Num6;
	m_keyMap["num7"]          = sf::Keyboard::Num7;
	m_keyMap["num8"]          = sf::Keyboard::Num8;
	m_keyMap["num9"]          = sf::Keyboard::Num9;
	m_keyMap["escape"]        = sf::Keyboard::Escape;
	m_keyMap["left_control"]  = sf::Keyboard::LControl;
	m_keyMap["left_shift"]    = sf::Keyboard::LShift;
	m_keyMap["left_alt"]      = sf::Keyboard::LAlt;
	m_keyMap["left_system"]   = sf::Keyboard::LSystem;
	m_keyMap["right_control"] = sf::Keyboard::RControl;
	m_keyMap["right_shift"]   = sf::Keyboard::RShift;
	m_keyMap["right_alt"]     = sf::Keyboard::RAlt;
	m_keyMap["right_System"]  = sf::Keyboard::RSystem;
	m_keyMap["menu"]          = sf::Keyboard::Menu;
	m_keyMap["left_bracket"]  = sf::Keyboard::LBracket;
	m_keyMap["right_bracket"] = sf::Keyboard::RBracket;
	m_keyMap["semicolon"]     = sf::Keyboard::SemiColon;
	m_keyMap["comma"]         = sf::Keyboard::Comma;
	m_keyMap["period"]        = sf::Keyboard::Period;
	m_keyMap["quote"]         = sf::Keyboard::Quote;
	m_keyMap["forward_slash"] = sf::Keyboard::Slash;
	m_keyMap["back_slash"]    = sf::Keyboard::BackSlash;
	m_keyMap["tilde"]         = sf::Keyboard::Tilde;
	m_keyMap["equal"]         = sf::Keyboard::Equal;
	m_keyMap["dash"]          = sf::Keyboard::Dash;
	m_keyMap["space"]         = sf::Keyboard::Space;
	m_keyMap["return"]        = sf::Keyboard::Return;
	m_keyMap["backspace"]     = sf::Keyboard::BackSpace;
	m_keyMap["tab"]           = sf::Keyboard::Tab;
	m_keyMap["pageup"]        = sf::Keyboard::PageUp;
	m_keyMap["pagedown"]      = sf::Keyboard::PageDown;
	m_keyMap["end"]           = sf::Keyboard::End;
	m_keyMap["home"]          = sf::Keyboard::Home;
	m_keyMap["insert"]        = sf::Keyboard::Insert;
	m_keyMap["delete"]        = sf::Keyboard::Delete;
	m_keyMap["add"]           = sf::Keyboard::Add;
	m_keyMap["subtract"]      = sf::Keyboard::Subtract;
	m_keyMap["multiply"]      = sf::Keyboard::Multiply;
	m_keyMap["divide"]        = sf::Keyboard::Divide;
	m_keyMap["left"]          = sf::Keyboard::Left;
	m_keyMap["right"]         = sf::Keyboard::Right;
	m_keyMap["up"]            = sf::Keyboard::Up;
	m_keyMap["down"]          = sf::Keyboard::Down;
	m_keyMap["numpad0"]       = sf::Keyboard::Numpad0;
	m_keyMap["numpad1"]       = sf::Keyboard::Numpad1;
	m_keyMap["numpad2"]       = sf::Keyboard::Numpad2;
	m_keyMap["numpad3"]       = sf::Keyboard::Numpad3;
	m_keyMap["numpad4"]       = sf::Keyboard::Numpad4;
	m_keyMap["numpad5"]       = sf::Keyboard::Numpad5;
	m_keyMap["numpad6"]       = sf::Keyboard::Numpad6;
	m_keyMap["numpad7"]       = sf::Keyboard::Numpad7;
	m_keyMap["numpad8"]       = sf::Keyboard::Numpad8;
	m_keyMap["numpad9"]       = sf::Keyboard::Numpad9;
	m_keyMap["f1"]            = sf::Keyboard::F1;
	m_keyMap["f2"]            = sf::Keyboard::F2;
	m_keyMap["f3"]            = sf::Keyboard::F3;
	m_keyMap["f4"]            = sf::Keyboard::F4;
	m_keyMap["f5"]            = sf::Keyboard::F5;
	m_keyMap["f6"]            = sf::Keyboard::F6;
	m_keyMap["f7"]            = sf::Keyboard::F7;
	m_keyMap["f8"]            = sf::Keyboard::F8;
	m_keyMap["f9"]            = sf::Keyboard::F9;
	m_keyMap["f10"]           = sf::Keyboard::F10;
	m_keyMap["f11"]           = sf::Keyboard::F11;
	m_keyMap["f12"]           = sf::Keyboard::F12;
	m_keyMap["f13"]           = sf::Keyboard::F13;
	m_keyMap["f14"]           = sf::Keyboard::F14;
	m_keyMap["f15"]           = sf::Keyboard::F15;
	m_keyMap["pause"]         = sf::Keyboard::Pause;

	///----mouse-----

	m_mouseMap["mouse_left"]   = sf::Mouse::Left;
	m_mouseMap["mouse_right"]  = sf::Mouse::Right;
	m_mouseMap["mouse_middle"] = sf::Mouse::Middle;
	m_mouseMap["mouse_x1"]     = sf::Mouse::XButton1;
	m_mouseMap["mouse_x2"]     = sf::Mouse::XButton2;

	///----controller----

	m_joyMap["joy_button_0"]  =  0;
	m_joyMap["joy_button_1"]  =  1;
	m_joyMap["joy_button_2"]  =  2;
	m_joyMap["joy_button_3"]  =  3;
	m_joyMap["joy_button_4"]  =  4;
	m_joyMap["joy_button_5"]  =  5;
	m_joyMap["joy_button_6"]  =  6;
	m_joyMap["joy_button_7"]  =  7;
	m_joyMap["joy_button_8"]  =  8;
	m_joyMap["joy_button_9"]  =  9;
	m_joyMap["joy_button_10"] = 10;
	m_joyMap["joy_button_11"] = 11;
	m_joyMap["joy_button_12"] = 12;
	m_joyMap["joy_button_13"] = 13;
	m_joyMap["joy_button_14"] = 14;
	m_joyMap["joy_button_15"] = 15;
	m_joyMap["joy_button_16"] = 16;
	m_joyMap["joy_button_17"] = 17;
	m_joyMap["joy_button_18"] = 18;
	m_joyMap["joy_button_19"] = 19;
	m_joyMap["joy_button_20"] = 20;
	m_joyMap["joy_button_21"] = 21;
	m_joyMap["joy_button_22"] = 22;
	m_joyMap["joy_button_23"] = 23;
	m_joyMap["joy_button_24"] = 24;
	m_joyMap["joy_button_25"] = 25;
	m_joyMap["joy_button_26"] = 26;
	m_joyMap["joy_button_27"] = 27;
	m_joyMap["joy_button_28"] = 28;
	m_joyMap["joy_button_29"] = 29;
	m_joyMap["joy_button_30"] = 30;
	m_joyMap["joy_button_31"] = 31;
	
	m_axisMap["joy_axis_x"] = sf::Joystick::X;
	m_axisMap["joy_axis_y"] = sf::Joystick::Y;
	m_axisMap["joy_axis_z"] = sf::Joystick::Z;
	m_axisMap["joy_axis_r"] = sf::Joystick::R;
	m_axisMap["joy_axis_u"] = sf::Joystick::U;
	m_axisMap["joy_axis_v"] = sf::Joystick::V;
	m_axisMap["joy_pov_x"]  = sf::Joystick::PovX;
	m_axisMap["joy_pov_y"]  = sf::Joystick::PovY;
}