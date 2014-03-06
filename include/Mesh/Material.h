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

////simple class to hold opengl material properties////
#ifndef MATERIAL_H_
#define MATERIAL_H_

#include <SFML/Graphics/Color.hpp>

namespace ml
{
	class Material final
	{
	public:
		Material();
		Material(const sf::Color& ambient, const sf::Color& diffuse, const sf::Color& specular, const sf::Color& emissive);

		void SetAmbientColour(const sf::Color& colour);
		const sf::Color& GetAmbientColour() const;
		void SetDiffuseColour(const sf::Color& colour);
		const sf::Color& GetDiffuseColour() const;
		void SetSpecularColour(const sf::Color& colour);
		const sf::Color& GetSpecularColour()const;
		void SetEmissiveColour(const sf::Color& colour);
		const sf::Color& GetEmissiveColour() const;
		void SetShininess(float amount);
		float GetShininess() const;
		void SetSmooth(bool b);
		bool Smooth() const;

	private:
		sf::Color m_ambientColour, m_diffuseColour;
		sf::Color m_specularColour, m_emissiveColour;
		float m_shininess;
		bool m_smooth;
	};
}

#endif //MATERIAL_H_
