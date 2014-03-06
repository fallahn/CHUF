/*********************************************************************
MeshLib - Library for easy integration of 3D meshes into a 2D scene built
on SFML.

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

//root node representing the 3D scene//
#ifndef MESH_SCENE_H_
#define MESH_SCENE_H_

#include <Mesh/Node.h>
#include <Mesh/StaticMeshNode.h>
#include <Mesh/MorphMeshNode.h>
#include <Mesh/SkeletalMeshNode.h>
#include <Mesh/Resources.h>
#include <Mesh/Camera.h>
#include <Mesh/Light.h>

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Window/Context.hpp>

#include <pugixml/pugixml.hpp>
#include <Game/ResourceManager.h>
#include <Game/ShaderManager.h>
#include <Game/ViewCamera.h>

#include <glm/matrix.hpp>

namespace ml
{
	class MeshScene final 
		:	private Node,
			public sf::Drawable
	{
	public:
		MeshScene(const sf::RenderWindow& rw, Game::ShaderResource& shaderResource);
		~MeshScene()
		{
			m_DeleteBuffers();
		}

		//creates a new light and returns a pointer to it
		LightPtr AddLight();
		//creates a new static mesh node and returns a pointer to it
		StaticMeshNode::Ptr AddStaticMesh(BufferCollection& meshBuffers);
		//creates a morph animated mesh node and returns a pointer to it
		MorphMeshNode::Ptr AddMorphMesh(BufferCollection& meshBuffers);
		//creates a skeletal animated mesh node and returns a pointer to it
		SkeletalMeshNode::Ptr AddSkeletalMesh(BufferCollection& meshBuffers);
		//sets the view for the scene to follow
		void SetView(const sf::View& view);
		//updates the internal render texture
		void UpdateScene(float dt, const Game::ViewCamera::Cameras& viewCams);
		//resizes gl viewport and internal render textures
		void Resize(); 
		//returns the texture containing rendered image (without shadows)
		const sf::Texture& GetTexture();
		//returns the scene scale for multiplying SFML values.
		//the mesh scene is scled to 1m per GL unit which is 0.01 times
		//SFML units when SFML is 100 units per metre.
		float GetSceneScale() const;

		//loads a scene from an XML file - TODO document scene file structure
		void LoadScene(const std::string& mapName);

	private:

		std::unique_ptr<Camera> m_camera;
		//creates a default directional light, overridden when lights are added manually
		LightPtr m_directionalLight;
		bool m_useDirectionalLight;

		Lights m_lights;
		//resource managers used when loading a scene
		MeshResource m_meshResource;
		Game::TextureResource m_textureResource;
		Game::ShaderResource& m_shaderResource;

		sf::Sprite m_shadowSprite;
		sf::Sprite m_colourSprite;
		sf::RenderTexture m_shadowTexture; //end shadow result, not the shadow map
		sf::RenderTexture m_colourTexture;
		const sf::RenderWindow& m_renderWindow;

		glm::mat4 m_biasMatrix;

		ShaderProgram& m_shadowShader;
		sf::Shader& m_blendShader;

		float m_cameraZ; //distance for camera from 2D scene
		const float m_sceneScale; //meshes are scaled by this so we can reduce cam distance

		GLuint m_depthTextureId;
		GLuint m_depthBufferId;
		sf::Context m_context;
		void m_CreateDepthBuffer(sf::Uint16 width, sf::Uint16 height);
		void m_DeleteBuffers();
		void draw(sf::RenderTarget& rt, sf::RenderStates states) const;
		void drawShadowMap(const sf::Vector2f& size) const;
		glm::mat4 m_ApplyDepthMatrices();
	};
	typedef std::unique_ptr<MeshScene> ScenePtr;
}

#endif //MESH_SCENE_H_