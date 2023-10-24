#version 330 core

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vColor;

out vec3 out_Color;

uniform mat4 transform;

void main(void)
{
	gl_Position = transform * vec4(vPos, 1.0);

	out_Color = vColor;
}