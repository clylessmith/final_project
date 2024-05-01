#version 410 core

in float Height;

out vec4 FragColor;

void main()
{
    float h = (Height + 16)/96.0f;
    vec3 color = vec3(0.14, 0.45 ,0.08);
    if (Height < 5) {
        color = vec3(0.58, 0.58, 0.58);
    } 
    else if (Height < 15) {
        color = vec3(1.0, 0.89, 0.8);
    } else if(Height < 30) {
        color = vec3(0.14, 0.45 ,0.08);
    } else {
        color = vec3(0.0,0.6 ,0.0);
    }
    FragColor = vec4(color + h, 1.0);
}