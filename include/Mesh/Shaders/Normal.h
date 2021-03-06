///normal map shader///
#ifndef NORMAL_MAPPING_H_
#define NORMAL_MAPPING_H_

const char normalVert[] =
"#version 120\n"

//---normal mapping---
"varying vec3 lightVec;"
"varying vec3 eyeVec;"

"attribute vec3 tangent;"
"attribute vec3 bitangent;"

//---cube mapping----
"uniform mat4 modelView4x4;"
"uniform vec3 cameraPos = vec3(0.0);"

"mat3 GetLinearPart(mat4 m)"
"{"
"	mat3 result;"

"	result[0][0] = m[0][0];"
"	result[0][1] = m[0][1];"
"	result[0][2] = m[0][2];"

"	result[1][0] = m[1][0];"
"	result[1][1] = m[1][1];"
"	result[1][2] = m[1][2];"

"	result[2][0] = m[2][0];"
"	result[2][1] = m[2][1];"
"	result[2][2] = m[2][2];"

"	return result;"
"}"

"void main(void)"
"{"
"	gl_Position = ftransform();"
"	gl_TexCoord[0] = (gl_TextureMatrix[0] * gl_MultiTexCoord0);"//.xy;

//----normal mapping calc--------------------//
"	vec3 n = normalize(gl_NormalMatrix * gl_Normal);"
"	vec3 t = normalize(gl_NormalMatrix * tangent);"
"	vec3 b = normalize(gl_NormalMatrix * bitangent);"

"	vec3 vVertex = vec3(gl_ModelViewMatrix * gl_Vertex);"
"	vec3 tmpVec = gl_LightSource[0].position.xyz - vVertex;"

"	lightVec.x = dot(tmpVec, t);"
"	lightVec.y = dot(tmpVec, b);"
"	lightVec.z = dot(tmpVec, n);"
"	lightVec = normalize(lightVec);"

"	tmpVec = -vVertex;"
"	eyeVec.x = dot(tmpVec, t);"
"	eyeVec.y = dot(tmpVec, b);"
"	eyeVec.z = dot(tmpVec, n);"
"	eyeVec = normalize(eyeVec);"
	
	//----tex coords for reflect map-----//
	"mat3 modelView3x3 = GetLinearPart(modelView4x4);"	
	//find world space position.
	"vec4 worldPos = modelView4x4 * gl_Vertex;"		
	//find world space normal.
	"vec3 N = normalize(modelView3x3 * gl_Normal);" 
	//find world space eye vector.
	"vec3 E = normalize(cameraPos.xyz - worldPos.xyz);"	
	// calculate the reflection vector in world space.
	"gl_TexCoord[1].xyz = reflect(E, N);"
	
"}";

const char normalFrag[] =
"	#version 120\n"

"	varying vec3 lightVec;"
"	varying vec3 eyeVec;"

//all maps should be set to prevent undefined artifacting
"	uniform sampler2D colorMap;"
"	uniform sampler2D normalMap;" //alpha channel controls specularity amount
"	uniform sampler2D reflectMap;"
"	uniform sampler2D maskMap;" //controls reflection, transparency, self illumination and shininess

"	void main()"
"	{"
"		vec2 uv = gl_TexCoord[0].st;"
"		vec4 base = texture2D(colorMap, uv);"
"		vec4 maskValues = texture2D(maskMap, uv);"

//send colour straight to output if illum
"		if(maskValues.b > 0.0)"
"		{"
"			gl_FragColor = vec4(base.rgb * maskValues.b, maskValues.g);"
"		}"
"		else"
"		{"
"			vec4 ambient = base * 0.8;"// gl_LightSource[0].ambient;"
"			vec3 bump = normalize(texture2D(normalMap, uv).xyz * 2.0 - 1.0);"
"			vec3 lVec = normalize(lightVec);"
"			vec3 vVec = normalize(eyeVec);"
"			vec3 R = reflect(-vVec, bump);"

"			float multiplier = dot(bump, lVec);" //hemispherical lighting
"			multiplier = multiplier * 0.5 + 0.5;"
"			vec4 final_colour = mix(base, ambient, multiplier);"

//reflectmap lookup
"			vec3 vR = normalize(gl_TexCoord[1].xyz);"
"			vec3 reflect_colour = texture2D(reflectMap, (vR.xy / (2.0*(1.0 - vR.z))) + 0.5).rgb;"
"			final_colour.rgb = mix(final_colour.rgb, reflect_colour, maskValues.r);"

//specular highlighting
"			float colourIntensity = max(dot(bump, vVec), 0.0);"
"			vec4 spec = vec4(0.0);"
"			vec4 diff = vec4(0.0);"

"			float specMultiplier = texture2D(normalMap, uv).a;"
"			if(colourIntensity > 0.0)"
"			{"
				//calc spec
"				float specAmount = pow(clamp(dot(R, lVec), 0.0, 1.0), gl_FrontMaterial.shininess) * specMultiplier;"
"				spec += gl_LightSource[0].specular * specAmount;"
"				diff += gl_LightSource[0].diffuse * final_colour * colourIntensity;"
"			}"
"			final_colour.a = maskValues.g;"
"			gl_FragColor = clamp(final_colour + spec + diff, 0.0, 1.0);"
"		}"
"	}";

#endif