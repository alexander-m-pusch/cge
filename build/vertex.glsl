#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 texCoords;

out vec2 aTexCoords;
out vec3 normal;
out vec3 fragPos;

uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform mat4 projectionMatrix;
uniform mat4 modelMatrixInverse;

void main() {
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position.xyz, 1.0f);
	fragPos = vec3(modelMatrix * vec4(position, 1.0f));
	aTexCoords = texCoords;
	normal = mat3(modelMatrixInverse) * inNormal;
}
