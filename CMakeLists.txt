project(CHUF)
cmake_minimum_required(VERSION 2.8)
include_directories(${CMAKE_SOURCE_DIR}/include)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(pugi_HDRS
        include/pugixml/pugiconfig.hpp
        include/pugixml/pugixml.hpp)

set(pugi_SRCS
        src/pugixml/pugixml.cpp)


#set(GLOB_RECURSE box2d_HDRS include/Box2D/*.h)

set(box2d_HDRS
	include/Box2D/Collision/Shapes/b2ChainShape.h
	include/Box2D/Collision/Shapes/b2CircleShape.h
	include/Box2D/Collision/Shapes/b2EdgeShape.h
        include/Box2D/Collision/Shapes/b2PolygonShape.h
	include/Box2D/Collision/Shapes/b2Shape.h
	include/Box2D/Collision/b2BroadPhase.h
	include/Box2D/Collision/b2Collision.h
	include/Box2D/Collision/b2Distance.h
	include/Box2D/Collision/b2DynamicTree.h
	include/Box2D/Collision/b2TimeOfImpact.h
	include/Box2D/Common/b2BlockAllocator.h
	include/Box2D/Common/b2Draw.h
	include/Box2D/Common/b2GrowableStack.h
	include/Box2D/Common/b2Math.h
	include/Box2D/Common/b2Settings.h
	include/Box2D/Common/b2StackAllocator.h
	include/Box2D/Common/b2Timer.h
	include/Box2D/Dynamics/Contacts/b2ChainAndCircleContact.h
	include/Box2D/Dynamics/Contacts/b2ChainAndPolygonContact.h
	include/Box2D/Dynamics/Contacts/b2CircleContact.h
	include/Box2D/Dynamics/Contacts/b2Contact.h
	include/Box2D/Dynamics/Contacts/b2ContactSolver.h
	include/Box2D/Dynamics/Contacts/b2EdgeAndCircleContact.h
	include/Box2D/Dynamics/Contacts/b2EdgeAndPolygonContact.h
	include/Box2D/Dynamics/Contacts/b2PolygonAndCircleContact.h
	include/Box2D/Dynamics/Contacts/b2PolygonContact.h
	include/Box2D/Dynamics/Joints/b2DistanceJoint.h
	include/Box2D/Dynamics/Joints/b2FrictionJoint.h
	include/Box2D/Dynamics/Joints/b2GearJoint.h
	include/Box2D/Dynamics/Joints/b2Joint.h
	include/Box2D/Dynamics/Joints/b2MotorJoint.h
	include/Box2D/Dynamics/Joints/b2MouseJoint.h
	include/Box2D/Dynamics/Joints/b2PrismaticJoint.h
	include/Box2D/Dynamics/Joints/b2PulleyJoint.h
	include/Box2D/Dynamics/Joints/b2RevoluteJoint.h
	include/Box2D/Dynamics/Joints/b2RopeJoint.h
	include/Box2D/Dynamics/Joints/b2WeldJoint.h
	include/Box2D/Dynamics/Joints/b2WheelJoint.h
	include/Box2D/Dynamics/b2Body.h
	include/Box2D/Dynamics/b2ContactManager.h
	include/Box2D/Dynamics/b2Fixture.h
	include/Box2D/Dynamics/b2Island.h
	include/Box2D/Dynamics/b2TimeStep.h
	include/Box2D/Dynamics/b2World.h
	include/Box2D/Dynamics/b2WorldCallbacks.h
	include/Box2D/Rope/b2Rope.h
        include/Box2D/Box2D.h)

