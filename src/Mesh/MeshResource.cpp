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

///implements mesh resource class///
#include <Mesh/Resources.h>
#include <Mesh/ObjParser.h>
#include <Mesh/MD2Parser.h>
#include <Mesh/MD5Parser.h>

#include <cassert>

using namespace ml;

//ctor
MeshResource::MeshResource(){}

//public
BufferCollection& MeshResource::Get(const std::string& path, ml::FileType type)
{
	assert(!path.empty());

	//check if mesh already loaded
	auto m = m_meshes.find(path);
	if(m != m_meshes.end())
		return m->second;

	//else attempt to load mesh via type parser
	BufferPtr mb;
	BufferCollection bc;
	m_meshes[path] = bc;
	switch(type)
	{
	case FileType::Obj:
		if(!ObjParser::LoadFromFile(path, m_meshes[path]))
			m_meshes[path].push_back(std::move(mb));
		break;
	case FileType::MD2:
		if(!MD2Parser::LoadFromFile(path, m_meshes[path]))
			m_meshes[path].push_back(std::move(mb));
		break;
	case FileType::MD5:
		if (!MD5Parser::LoadFromFile(path, m_meshes[path]))
			m_meshes[path].push_back(std::move(mb));
		break;
	default: 
		m_meshes[path].push_back(std::move(mb));
		break;
	}

	return m_meshes[path];
}

//private