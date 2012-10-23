#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;

uniform mat4 MVP;

out vec2 UV;

void main() {
    vec4 v = vec4(vertexPosition_modelspace,1);
	gl_Position = MVP * v;

	UV = vertexUV;
}

