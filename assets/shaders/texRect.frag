#version 460 core

uniform sampler2D tex;

in vec2 TexCoords;
out vec4 FragColor;

void main()
{
	// FragColor = texture(tex, TexCoords);
	FragColor = vec4(texture(tex, TexCoords).x, 0.0f, 0.0f, 1.0f);
	// FragColor = vec4(1.0, 0.0f, 0.0f, 1.0f);;
}
