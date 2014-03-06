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

/*Animated mesh node which uses skeletal animation*/

#ifndef SKELETAL_H_
#define SKELETAL_H_

#include <Mesh/StaticMeshNode.h>
#include <Mesh/Skeleton.h>
#include <Mesh/SkeletalAnimation.h>

namespace ml
{
	class SkeletalMeshNode final : public StaticMeshNode
	{
	public:
		typedef std::shared_ptr<SkeletalMeshNode> Ptr;

		SkeletalMeshNode();
		SkeletalMeshNode(BufferCollection& bc);

		void SetBufferCollection(BufferCollection& bc);
		void Play(bool loop = true);
		void Play(SkeletalAnimation& anim, bool loop = true);
		void Pause();
		void Stop();

	private:
		SkeletalAnimation* m_anim;

		MeshBuffer::Joints* m_jointList;
		BufferCollection m_animatedBuffers;
		BufferCollection* m_defaultBuffers;

		Skeleton m_bindPose; //draw this if no animation loaded
		void m_CreateBindPose(); //creates the bind pose skeleton

		void m_ApplyBoneWeights(const Skeleton& skeleton);

		void m_UpdateSelf(float dt);
		void m_DrawDebug();
	};
}

#endif //SKELETAL_H_