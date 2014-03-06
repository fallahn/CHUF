#ifndef SHADOW_H_
#define SHADOW_H_

//projects shadow map into camera space

const char shadowVert[] =
"#version 120\n"

"varying vec4 shadowCoord;"
"varying vec3 lightDir;"
"varying vec3 normalDir;"
"varying vec4 worldSpace;"

"uniform mat4 depthMVP;"
"uniform mat4 depthM;"
"uniform mat4 depthV;"
"uniform vec3 lightPos;"

"void main()"
"{"
"	shadowCoord = depthMVP * gl_Vertex;"
"	gl_Position = ftransform();"

"	lightDir = (depthV * vec4(lightPos, 0.0)).xyz;"
"	normalDir = (depthV * depthM * vec4(gl_Normal, 0.0)).xyz;"

"	worldSpace = depthM * gl_Vertex;"
"}";

const char shadowFrag[] =
"#version 120\n"
"varying vec4 shadowCoord;"
"varying vec3 lightDir;"
"varying vec3 normalDir;"
"varying vec4 worldSpace;"

"uniform sampler2DShadow shadowMap;"
"uniform vec2 mapScale = vec2(1.0 / 800, 1.0 / 600.0);"

"void main()"
"{"

"	vec3 n = normalize(normalDir);"
"	vec3 l = normalize(lightDir);"

"	float bias = 0.0005 * tan(acos(clamp(dot(n, l), 0.0, 1.0)));"
//------------------------------------Comparison version------------------------------------
//"	float value = 1.0;"
//"	if(shadowCoord.x >= 0.0 && shadowCoord.x <= 1.0 && shadowCoord.y >= 0.0 && shadowCoord.y <= 1.0 && shadowCoord.w > 0.0)"
//"	{"
//"		value = shadow2D(shadowMap, shadowCoord.xyz).x < ((shadowCoord.z /*/ shadowCoord.w*/) + bias) ? 0.6 : 1.0;"
//"	}"
//" gl_FragColor = vec4(vec3(value), 1.0);"

//------------------------------------------------------------------------------------------
"	vec4 sum = vec4(0.0);"
"	if(shadowCoord.x >= 0.0 && shadowCoord.x <= 1.0 && shadowCoord.y >= 0.0 && shadowCoord.y <= 1.0 && shadowCoord.w > 0.0)"
"	{"
"	bias = clamp(bias, 0.0, 0.01);"
//----------------------------------PCF version 16x-----------------------------------------
//"		float x, y;"
//"		const float dist = 1.5;"
//"		const float step = 1.0;"
//"		for (y = -dist; y <= dist; y += step)"
//"		{"
//"			for (x = -dist; x <= dist; x += step)"
//"			{"
//"				sum += shadow2D(shadowMap, vec3(shadowCoord.xy + vec2(x, y) * mapScale * shadowCoord.w, shadowCoord.z + bias));"
//"			}"
//"		}"
//"		sum /= 16.0;"
//---------------------------------PCF version 4x-------------------------------------------
"	vec2 offset;"
"	offset.x = mod(worldSpace.x, 2.0);"
"	offset.y = mod(worldSpace.y, 2.0);"
"	offset.y += offset.x;"
"	if(offset.y > 1.1) offset.y = 0.0;"

"	sum = (shadow2D(shadowMap, vec3(shadowCoord.xy + (offset + vec2(-3.0, 1.0)) * mapScale * shadowCoord.w, shadowCoord.z + bias)) +"
"		shadow2D(shadowMap, vec3(shadowCoord.xy + (offset + vec2(1.0, 1.0)) * mapScale * shadowCoord.w, shadowCoord.z + bias)) +"
"		shadow2D(shadowMap, vec3(shadowCoord.xy + (offset + vec2(-3.0, -3.0)) * mapScale * shadowCoord.w, shadowCoord.z + bias)) +"
"		shadow2D(shadowMap, vec3(shadowCoord.xy + (offset + vec2(1.0, -3.0)) * mapScale * shadowCoord.w, shadowCoord.z + bias))) * 0.25;"
//------------------------------------------------------------------------------------------
"	}"
"	else"
"	{"
"		sum = vec4(1.0);"
"	}"
"	gl_FragColor = vec4(clamp(sum.rgb + 0.4, 0.0, 1.0), 1.0);"
//------------------------------------------------------------------------------------------

"}";

#endif