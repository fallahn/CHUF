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

///definitions for random fucntions///
#include <Helpers.h>
#include <random>
#include <ctime>
#include <cassert>

namespace Helpers
{
	namespace Random
	{
		std::default_random_engine Engine(static_cast<unsigned long>(std::time(0)));
	};
};

const float Helpers::Random::Float(float begin, float end)
{
	assert(begin < end);
	std::uniform_real_distribution<float> dist(begin, end);
	return dist(Engine);
}

const int Helpers::Random::Int(int begin, int end)
{
	assert(begin < end);
	std::uniform_int_distribution<int> dist(begin, end);
	return dist(Engine);
}