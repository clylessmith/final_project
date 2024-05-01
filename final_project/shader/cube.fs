#version 410

in vec2 texCoord;

out vec4 FragColor;

uniform sampler2D cubeTex;

void main() {
    FragColor = texture(cubeTex, texCoord);
}
