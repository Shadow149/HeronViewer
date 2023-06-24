#version 440 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec3 ourColor;
out vec2 TexCoord;

out vec2 texOffsets[9];

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

uniform float texelWidth;
uniform float texelHeight;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	ourColor = aColor;

	TexCoord = vec2(aTexCoord.x, aTexCoord.y);

	for (int i = -1; i <= 1; i ++) {
		for (int j = -1; j <= 1; j ++) {
			texOffsets[(3*(i+1)) + (j+1)] = vec2(aTexCoord.x + (i * texelWidth), aTexCoord.y + (j * texelHeight));
		}
	}
}