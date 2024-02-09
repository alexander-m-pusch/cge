#ifndef MAP_H_
#define MAP_H_

#include <cge/mesh.h>
#include <cge/texture.h>
#include <stdio.h>

#define FLAG_WALKABLE 0x0001
#define FLAG_INDOORS 0x0002

struct cgeMapTile { //make sure to create with calloc
	unsigned short UUID;
	unsigned short flags;
};

struct cgeTileSet {
	unsigned short tileCount;
	struct cgeTexture* texture;
	struct cgeMapTile* tiles;
};

//chunk size in tiles: 32 * 32 * 2 = 2KiB of tile names
struct cgeMapChunk {
	unsigned short* tiles; //Format: tiles[tileX * 32 + tileY] = tileType; so we must rangecheck tileX, tileY < 32
	struct cge3DMesh* mesh; //pre-baked while loading
	short chunkPosX, chunkPosY, chunkPosZ; //chunk position in chunk space
};

struct cgeMapProperties {
	struct cgeTileSet* tileSet;
	char* displayName;
	char* scriptFile;
};

//map size in chunks: 256 * 256 * 256 = 24bits of position data
struct cgeMap {
	struct cgeMapDirectory* direcory;
	char* internalName;
	unsigned int chunkCount;
	short playerPosX, playerPosY, playerPosZ; //player position in inter-chunk coords, chunk size 32 tiles
};

struct cgeMapDirectory {
	FILE* mapFile;
	short mapCount;
	char* openMap;
};

static inline int getPlayerPosX(struct cgeMap* map, struct cgeMapChunk* chunk) {
	return map->playerPosX * 32 + chunk->playerPosX;
}

static inline int getPlayerPosY(struct cgeMap* map, struct cgeMapChunk* chunk) {
	return map->playerPosY * 32 + chunk->playerPosY;
}

static inline int getPlayerPosZ(struct cgeMap* map, struct cgeMapChunk* chunk) {
	return map->playerPosZ * 32 + chunk->playerPosZ;
}

struct cgeMapDirectory* cgeLoadMapDirectory(char* filepath);
void cgeSaveMapDirectory(struct cgeMapDirectory* directory, char* filepath);
void cgeUnloadMapDirectory(struct cgeMapDirectory* directory);
struct cgeMap* cgeLoadMapByName(struct cgeMapDirectory* directory, char* name);
void cgePlaceMap(struct cgeMapDirectory* directory, struct cgeMap* map);
void cgeUnloadMap(struct cgeMap* map);
struct cgeMapChunk* cgeGetMapChunk(struct cgeMap* map, short x, short y, short z);
void cgePlaceMapChunk(struct cgeMap* map, struct cgeMapChunk* chunk);
void cgeUnloadMapChunk(struct cgeMapChunk* chunk);

#endif
