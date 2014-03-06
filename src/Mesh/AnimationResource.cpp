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

#include <Mesh/Resources.h>
#include <Mesh/MD5Parser.h>

#include <cassert>
#include <iostream>

using namespace ml;

AnimationResource::AnimationResource(){}

SkeletalAnimation& AnimationResource::Get(const std::string& path, ml::FileType type)
{
	assert(!path.empty());

	auto a = m_anims.find(path);
	if (a != m_anims.end())
		return *a->second;

	AnimPtr animation = std::make_unique<SkeletalAnimation>();
	switch (type)
	{
	case ml::FileType::MD5:
		if (!MD5Parser::LoadAnimation(path, *animation))
			std::cerr << "Failed to load animation from: " << path << std::endl;
		break;
	default: break;
	}
	m_anims.insert(std::make_pair(path, std::move(animation)));
	return *m_anims[path];
}
