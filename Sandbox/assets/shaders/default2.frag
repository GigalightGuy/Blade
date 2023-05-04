#version 450

layout(binding = 1) uniform sampler2D texureSampler;

layout(binding = 2) uniform ExtraData
{
    vec4 testColor;
}extra;

layout(location = 0) in vec3 fragmentColor;
layout(location = 1) in vec2 fragmentTextureCoordinate;

layout(location = 0) out vec4 outColor;

void main() {
    
    outColor = texture(texureSampler, fragmentTextureCoordinate) * vec4(fragmentColor,1) * extra.testColor;
    
    if(outColor.a <= 0.01)
    {
        discard;
    }
}
