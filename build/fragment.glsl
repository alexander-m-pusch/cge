#version 330 core

in vec2 aTexCoords;
in vec3 normal;
in vec3 fragPos;

out vec4 FragColor;

uniform sampler2D tex;
uniform vec3 ambient;

void main() {
	if(length(normalize(ambient)) < 0.1f) {
		FragColor = texture(tex, aTexCoords);
	} else {
		vec3 norm_ambient = normalize(ambient);
		FragColor = vec4(texture(tex, aTexCoords).xyz * min(1.0f, max(dot(normal, norm_ambient), 0.25f)), 1.0f);
	}
}
