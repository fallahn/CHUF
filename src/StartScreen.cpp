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

///source code for default start up screen / main menu///
#include <Game/StartScreen.h>
#include <Helpers.h>

using namespace Game;

namespace
{
	const char testShader[] =
		"#version 120\n"
		"uniform sampler2D tex;"

		"void main()"
		"{"
		"gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);"
		"}";

}


StartScreen::StartScreen(sf::RenderWindow& renderWindow, SharedData& sharedData)
	: BaseScreen		(renderWindow, sharedData),
	m_optionsContainer	(renderWindow),
	m_rootNode			(sharedData.audioManager)
{
	m_text.setFont(m_font);
	m_text.setString("Press Space to Start or O for options");

	//calc view size based on aspect ratio
	m_UpdateViewRatio();

	//create options menu
	m_BuildOptionsMenu();

	//create console vars
	Console& console = sharedData.console;
	Console::CommandData cd;
	cd.action = [&renderWindow](Console::CommandList l)->std::string
	{
		if (l.empty())
		{
			return "'enable_vsync' usage: <bool> true or false.";
		}
		else if (l[0] == "true")
		{
			renderWindow.setVerticalSyncEnabled(true);
			return "vertical sync enabled.";
		}
		else if (l[0] == "false")
		{
			renderWindow.setVerticalSyncEnabled(false);
			return "vertical sync disabled.";
		}
		return l[0] + ": parameter not recognised. (Did you mean to use lower case?)";
	};
	cd.help = "param <bool> enable or disable vertical sync.";
	cd.commandFlags = Console::Config;
	console.AddItem("vsync_enable", cd);

	AudioManager& am = sharedData.audioManager;
	cd.action = [&am](Console::CommandList l)->std::string
	{
		if (l.empty())
		{
			return "music_volume: " + std::to_string(am.GetGlobalMusicVolume());
		}
		float value = Helpers::String::GetFromString(l[0], am.GetGlobalMusicVolume());
		if (value < 0.f || value > 100.f)
		{
			return "value must be between 0.0 and 100.0";
		}
		am.SetGlobalMusicVolume(value);
		return "music_volume set to: " + l[0];
	};
	cd.help = "param <float> 0.0 - 100.0";
	cd.commandFlags = Console::Config;
	console.AddItem("music_volume", cd);

	cd.action = [&am](Console::CommandList l)->std::string
	{
		if (l.empty())
		{
			return "effects_volume: " + std::to_string(am.GetGlobalEffectsVolume());
		}
		float value = Helpers::String::GetFromString(l[0], am.GetGlobalEffectsVolume());
		if (value < 0.f || value > 100.f)
		{
			return "value must be between 0.0 and 100.0";
		}
		am.SetGlobalEffectsVolume(value);
		return "effects_volume set to : " + l[0];
	};
	cd.help = "param <float> 0.0 - 100.0";
	cd.commandFlags = Console::Config;
	console.AddItem("effects_volume", cd);
}

void StartScreen::m_Update(float dt)
{
	//update physics world

	//update ai/behaviors

	//update scene graph
	while (!m_commandQueue.empty())
		m_rootNode.OnCommand(m_commandQueue.pop(), dt);
	m_rootNode.Update(dt);

	//update mesh scene graph


	//update component positioning in options menu
	m_LayoutOptionsMenu(dt);
}

void StartScreen::m_Render()
{
	m_renderWindow.clear(sf::Color(100u, 149u, 237u));
	m_renderWindow.draw(m_text);

	//draw ui on top
	m_renderWindow.draw(m_optionsContainer);
	m_renderWindow.draw(m_sharedData.console);
	m_renderWindow.display();
	m_DrawFps();
}

void StartScreen::m_HandleCustomEvent()
{
	if (m_sharedData.console.HandleEvent(m_event)) return;
	m_optionsContainer.HandleEvent(m_event);

	if(m_event.type == sf::Event::KeyPressed
		&& m_hasFocus)
	{
		switch(m_event.key.code)
		{
		case sf::Keyboard::Q:

			break;
		case sf::Keyboard::Return:

			break;
		default: break;
		}
	}
	if(m_event.type == sf::Event::Resized)
	{

	}
}

void StartScreen::m_HandleRealtimeEvent()
{
	if (m_sharedData.console.Visible())return;
	m_sharedData.console.HandleRealtimeEvent();
}

