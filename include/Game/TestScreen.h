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

///header file for initial start screen class///
#ifndef TEST_SCREEN_H_
#define TEST_SCREEN_H_

#include <Game/BaseScreen.h>

#include <Game/UIContainer.h>
#include <Game/UIButton.h>
#include <Game/UIScrollSprite.h>
#include <Game/UITextBox.h>
#include <Game/UICheckBox.h>
#include <Game/UIComboBox.h>
#include <Game/UISlider.h>

#include <Game/SceneNode.h>
#include <Game/ViewCamera.h>

#include <Mesh/MeshLib.h>
#include <tmx/MapLoader.h>
#include <Box2D/Box2D.h>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

#include <DebugShape.h>

namespace Game
{
	class TestScreen final : public BaseScreen
	{
	public:
		TestScreen(sf::RenderWindow& renderWindow, SharedData& sharedData);

	private:

		ml::ScenePtr m_meshScene;
		ml::MeshResource m_meshResource;
		ml::AnimationResource m_animResource;
		ml::Camera m_meshCam;
		sf::View view;
		ml::LightPtr light;

		ml::StaticMeshNode::Ptr boxMesh;
		ml::SkeletalMeshNode::Ptr sm;

		tmx::MapLoader m_map;
		CameraFactory m_camFactory;

		CommandQueue m_commandQueue;
		SceneNode m_rootNode;

		void m_Update(float dt);
		void m_Render();
		void m_HandleCustomEvent();
		void m_HandleRealtimeEvent();

		void m_OnStart();
		void m_OnFinish();
		void m_OnContextCreate(const sf::VideoMode& v);

		void m_LoadConsoleCommands();

		//----options menu----//
		UI::UIContainer m_optionsContainer;
		UI::UIButton::Ptr m_optionsApplyButton;
		UI::UIButton::Ptr m_optionsCloseButton;
		UI::UICheckBox::Ptr m_optionsFullScreen;
		UI::UICheckBox::Ptr m_optionsVSync;
		UI::UIComboBox::Ptr m_optionsResolution;
		UI::UIComboBox::Ptr m_optionsMultisampling;
		UI::UISlider::Ptr m_optionsMusicVolume;
		UI::UISlider::Ptr m_optionsEffectsVolume;
		void m_BuildOptionsMenu();
		void m_ParseOptions();
		void m_UpdateViewRatio();
		void m_LayoutOptionsMenu(float dt);

		std::string m_VideoModeToString(const sf::VideoMode& mode);
		float m_GetVideoModeRatio(const sf::VideoMode& mode);

	};
}

#endif //TEST_SCREEN_H_