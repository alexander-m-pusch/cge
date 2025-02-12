#ifndef MAP_H_
#define MAP_H_

#include <cge/mesh.h>
#include <cge/texture.h>
#include <stdio.h>

#define CGE_MAGICNUM "CMDF"

#define CGE_TILE_FLAG_WALKABLE 0x0001
#define CGE_TILE_FLAG_INDOORS 0x0002
#define CGE_TILE_FLAG_DEADLY 0x0004

#define CGE_NORTH 0
#define CGE_SOUTH 1
#define CGE_WEST 2
#define CGE_EAST 3
#define CGE_UP 4
#define CGE_DOWN 5

struct cgeMapTile { //make sure to create with calloc
	unsigned short UUID;
	unsigned short flags;
	unsigned int fileaddr;
	float u, v;
};

struct cgeTileSet {
	unsigned short tileCount;
	unsigned int fileaddr;
	struct cgeTexture* texture;
	struct cgeMapTile* tiles;
};

//chunk size in tiles: 32 * 32 * 2 = 1KiB of tile data
struct cgeMapChunk {
	unsigned int fileaddr;
	unsigned short* tiles; //Format: tiles[tileX * 32 + tileY] = tileType; so we must rangecheck tileX, tileY < 32
	struct cge3DMesh* mesh; //pre-baked while loading
	short chunkPosX, chunkPosY, chunkPosZ; //chunk position in chunk space
	short playerPosX, playerPosY, playerPosZ; //player position in chunk
};

struct cgeMapProperties {
	struct cgeTileSet* tileSet;
	char* displayName;
	char* scriptFile;
	unsigned int fileaddr;
};

//map size in chunks: 256 * 256 * 256 = 24bits of position data
struct cgeMap {
	struct cgeMapDirectory* directory;
	struct cgeMapProperties* properties;
	char* internalName;
	unsigned int fileaddr;
	unsigned int originChunk;
	char playerPosX, playerPosY, playerPosZ; //player position in chunk coords, chunk size 32 tiles
};

struct cgeMapDirectory {
	FILE* mapFile;
	unsigned short mapCount;
	unsigned int* mapPointers;
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
void cgeSaveMapDirectory(struct cgeMapDirectory* directory);
void cgeUnloadMapDirectory(struct cgeMapDirectory* directory);
struct cgeMap* cgeLoadMapByName(struct cgeMapDirectory* directory, char* name);
void cgePlaceMap(struct cgeMapDirectory* directory, struct cgeMap* map);
void cgeUnloadMap(struct cgeMap* map);
struct cgeMapChunk* cgeGetNeighbourChunk(struct cgeMapChunk* chunk, short direction);
struct cgeMapChunk* cgeGetMapChunk(struct cgeMap* map, short x, short y, short z, int load);
void cgePlaceMapChunk(struct cgeMap* map, struct cgeMapChunk* chunk);
void cgeUnloadMapChunk(struct cgeMapChunk* chunk, int load);
struct cgeMapProperties* cgeCreateMapPropertiesFromScratch(struct cgeTileSet* tileset, char* displayName, char* scriptFile);
struct cgeMap* cgeCreateMapFromScratch(struct cgeMapProperties* properties, struct cgeMapDirectory* directory, char* internalName);
struct cgeMapChunk* cgeCreateMapChunkFromScratch(short x, short y, short z);

#endif
