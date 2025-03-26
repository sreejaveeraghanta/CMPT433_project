#version 300 es

in highp vec2 TexCoords;
in highp vec4 ParticleColor;
out highp vec4 color;

uniform sampler2D sprite;

void main()
{
    color = (texture(sprite, TexCoords) * ParticleColor);
}  
