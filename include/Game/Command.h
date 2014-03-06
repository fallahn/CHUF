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

///header for command and command queue classes which send commands down the scene node tree///
#ifndef COMMAND_H_
#define COMMAND_H_

#include <SFML/Config.hpp>

#include <Game/Component.h>

#include <functional>
#include <queue>

namespace Game
{
	struct ComponentCommand
	{
		explicit ComponentCommand(Component::Type target) : targetType(target){}
		std::function<void(Component& component, float dt)> action;
		Component::Type targetType;
	};

	class SceneNode;
	struct NodeCommand
	{
		enum Category //allows us to set the destination node by category
		{
			None	= 0,
			Scene	= 1 << 0,
			Player	= 1 << 1,
			Trigger	= 1 << 2,
			Coin	= 1 << 3
			//add other node types here, player enemy etc
		};
		NodeCommand() : 
			category (0){};
		std::function<void(SceneNode& node, float dt)> action;
		sf::Uint16 category; //bitwise OR multiple categories together to set the command's destination
		std::vector<ComponentCommand> componentCommands;
	};


	struct CommandQueue
	{
		void push_back(NodeCommand& command)
		{
			m_queue.push(command);
		}
		NodeCommand pop()
		{
			NodeCommand c = m_queue.front();
			m_queue.pop();
			return c;
		}
		bool empty()
		{
			return m_queue.empty();
		}

	private:
		std::queue<NodeCommand> m_queue;
	};
	
}

#endif //COMMAND_H_