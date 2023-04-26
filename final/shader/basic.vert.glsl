#version 410

layout (location=0) in vec3 VertexPosition;
layout (location=1) in vec2 vTexCoord;

layout (location=0) out vec2 texCoord;

uniform mat4 MVP;

void main()
{
	texCoord = vTexCoord;
	gl_Position = vec4(VertexPosition,1.0);
}
