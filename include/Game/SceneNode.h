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

///class for scene nodes in node graph///

#ifndef SCENE_NODE_H_
#define SCENE_NODE_H_

#include <Game/Command.h>
#include <Game/PhysEntity.h>
#include <Game/Behaviors.h>
#include <Game/Component.h>
#include <Game/ViewCamera.h>
#include <Game/BaseParticle.h>
#include <Game/Health.h>
#include <Game/AudioManager.h>

#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/NonCopyable.hpp>
#include <SFML/System/Vector2.hpp>

#include <Mesh/Node.h>

#include <memory>
#include <vector>
#include <map>

namespace Game
{
	class SceneNode 
		: public sf::Transformable,
		public sf::Drawable,
		private sf::NonCopyable,
		public Deletable
	{
	public:
		enum InputBit
		{
			Up       = 1 << 0,
			Down     = 1 << 1,
			Left     = 1 << 2,
			Right    = 1 << 3,
			Button0  = 1 << 4,
			Button1  = 1 << 5,
			Button2  = 1 << 6,
			Button3  = 1 << 7,
			Button4  = 1 << 8,
			Button5  = 1 << 9,
			Button6  = 1 << 10,
			Button7  = 1 << 11,
			Button8  = 1 << 12,
			Button9  = 1 << 13,
			Button10 = 1 << 14,
			Button11 = 1 << 15
		};

		enum InputAxis
		{
			X,
			Y,
			Z
		};

		typedef std::unique_ptr<SceneNode> Ptr;

		explicit SceneNode(AudioManager& am);
		~SceneNode();
		//adds a node to the current node's children
		void AddChild(Ptr& child);
		//removes a node from the children if found, and returns a pointer to the node
		Ptr RemoveChild(const SceneNode& node);
		//clears all children from the node
		void RemoveChildren();
		//performs any updates on the node and subsequent child nodes
		void Update(float dt);
		//returns the node's current category
		sf::Uint16 GetCategory() const;
		//sets the receiver category for node commands
		void SetCategory(sf::Uint16 category);
		//executes any commands passed from command queue and passes to any children
		void OnCommand(const NodeCommand& command, float dt);
		//sets the node's sprite
		void SetSprite(const sf::Sprite& sprite);
		sf::Sprite& GetSprite();
		//gets the node's accumulated trandsform
		sf::Transform GetWorldTransform() const;
		//gets the node's world position
		sf::Vector2f GetWorldPosition() const;
		//sets the node's world position
		void SetWorldPosition(sf::Vector2f position);
		//translates any attached mesh relative to the node
		void SetPosition3D(const sf::Vector3f& position);
		void SetPosition3D(float x, float y, float z);
		void Move3D(const sf::Vector3f& amount);
		void Move3D(float x, float y, float z);
		const sf::Vector3f& GetPosition3D() const;
		//rotates any attached mesh relative to nodes position
		void SetRotation3D(const sf::Vector3f& rotation);
		void SetRotation3D(float x, float y, float z);
		void Rotate3D(const sf::Vector3f& rotation);
		void Rotate3D(float x, float y, float z);
		const sf::Vector3f& GetRotation3D() const;
		//node's delete implementation
		void Delete() override;
		//attach a component
		void AttachComponent(Component::Ptr& component);
		//make phys drawing visible
		void DrawPhysEnt(bool b);
		//plays a sound at the node's current position
		void PlaySound(Audio::Effect effect);
		//set / get input
		void SetInput(InputBit bit, bool b);
		void SetInput(InputAxis axis, float value);
		sf::Uint16 GetInputBits() const;
		sf::Vector3f GetInputAxis() const;
		void ResetInput();

		sf::Uint32 UniqueId() const;
	private:
		//pointers to all children attached this node
		std::vector<Ptr> m_children;
		//pointer to parent
		SceneNode* m_parent;
		//represents the nodes category for targetting commands
		sf::Uint16 m_category;
		//attachable components
		std::vector<Component::Ptr> m_components;
		sf::Sprite m_sprite;

		//for modulating 3D mesh positions *doesn't affect node itself*
		//these will always be added to any node movement
		sf::Vector3f m_3dRotation;
		sf::Vector3f m_3dPosition;

		//input mask members - enables input from controls, network or AI
		sf::Uint16 m_inputBits;
		float m_xAxisInput;
		float m_yAxisInput;
		float m_zAxisInput;

		AudioManager& m_audioManager;
		sf::Uint32 m_uniqueId;

		bool m_drawPhysEnt;
		//override for drawable
		void draw(sf::RenderTarget& rt, sf::RenderStates states) const;
		//allows us to draw just the current node
		void drawSelf(sf::RenderTarget& rt, sf::RenderStates states) const;

		void m_SetPhysCategory();
		std::vector<Component::Ptr>::const_iterator m_FindComponent(Component::Type type) const;

		//updates the current state of the node
		void updateSelf(float dt);
	};
}

#endif //SCENE_NODE_H_