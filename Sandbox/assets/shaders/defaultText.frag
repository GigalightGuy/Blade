#version 450

layout(binding = 1) uniform sampler2D msdf;

layout(location = 0) in vec4 fragmentColor;
layout(location = 1) in vec2 fragmentTextureCoordinate;

layout(location = 0) out vec4 outColor;


float screenPxRange() {
    const float pxRange = 2.0;
    vec2 unitRange = vec2(pxRange)/vec2(textureSize(msdf, 0));
    vec2 screenTexSize = vec2(1.0)/fwidth(fragmentTextureCoordinate);
    return max(0.5*dot(unitRange, screenTexSize), 1.0);
}

float median(float r, float g, float b) 
{
    return max(min(r, g), min(max(r, g), b));
}

void main() 
{
    vec3 msd = texture(msdf, fragmentTextureCoordinate).rgb;
    float sd = median(msd.r, msd.g, msd.b);
    float screenPxDistance = screenPxRange()*(sd - 0.5);
    float opacity = clamp(screenPxDistance + 0.5, 0.0, 1.0);
    if (opacity == 0.0) discard;

    outColor = mix(vec4(0.0), fragmentColor, opacity);
}
