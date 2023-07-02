#version 440 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

in vec2 texOffsets[9];
	
uniform sampler2D texture1;
layout(binding=2) uniform sampler2D texture1_small;
uniform bool low_res;
	
void main()
{            
	vec3 texCol = vec3(0);
	if (low_res)
		texCol = texture(texture1_small, TexCoord).rgb;      
	else
		texCol = texture(texture1, TexCoord).rgb; 
    FragColor = vec4(texCol, 1.0);
}