void StartScreen::m_OnStart()
{
	m_LoadConsoleCommands();
}

void StartScreen::m_OnFinish()
{
	m_UnloadConsoleCommands();
}

void StartScreen::m_OnContextCreate(const sf::VideoMode& v)
{

}

void StartScreen::m_LoadConsoleCommands()
{
	Console& console = m_sharedData.console;
	std::string commandName = "quit";
	Console::CommandData cd;
	cd.action = [this](Console::CommandList l)->std::string
	{
		m_return = QUIT;
		m_running = false;
		return "Quitting...";
	};
	cd.help = "quit the game";
	cd.commandFlags = Console::None;
	console.AddItem(commandName, cd);
	m_commandList.push_back(commandName);

	commandName = "start";
	cd.action = [this](Console::CommandList l)->std::string
	{
		m_return = GAME_SCREEN;
		m_running = false;
		return "Starting...";
	};
	cd.help = "start the game";
	cd.commandFlags = Console::None;
	console.AddItem(commandName, cd);
	m_commandList.push_back(commandName);

	commandName = "show_options";
	cd.action = [this](Console::CommandList l)->std::string
	{
		m_optionsContainer.SetVisible(true);
		return "";
	};
	cd.help = "display options window";
	cd.commandFlags = Console::None;
	console.AddItem(commandName, cd);
	m_commandList.push_back(commandName);

	console.Exec("bind space start");
	console.Exec("bind o show_options");
	console.Exec("bind escape quit");
	console.Exec("bind tab show_console");
}

//----build UI----//
void StartScreen::m_BuildOptionsMenu()
{
	//drop down lists
	m_optionsResolution = std::make_shared<UI::UIComboBox>(m_sharedData.fonts.Get("assets/fonts/console.ttf"),
												m_sharedData.textures.Get("assets/textures/ui/resolution_dd.png"));

	const float currentRatio = m_GetVideoModeRatio(sf::VideoMode::getDesktopMode());
	const std::vector<sf::VideoMode>& modes = sf::VideoMode::getFullscreenModes();
	for(auto& m : modes)
		if(m.bitsPerPixel == 32 && m.width != 720u && m_GetVideoModeRatio(m) == currentRatio)
			m_optionsResolution->AddItem(m_VideoModeToString(m), (m.width << 16 | m.height)); //TODO set data val to bitshift

	//debug
	m_optionsResolution->AddItem(m_VideoModeToString(sf::VideoMode(800u, 600u)), (800 << 16 | 600));

	m_optionsMultisampling = std::make_shared<UI::UIComboBox>(m_sharedData.fonts.Get("assets/fonts/console.ttf"),
												m_sharedData.textures.Get("assets/textures/ui/multisample_dd.png"));
	m_optionsMultisampling->AddItem("Off", 0);
	m_optionsMultisampling->AddItem("1x", 1);
	m_optionsMultisampling->AddItem("2x", 2);
	m_optionsMultisampling->AddItem("4x", 4);
	m_optionsMultisampling->AddItem("8x", 8);


	//check boxes
	m_optionsVSync = std::make_shared<UI::UICheckBox>(m_sharedData.fonts.Get("assets/fonts/console.ttf"),
												m_sharedData.textures.Get("assets/textures/ui/check.png"));
	m_optionsVSync->SetAlignment(UI::UICheckBox::Alignment::Right);
	m_optionsVSync->SetText("Enable VSync");

	m_optionsFullScreen = std::make_shared<UI::UICheckBox>(m_sharedData.fonts.Get("assets/fonts/console.ttf"),
												m_sharedData.textures.Get("assets/textures/ui/check.png"));
	m_optionsFullScreen->SetAlignment(UI::UICheckBox::Alignment::Right);
	m_optionsFullScreen->SetText("Full Screen");

	//sliders
	m_optionsMusicVolume = std::make_shared<UI::UISlider>(m_sharedData.fonts.Get("assets/fonts/console.ttf"),
														m_sharedData.textures.Get("assets/textures/ui/slider_handle.png"));
	m_optionsMusicVolume->SetText("Music Volume:");
	m_optionsEffectsVolume = std::make_shared<UI::UISlider>(m_sharedData.fonts.Get("assets/fonts/console.ttf"),
														m_sharedData.textures.Get("assets/textures/ui/slider_handle.png"));
	m_optionsEffectsVolume->SetText("Effects Volume:");

	//buttons
	m_optionsApplyButton = std::make_shared<UI::UIButton>(m_sharedData.fonts.Get(),
														m_sharedData.textures.Get("assets/textures/ui/buttons/apply.png"));
	m_optionsApplyButton->SetCallback([this]()
	{
		m_HandleApplySettings();
	});

	m_optionsCloseButton = std::make_shared<UI::UIButton>(m_sharedData.fonts.Get(),
														m_sharedData.textures.Get("assets/textures/ui/buttons/close.png"));
	m_optionsCloseButton->SetCallback([this]()
	{
		//m_mainMenuContainer.SetVisible();
		m_optionsContainer.SetVisible(false);
	});

	m_optionsContainer.AddComponent(m_optionsCloseButton);
	m_optionsContainer.AddComponent(m_optionsApplyButton);
	m_optionsContainer.AddComponent(m_optionsEffectsVolume);
	m_optionsContainer.AddComponent(m_optionsMusicVolume);
	m_optionsContainer.AddComponent(m_optionsFullScreen);
	m_optionsContainer.AddComponent(m_optionsVSync);
	m_optionsContainer.AddComponent(m_optionsMultisampling);
	m_optionsContainer.AddComponent(m_optionsResolution);

	m_optionsContainer.SetVisible(false);

	m_ParseOptions();
}

