#ifndef CGE_TEXTURE_H
#define CGE_TEXTURE_H

struct cgeTexture {
	char* filename;
	unsigned int texture;
	int width;
	int height;
	int channelCount;
};

struct cgeTexture* cgeLoadTexture(char* filepath);

#endif
