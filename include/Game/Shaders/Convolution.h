//***convolution shader. Works on a 3x3 matrix for different effects***//
// See http://docs.gimp.org/en/plug-in-convmatrix.html for a list of example matrices
// Includes blur matrix by default

#ifndef CONVOLUTION_H_
#define CONVOLUTION_H_

//creates convolution effects based on a 3x3 matrix
const char convolution[] =
"#version 120\n"
"uniform sampler2D texture;"
//this is a blur matrix but we can plug in other matrices for other effects
"uniform float matrix[9] = float[9](1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0);"
"uniform float width;"
"uniform float height;"
"const float weight = 1.0 / 9.0;"
"vec2 pixel = vec2(1.0 / width, 1.0 / height);" //view / render target size

"void main(void)"
"{"
	"vec4 colour = vec4(0.0);"
	"vec2 coord = gl_TexCoord[0].xy;"
	"vec2 offset = pixel * 1.5;"
	"vec2 start = coord - offset;"
	"vec2 current = start;"

	"for(int i = 0; i < 9; i++)"
	"{"
		"colour += texture2D(texture, current) * matrix[i];"
		"current.x += pixel.x;"
		"if(i == 2 || i == 5)"
		"{"
			"current.x == start.x;"
			"current.y += pixel.y;"
		"}"
	"}"
	"gl_FragColor = colour * weight;"
"}";

//blends a shadow map with existing scene applying a gaussian blur to it
const char shadowBlend[] =
"#version 120\n"

"uniform sampler2D colorMap;"
"uniform sampler2D shadowMap;"
"uniform float width;"
"uniform float height;"

"const int KERNEL_SIZE = 9;"
"float kernel[KERNEL_SIZE] = { 1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0,"
"								2.0 / 16.0, 4.0 / 16.0, 2.0 / 16.0,"
"								1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0 };"

"float step_w = 1.0 / width;"
"float step_h = 1.0 / height;"

"const vec2 offset[KERNEL_SIZE] = { vec2(-step_w, -step_h), vec2(0.0, -step_h), vec2(step_w, -step_h),"
"									vec2(-step_w, 0.0), vec2(0.0, 0.0), vec2(step_w, 0.0),"
"									vec2(-step_w, step_h), vec2(0.0, step_h), vec2(step_w, step_h) };"

"void main(void)"
"{"
"	int i = 0;"
"	vec4 sum = vec4(0.0);"
"	vec2 coord = gl_TexCoord[0].xy;"
"		for (i = 0; i<KERNEL_SIZE; i++)"
"		{"
"			vec4 tmp = texture2D(shadowMap, coord + offset[i]);"
"			sum += tmp * kernel[i];"
"		}"
"	gl_FragColor = texture2D(colorMap, coord) * /*texture2D(shadowMap, coord);*/ sum;"
"}";

#endif