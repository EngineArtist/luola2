#version 330 core

layout(location = 0) in vec2 vertexXY;

uniform mat4 MVP;

void main() {
	vec4 v = vec4(vertexXY.xy, 0, 1);
	gl_Position = MVP * v;
}

