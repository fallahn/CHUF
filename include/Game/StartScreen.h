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
#ifndef START_SCREEN_H_
#define START_SCREEN_H_

#include <Game/BaseScreen.h>

#include <Game/UIContainer.h>
#include <Game/UIButton.h>
#include <Game/UIScrollSprite.h>
#include <Game/UITextBox.h>
#include <Game/UICheckBox.h>
#include <Game/UIComboBox.h>
#include <Game/UISlider.h>

#include <Game/SceneNode.h>

#include <sstream>

namespace Game
{
	class StartScreen final : public BaseScreen
	{
	public:
		StartScreen(sf::RenderWindow& renderWindow, SharedData& sharedData);

	private:

		CommandQueue m_commandQueue;
		SceneNode m_rootNode;

		void m_Update(float dt) override;
		void m_Render() override;
		void m_HandleCustomEvent() override;
		void m_HandleRealtimeEvent() override;

		void m_OnStart() override;
		void m_OnFinish() override;
		void m_OnContextCreate(const sf::VideoMode& v) override;

		void m_LoadConsoleCommands() override;

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
		void m_HandleApplySettings();
		void m_UpdateViewRatio();
		void m_LayoutOptionsMenu(float dt);

		std::string m_VideoModeToString(const sf::VideoMode& mode);
		float m_GetVideoModeRatio(const sf::VideoMode& mode);

	};
}

#endif //START_SCREEN_H_