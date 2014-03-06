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

/*represents a series of frames in a skeletal animation*/

#ifndef SKANIM_H_
#define SKANIM_H_

#include <Mesh/Skeleton.h>

namespace ml
{
	class SkeletalAnimation final : private sf::NonCopyable
	{
	public:
		SkeletalAnimation();

		//returns current skeletal pose of animation
		const Skeleton& GetSkeleton() const;
		void Update(float dt);
		void AddFrame(SkeletonPtr frame);
		void SetFrameRate(float framerate);
		bool CheckJoints(const MeshBuffer::Joints& joints) const;
		JointInfoList& GetJointInfo();

		void Play(bool loop = true);
		void Pause();
		void Stop();

	private:

		float m_frameTime;
		float m_currentTime;
		SkeletonList m_frames;
		SkeletonPtr m_animatedFrame;

		sf::Uint16 m_currentFrame, m_nextFrame;
		JointInfoList m_jointInfo;

		bool m_playing;
		bool m_loop;

		void m_Interpolate(const Skeleton& currentFrame, const Skeleton& nextFrame, float percent);
		glm::quat m_Slerp(const glm::quat& start, const glm::quat& end, float percent);
	};

	typedef std::unique_ptr<SkeletalAnimation> AnimPtr;
}

#endif //SKANIM_H_