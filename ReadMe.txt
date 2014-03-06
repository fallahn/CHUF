CHUF - Carefully Handcrafted Unnamed Framework
March 2014


What is this?
-------------

This, as yet, unnamed piece of software is a framework designed for building games.
It came about after years of developing small projects, where new features from a project
looked like they could lend themselves to a reusable framework. Each new project was
forked from this code, and any new features were rolled back into it if I felt it could
be useful for other projects in the future. This code has been made public in the hope
that others may find it useful, if not for development then perhaps for education. It
is written in C++ and makes extensive use of C++11 features.


What does it do?
----------------

CHUF provides the building blocks for 2D and 2.5D games. At it's heart is SFML, which
provides all the interfacing for cross platform multimedia accessability. It can do
anything SFML can do. This framework provides higher level abstract classes many of which
are based on resources published by third parties, least of all the SFML Game Development
book. There is a scene node graph which provides a hierachy of composable entities - entities
which can be built from components providing physics, AI, audio, input, particle systems and
more. The component system is designed to be extensible so that custom components can quickly
be derived from existing types, or whole new types added from scratch. CHUF also provides
direct access to OpenGL and supplies classes for importing and rendering 3D meshes,
which can be static, morph animated or skeletally animated. The 3D components of CHUF are
designed specifically to interface with the 2D world of SFML, overlaying the 3D images on a 
2D scene hence providing the 2.5D capability. The 3D system can be built independently and 
used as a library with other SFML based projects, and is designed so that other mesh formats
can be easily imported using custom import classes written to parse file formats of your
choice. CHUF uses the Tiled tmx map format for 2D tile maps, and the included parser exposes
all the current features of Tiled maps, as well as providing utilities for parsing object data
into Box2D physics bodies - the library used by CHUF for physics and collision detection.
Resources are managed in CHUF with extensible resource managers which have the option of
integrating PhysFS for archived assets. There is also a Quake style command console available
which provides an interface for quickly and easily adding custom commands leveraging the
power of lambda expressions in C++11. CHUF also has basic state stack and game loop
functionality ready to be extended to the requirements of any project. For menuing CHUF has
a UI system with classes providing the most commonly used components - buttons, sliders,
combo boxes etc. and an interface for easily creating more.


What doesn't it do?
-------------------

There are many features not provided either because they are out of the scope of the framework
or simply because I have not yet needed them in any particular project. The most noteworthy of
these is the lack of any networking capability outside of that provided by SFML. This doesn't
mean that things will not change in the future, however. There is also currently limited
documentation.


What does it depend on?
-----------------------

Besides SFML CHUF also requires:
zlib - used by the tile map parser
glew - used by the 3D scene graph
box2D - required for physics
PhysFS (optional) - used by resource managers when opening archived resources.


How do I get started?
---------------------

For now there's not much here other than the source. Adding all the files to your project
is a good diving in point. By default the CHUF source builds an executable with two states
 on the stack representing the 'start' screen and the 'game' screen, so when you see these
you're on the right track. CHUF is cross platform, developed mainly on Windows with Visual
Studio 2013. It should compile with gcc on both Windows and linux although there are likely
to be a lot of warnings. Decent C++11 support is essential. At the very least I plan to
provide a CMake file to help get things up and running more quickly. Currently I'm assuming
anyone reading this has no problem building and linking their own projects - I'll not be 
providing a huge amount of support as this is just a personal project of mine which I've
decided to share with the wider community.




Some links:
-----------

SFML	http://sfml-dev.org/
Box2D 	http://box2d.org/
Tiled	http://www.mapeditor.org/
tmx map parser used by this framework https://github.com/fallahn/sfml-tmxloader

Projects and examples built on past versions of this framework:

http://www.youtube.com/watch?v=_vd-gULMq0U
http://www.youtube.com/watch?v=L61ssYvp2Bg
http://www.youtube.com/watch?v=nFG4K__D6k8
http://www.youtube.com/watch?v=bhiYif8WKu0
http://www.youtube.com/watch?v=3EukqhrU8ms
http://www.youtube.com/watch?v=_OeJcrWU4Uw


License
-------
This software is released under the liberal zlib license:

/*********************************************************************
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



Credits:
--------

Many people have helped me during the development of
CHUF, particularly those of the SFML community (in no order):

The authors of the SFML book (Nexus, Groogy, Grimshaw), eXpl0it3r,
Tank and Laurent (and everyone else!)

as well as anyone who has ever taken the time to publish anything
on the internet about game programming. Thanks!