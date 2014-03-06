//***normal / specular map shader for 2D images***//
// Only supports a single light source
#ifndef NORMAL_H_
#define NORMAL_H_

const char NormalMap[] =
	"#version 120\n"
	//input textures
	"uniform sampler2D colourMap;"
	"uniform sampler2D normalMap;"
	"uniform sampler2D reflectMap;"
	"uniform sampler2D specularMap;"
	//light properties
	"uniform vec3 lightPosition = vec3(0.0);"
	"uniform vec3 lightColour = vec3(0.6, 0.6, 0.6);"
	"uniform float lightPower = 0.6;"
	"uniform vec4 lightSpecColour = vec4(0.6, 0.6, 0.6, 1.0);"
	"uniform float lightSpecPower = 0.08;" //smaller is more glossy

	"uniform vec2 reflectOffset;" //offset the reflection map by sprite coords
					//so that the reflection appears static when the sprite moves
	"uniform vec2 resolution = vec2(60.0, 102.0);" //set this to size of the sprite to which this shader is applied
	"uniform bool invertBumpY = true;" //some normal maps require inverted Y channel

	"void main()"
	"{"
		//sample our diffuse and normal maps
		"vec2 coord = gl_TexCoord[0].xy;"
		"vec4 diffuseColour = texture2D(colourMap, coord);"
		"vec3 normalColour = texture2D(normalMap, coord).rgb;"

		//get normal value from sample
		"normalColour.g = invertBumpY ? 1.0 - normalColour.g : normalColour.g;"
		"vec3 normal = normalize(normalColour * 2.0 - 1.0);"

		//mix reflection map with colour using normal map alpha
		"float blendVal = texture2D(normalMap, coord).a;"
		"coord = mod(reflectOffset, resolution) / resolution;" //add offset to coord
		"coord.y = 1.0 - coord.y;"
		"vec3 reflectColour = texture2D(reflectMap, coord + (normal.rg * 2.0)).rgb;" //adding normal distorts reflection
		"diffuseColour.rgb = mix(diffuseColour.rgb, reflectColour, blendVal);"

		//calculate the light vector
		"vec3 lightDir = vec3((lightPosition.xy - gl_FragCoord.xy) / resolution, lightPosition.z);"
		"lightDir.y = 1.0 - lightDir.y;" //invert Y

		//calculate the colour intensity based on normal and add specular to pixels facing light
		"float colourIntensity = max(dot(normal, normalize(lightDir)), 0.0);"
		"vec4 specular = vec4(0.0);"
		"vec4 diffuse = vec4(0.0);"
		"if(colourIntensity > 0.0)"
		"{"
			//vector half way between light and view direction
			"vec3 halfVec = normalize(lightDir + vec3(0.5, 0.5, 0.5));" //fixed 2D view, so view is centred
			//get specular value from map
			"vec4 specColour = vec4(texture2D(specularMap, gl_TexCoord[0].xy).rgb, 1.0);"
			"float specModifier = max(dot(normal, halfVec), 0.0);"
			"specular = pow(specModifier, lightSpecPower) * specColour * lightSpecColour;"
			"specular.a *= diffuseColour.a;"
			"diffuse = lightSpecColour * diffuseColour * colourIntensity;"
		"}"

		//add light colour
		"diffuseColour.rgb += ((lightColour * lightPower) * colourIntensity);"
		"diffuseColour.rgb *= diffuseColour.rgb;"

		//output sum
		"gl_FragColor = clamp(specular + diffuse + diffuseColour, 0.0, 1.0);"
	"}";

#endif