set(box2d_SRCS
	src/Box2D/Collision/Shapes/b2ChainShape.cpp
        src/Box2D/Collision/Shapes/b2CircleShape.cpp
        src/Box2D/Collision/Shapes/b2EdgeShape.cpp
        src/Box2D/Collision/Shapes/b2PolygonShape.cpp
        src/Box2D/Collision/b2BroadPhase.cpp
        src/Box2D/Collision/b2CollideCircle.cpp
        src/Box2D/Collision/b2CollideEdge.cpp
        src/Box2D/Collision/b2CollidePolygon.cpp
        src/Box2D/Collision/b2Collision.cpp
        src/Box2D/Collision/b2Distance.cpp
        src/Box2D/Collision/b2DynamicTree.cpp
        src/Box2D/Collision/b2TimeOfImpact.cpp
        src/Box2D/Common/b2BlockAllocator.cpp
        src/Box2D/Common/b2Draw.cpp
        src/Box2D/Common/b2Math.cpp
        src/Box2D/Common/b2Settings.cpp
        src/Box2D/Common/b2StackAllocator.cpp
        src/Box2D/Common/b2Timer.cpp
        src/Box2D/Dynamics/Contacts/b2ChainAndCircleContact.cpp
        src/Box2D/Dynamics/Contacts/b2ChainAndPolygonContact.cpp
        src/Box2D/Dynamics/Contacts/b2CircleContact.cpp
        src/Box2D/Dynamics/Contacts/b2Contact.cpp
        src/Box2D/Dynamics/Contacts/b2ContactSolver.cpp
        src/Box2D/Dynamics/Contacts/b2EdgeAndCircleContact.cpp
        src/Box2D/Dynamics/Contacts/b2EdgeAndPolygonContact.cpp
        src/Box2D/Dynamics/Contacts/b2PolygonAndCircleContact.cpp
        src/Box2D/Dynamics/Contacts/b2PolygonContact.cpp
        src/Box2D/Dynamics/Joints/b2DistanceJoint.cpp
        src/Box2D/Dynamics/Joints/b2FrictionJoint.cpp
        src/Box2D/Dynamics/Joints/b2GearJoint.cpp
        src/Box2D/Dynamics/Joints/b2Joint.cpp
        src/Box2D/Dynamics/Joints/b2MotorJoint.cpp
        src/Box2D/Dynamics/Joints/b2MouseJoint.cpp
        src/Box2D/Dynamics/Joints/b2PrismaticJoint.cpp
        src/Box2D/Dynamics/Joints/b2PulleyJoint.cpp
        src/Box2D/Dynamics/Joints/b2RevoluteJoint.cpp
        src/Box2D/Dynamics/Joints/b2RopeJoint.cpp
        src/Box2D/Dynamics/Joints/b2WeldJoint.cpp
        src/Box2D/Dynamics/Joints/b2WheelJoint.cpp
        src/Box2D/Dynamics/b2Body.cpp
        src/Box2D/Dynamics/b2ContactManager.cpp
        src/Box2D/Dynamics/b2Fixture.cpp
        src/Box2D/Dynamics/b2Island.cpp
        src/Box2D/Dynamics/b2World.cpp
        src/Box2D/Dynamics/b2WorldCallbacks.cpp
        src/Box2D/Rope/b2Rope.cpp)


set(meshlib_HDRS
	include/Mesh/GL/GLCheck.hpp
	include/Mesh/Shaders/DepthMap.h
	include/Mesh/Shaders/Normal.h
	include/Mesh/Shaders/Phong.h
	include/Mesh/Shaders/Shadow.h
	include/Mesh/Camera.h
	include/Mesh/FpsCamera.h
	include/Mesh/Light.h
	include/Mesh/Material.h
	include/Mesh/MD2Parser.h
	include/Mesh/MD5Parser.h
	include/Mesh/MeshBuffer.h
	include/Mesh/MeshHelpers.h
	include/Mesh/MeshLib.h
	include/Mesh/MeshNode.h
	include/Mesh/MeshScene.h
	include/Mesh/MorphMeshNode.h
	include/Mesh/Node.h
	include/Mesh/ObjParser.h
	include/Mesh/Resources.h
	include/Mesh/ShaderProgram.h
	include/Mesh/SkeletalAnimation.h
	include/Mesh/SkeletalMeshNode.h
	include/Mesh/Skeleton.h
	include/Mesh/StaticMeshNode.h)
	
