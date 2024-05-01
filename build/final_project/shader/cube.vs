#version 410

layout (location=0) in vec2 VertexPosition;
layout (location=1) in vec2 VertexTexCoord;

out vec2 texCoord;

uniform mat4 MVP;

void main()
{
    gl_Position = MVP * vec4(VertexPosition,0.0,1.0);

    texCoord = VertexTexCoord;
}
