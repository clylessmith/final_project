#version 410 

in vec4 clipSpace;
in vec3 camVec;
in vec2 TCoord;
in float timeOffset1;
in float timeOffset2;

out vec4 FragColor;

uniform sampler2D reflectTex;
uniform sampler2D refractTex;
uniform sampler2D duDvMap;
uniform sampler2D flowMap;

void main()
{
    vec2 normalizedDeviceCoords = (clipSpace.xy / clipSpace.w)/2.0 + 0.5;
    vec2 refractCoords = vec2(normalizedDeviceCoords.x, normalizedDeviceCoords.y);
    vec2 reflectCoords = vec2(normalizedDeviceCoords.x, -normalizedDeviceCoords.y); 

    vec2 waves = (texture(duDvMap, vec2(TCoord.x + 1.0 * timeOffset1, TCoord.y)).rg * 2.0 - 1.0) * 0.04;
    
    vec2 flowVec = (texture(flowMap, vec2(TCoord.x + 1.0 * timeOffset1, TCoord.y)).rg * 2.0f - 1.0f) * 0.02;

    vec2 normalT1 = texture(duDvMap, flowVec).rg;
    vec2 normalT2 = texture(duDvMap, flowVec).rg;
    normalT1 = (2.0*normalT1 - 1.0f) * 0.06;
    normalT2 = (2.0*normalT1 - 1.0f) * 0.0;

    float flowLerp = ( abs( 0.5 - timeOffset1 ) / 0.5 );
    vec2 normal = mix( normalT1, normalT2, flowLerp );

    refractCoords += flowVec;
    reflectCoords += flowVec;

    vec4 reflectColor = texture(reflectTex, reflectCoords);
    vec4 refractColor = texture(refractTex, refractCoords);
    
    float refractFact = dot(normalize(camVec), vec3(0.0, 10.0, 0.0));

    FragColor = mix(reflectColor, refractColor, 0.5);
}