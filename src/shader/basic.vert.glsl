#version 410

layout (location=0) in vec3 VertexPosition;
layout (location=2) in vec3 VertexColor;

layout (location=0) out vec3 vColor;

layout (std140) uniform vertexBlock{
    mat4 model;
    mat4 view;
    mat4 projection;
};

void main()
{
    vColor = VertexColor;

    gl_Position = projection * view * model * vec4(VertexPosition,1.0);
}
