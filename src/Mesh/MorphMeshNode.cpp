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

#include <Mesh/MorphMeshNode.h>

#include <cassert>

using namespace ml;

//static arrays of animations
const std::array<MorphAnimation, 21> MorphMeshNode::MD2Animations =
{
	MorphAnimation(0u, 39u, 9.f),		//STAND
	MorphAnimation(40u, 45u, 10.f),		//RUN
	MorphAnimation(46u, 53u, 10.f),		//ATTACK
	MorphAnimation(54u, 57u, 7.f),		//PAIN_A
	MorphAnimation(58u, 61u, 7.f),		//PAIN_B
	MorphAnimation(62u, 65u, 7.f),		//PAIN_C
	MorphAnimation(66u, 71u, 7.f),		//JUMP
	MorphAnimation(72u, 83u, 7.f),		//FLIP
	MorphAnimation(84u, 94u, 7.f),		//SALUTE
	MorphAnimation(95u, 111u, 10.f),	//FALLBACK
	MorphAnimation(112u, 122u, 7.f),	//WAVE
	MorphAnimation(123u, 134u, 6.f),	//POINT
	MorphAnimation(135u, 153u, 10.f),	//CROUCH_STAND
	MorphAnimation(154u, 159u, 7.f),	//CROUCH_WALK
	MorphAnimation(160u, 168u, 10.f),	//CROUCH_ATTACK
	MorphAnimation(196u, 172u, 7.f),	//CROUCH_PAIN
	MorphAnimation(173u, 177u, 5.f),	//CROUCH_DEATH
	MorphAnimation(178u, 183u, 7.f),	//DEATH_FALLBACK
	MorphAnimation(184u, 189u, 7.f),	//DEATH_FALLFORWARD
	MorphAnimation(190u, 197u, 7.f),	//DEATH_FALLBACKSLOW
	MorphAnimation(198u, 198u, 5.f)		//BOOM
};


//ctors
MorphMeshNode::MorphMeshNode()
	: m_currentFrame(0),
	m_nextFrame		(1),
	m_currentTime	(0.f),
	m_playing		(false),
	m_loop			(true)
{}

MorphMeshNode::MorphMeshNode(BufferCollection& meshBuffers)
	: m_currentFrame(0),
	m_currentTime	(0.f),
	m_nextFrame		(1),
	m_playing		(false),
	m_loop			(true)
{
	SetBufferCollection(meshBuffers);
}

//public
void MorphMeshNode::SetBufferCollection(BufferCollection& meshBuffers)
{
	m_SetBufferCollection(&meshBuffers);
	MorphAnimation defaultAnim;
	defaultAnim.end = meshBuffers.size() - 1;
	m_currentAnimation = defaultAnim;
	m_currentFrame = defaultAnim.start;
	m_nextFrame = (m_currentFrame < defaultAnim.end) ? m_currentFrame + 1 : defaultAnim.start;

	//copy the frame into its own buffer as we'll be modifying it
	MeshBuffer& frame = *meshBuffers.front();
	m_animBuffer.SetVertices(frame.GetVertices());
	m_animBuffer.GetFaces() = frame.GetFaces();
	m_animBuffer.GetNormals() = frame.GetNormals();
	m_animBuffer.Allocate(false);

	m_SetMeshBuffer(&m_animBuffer);
}

void MorphMeshNode::Play(bool loop)
{
	m_playing = true;
	m_loop = loop;
}

void MorphMeshNode::Play(MorphAnimation animation, bool loop)
{
	m_currentAnimation = animation;
	assert(animation.start < m_BufferCollection()->size() && animation.start >= 0);
	m_currentFrame = animation.start;
	m_nextFrame = (m_currentFrame < animation.end) ? m_currentFrame + 1 : animation.start;
	m_loop = loop;
	m_playing = true;
}

void MorphMeshNode::Pause()
{
	m_playing = false;
}

void MorphMeshNode::Stop()
{
	m_playing = false;
	m_currentFrame = m_currentAnimation.start;
}

//private
void MorphMeshNode::m_UpdateSelf(float dt)
{
	if (m_playing)
	{
		//update currently selected animation
		const float frameTime = 1.f / m_currentAnimation.fps;
		m_currentTime += dt;
		if (m_currentTime > frameTime)
		{
			//move to next frame
			if (m_currentFrame < m_currentAnimation.end)
			{
				m_currentFrame++;
			}
			else
			{
				if (m_loop)
					m_currentFrame = m_currentAnimation.start;
				else
					Stop();
			}

			m_nextFrame = m_currentFrame + 1;
			if (m_nextFrame > m_currentAnimation.end)
				m_nextFrame = m_currentAnimation.start;

			m_currentTime = 0.f;
		}

		//interpolate current values between frames
		auto& bc = *m_BufferCollection();
		auto& currentVerts = bc[m_currentFrame]->GetVertices();
		auto& nextVerts = bc[m_nextFrame]->GetVertices();

		MeshBuffer::Vertices& newVerts = m_animBuffer.GetVertices();
		auto s = currentVerts.size();
		const float time = m_currentTime / frameTime;
		for (auto i = 0u; i < s; ++i)
		{
			MeshBuffer::Vertex& v = newVerts[i];
			v = currentVerts[i];
			v.position = Helpers::Vectors::Lerp(currentVerts[i].position, nextVerts[i].position, time);
			v.normal = Helpers::Vectors::Lerp(currentVerts[i].normal, nextVerts[i].normal, time);
			v.tangent = Helpers::Vectors::Lerp(currentVerts[i].tangent, nextVerts[i].tangent, time);
			v.bitangent = Helpers::Vectors::Lerp(currentVerts[i].bitangent, nextVerts[i].bitangent, time);
		}
		m_animBuffer.Update();
	}
}

