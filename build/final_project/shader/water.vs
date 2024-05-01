#version 410 

layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 VertexTexCoord;

out vec4 clipSpace;
out vec3 camVec;
out vec2 TCoord;
out float timeOffset1;
out float timeOffset2;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 NormalMatrix;
uniform vec3 cam;
uniform float time1;
uniform float time2;

void main()
{
    clipSpace = model * projection * view * vec4(pos.x, 10, pos.y, 1.0);
    gl_Position = clipSpace;
    camVec = cam - (model * vec4(pos.x, 10, pos.y, 1.0)).xyz;
    TCoord = vec2(pos.x/2.0 + 0.5, pos.y/2.0 + 0.5);
    timeOffset1 = time1;
    timeOffset2 = time2;
}