#version 300 es

precision mediump float;

in highp vec2 TexCoords;
out highp vec4 color;

uniform sampler2D scene;
uniform vec2 offsets[9];
uniform int edge_kernel[9];
uniform float blur_kernel[9];

uniform bool chaos;
uniform bool confuse;
uniform bool shake;

void main()
{
    color = vec4(0.0);
    vec3 texSamples[9];  // Renamed 'sample' to 'texSamples'
    
    // Sample from texture offsets if using a convolution matrix
    if (chaos || shake)
    {
        for (int i = 0; i < 9; i++)
        {
            texSamples[i] = vec3(texture(scene, TexCoords.st + offsets[i]));
        }
    }

    // Process effects
    if (chaos)
    {           
        for (int i = 0; i < 9; i++)
        {
            color += vec4(texSamples[i] * float(edge_kernel[i]), 0.0);
        }
        color.a = 1.0;
    }
    else if (confuse)
    {
        color = vec4(1.0 - texture(scene, TexCoords).rgb, 1.0);
    }
    else if (shake)
    {
        for (int i = 0; i < 9; i++)
        {
            color += vec4(texSamples[i] * blur_kernel[i], 0.0);
        }
        color.a = 1.0;
    }
    else
    {
        color = texture(scene, TexCoords);
    }
}
