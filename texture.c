#include <cge/texture.h>
#include <stdlib.h>
#include <stdio.h>

#include <GL/glew.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

struct cgeTexture* cgeLoadTexture(char* path) {
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	int width, height, channelCount;

	stbi_set_flip_vertically_on_load(1);
	unsigned char* data = stbi_load(path, &width, &height, &channelCount, 0);
	if(data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	} else {
		fprintf(stderr, "Could not load texture: %s\n", path);
	}

	stbi_image_free(data);

	struct cgeTexture* returnTexture = malloc(sizeof(struct cgeTexture));

	returnTexture->texture = texture;
	returnTexture->width = width;
	returnTexture->height = height;
	returnTexture->channelCount = channelCount;

	glBindTexture(GL_TEXTURE_2D, 0);

	return returnTexture;
}
