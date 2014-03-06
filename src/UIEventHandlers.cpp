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

#include <Game/StartScreen.h>

using namespace Game;

void StartScreen::m_HandleApplySettings()
{
	VideoSettings settings("config.cfg");
	//recreate window if video settings have changed
	if ((m_optionsResolution->GetSelectedText() != m_VideoModeToString(settings.getResolution()))
		|| (m_optionsMultisampling->GetSelectedValue() != settings.getMultiSamples())
		|| (m_optionsFullScreen->Checked() != settings.getFullcreen()))
	{
		//store current settings as fall back
		sf::ContextSettings currentContext, newContext;
		currentContext.antialiasingLevel = settings.getMultiSamples();
		newContext.antialiasingLevel = m_optionsMultisampling->GetSelectedValue();
		newContext.depthBits = currentContext.depthBits = 24u;

		sf::VideoMode currentMode, newMode;
		currentMode = settings.getResolution();
		const std::vector<sf::VideoMode>& modes = sf::VideoMode::getFullscreenModes();
		for (auto& m : modes)
		{
			if (m_VideoModeToString(m) == m_optionsResolution->GetSelectedText())
			{
				newMode = m;
				break;
			}
		}

		//attempt to apply new settings
		try
		{
			m_renderWindow.create(newMode, "", m_optionsFullScreen->Checked() ? sf::Style::Fullscreen : sf::Style::Close, newContext);
			m_UpdateViewRatio();
			m_renderWindow.setView(m_sharedData.Screen.DefaultView);

			settings.setResolution(newMode);
			settings.setFullScreen(m_optionsFullScreen->Checked());
			settings.setMultiSamples(m_optionsMultisampling->GetSelectedValue());

			m_OnContextCreate(newMode);
		}
		catch (...)
		{
			m_renderWindow.create(currentMode, "", settings.getFullcreen() ? sf::Style::Fullscreen : sf::Style::Close, currentContext);
		}
	}
	//apply vsync
	m_renderWindow.setVerticalSyncEnabled(m_optionsVSync->Checked());
	settings.setVsync(m_optionsVSync->Checked());

	//apply audio settings
	m_sharedData.AudioManager.SetGlobalMusicVolume(m_optionsMusicVolume->GetValue());
	settings.setMusicVolume(m_optionsMusicVolume->GetValue());

	m_sharedData.AudioManager.SetGlobalEffectsVolume(m_optionsEffectsVolume->GetValue());
	settings.setSfxVolume(m_optionsEffectsVolume->GetValue());
}