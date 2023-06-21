#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <cge/spritesheet.h>
#include <cge/texture.h>

struct cgeSpritesheet* cgeCreateSpritesheet(struct cgeTexture* texture, unsigned int spriteSizePixels){
	struct cgeSpritesheet* sheet = malloc(sizeof(struct cgeSpritesheet));

	sheet->TEXTURE = texture;
	sheet->SPRITESIZE = spriteSizePixels;
	sheet->spritesX = texture->width / spriteSizePixels;
	sheet->spritesY = texture->height / spriteSizePixels;
	sheet->sprites = calloc(sheet->spritesX * sheet->spritesY, sizeof(struct cgeSprite));
	
	return sheet;
}

void cgeRegisterSprite(struct cgeSpritesheet* sheet, char* name, unsigned int X, unsigned int Y) {
	sheet->sprites[sheet->spritesX * Y + X].name = name;
	sheet->sprites[sheet->spritesX * Y + X].texture = sheet->TEXTURE;
	sheet->sprites[sheet->spritesX * Y + X].uLeft = ((float) X * (float) sheet->SPRITESIZE) / ((float) sheet->SPRITESIZE * (float) sheet->spritesX);
	sheet->sprites[sheet->spritesX * Y + X].uRight = (((float) X + 1.0f) * (float) sheet->SPRITESIZE) / ((float) sheet->SPRITESIZE * (float) sheet->spritesX);
	sheet->sprites[sheet->spritesX * Y + X].vBottom = ((float) Y * (float) sheet->SPRITESIZE) / ((float) sheet->SPRITESIZE * (float) sheet->spritesY);
	sheet->sprites[sheet->spritesX * Y + X].vTop = (((float) Y + 1.0f)* (float) sheet->SPRITESIZE) / ((float) sheet->SPRITESIZE * (float) sheet->spritesY);
}

struct cgeSprite* cgeGetSprite(struct cgeSpritesheet* sheet, char* name) {
	for(int i = 0; i < (sheet->spritesX * sheet->spritesY); i++) {
		if(sheet->sprites[i].name) {
			if(strcmp(name, sheet->sprites[i].name) == 0) {
				return &(sheet->sprites[i]);
			}
		}
	}

	fprintf(stderr, "Could not load sprite \"%s\"\n", name);
	return NULL;
}

void cgeDestroySpritesheet(struct cgeSpritesheet* sheet) {
	free(sheet->sprites);
	free(sheet);
}
