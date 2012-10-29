#version 330 core

in vec2 UV;
out vec4 clr;

uniform sampler2D fontSampler;
uniform vec4 color;

void main()
{
	clr = texture(fontSampler, UV) * color;
}

