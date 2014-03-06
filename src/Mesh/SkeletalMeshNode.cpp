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

#include <Mesh/SkeletalMeshNode.h>

#include <cassert>
#include <iostream>

using namespace ml;

//ctor
SkeletalMeshNode::SkeletalMeshNode()
: m_anim	(nullptr),
m_jointList	(nullptr)
{}

SkeletalMeshNode::SkeletalMeshNode(BufferCollection& bc)
: m_anim	(nullptr),
m_jointList	(nullptr)
{
	SetBufferCollection(bc);
}

//public
void SkeletalMeshNode::SetBufferCollection(BufferCollection& bc)
{
	assert(bc.size());
	m_defaultBuffers = &bc;
	m_jointList = bc[0]->GetJoints().get();

	//copy mesh data to animatable buffer
	m_animatedBuffers.clear();
	for (auto& b : bc)
	{
		BufferPtr mb = std::make_shared<MeshBuffer>();
		mb->SetVertices(b->GetVertices());
		mb->GetFaces() = b->GetFaces();
		mb->GetNormals() = b->GetNormals();
		mb->SetJoints(b->GetJoints());
		mb->GetWeights() = b->GetWeights();
		mb->Allocate(false);
		m_animatedBuffers.push_back(mb);
	}
	m_SetBufferCollection(&m_animatedBuffers);

	m_CreateBindPose();
}

void SkeletalMeshNode::Play(bool loop)
{
	assert(m_anim);
	m_anim->Play(loop);
}

void SkeletalMeshNode::Play(SkeletalAnimation& anim, bool loop)
{
	assert(anim.CheckJoints(*m_jointList));
	m_anim = &anim;
	m_anim->Play(loop);
}

void SkeletalMeshNode::Pause()
{
	assert(m_anim);
	m_anim->Pause();
}

void SkeletalMeshNode::Stop()
{
	assert(m_anim);
	m_anim->Stop();
}

//private
void SkeletalMeshNode::m_CreateBindPose()
{
	//create default skeleton
	for (auto& j : *m_jointList)
		m_bindPose.joints.push_back(SkeletonJoint(j));

	for (auto& m : m_animatedBuffers)
		m_ApplyBoneWeights(m_bindPose);

	//create bounds for bind pose
	float minX = 0.f;
	float minY = 0.f;
	float minZ = 0.f;
	float maxX = 0.f;
	float maxY = 0.f;
	float maxZ = 0.f;

	const auto& buffers = *m_defaultBuffers;
	for (const auto& b : buffers)
	{
		const auto& verts = b->GetVertices();
		for (const auto& v : verts)
		{
			if (v.position.x < minX) minX = v.position.x;
			else if (v.position.x > maxX) maxX = v.position.x;

			if (v.position.y < minY) minY = v.position.y;
			else if (v.position.y > maxY) maxY = v.position.y;

			if (v.position.z < minZ) minZ = v.position.z;
			else if (v.position.z > maxZ) maxZ = v.position.z;
		}
	}
	std::array<glm::vec4, 8u>& boundBox = m_GetBoundingBox();
	boundBox[0] = glm::vec4(minX, minY, minZ, 0.f);
	boundBox[1] = glm::vec4(maxX, minY, minZ, 0.f);
	boundBox[2] = glm::vec4(maxX, minY, maxZ, 0.f);
	boundBox[3] = glm::vec4(minX, minY, maxZ, 0.f);
	boundBox[4] = glm::vec4(minX, maxY, minZ, 0.f);
	boundBox[5] = glm::vec4(maxX, maxY, minZ, 0.f);
	boundBox[6] = glm::vec4(maxX, maxY, maxZ, 0.f);
	boundBox[7] = glm::vec4(minX, maxY, maxZ, 0.f);
}

void SkeletalMeshNode::m_ApplyBoneWeights(const Skeleton& skeleton)
{
	auto s = m_defaultBuffers->size();
	const auto& sourceBuffers = *m_defaultBuffers;

	for (auto i = 0u; i < s; ++i)
	{
		auto& verts = m_animatedBuffers[i]->GetVertices();
		const auto& weights = m_animatedBuffers[i]->GetWeights();

		const auto& normals = sourceBuffers[i]->GetVertices();
		auto vc = verts.size();

		for (auto v = 0u; v < vc; ++v)
		{
			verts[v].position = glm::vec3(0);
			verts[v].normal = glm::vec3(0);
			verts[v].tangent = glm::vec3(0);
			verts[v].bitangent = glm::vec3(0);

			for (auto j = 0u; j < verts[v].weightCount; ++j)
			{
				const auto& weight = weights[verts[v].startWeight + j];
				const auto& joint = skeleton.joints[weight.jointId];

				const glm::vec3 rotPos = joint.rotation * weight.position;
				verts[v].position += (joint.position + rotPos) * weight.bias;

				//this is a bottleneck on large meshes
				//is this where hardware skinning will come to play?
				verts[v].normal += (joint.rotation * normals[v].normal) * weight.bias;
				verts[v].tangent += (joint.rotation * normals[v].tangent) * weight.bias;
				verts[v].bitangent += (joint.rotation * normals[v].bitangent) * weight.bias;
			}
		}
		m_animatedBuffers[i]->Update();
	}
}

void SkeletalMeshNode::m_UpdateSelf(float dt)
{
	//update animation
	if (m_anim)
	{
		m_anim->Update(dt);
		m_ApplyBoneWeights(m_anim->GetSkeleton());
	}
}

void SkeletalMeshNode::m_DrawDebug()
{
	glPointSize(5.0f);
	glColor3f(1.0f, 0.0f, 1.0f);

	glPushAttrib(GL_ENABLE_BIT);

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	SkeletonJointList jList = (m_anim) ? m_anim->GetSkeleton().joints : m_bindPose.joints;

	// Draw the joint positions
	glBegin(GL_POINTS);
	{
		for (const auto& j : jList)
		{
			const std::array<float, 3u> pos = { j.position.x, j.position.y, j.position.z };
			glVertex3fv(&pos[0]);
		}
		/*glColor3f(1.f, 1.f, 0.f);
		const auto& weights = m_MeshBuffer()->GetWeights();
		for (const auto& w : weights)
		{
			const std::array<float, 3u> pos = {w.position.x, w.position.y, w.position.z};
			glVertex3fv(&pos[0]);
		}*/
	}
	glEnd();

	// Draw the bones
	glColor3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_LINES);
	{
		for (const auto& j : jList)
		{
			if (j.parent != -1)
			{
				const SkeletonJoint& j1 = jList[j.parent];
				const std::array<float, 3u> pos = { j.position.x, j.position.y, j.position.z };
				const std::array<float, 3u> pos1 = { j1.position.x, j1.position.y, j1.position.z };
				glVertex3fv(&pos[0]);
				glVertex3fv(&pos1[0]);
			}
		}
	}
	glEnd();

	glEnable(GL_DEPTH_TEST);

	//bounding box
	glColor3f(1.f, 0.f, 0.f);
	glBegin(GL_LINE_STRIP);
	
	std::array<glm::vec4, 8u>& box = m_GetBoundingBox();
	for (const auto& v : box)
	{
		std::array<float, 3u> a = { v.x, v.y, v.z };
		glVertex3fv(&a[0]);
	}

	glEnd();

	glEnable(GL_LIGHTING);
	

	glPopAttrib();
}