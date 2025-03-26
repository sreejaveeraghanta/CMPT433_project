#version 300 es
layout (location = 0) in highp vec4 vertex; // <vec2 position, vec2 texCoords>

out highp vec2 TexCoords;
out highp vec4 ParticleColor;

uniform mediump mat4 projection;
uniform mediump vec2 offset;
uniform mediump vec4 color;

void main()
{
    float scale = 8.0f;
    TexCoords = vertex.zw;
    ParticleColor = color;
    gl_Position = projection * vec4((vertex.xy * scale) + offset, 0.0, 1.0);
}
