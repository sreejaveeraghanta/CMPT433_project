#version 300 es

layout (location = 0) in highp vec4 vertex; // <vec2 position, vec2 texCoords>

out highp vec2 TexCoords;

uniform bool  chaos;
uniform bool  confuse;
uniform bool  shake;
uniform float time;

void main()
{
    gl_Position = vec4(vertex.xy, 0.0, 1.0); 
    vec2 texture = vertex.zw;

    if (chaos)
    {
        float strength = 0.3;
        vec2 pos = vec2(texture.x + sin(float(time)) * strength, 
                         texture.y + cos(float(time)) * strength);        
        TexCoords = pos;
    }
    else if (confuse)
    {
        TexCoords = vec2(1.0 - texture.x, 1.0 - texture.y);
    }
    else
    {
        TexCoords = texture;
    }
    if (shake)
    {
        float strength = 0.01;
        gl_Position.x += cos(float(time) * 10.0) * strength;        
        gl_Position.y += cos(float(time) * 15.0) * strength;        
    }
}  
