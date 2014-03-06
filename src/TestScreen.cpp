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
#include <Game/TestScreen.h>

#include <glm/vec3.hpp>

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


TestScreen::TestScreen(sf::RenderWindow& renderWindow, SharedData& sharedData)
	: BaseScreen		(renderWindow, sharedData),
	m_optionsContainer	(renderWindow),
	m_map				("assets/maps"),
	m_rootNode			(sharedData.AudioManager)
{
	m_text.setFont(m_font);
	m_text.setString("Press Return to Reset or O for options");
	
	//calc view size based on aspect ratio
	m_UpdateViewRatio();

	//create options menu
	m_BuildOptionsMenu();
}

void TestScreen::m_Update(float dt)
{	
	//update physics world

	//update ai/behaviors
	
	//boxMesh->Rotate(0.f, 30.f * dt, 0.f);
	//sm->Rotate(0.f, 30.f * dt, 0.f);

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		sm->Move(-4.f * dt, 0.f, 0.f);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		sm->Move(4.f * dt, 0.f, 0.f);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		sm->Move(0.f, 0.f, -4.f * dt);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		sm->Move(0.f, 0.f, 4.f * dt);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
		sm->Move(0.f, 4.f * dt, 0.f);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
		sm->Move(0.f, -4.f * dt, 0.f);

	//update scene graph
	while (!m_commandQueue.empty())
		m_rootNode.OnCommand(m_commandQueue.pop(), dt);
	m_rootNode.Update(dt);
	
	//update mesh scene graph
	m_meshScene->SetView(m_renderWindow.getView());
	m_meshScene->UpdateScene(dt, m_camFactory.GetCameras());

	//update component positioning in options menu
	m_LayoutOptionsMenu(dt);
}

void TestScreen::m_Render()
{
	m_renderWindow.clear(sf::Color::White);
	m_renderWindow.draw(m_text);

	m_renderWindow.draw(m_map);
	m_renderWindow.draw(m_rootNode);
	m_renderWindow.draw(*m_meshScene);
	
	//draw ui on top
	m_renderWindow.draw(m_optionsContainer);
	m_renderWindow.draw(m_sharedData.Console);
	m_renderWindow.display();
	m_DrawFps();
}

