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

///source for material class///
#include <Mesh/Material.h>
#include <cassert>

using namespace ml;

namespace
{
	const float defShininess = 80.f;
}

//ctor
Material::Material()
    :   m_ambientColour		(sf::Color(123u, 123u, 123u)),
        m_diffuseColour		(sf::Color(255u, 255u, 255u)),
        m_specularColour	(sf::Color(0u, 0u, 0u)),
		m_emissiveColour	(sf::Color(0u, 0u, 0u)),
		m_shininess			(defShininess),
        m_smooth			(true)
{

}

Material::Material(const sf::Color& ambient, const sf::Color& diffuse, const sf::Color& specular, const sf::Color& emissive)
	:	m_ambientColour		(ambient),
	m_diffuseColour			(diffuse),
	m_specularColour		(specular),
	m_emissiveColour		(emissive),
	m_shininess				(defShininess),
	m_smooth				(true)
{

}

//public
void Material::SetAmbientColour(const sf::Color& colour)
{
	m_ambientColour = colour;
}

const sf::Color& Material::GetAmbientColour() const
{
	return m_ambientColour;
}

void Material::SetDiffuseColour(const sf::Color& colour)
{
	m_diffuseColour = colour;
}

const sf::Color& Material::GetDiffuseColour() const
{
	return m_diffuseColour;
}

void Material::SetSpecularColour(const sf::Color& colour)
{
	m_specularColour = colour;
}

const sf::Color& Material::GetSpecularColour() const
{
	return m_specularColour;
}

void Material::SetEmissiveColour(const sf::Color& colour)
{
	m_emissiveColour = colour;
}

const sf::Color& Material::GetEmissiveColour() const
{
	return m_emissiveColour;
}

void Material::SetShininess(float shininess)
{
	assert(shininess >= 0.f && shininess <= 128.f);
	m_shininess = shininess;
}

float Material::GetShininess() const
{
	return m_shininess;
}

void Material::SetSmooth(bool b)
{
	m_smooth = b;
}

bool Material::Smooth() const
{
	return m_smooth;
}