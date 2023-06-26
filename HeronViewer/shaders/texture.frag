#version 440 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

in vec2 texOffsets[9];
	
uniform sampler2D texture1;
	
void main()
{             
    vec3 texCol = texture(texture1, TexCoord).rgb;      
    FragColor = vec4(texCol, 1.0);
}