void TestScreen::m_HandleCustomEvent()
{
	if (m_sharedData.Console.HandleEvent(m_event)) return;
	m_optionsContainer.HandleEvent(m_event);

	if(m_event.type == sf::Event::KeyPressed
		&& m_hasFocus)
	{
		switch(m_event.key.code)
		{
		//case sf::Keyboard::Escape:
		//case sf::Keyboard::BackSpace:
		//	m_return = QUIT;
		//	m_running = false;
		//	break;
		//case sf::Keyboard::Space:
		//	m_return = GAME_SCREEN;
		//	m_running = false;
		//	break;
		//case sf::Keyboard::O:
		//	//TODO display options window
		//	m_optionsContainer.SetVisible(true);	
		//	break;
		case sf::Keyboard::Q: //TODO make this console command
			sm->DrawVertexNormals(!sm->VisibleVertexNormals());
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

void TestScreen::m_HandleRealtimeEvent()
{
	if (m_sharedData.Console.Visible()) return;
	m_sharedData.Console.HandleRealtimeEvent();
}

void TestScreen::m_OnStart()
{
	m_LoadConsoleCommands();
	
	m_rootNode.AttachComponent(static_cast<Component::Ptr>(m_camFactory.Create(m_renderWindow.getView())));
	
	glm::vec3 modelPos(6.f, -10.1f, 3.5f);
	m_meshScene.reset(new ml::MeshScene(m_renderWindow, m_sharedData.Shaders));
	light = m_meshScene->AddLight();
	//lets pretend this is from a loaded tmx
	sf::Vector2f lightPos(1024.f, 512.f);
	lightPos *= m_meshScene->GetSceneScale();
	light->SetPosition(lightPos.x, -lightPos.y, 13.f);
	sf::Vector2f t = view.getCenter() * m_meshScene->GetSceneScale();
	light->SetTarget(t.x, -t.y, -5.f);

	//boxMesh = m_meshScene->AddStaticMesh(m_meshResource.Get("assets/meshes/mole.obj", ml::FileType::Obj));
	//boxMesh->SetPosition(modelPos);
	//boxMesh->SetScale(glm::vec3(0.05f, 0.05f, 0.05f));
	//ml::ShaderProgram& shader = m_sharedData.Shaders.Get(ml::MeshShader::Normal);
	//shader.setParameter("reflectMap", m_sharedData.Textures.Get("assets/textures/skymap.jpg"));
	//boxMesh->SetShader(m_sharedData.Shaders.Get(ml::MeshShader::Normal), ml::MeshShader::Normal);
	//boxMesh->AddDiffuseTexture(m_sharedData.Textures.Get("assets/textures/mole_color.png"));
	//boxMesh->AddDiffuseTexture(m_sharedData.Textures.Get("assets/textures/sofa.png"));
	//boxMesh->AddNormalTexture(m_sharedData.Textures.Get("assets/textures/mole_normal.tga"));
	//boxMesh->AddMaskTexture(m_sharedData.Textures.Get("assets/textures/mole_mask.png"));
	////boxMesh->SetRotation(25.f, 0.f, 0.f);
	//boxMesh->SetCastShadows(true);
	//boxMesh->SetShader(m_sharedData.Shaders.Get(ml::MeshShader::Shadow), ml::MeshShader::Shadow);
	//boxMesh->DrawVertexNormals(true);

	/*ml::MorphMeshNode* morphMesh = m_meshScene->AddMorphMesh(m_meshResource.Get("assets/meshes/tris.md2", ml::FileType::MD2));
	morphMesh->SetScale(glm::vec3(0.13f, 0.13f, 0.13f));
	morphMesh->Move(11.4f, -7.f, 3.7f);
	morphMesh->SetRotation(0.f, -90.f, 0.f);
	morphMesh->SetShader(shader, ml::MeshShader::Normal);
	morphMesh->SetDiffuseTexture(m_sharedData.Textures.Get("assets/textures/tris.tga"));
	morphMesh->SetNormalTexture(m_sharedData.Textures.Get("assets/textures/tris_normal.tga"));
	morphMesh->SetMaskTexture(m_sharedData.Textures.Get("assets/textures/mask.png"));
	morphMesh->SetCastShadows(true);
	morphMesh->SetShader(m_sharedData.Shaders.Get(ml::MeshShader::Shadow), ml::MeshShader::Shadow);
	morphMesh->Play(ml::MorphMeshNode::MD2Animations[ml::MorphMeshNode::Stand]);
	morphMesh->DrawVertexNormals(true);*/

	auto& skeletalMeshes = m_meshResource.Get("assets/meshes/hellknight.md5mesh", ml::FileType::MD5);
	sm = m_meshScene->AddSkeletalMesh(skeletalMeshes);
	sm->SetShader(m_sharedData.Shaders.Get(ml::MeshShader::Normal), ml::MeshShader::Normal);
	for (auto& m : skeletalMeshes)
	{
		const auto& name = m->GetTextureName();
		sm->AddDiffuseTexture(m_sharedData.Textures.Get("assets/textures/" + name + ".tga"));
		sm->AddNormalTexture(m_sharedData.Textures.Get("assets/textures/" + name + "_normal.tga"));
		sm->AddMaskTexture(m_sharedData.Textures.Get("assets/textures/" + name + "_mask.png"));
	}
	sm->SetScale(glm::vec3(0.06f, 0.06f, 0.06f));
	sm->SetPosition(11.f, -10.f, 3.5f);
	sm->SetCastShadows(true);
	sm->SetShader(m_sharedData.Shaders.Get(ml::MeshShader::Shadow), ml::MeshShader::Shadow);
	//sm->DrawVertexNormals(true);
	sm->Rotate(-90.f, -90.f, 0.f);

	sm->Play(m_animResource.Get("assets/meshes/idle2.md5anim", ml::FileType::MD5));

	ml::StaticMeshNode::Ptr backGround = m_meshScene->AddStaticMesh(m_meshResource.Get("assets/meshes/background.obj", ml::FileType::Obj));
	backGround->SetScale(glm::vec3(0.4f, 0.4f, 0.4f));
	backGround->SetShader(m_sharedData.Shaders.Get(ml::MeshShader::Phong), ml::MeshShader::Phong);
	backGround->SetShader(m_sharedData.Shaders.Get(ml::MeshShader::Shadow), ml::MeshShader::Shadow);
	backGround->SetPosition(0.f, 0.f, -1.f);

	ml::Material m;
	m.SetDiffuseColour(sf::Color(230u, 110u, 129u));
	m.SetAmbientColour(sf::Color(120u, 35u, 64u));
	m.SetSpecularColour(sf::Color::White);
	m.SetShininess(100.f);
	//boxMesh->SetMaterial(m);
	
	m.SetDiffuseColour(sf::Color(120u, 120u, 120u));
	m.SetAmbientColour(sf::Color(80u, 80u, 80u));
	backGround->SetMaterial(m);

	m.SetAmbientColour(sf::Color(0u, 0u, 120u));
	m.SetDiffuseColour(sf::Color::Blue);
	//sm->SetMaterial(m);

	//m_map.Load("b2d.tmx");
}

void TestScreen::m_OnFinish()
{
	m_meshScene.reset();
	m_UnloadConsoleCommands();
}

void TestScreen::m_OnContextCreate(const sf::VideoMode& v)
{
	//normally called from GameScreen onStart function
	if(m_meshScene) m_meshScene->Resize();
}

void TestScreen::m_LoadConsoleCommands()
{
	Console& console = m_sharedData.Console;
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
void TestScreen::m_BuildOptionsMenu()
{
	//drop down lists
	m_optionsResolution = std::make_shared<UI::UIComboBox>(m_sharedData.Fonts.Get("assets/fonts/console.ttf"),
												m_sharedData.Textures.Get("assets/textures/ui/resolution_dd.png"));

	const float currentRatio = m_GetVideoModeRatio(sf::VideoMode::getDesktopMode());
	const std::vector<sf::VideoMode>& modes = sf::VideoMode::getFullscreenModes();
	for(auto& m : modes)
		if(m.bitsPerPixel == 32 && m.width != 720u && m_GetVideoModeRatio(m) == currentRatio)
			m_optionsResolution->AddItem(m_VideoModeToString(m), (m.width << 16 | m.height)); //TODO set data val to bitshift

	//debug
	m_optionsResolution->AddItem(m_VideoModeToString(sf::VideoMode(800u, 600u)), (800 << 16 | 600));

	m_optionsMultisampling = std::make_shared<UI::UIComboBox>(m_sharedData.Fonts.Get("assets/fonts/console.ttf"),
												m_sharedData.Textures.Get("assets/textures/ui/multisample_dd.png"));
	m_optionsMultisampling->AddItem("Off", 0);
	m_optionsMultisampling->AddItem("1x", 1);
	m_optionsMultisampling->AddItem("2x", 2);
	m_optionsMultisampling->AddItem("4x", 4);
	m_optionsMultisampling->AddItem("8x", 8);


	//check boxes
	m_optionsVSync = std::make_shared<UI::UICheckBox>(m_sharedData.Fonts.Get("assets/fonts/console.ttf"),
												m_sharedData.Textures.Get("assets/textures/ui/check.png"));
	m_optionsVSync->SetAlignment(UI::UICheckBox::Alignment::Right);
	m_optionsVSync->SetText("Enable VSync");

	m_optionsFullScreen = std::make_shared<UI::UICheckBox>(m_sharedData.Fonts.Get("assets/fonts/console.ttf"),
												m_sharedData.Textures.Get("assets/textures/ui/check.png"));
	m_optionsFullScreen->SetAlignment(UI::UICheckBox::Alignment::Right);
	m_optionsFullScreen->SetText("Full Screen");

	//sliders
	m_optionsMusicVolume = std::make_shared<UI::UISlider>(m_sharedData.Fonts.Get("assets/fonts/console.ttf"),
														m_sharedData.Textures.Get("assets/textures/ui/slider_handle.png"));
	m_optionsMusicVolume->SetText("Music Volume:");
	m_optionsEffectsVolume = std::make_shared<UI::UISlider>(m_sharedData.Fonts.Get("assets/fonts/console.ttf"),
														m_sharedData.Textures.Get("assets/textures/ui/slider_handle.png"));
	m_optionsEffectsVolume->SetText("Effects Volume:");	
	
	//buttons
	m_optionsApplyButton = std::make_shared<UI::UIButton>(m_sharedData.Fonts.Get(),
														m_sharedData.Textures.Get("assets/textures/ui/buttons/apply.png"));
	m_optionsApplyButton->SetCallback([this]()
	{
				
	});

	m_optionsCloseButton = std::make_shared<UI::UIButton>(m_sharedData.Fonts.Get(),
														m_sharedData.Textures.Get("assets/textures/ui/buttons/close.png"));
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

void TestScreen::m_ParseOptions()
{
	VideoSettings settings("config.cfg");
	m_optionsResolution->SelectItem(m_VideoModeToString(settings.getResolution()));
	m_optionsMultisampling->SelectItem(settings.getMultiSamples());
	m_optionsVSync->Check(settings.getVsync());
	m_optionsFullScreen->Check(settings.getFullcreen());
	m_optionsMusicVolume->SetValue(settings.getMusicVolume());
	m_optionsEffectsVolume->SetValue(settings.getSfxVolume());
}

void TestScreen::m_UpdateViewRatio()
{
	float ratio = (static_cast<float>(m_renderWindow.getSize().x) / static_cast<float>(m_renderWindow.getSize().y));
	m_sharedData.Screen.ViewSize.x = (ratio < 1.4f) ? 1440.f : 1920.f;
	m_sharedData.Screen.ViewSize.y = (ratio > 1.65f || ratio < 1.4f) ? 1080.f : 1200.f;
	m_sharedData.Screen.DefaultView.setSize(m_sharedData.Screen.ViewSize);
	m_sharedData.Screen.DefaultView.setCenter(m_sharedData.Screen.ViewSize / 2.f);
	m_sharedData.Screen.WindowScale = static_cast<float>(m_renderWindow.getSize().x) / m_sharedData.Screen.ViewSize.x;
	m_renderWindow.setView(m_sharedData.Screen.DefaultView);
}

std::string TestScreen::m_VideoModeToString(const sf::VideoMode& mode)
{
	std::stringstream stream;
	stream << mode.width << " x " << mode.height;
	return stream.str();
}

float TestScreen::m_GetVideoModeRatio(const sf::VideoMode& mode)
{
	return static_cast<float>(mode.width) / mode.height;
}

void TestScreen::m_LayoutOptionsMenu(float dt)
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