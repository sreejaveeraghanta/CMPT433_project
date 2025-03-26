#version 300 es

in highp vec4 vertex;

uniform highp mat4 model;
uniform mediump mat4 projection;

out mediump vec2 TexCoords;

void main()
{
    TexCoords = vertex.zw;
    gl_Position = projection * model * vec4(vertex.xy, 0.0, 1.0);
}
