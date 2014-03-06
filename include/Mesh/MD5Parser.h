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

#ifndef MD5_PARSER_H_
#define MD5_PARSER_H_

#include <Mesh/MeshBuffer.h>
#include <Mesh/SkeletalAnimation.h>

#include <SFML/System/NonCopyable.hpp>

namespace ml
{	
	class MD5Parser final : private sf::NonCopyable
	{
	public:
		static bool LoadFromFile(const std::string& path, BufferCollection& destination);
		static bool LoadAnimation(const std::string& path, SkeletalAnimation& destination);

	private:
		struct MeshHeader
		{
			MeshHeader()
				: version			(0),
				numJoints			(0),
				numMeshes			(0){}
			sf::Uint32 version;
			sf::Uint32 numJoints;
			sf::Uint32 numMeshes;
		};
		struct AnimHeader
		{
			AnimHeader()
				: version			(0),
				numFrames			(0),
				numJoints			(0),
				numAnimComponents	(0),
				framerate			(0.f){}
			sf::Uint32 version;
			sf::Uint32 numFrames;
			sf::Uint32 numJoints;
			sf::Uint32 numAnimComponents;
			float framerate;
		};

		//assumes quaternion is of unit length
		static void m_ComputeQuatW(glm::quat& quat);
		static void m_ApplyBoneWeights(MeshBuffer& buffer);
		static void m_CalcNormals(MeshBuffer& mesh);
		static SkeletonPtr m_BuildFrame(const JointInfoList& ji, const BaseFrameList& bf, const FrameData& frame);
	};
}

#endif //MD5_PARSER_H_