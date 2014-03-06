#ifndef DEPTHMAP_H_
#define DEPTHMAP_H_

const char depthVert[] =
"#version 120\n"

//"varying float depth;"

"uniform mat4 depthMVP;"
"uniform float farPlane = 600.0;" //ideally a little more than the camera z distance

"void main()"
"{"
"	gl_Position = depthMVP * gl_Vertex;"
//"	depth = gl_Position.z / farPlane;"
"}";

const char depthFrag[] =
"#version 120\n"

//"varying float depth;"

"void main()"
"{"
"	gl_FragColor = vec4(vec3(gl_FragCoord.z), 1.0);"
"}";



#endif //DEPTHMAP_H_