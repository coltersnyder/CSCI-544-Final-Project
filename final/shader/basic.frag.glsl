#version 410

in float Height;

layout (location=0) out vec4 FragColor;

void main() {
	 float h = (Height + 16)/64.0f;
	 FragColor = vec4(0.5, h, 0.5, 1.0);
}
