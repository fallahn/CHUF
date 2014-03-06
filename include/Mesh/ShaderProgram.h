////shader program based on SFML shader class with added vertex shader functionality///
////Falls under the SFML license ////
#ifndef SHADER_PROGRAM_H_
#define SHADER_PROGRAM_H_

#include <SFML/Graphics/Export.hpp>
#include <SFML/Graphics/Transform.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Window/GlResource.hpp>
#include <SFML/System/NonCopyable.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>

#include <glm/matrix.hpp>
#include <glm/vec3.hpp>

#include <map>
#include <string>
#include <memory>

namespace sf
{
	class InputStream;
	class Texture;
}

namespace ml
{

	class SFML_GRAPHICS_API ShaderProgram : sf::GlResource, sf::NonCopyable
	{
	public :

		enum Type
		{
			Vertex, ///< Vertex shader
			Fragment ///< Fragment (pixel) shader
		};

		struct CurrentTextureType {};
		static CurrentTextureType CurrentTexture;

	public :

		ShaderProgram();
		~ShaderProgram();


		bool loadFromFile(const std::string& filename, Type type);
		bool loadFromFile(const std::string& vertexShaderFilename, const std::string& fragmentShaderFilename);
		bool loadFromMemory(const std::string& shader, Type type);
		bool loadFromMemory(const std::string& vertexShader, const std::string& fragmentShader);
		bool loadFromStream(sf::InputStream& stream, Type type);
		bool loadFromStream(sf::InputStream& vertexShaderStream, sf::InputStream& fragmentShaderStream);
		void setParameter(const std::string& name, float x);
		void setParameter(const std::string& name, float x, float y);
		void setParameter(const std::string& name, float x, float y, float z);
		void setParameter(const std::string& name, float x, float y, float z, float w);
		void setParameter(const std::string& name, const sf::Vector2f& vector);
		void setParameter(const std::string& name, const sf::Vector3f& vector);
		void setParameter(const std::string& name, const glm::vec3& vector);
		void setParameter(const std::string& name, const sf::Color& color);
		void setParameter(const std::string& name, const sf::Transform& transform);
		void setParameter(const std::string& name, const sf::Texture& texture);
		void setParameter(const std::string& name, sf::Uint32 glTexId);
		void setParameter(const std::string& name, CurrentTextureType);

		void setVertexAttribute(const std::string& name, int dataSize, const void* dataBuffer);
		void addVertexAttribute(const std::string& name);
		void setParameter(const std::string& name, const float* matrix);
		void setParameter(const std::string& name, const glm::mat4& matrix);

		static void bind(const ShaderProgram* shader);
		static bool isAvailable();

	private :

		////////////////////////////////////////////////////////////
		/// \brief Compile the shader(s) and create the program
		///
		/// If one of the arguments is NULL, the corresponding shader
		/// is not created.
		///
		/// \param vertexShaderCode Source code of the vertex shader
		/// \param fragmentShaderCode Source code of the fragment shader
		///
		/// \return True on success, false if any error happened
		///
		////////////////////////////////////////////////////////////
		bool compile(const char* vertexShaderCode, const char* fragmentShaderCode);

		////////////////////////////////////////////////////////////
		/// \brief Bind all the textures used by the shader
		///
		/// This function each texture to a different unit, and
		/// updates the corresponding variables in the shader accordingly.
		///
		////////////////////////////////////////////////////////////
		void bindTextures() const;

		////////////////////////////////////////////////////////////
		/// \brief Get the location ID of a shader parameter
		///
		/// \param name Name of the parameter to search
		///
		/// \return Location ID of the parameter, or -1 if not found
		///
		////////////////////////////////////////////////////////////
		int getParamLocation(const std::string& name);

		////////////////////////////////////////////////////////////
		int getAttribLocation(const std::string& name);
		////////////////////////////////////////////////////////////

		////////////////////////////////////////////////////////////
		// Types
		////////////////////////////////////////////////////////////
		typedef std::map<int, std::pair<const sf::Texture*, sf::Uint32>> TextureTable;
		typedef std::map<std::string, int> ParamTable;
		typedef std::map<std::string, int> AttribTable;

		////////////////////////////////////////////////////////////
		// Member data
		////////////////////////////////////////////////////////////
		unsigned int m_shaderProgram; ///< OpenGL identifier for the program
		int m_currentTexture; ///< Location of the current texture in the shader
		TextureTable m_textures; ///< Texture variables in the shader, mapped to their location
		ParamTable m_params; ///< Parameters location cache
		AttribTable m_attribs;
	};

	typedef std::shared_ptr<ShaderProgram> ShaderPtr;
}

#endif