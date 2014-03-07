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

#include <Mesh/SkeletalAnimation.h>

//#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/compatibility.hpp>

#include <cassert>

#include <SFML/Window/Keyboard.hpp>
#include <iostream>

using namespace ml;

//ctor
SkeletalAnimation::SkeletalAnimation()
: m_frameTime	(0.f),
m_currentTime	(0.f),
m_currentFrame	(0u),
m_nextFrame		(0u),
m_playing		(false),
m_loop			(false)
{

}

//public
const Skeleton& SkeletalAnimation::GetSkeleton() const
{
	assert(m_animatedFrame != nullptr);
	return *m_animatedFrame;
}

void SkeletalAnimation::Update(float dt)
{
	if (m_playing)
	{
		m_currentTime += dt;

		if (m_currentTime > m_frameTime)
		{
			if (m_currentFrame < m_frames.size() - 1)
			{
				m_currentFrame++;
			}
			else
			{
				m_currentFrame = 0u;
				if (!m_loop) Stop();
			}
			m_nextFrame = m_currentFrame + 1u;
			if (m_nextFrame >= m_frames.size())
				m_nextFrame = 0u;

			m_currentTime = 0.f;
		}

		if (m_currentTime)
			m_Interpolate(*m_frames[m_currentFrame], *m_frames[m_nextFrame], m_currentTime / m_frameTime);
	}
}

void SkeletalAnimation::AddFrame(SkeletonPtr frame)
{
	if (m_animatedFrame == nullptr)
	{
		auto s = frame->joints.size();
		m_animatedFrame = std::unique_ptr<Skeleton>(new Skeleton());// = std::make_unique<Skeleton>(); //not supported by gcc
		m_animatedFrame->joints.assign(s, SkeletonJoint());
	}
	m_frames.push_back(std::move(frame));
	if (m_frames.size() > 1) m_nextFrame = m_currentFrame + 1u;
}

void SkeletalAnimation::SetFrameRate(float framerate)
{
	m_frameTime = 1.f / framerate;
}

bool SkeletalAnimation::CheckJoints(const MeshBuffer::Joints& joints) const
{
	auto s = joints.size();
	if (s != m_jointInfo.size())
		return false;

	for (auto i = 0u; i < s; ++i)
	{
		if (joints[i].parentId != m_jointInfo[i].parentID
			|| joints[i].name != m_jointInfo[i].name)
		{
			return false;
		}
	}
	return true;
}

JointInfoList& SkeletalAnimation::GetJointInfo()
{
	return m_jointInfo;
}

void SkeletalAnimation::Play(bool loop)
{
	m_playing = true;
	m_loop = loop;
}

void SkeletalAnimation::Pause()
{
	m_playing = false;
}

void SkeletalAnimation::Stop()
{
	m_playing = false;
	m_currentFrame = 0u;
}

//private
void SkeletalAnimation::m_Interpolate(const Skeleton& startFrame, const Skeleton& endFrame, float percent)
{
	auto s = m_animatedFrame->joints.size();
	for (auto i = 0u; i < s; ++i)
	{
		auto& jFinal = m_animatedFrame->joints[i];
		const auto& jStart = startFrame.joints[i];
		const auto& jEnd = endFrame.joints[i];

		jFinal.parent = jStart.parent;

		jFinal.position = glm::lerp(jStart.position, jEnd.position, percent);
		jFinal.rotation = glm::shortMix/*m_Slerp*/(jStart.rotation, jEnd.rotation, percent);
	}
}

glm::quat SkeletalAnimation::m_Slerp(const glm::quat& start, const glm::quat& end, float percent)
{
	//check for out-of range parameter and return end points if so
	if (percent <= 0.0)
		return start;
	else if (percent >= 1.0)
		return end;

	float cosOhm = glm::dot(start, end);
	//if result negative invert end
	glm::quat newEnd = end;
	if (cosOhm < 0.f)
	{
		newEnd.x = -newEnd.x;
		newEnd.y = -newEnd.y;
		newEnd.z = -newEnd.z;
		newEnd.w = -newEnd.w;
	}

	float k0 = 0.f;
	float k1 = 0.f;

	if (cosOhm > 0.999f)
	{
		//very close - just use linear interpolation,
		//which will protect againt a divide by zero
		k0 = 1.0f - percent;
		k1 = percent;
	}
	else if (cosOhm < 0.01f)
	{
		k0 = percent;
		k1 = 1.0f - percent;
	}
	else
	{
		//compute the sin of the angle using the
		//trig identity sin^2(omega) + cos^2(omega) = 1
		float sinOhm = std::sqrt(1.0f - (cosOhm * cosOhm));

		//compute the angle from its sin and cosine
		float ohm = std::atan2(sinOhm, cosOhm);

		//compute inverse of denominator, so we only have
		//to divide once
		float oneOverSinOhm = 1.f / sinOhm;

		//compute interpolation parameters
		k0 = std::sin((1.f - percent) * ohm) * oneOverSinOhm;
		k1 = std::sin(percent * ohm) * oneOverSinOhm;
	}

	return glm::quat((k0 * start.w) + (k1 * newEnd.w),
					(k0 * start.x) + (k1 * newEnd.x),
					(k0 * start.y) + (k1 * newEnd.y),
					(k0 * start.z) + (k1 * newEnd.z));
}
