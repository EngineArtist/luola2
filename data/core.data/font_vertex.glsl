#version 330 core

layout(location = 0) in vec2 vertexXY;
layout(location = 1) in vec2 vertexUV;

uniform vec2 offset;
uniform float scale;

out vec2 UV;

void main() {
	gl_Position = vec4(vertexXY * scale + offset, 0, 1);

	UV = vertexUV;
}

