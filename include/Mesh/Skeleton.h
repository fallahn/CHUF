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

/*Used to build a skeleton for a single frame of skeletal animation*/

#ifndef SKELETON_H_
#define SKELETON_H_

#include <SFML/System/Vector3.hpp>
#include <SFML/System.hpp>

#include <Mesh/MeshBuffer.h>

#include <glm/gtc/quaternion.hpp>

#include <vector>
#include <memory>

namespace ml
{
	//----used by animation parsers to build a frame-------
	struct JointInfo
	{
		std::string name;
		sf::Int32   parentID;
		sf::Int32   flags;
		sf::Int32   startIndex;
	};
	typedef std::vector<JointInfo> JointInfoList;

	struct Bound
	{
		glm::vec3   min;
		glm::vec3   max;
	};
	typedef std::vector<Bound> BoundList;

	struct BaseFrame
	{
		glm::vec3   position;
		glm::quat   rotation;
	};
	typedef std::vector<BaseFrame> BaseFrameList;

	struct FrameData
	{
		sf::Int32 frameID;
		std::vector<float> frameData;
	};
	typedef std::vector<FrameData> FrameDataList;



	//----------Used to definine a skeletal animation frame-------------
	struct SkeletonJoint
	{
		SkeletonJoint()
			: parent(-1)
		{}

		SkeletonJoint(const BaseFrame& frame)
			: parent	(-1),
			position	(frame.position),
			rotation	(frame.rotation)
		{}

		SkeletonJoint(const MeshBuffer::Joint& joint)
			: parent	(joint.parentId),
			position	(joint.position.x, joint.position.y, joint.position.z),
			rotation	(joint.rotation)
		{}

		sf::Int32 parent;
		glm::vec3 position;
		glm::quat rotation;
	};
	typedef std::vector<SkeletonJoint> SkeletonJointList;

	//contains the list of skeleton joints which make a frame
	struct Skeleton
	{
		SkeletonJointList joints;
	};
	typedef std::unique_ptr<Skeleton> SkeletonPtr;
	typedef std::vector<SkeletonPtr> SkeletonList;
}

#endif