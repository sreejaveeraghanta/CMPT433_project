#version 300 es

in mediump vec2 TexCoords;

uniform sampler2D image;
uniform mediump vec3 spriteColor;

out mediump vec4 color;

void main()
{    
    color = vec4(spriteColor, 1.0) * texture(image, TexCoords);
}  

