#version 410 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;

out vec2 TexCoord;

uniform vec4 plane;
uniform mat4 view;
uniform mat4 model;
uniform mat4 projection;

void main()
{
    gl_Position = vec4(aPos, 1.0);
    TexCoord = aTex;
    //gl_ClipDistance[0] = -1;

}