set(meshlib_SRCS
	src/Mesh/AnimationResource.cpp
	src/Mesh/Camera.cpp
	src/Mesh/FpsCamera.cpp
	src/Mesh/Light.cpp
	src/Mesh/Material.cpp
	src/Mesh/MD2Parser.cpp
	src/Mesh/MD5Anim.cpp
	src/Mesh/MD5Parser.cpp
	src/Mesh/MeshBuffer.cpp
	src/Mesh/MeshNode.cpp
	src/Mesh/MeshResource.cpp
	src/Mesh/MeshScene.cpp
	src/Mesh/MorphMeshNode.cpp
	src/Mesh/Node.cpp
	src/Mesh/ObjParser.cpp
	src/Mesh/ShaderProgram.cpp
	src/Mesh/SkeletalAnimation.cpp
	src/Mesh/SkeletalMeshNode.cpp
	src/Mesh/StaticMeshNode.cpp)



add_library(pugi SHARED ${pugi_HDRS} ${pugi_SRCS})
add_library(box2d SHARED ${box2d_HDRS} ${box2d_SRCS})
add_library(meshlib SHARED ${meshlib_HDRS} ${meshlib_SRCS})

#install(TARGETS pugi ${PROJECT_NAME} LIBRARY DESTINATION lib)
#install(TARGETS box2d ${PROJECT_NAME} LIBRARY DESTINATION lib)
#install(TARGETS meshlib ${PROJECT_NAME} LIBRARY DESTINATION lib)

install(FILES ${box2d_HDRS} DESTINATION include/box2d)
install(FILES ${pugi_HDRS} DESTINATION include/pugixml)
install(FILES ${meshlib_HDRS} DESTINATION include/meshlib)


#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
find_package(SFML 2 REQUIRED system window graphics audio)
find_package(ZLIB REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLM REQUIRED)

include_directories(${OPENGL_INCLUDE_DIRS})
include_directories(${GLEW_INCLUDE_DIRS})
include_directories(${GLM_INCLUDE_DIRS})

include_directories(${CMAKE_SOURCE_DIR}/include/Game)

add_executable(
	CHUF_bin src/Main.cpp
	src/AnimatedSprite.cpp
	src/AudioManager.cpp
	src/BaseParticle.cpp
	src/BaseScreen.cpp
	src/Behavior.cpp
	src/CameraFactory.cpp
	src/Canvas.cpp
	src/Component.cpp
	src/Console.cpp
	src/ContactListener.cpp
	src/DebugShape.cpp
	src/Deletable.cpp
	src/FontResource.cpp
	src/GameScreen.cpp
	src/Health.cpp
	src/MapLayer.cpp
	src/MapLoaderPrivate.cpp
	src/MapLoaderPublic.cpp
	src/MapObject.cpp
	src/ParticleFactory.cpp
	src/PhysEntity.cpp
	src/QuadTreeNode.cpp
	src/Random.cpp
	src/SceneNode.cpp
	src/ShaderManager.cpp
	src/SmokeParticle.cpp
	src/StartScreen.cpp
	src/tmx2box2d.cpp
	src/UIButton.cpp
	src/UICheckBox.cpp
	src/UIComboBox.cpp
	src/UIComponent.cpp
	src/UIContainer.cpp
	src/UIEventHandlers.cpp
	src/UILabel.cpp
	src/UIScrollSprite.cpp
	src/UISlider.cpp
	src/UITextBox.cpp
	src/ViewCamera.cpp)
	
	
target_link_libraries(CHUF_bin box2d)
target_link_libraries(CHUF_bin pugi)
target_link_libraries(CHUF_bin meshlib)
target_link_libraries(CHUF_bin ${SFML_LIBRARIES})
target_link_libraries(CHUF_bin ${ZLIB_LIBRARIES})

		

