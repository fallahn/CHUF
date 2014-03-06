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

#include <Game/ViewCamera.h>

using namespace Game;

//ctor
CameraFactory::CameraFactory()
{

}

//public
void CameraFactory::Update(float dt)
{
	m_cameras.erase(std::remove_if(m_cameras.begin(),
								m_cameras.end(),
								[](const ViewCamera::Ptr& c)->bool
								{
									return (c->Deleted() || !c->Attached());
								}), m_cameras.end());

	for (auto& c : m_cameras)
		c->m_Update(dt);
}

ViewCamera::Ptr Game::CameraFactory::Create()
{
	m_cameras.push_back(std::make_shared<ViewCamera>());
	return m_cameras.back();
}

ViewCamera::Ptr Game::CameraFactory::Create(const sf::View& view)
{
	m_cameras.push_back(std::make_shared<ViewCamera>(view));
	return m_cameras.back();
}

void CameraFactory::Reset()
{
	m_cameras.clear();
}

const ViewCamera::Cameras& CameraFactory::GetCameras()const
{
	return m_cameras;
}