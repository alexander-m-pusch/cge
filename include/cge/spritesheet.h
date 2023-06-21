#ifndef CGE_SPRITESHEET_H
#define CGE_SPRITESHEET_H

#include <cge/texture.h>

struct cgeSprite {
	char* name;
	struct cgeTexture* texture;
	float uLeft;
	float uRight;
	float vBottom;
	float vTop;
};

struct cgeSpritesheet {
	struct cgeTexture* TEXTURE;
	unsigned int SPRITESIZE;
	unsigned int spritesX;
	unsigned int spritesY;
	struct cgeSprite* sprites;
};

struct cgeSpritesheet* cgeCreateSpritesheet(struct cgeTexture* texture, unsigned int spriteSizePixels);
void cgeRegisterSprite(struct cgeSpritesheet* sheet, char* name, unsigned int X, unsigned int Y);
struct cgeSprite* cgeGetSprite(struct cgeSpritesheet* sheet, char* name);

#endif
