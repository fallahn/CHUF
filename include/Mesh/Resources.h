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

///provides a resource manager for the uniform MeshBuffer class
///with the ability to load mesh data from various file types

#ifndef MESH_RESOURCE_H_
#define MESH_RESOURCE_H_

#include <Mesh/MeshBuffer.h>
#include <Mesh/SkeletalAnimation.h>

#include <map>
#include <string>

namespace ml
{
	enum class FileType
	{
		Obj,
		MD2,
		MD5
		//3ds etc
	};

	class MeshResource : private sf::NonCopyable
	{
	public:

		MeshResource();
		BufferCollection& Get(const std::string& path, FileType type);

	private:
		std::map<std::string, BufferCollection> m_meshes;
	};

	class AnimationResource : private sf::NonCopyable
	{
	public:
		AnimationResource();
		SkeletalAnimation& Get(const std::string& path, FileType type);

	private:
		std::map<std::string, AnimPtr> m_anims;
	};
}

#endif