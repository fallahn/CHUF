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

/*parses the mesh info of the Doom 3 md5 mesh format*/
/*Reference: http://3dgep.com/?p=1053 */
/*and http://tfc.duke.free.fr/coding/md5-specs-en.html */

#include <Mesh/MD5Parser.h>

#include <Helpers.h>

#include <fstream>
#include <iostream>
#include <cassert>
#include <string>

using namespace ml;


bool MD5Parser::LoadAnimation(const std::string& path, SkeletalAnimation& destination)
{
	std::fstream file(path.c_str());
	if (file.fail())
	{
		std::cerr << "Could not open animation file: " << path << std::endl;
		return false;
	}

	file.seekg(0, file.end);
	sf::Int32 fileLength = static_cast<sf::Int32>(file.tellg());
	assert(fileLength > 0);
	file.seekg(0, file.beg);

	std::string param, junk;
	AnimHeader header;

	JointInfoList& jointInfo = destination.GetJointInfo();
	BoundList bounds;
	FrameDataList frames;
	BaseFrameList baseFrames;

	file >> param;

	while (!file.eof())
	{
		if (param == "MD5Version")
		{
			file >> header.version;
			assert (header.version == 10);
		}
		else if (param == "commandline")
		{
			file.ignore(fileLength, '\n');
		}
		else if (param == "numFrames")
		{
			file >> header.numFrames;
			file.ignore(fileLength, '\n');
		}
		else if (param == "numJoints")
		{
			file >> header.numJoints;
			file.ignore(fileLength, '\n');
		}
		else if (param == "frameRate")
		{
			file >> header.framerate;
			destination.SetFrameRate(header.framerate);
			file.ignore(fileLength, '\n');
		}
		else if (param == "numAnimatedComponents")
		{
			file >> header.numAnimComponents;
			file.ignore(fileLength, '\n');
		}
		else if (param == "hierarchy")
		{
			file >> junk;
			for (auto i = 0u; i < header.numJoints; ++i)
			{
				JointInfo joint;
				file >> joint.name >> joint.parentID >> joint.flags >> joint.startIndex;
				Helpers::String::RemoveChar(joint.name, '\"');

				jointInfo.push_back(joint);
				file.ignore(fileLength, '\n');
			}
			file >> junk;
		}
		else if (param == "bounds")
		{
			file >> junk;
			file.ignore(fileLength, '\n');
			for (auto i = 0u; i < header.numFrames; ++i)
			{
				Bound bound;
				file >> junk;
				file >> bound.min.x >> bound.min.y >> bound.min.z;
				file >> junk >> junk;
				file >> bound.max.x >> bound.max.y >> bound.max.z;
				bounds.push_back(bound);
				file.ignore(fileLength, '\n');
			}
			file >> junk;
			file.ignore(fileLength, '\n');
		}
		else if (param == "baseframe")
		{
			file >> junk;
			file.ignore(fileLength, '\n');

			for (auto i = 0u; i < header.numJoints; ++i)
			{
				BaseFrame baseFrame;
				file >> junk;
				file >> baseFrame.position.x >> baseFrame.position.y >> baseFrame.position.z;
				file >> junk >> junk;
				file >> baseFrame.rotation.x >> baseFrame.rotation.y >> baseFrame.rotation.z;
				file.ignore(fileLength, '\n');

				baseFrames.push_back(baseFrame);
			}

			file >> junk;
			file.ignore(fileLength, '\n');
		}
		else if (param == "frame")
		{
			FrameData frame;
			file >> frame.frameID >> junk;
			file.ignore(fileLength, '\n');

			for (auto i = 0u; i < header.numAnimComponents; ++i)
			{
				float frameData;
				file >> frameData;
				frame.frameData.push_back(frameData);
			}
			frames.push_back(frame);

			destination.AddFrame(m_BuildFrame(jointInfo, baseFrames, frame));

			file >> junk;
			file.ignore(fileLength, '\n');
		}
		file >> param;
	}

	return true;
}

SkeletonPtr MD5Parser::m_BuildFrame(const JointInfoList& ji, const BaseFrameList& bf, const FrameData& frame)
{
	SkeletonPtr skeleton(new Skeleton);// = std::make_unique<Skeleton>(); //not supported by gcc
	auto s = ji.size();
	for (auto i = 0u; i < s; ++i)
	{
		auto j = 0u;
		const auto& info = ji[i];
		SkeletonJoint animatedJoint = bf[i];
		animatedJoint.parent = info.parentID;
		//position
		if (info.flags & 1)
			animatedJoint.position.x = frame.frameData[info.startIndex + j++];
		if (info.flags & 2)
			animatedJoint.position.y = frame.frameData[info.startIndex + j++];
		if (info.flags & 4)
			animatedJoint.position.z = frame.frameData[info.startIndex + j++];
		//rotation
		if (info.flags & 8)
			animatedJoint.rotation.x = frame.frameData[info.startIndex + j++];
		if (info.flags & 16)
			animatedJoint.rotation.y = frame.frameData[info.startIndex + j++];
		if (info.flags & 32)
			animatedJoint.rotation.z = frame.frameData[info.startIndex + j++];
		m_ComputeQuatW(animatedJoint.rotation);

		if (animatedJoint.parent >= 0)
		{
			SkeletonJoint& parentJoint = skeleton->joints[animatedJoint.parent];
			glm::vec3 rotPos = parentJoint.rotation * animatedJoint.position;

			animatedJoint.position = parentJoint.position + rotPos;
			animatedJoint.rotation = parentJoint.rotation * animatedJoint.rotation;

			animatedJoint.rotation = glm::normalize(animatedJoint.rotation);
		}

		skeleton->joints.push_back(animatedJoint);
	}

	return std::move(skeleton);
}
