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

///mesh node for models which support morph animations///
#ifndef MORPH_NODE_H_
#define MORPH_NODE_H_

#include <Mesh/MeshBuffer.h>
#include <Mesh/StaticMeshNode.h>

namespace ml
{
	class MorphMeshNode final : public MeshNode
	{
	public:
		typedef std::shared_ptr<MorphMeshNode> Ptr;

		static const std::array<MorphAnimation, 21> MD2Animations;
		enum MD2Anim //used to index above array
		{
			Stand				 = 0u,
			Run					 = 1u,
			Attack				 = 2u,
			PainA				 = 3u,
			PainB				 = 4u,
			PainC				 = 5u,
			Jump				 = 6u,
			Flip				 = 7u,
			Salute				 = 8u,
			Fallback			 = 9u,
			Wave				 = 10u,
			Point				 = 11u,
			CrouchStand			 = 12u,
			CrouchWalk			 = 13u,
			CrouchAttack		 = 14u,
			CrouchPain			 = 15u,
			CrouchDeath			 = 16u,
			DeathFallBack		 = 17u,
			DeathFallForward	 = 18u,
			DeathFallBackSlow	 = 19u,
			Boom				 = 20u
		};

		MorphMeshNode();
		MorphMeshNode(BufferCollection& meshBuffers);

		void SetBufferCollection(BufferCollection& meshBuffers);

		void Play(bool loop = true);
		void Play(MorphAnimation animation, bool loop = true);
		void Pause();
		void Stop();


	private:
		MorphAnimation m_currentAnimation;

		sf::Uint16 m_currentFrame;
		sf::Uint16 m_nextFrame;
		float m_currentTime;
		MeshBuffer m_animBuffer;

		bool m_playing;
		bool m_loop;

		void m_UpdateSelf(float dt);
	};
}

#endif //MOPRH_NODE_H_