#version 450

layout(binding = 2) uniform sampler2D texureSampler;

layout(location = 0) in vec2 fragmentTextureCoordinate;

layout(location = 0) out vec4 outColor;

void main() {
    
    outColor = texture(texureSampler, fragmentTextureCoordinate);
    
    if(outColor.a <= 0)
    {
        discard;
    }
}
