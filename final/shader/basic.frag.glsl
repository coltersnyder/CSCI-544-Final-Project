#version 410

layout (location=0) in vec2 texCoord;

layout (location=0) out vec4 FragColor;

uniform sampler2D texMap;
void main() {
	 FragColor = texture(texMap, texCoord);
}
