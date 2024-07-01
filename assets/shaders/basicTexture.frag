#version 460 core

uniform sampler2D tex;

in vec2 TexCoords;
out vec4 FragColor;

void main()
{
	FragColor = vec4(texture(tex, TexCoords));
}