void StartScreen::m_ParseOptions()
{
	VideoSettings settings("config.cfg");
	m_optionsResolution->SelectItem(m_VideoModeToString(settings.getResolution()));
	m_optionsMultisampling->SelectItem(settings.getMultiSamples());
	m_optionsVSync->Check(settings.getVsync());
	m_optionsFullScreen->Check(settings.getFullcreen());
	m_optionsMusicVolume->SetValue(settings.getMusicVolume());
	m_optionsEffectsVolume->SetValue(settings.getSfxVolume());
}

void StartScreen::m_UpdateViewRatio()
{
	float ratio = (static_cast<float>(m_renderWindow.getSize().x) / static_cast<float>(m_renderWindow.getSize().y));
	m_sharedData.Screen.ViewSize.x = (ratio < 1.4f) ? 1440.f : 1920.f;
	m_sharedData.Screen.ViewSize.y = (ratio > 1.65f || ratio < 1.4f) ? 1080.f : 1200.f;
	m_sharedData.Screen.DefaultView.setSize(m_sharedData.Screen.ViewSize);
	m_sharedData.Screen.DefaultView.setCenter(m_sharedData.Screen.ViewSize / 2.f);
	m_sharedData.Screen.WindowScale = static_cast<float>(m_renderWindow.getSize().x) / m_sharedData.Screen.ViewSize.x;
	m_renderWindow.setView(m_sharedData.Screen.DefaultView);
}

std::string StartScreen::m_VideoModeToString(const sf::VideoMode& mode)
{
	std::stringstream stream;
	stream << mode.width << " x " << mode.height;
	return stream.str();
}

float StartScreen::m_GetVideoModeRatio(const sf::VideoMode& mode)
{
	return static_cast<float>(mode.width) / mode.height;
}

void StartScreen::m_LayoutOptionsMenu(float dt)
{
	sf::Vector2f c = m_renderWindow.getView().getCenter();

	//update position of options menu components
	m_optionsApplyButton->setPosition(c + sf::Vector2f(-110.f, 60.f));
	m_optionsCloseButton->setPosition(c + sf::Vector2f(110.f, 60.f));
	m_optionsMusicVolume->setPosition(c + sf::Vector2f(-125.f, -10.f));
	m_optionsEffectsVolume->setPosition(c + sf::Vector2f(-125.f, 20.f));
	m_optionsResolution->setPosition(c + sf::Vector2f(-146.f, -180.f));
	m_optionsMultisampling->setPosition(m_optionsResolution->getPosition() + sf::Vector2f(190.f, 0.f));
	m_optionsVSync->setPosition(c + sf::Vector2f(-120.f, -120.f));
	m_optionsFullScreen->setPosition(c + sf::Vector2f(-120.f, -80.f));

	m_optionsContainer.Update(dt);
}
