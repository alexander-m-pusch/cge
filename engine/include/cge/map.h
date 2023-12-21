#ifndef MAP_H_
#define MAP_H_

#include <cge/mesh.h>

#define FLAG_WALKABLE 0x0001
#define FLAG_INDOORS 0x0002

struct cgeMapTile { //make sure to create with calloc
	unsigned short tileName;
	struct cge3DMesh* mesh;
	unsigned short flags;
};

struct cgeTileSet {
	unsigned int tileCount;
	struct cgeMapTile** tiles;
}

//chunk size in tiles: 32 * 32 * 2 = 2KiB of tile Names
struct cgeMapChunk {
	unsigned short* tiles; //Format: tiles[tileX * 32 + tileY] = tileType; so we must rangecheck tileX, tileY < 32
	unsigned short tileset; //Which tileset this mapchunk uses
	short chunkPosX, chunkPosY, chunkPosZ; //chunk position in chunk space
	short playerPosX, playerPosY, playerPosZ; //player position in intra-chunk coords, playerPos = inter-chunk coord * 32 + intra-chunk coord
};

//map size in chunks: 256 * 256 * 256 = 24bits of position data
struct cgeMap {
	char* name;
	unsigned int chunkCount;
	unsigned int tilesetCount
	struct cgeMapChunk** chunks;
	struct cgeTileSet** tileSets;
	short playerPosX, playerPosY, playerPosZ; //player position in inter-chunk coords
};	

struct cgeMapDirectory {
	short mapCount;
	char* openMap;
	struct cgeMap** maps;
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
