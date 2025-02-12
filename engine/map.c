#include <cge/map.h>
#include <cge/misc.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct cgeMapDirectory* cgeLoadMapDirectory(char* filepath) {
	FILE* mapTest = fopen(filepath, "r");
	if(mapTest == 0) {
#ifdef CGE_DEBUG
		printf("DEBUG: CGE Map Directory %s doesn't exist, creating it\n", filepath);
#endif
		FILE* newMap = fopen(filepath, "wb");

		if(newMap == 0) {
			cgeCrash("New map file could not be created.");
		}

		char magicnum[] = CGE_MAGICNUM;
		unsigned int nextHeader = 0;
		unsigned int reserved[] = {0, 0, 0, 0, 0, 0, 0, 0};

		fseek(newMap, 0L, SEEK_SET);
		fwrite(magicnum, sizeof(char), 4, newMap);
		fwrite(&nextHeader, sizeof(int), 1, newMap);
		fwrite(reserved, sizeof(int), 8, newMap);
		
		fclose(newMap);

#ifdef CGE_DEBUG
		printf("DEBUG: CGE Map Directory %s skeleton created.\n", filepath);
#endif
	} else {
#ifdef CGE_DEBUG
		printf("DEBUG: CGE Map Directory %s exists.\n", filepath);
#endif
		fclose(mapTest);
	}
	
	FILE* mapDirectoryFile = fopen(filepath, "rb+");
	
	if(mapDirectoryFile == 0) {
		cgeCrash("Map file could not be opened in write mode.");
	}

	struct cgeMapDirectory* directory = calloc(1, sizeof(struct cgeMapDirectory));

	directory->mapFile = mapDirectoryFile;

	char checknum[4];

	unsigned int nextMapHeader = 0;

	fseek(mapDirectoryFile, 0L, SEEK_SET);
	fread(checknum, sizeof(char), 4, mapDirectoryFile);
	fread(&nextMapHeader, sizeof(unsigned int), 1, mapDirectoryFile);

	if(strncmp(CGE_MAGICNUM, checknum, 4) == 0) {
#ifdef CGE_DEBUG
		printf("DEBUG: CGE Map Directory File Header valid!\n");
#endif
		//now, we iterate over the map headers to make a table

		while(nextMapHeader != 0) {
			directory->mapCount++;
			directory->mapPointers = realloc(directory->mapPointers, directory->mapCount * sizeof(unsigned int));
			directory->mapPointers[directory->mapCount - 1] = nextMapHeader;
			
			fseek(mapDirectoryFile, (unsigned long long int) nextMapHeader + 4, SEEK_SET);
			fread(&nextMapHeader, sizeof(unsigned int), 1, mapDirectoryFile);
#ifdef CGE_DEBUG
			printf("DEBUG: Read map header no. %d; offset %d\n", directory->mapCount, nextMapHeader);
#endif
		}
	} else {
		fprintf(stderr, "Broken CGE Map Directory File: %s\n", filepath);
		fclose(mapDirectoryFile);
		free(directory);
		return NULL;
	}

#ifdef CGE_DEBUG
		printf("DEBUG: Successfully read CGE Map Directory File, read %d map headers\n", directory->mapCount);
#endif

	return directory;
}

void cgeSaveMapDirectory(struct cgeMapDirectory* directory) {
	fflush(directory->mapFile);
	return;
}

void cgeUnloadMapDirectory(struct cgeMapDirectory* directory) {
	fclose(directory->mapFile);
	free(directory->mapPointers);
	free(directory);
}

struct cgeMap* cgeLoadMapByName(struct cgeMapDirectory* directory, char* name) {
	struct cgeMap* map = calloc(1, sizeof(struct cgeMap));

	for(short i = 0; i < directory->mapCount; i++) {
		fseek(directory->mapFile, (unsigned long long int) directory->mapPointers[i] + 12, SEEK_SET);
		unsigned int mapNameSize = 0;
		fread(&map->originChunk, sizeof(unsigned int), 1, directory->mapFile);
		fread(&mapNameSize, sizeof(unsigned int), 1, directory->mapFile);
		char* mapNameString = malloc((mapNameSize + 1) * sizeof(char));
		fseek(directory->mapFile, (unsigned long long int) directory->mapPointers[i] + 36, SEEK_SET);
		fread(mapNameString, sizeof(char), mapNameSize, directory->mapFile);
		mapNameString[mapNameSize] = '\0'; //c strings ftw
		if(strcmp(name, mapNameString) == 0) {
			map->directory = directory;
			map->internalName = mapNameString;
			map->fileaddr = directory->mapPointers[i];
			
			//now we read the map properties
			unsigned int mapPropertyHeader = 0;
			
			fseek(directory->mapFile, (unsigned long long int) directory->mapPointers[i] + 8, SEEK_SET);
			fread(&mapPropertyHeader, sizeof(unsigned int), 1, directory->mapFile);

			struct cgeMapProperties* properties = calloc(1, sizeof(struct cgeMapProperties));
			map->properties = properties;

			fseek(directory->mapFile, (unsigned long long int) mapPropertyHeader + 8, SEEK_SET);
			unsigned int mapScriptFilenameSize = 0;
			fread(&mapScriptFilenameSize, sizeof(unsigned int), 1, directory->mapFile);
			char* mapScriptFilenameString = malloc((mapScriptFilenameSize + 1) * sizeof(char));
			fseek(directory->mapFile, (unsigned long long int) mapPropertyHeader + 32, SEEK_SET);
			fread(mapScriptFilenameString, sizeof(char), mapScriptFilenameSize, directory->mapFile);
			mapScriptFilenameString[mapScriptFilenameSize] = '\0'; //c strings ftw

			//TODO load map script

			fseek(directory->mapFile, (unsigned long long int) mapPropertyHeader + 12, SEEK_SET);
			unsigned int mapTextureFilenameSize = 0;
			fread(&mapTextureFilenameSize, sizeof(unsigned int), 1, directory->mapFile);
			char* mapTextureFilenameString = malloc((mapTextureFilenameSize + 1) * sizeof(char));
			fseek(directory->mapFile, (unsigned long long int) mapPropertyHeader + mapScriptFilenameSize + 32, SEEK_SET);
			fread(mapTextureFilenameString, sizeof(char), mapTextureFilenameSize, directory->mapFile);
			mapTextureFilenameString[mapTextureFilenameSize] = '\0'; //c strings ftw

			struct cgeTileSet* tileset = calloc(1, sizeof(struct cgeTileSet));
			struct cgeTexture* tilesetTexture = cgeLoadTexture(mapTextureFilenameString);
			tileset->texture = tilesetTexture;

			properties->tileSet = tileset;

			unsigned int nextTileHeader = 0;

			fseek(directory->mapFile, (unsigned long long int) mapPropertyHeader + 4, SEEK_SET);
			fread(&nextTileHeader, sizeof(unsigned int), 1, directory->mapFile);
			
			while(nextTileHeader != 0) {
				tileset->tileCount++;
				tileset->tiles = realloc(tileset->tiles, tileset->tileCount * sizeof(struct cgeMapTile));

				fseek(directory->mapFile, (unsigned long long int) nextTileHeader, SEEK_SET);
				unsigned int currloc = nextTileHeader;
				fread(&nextTileHeader, sizeof(unsigned int), 1, directory->mapFile);
#ifdef CGE_DEBUG
				printf("DEBUG: Read tilset header no. %d; offset %d\n", tileset->tileCount, nextTileHeader);
#endif
				
				unsigned short UUID;
				unsigned short flags;
				float u, v;
				fread(&UUID, sizeof(unsigned short), 1, directory->mapFile);
				fread(&flags, sizeof(unsigned short), 1, directory->mapFile);
				fread(&u, sizeof(float), 1, directory->mapFile);
				fread(&v, sizeof(float), 1, directory->mapFile);
				tileset->tiles[tileset->tileCount - 1].UUID = UUID;
				tileset->tiles[tileset->tileCount - 1].flags = flags;
				tileset->tiles[tileset->tileCount - 1].fileaddr = currloc;
				tileset->tiles[tileset->tileCount - 1].u = u;
				tileset->tiles[tileset->tileCount - 1].v = v;
			}

			//TODO execute map loading scripts
			return map;
		} else {
			free(mapNameString);
		}
	}

	free(map);

	return NULL;
}

static unsigned int cgeGetLastMapChunkInternal(struct cgeMap* map) {
	fseek(map->directory->mapFile, map->fileaddr + 12, SEEK_SET);
	unsigned int originChunkAddr = 0;
	fread(&originChunkAddr, sizeof(unsigned int), 1, map->directory->mapFile);
	unsigned int lastReadChunkAddr = 0;
	while(originChunkAddr) {
		lastReadChunkAddr = originChunkAddr;
		fseek(map->directory->mapFile, originChunkAddr + 12, SEEK_SET);
		fread(&originChunkAddr, sizeof(unsigned int), 1, map->directory->mapFile);
		
	}
	return lastReadChunkAddr;
}

void cgePlaceMap(struct cgeMapDirectory* directory, struct cgeMap* map) {
#ifdef CGE_DEBUG
	printf("Placing map %s\n", map->internalName);
#endif
	
	map->directory = directory;
	fseek(map->directory->mapFile, 0L, SEEK_END);
	unsigned int mapaddr = (unsigned int) ftell(map->directory->mapFile);
	unsigned int prevoff;
	if(map->directory->mapCount != 0) {
		prevoff = map->directory->mapPointers[map->directory->mapCount - 1];
	} else {
#ifdef CGE_DEBUG
		printf("Placing first map in directory\n");
#endif
		prevoff = 0;
	}

	unsigned int header[] = {prevoff, 0, 0, 0, strlen(map->internalName), 0, 0, 0, 0};

	fwrite(&header, sizeof(unsigned int), 9, map->directory->mapFile);
	fwrite(map->internalName, sizeof(char), strlen(map->internalName), map->directory->mapFile);
	if(prevoff == 0) {
		fseek(map->directory->mapFile, 4, SEEK_SET);
		fwrite(&mapaddr, sizeof(unsigned int), 1, map->directory->mapFile);
	} else {
		fseek(map->directory->mapFile, prevoff + 4, SEEK_SET);
		fwrite(&mapaddr, sizeof(unsigned int), 1, map->directory->mapFile);
	}
	map->fileaddr = mapaddr;
	directory->mapCount++;
	directory->mapPointers = realloc(directory->mapPointers, directory->mapCount * sizeof(unsigned int));

#ifdef CGE_DEBUG
	printf("Placed map header, continuing with properties\n");
#endif

	//and now to place the map properties header
	
	fseek(map->directory->mapFile, 0L, SEEK_END);
	unsigned int propaddr = (unsigned int) ftell(map->directory->mapFile);
	unsigned int headerProp[] = {0, 0, strlen(map->properties->scriptFile), strlen(map->properties->tileSet->texture->filename), 0, 0, 0, 0};
	fwrite(&headerProp, sizeof(unsigned int), 8, map->directory->mapFile);
	fwrite(map->properties->scriptFile, sizeof(char), strlen(map->properties->scriptFile), map->directory->mapFile);
	fwrite(map->properties->tileSet->texture->filename, sizeof(char), strlen(map->properties->tileSet->texture->filename), map->directory->mapFile);

	fseek(map->directory->mapFile, mapaddr + 8, SEEK_SET);
	fwrite(&propaddr, sizeof(unsigned int), 1, map->directory->mapFile);

#ifdef CGE_DEBUG
	printf("Properties header placed, continuing with tileset\n");
#endif

	unsigned int prev_tileaddr = propaddr;
	for(int i = 0; i < map->properties->tileSet->tileCount; i++) {
		fseek(map->directory->mapFile, 0L, SEEK_END);
		unsigned int tileaddr = (unsigned int) ftell(map->directory->mapFile);
		
		unsigned int intVals[] = {0, map->properties->tileSet->tiles[i].UUID, map->properties->tileSet->tiles[i].flags};
		float floatVals[] = {map->properties->tileSet->tiles[i].u, map->properties->tileSet->tiles[i].v};
		fwrite(intVals, sizeof(unsigned int), 3, map->directory->mapFile);
		fwrite(floatVals, sizeof(unsigned int), 2, map->directory->mapFile);

		if(prev_tileaddr == propaddr) {
			fseek(map->directory->mapFile, prev_tileaddr + 4, SEEK_SET);
			fwrite(&tileaddr, sizeof(unsigned int), 1, map->directory->mapFile);
		} else {
			fseek(map->directory->mapFile, prev_tileaddr, SEEK_SET);
			fwrite(&tileaddr, sizeof(unsigned int), 1, map->directory->mapFile);
		}

		prev_tileaddr = tileaddr;
		
#ifdef CGE_DEBUG
		printf("Placed tile %d\n", i);
#endif
	}
}

void cgeUnloadMap(struct cgeMap* map) {
	//TODO execute map unloading scripts

	free(map->properties->tileSet->tiles);
	free(map->properties->tileSet);
	free(map->properties);
	free(map->internalName);
	free(map);
}

struct cgeMapChunk* cgeGetNeighbourChunk(struct cgeMapChunk* chunk, short direction) {
	return NULL;
}

struct cgeMapChunk* cgeGetMapChunk(struct cgeMap* map, short x, short y, short z, int load) {
#ifdef CGE_DEBUG
	printf("Loading chunk at (%d|%d|%d)\n", x, y, z);
#endif
	fseek(map->directory->mapFile, map->fileaddr + 12, SEEK_SET);
	unsigned int originChunkAddr = 0;
	fread(&originChunkAddr, sizeof(unsigned int), 1, map->directory->mapFile);
	while(originChunkAddr) {
		unsigned int chunkCoords[3];
		fseek(map->directory->mapFile, originChunkAddr + 40, SEEK_SET);
		fread(&chunkCoords, sizeof(unsigned int), 3, map->directory->mapFile);
#ifdef CGE_DEBUG
		printf("Checking chunk (%d|%d|%d)\n", chunkCoords[0], chunkCoords[1], chunkCoords[2]);
#endif
		if(chunkCoords[0] == (unsigned int) x && chunkCoords[1] == (unsigned int) y && chunkCoords[2] == (unsigned int) z) {
#ifdef CGE_DEBUG
			printf("Found chunk, loading and returning it\n");
#endif
			struct cgeMapChunk* chunk = calloc(sizeof(struct cgeMapChunk), 1);
			chunk->chunkPosX = x;
			chunk->chunkPosY = y;
			chunk->chunkPosZ = z;
			chunk->fileaddr = originChunkAddr;
		
			chunk->tiles = calloc(sizeof(unsigned short), 32 * 32);

			fseek(map->directory->mapFile, originChunkAddr + 88, SEEK_SET);
			fread(chunk->tiles, sizeof(unsigned short), 32 * 32, map->directory->mapFile);

			if(load) {
				//TODO execute chunk loading script
				//TODO map loading
			}
			
			return chunk;
		}

		fseek(map->directory->mapFile, originChunkAddr + 12, SEEK_SET);
		fread(&originChunkAddr, sizeof(unsigned int), 1, map->directory->mapFile);
		
	}
	return NULL;
}

void cgePlaceMapChunk(struct cgeMap* map, struct cgeMapChunk* chunk) {
	fseek(map->directory->mapFile, 0L, SEEK_END);
	unsigned int chunkaddr = (unsigned int) ftell(map->directory->mapFile);
	/*
	 *
	 * Explanation of the following array:
	 *
	 * 0 Nonsense (Fuckup while designing file format)
	 * 0 Nonsense (Fuckup while designing file format)
	 * 0 prev chunk header (gets filled in later)
	 * 0 next chunk header (remains 0)
	 * 0 neighbour chunk east
	 * 0 neighbour chunk west
	 * 0 neighbour chunk north
	 * 0 neighbour chunk south
	 * 0 neighbour chunk up
	 * 0 neighbour chunk down
	 * chunkPosX chunk pos x
	 * chunkPosY chunk pos y
	 * chunkPosZ chunk pos z
	 * 0 tileset link UNUSED
	 * 0 reserved
	 * 0 reserved
	 * 0 reserved
	 * 0 reserved
	 *
	 *
	 */
	unsigned int padding[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, (unsigned int) chunk->chunkPosX, (unsigned int) chunk->chunkPosY, (unsigned int) chunk->chunkPosZ, 0, 0, 0, 0, 0};
	fwrite(&padding, sizeof(unsigned int), 88, map->directory->mapFile);
	fwrite(chunk->tiles, sizeof(unsigned short), 32 * 32, map->directory->mapFile);
	chunk->fileaddr = chunkaddr;

	unsigned int lastReadChunkAddr = cgeGetLastMapChunkInternal(map);
	fseek(map->directory->mapFile, lastReadChunkAddr + 12, SEEK_SET);
	fwrite(&chunkaddr, sizeof(unsigned int), 1, map->directory->mapFile);
	fseek(map->directory->mapFile, chunkaddr + 8, SEEK_SET);
	fwrite(&lastReadChunkAddr, sizeof(unsigned int), 1, map->directory->mapFile);

	if(chunk->chunkPosX == 0 && chunk->chunkPosY == 0 && chunk->chunkPosZ == 0) {
		//we are the origin chunk
		fseek(map->directory->mapFile, map->fileaddr + 12, SEEK_SET);
		fwrite(&chunkaddr, sizeof(unsigned int), 1, map->directory->mapFile);
	}

	struct cgeMapChunk* chunk_e = cgeGetMapChunk(map, chunk->chunkPosX + 1, chunk->chunkPosY, chunk->chunkPosZ, 0);
	
	if(chunk_e) {
		fseek(map->directory->mapFile, chunk_e->fileaddr + 20, SEEK_SET);
		unsigned int e_addr = (unsigned int) ftell(map->directory->mapFile);
		fwrite(&chunkaddr, sizeof(unsigned int), 1, map->directory->mapFile);
		fseek(map->directory->mapFile, chunkaddr + 16, SEEK_SET);
		fwrite(&e_addr, sizeof(unsigned int), 1, map->directory->mapFile);
		cgeUnloadMapChunk(chunk_e, 0);
	}

	struct cgeMapChunk* chunk_w = cgeGetMapChunk(map, chunk->chunkPosX - 1, chunk->chunkPosY, chunk->chunkPosZ, 0);
	
	if(chunk_w) {
		fseek(map->directory->mapFile, chunk_w->fileaddr + 16, SEEK_SET);
		unsigned int w_addr = (unsigned int) ftell(map->directory->mapFile);
		fwrite(&chunkaddr, sizeof(unsigned int), 1, map->directory->mapFile);
		fseek(map->directory->mapFile, chunkaddr + 20, SEEK_SET);
		fwrite(&w_addr, sizeof(unsigned int), 1, map->directory->mapFile);
		cgeUnloadMapChunk(chunk_w, 0);
	}
	struct cgeMapChunk* chunk_n = cgeGetMapChunk(map, chunk->chunkPosX, chunk->chunkPosY + 1, chunk->chunkPosZ, 0);
	
	if(chunk_n) {
		fseek(map->directory->mapFile, chunk_n->fileaddr + 28, SEEK_SET);
		unsigned int n_addr = (unsigned int) ftell(map->directory->mapFile);
		fwrite(&chunkaddr, sizeof(unsigned int), 1, map->directory->mapFile);
		fseek(map->directory->mapFile, chunkaddr + 24, SEEK_SET);
		fwrite(&n_addr, sizeof(unsigned int), 1, map->directory->mapFile);
		cgeUnloadMapChunk(chunk_n, 0);
	}
	struct cgeMapChunk* chunk_s = cgeGetMapChunk(map, chunk->chunkPosX, chunk->chunkPosY - 1, chunk->chunkPosZ, 0);
	
	if(chunk_s) {
		fseek(map->directory->mapFile, chunk_s->fileaddr + 24, SEEK_SET);
		unsigned int s_addr = (unsigned int) ftell(map->directory->mapFile);
		fwrite(&chunkaddr, sizeof(unsigned int), 1, map->directory->mapFile);
		fseek(map->directory->mapFile, chunkaddr + 28, SEEK_SET);
		fwrite(&s_addr, sizeof(unsigned int), 1, map->directory->mapFile);
		cgeUnloadMapChunk(chunk_s, 0);
	}
	struct cgeMapChunk* chunk_u = cgeGetMapChunk(map, chunk->chunkPosX, chunk->chunkPosY, chunk->chunkPosZ + 1, 0);
	
	if(chunk_u) {
		fseek(map->directory->mapFile, chunk_u->fileaddr + 36, SEEK_SET);
		unsigned int u_addr = (unsigned int) ftell(map->directory->mapFile);
		fwrite(&chunkaddr, sizeof(unsigned int), 1, map->directory->mapFile);
		fseek(map->directory->mapFile, chunkaddr + 32, SEEK_SET);
		fwrite(&u_addr, sizeof(unsigned int), 1, map->directory->mapFile);
		cgeUnloadMapChunk(chunk_u, 0);
	}
	
	struct cgeMapChunk* chunk_d = cgeGetMapChunk(map, chunk->chunkPosX, chunk->chunkPosY, chunk->chunkPosZ - 1, 0);
	if(chunk_d) {
		fseek(map->directory->mapFile, chunk_d->fileaddr + 32, SEEK_SET);
		unsigned int d_addr = (unsigned int) ftell(map->directory->mapFile);
		fwrite(&chunkaddr, sizeof(unsigned int), 1, map->directory->mapFile);
		fseek(map->directory->mapFile, chunkaddr + 36, SEEK_SET);
		fwrite(&d_addr, sizeof(unsigned int), 1, map->directory->mapFile);
		cgeUnloadMapChunk(chunk_d, 0);
	}

	fflush(map->directory->mapFile);
}

void cgeUnloadMapChunk(struct cgeMapChunk* chunk, int load) {
	if(load) {
		//TODO execute chunk exit scripts
		//TODO map unloading
	}

	free(chunk->tiles);
	free(chunk);
}

struct cgeMapProperties* cgeCreateMapPropertiesFromScratch(struct cgeTileSet* tileset, char* displayName, char* scriptFile) {
	struct cgeMapProperties* properties = calloc(sizeof(struct cgeMapProperties), 1);
	
	if(tileset == NULL) {
		cgeCrash("Tried creating cgeMapProperties without a valid tileset.");
	}
	
	properties->tileSet = tileset;
	properties->displayName = displayName;
	properties->scriptFile = scriptFile;

	return properties;
}

struct cgeMap* cgeCreateMapFromScratch(struct cgeMapProperties* properties, struct cgeMapDirectory* directory, char* internalName) {
	struct cgeMap* map = calloc(sizeof(struct cgeMap), 1);

	if(properties == NULL) {
		cgeCrash("Tried creating cgeMap without properties.");
	}

	if(directory == NULL) {
		cgeCrash("Tried creating cgeMap without directory.");
	}

	map->properties = properties;
	map->directory = directory;
	map->internalName = internalName;

	return map;
}

struct cgeMapChunk* cgeCreateMapChunkFromScratch(short x, short y, short z) {
	struct cgeMapChunk* chunk = calloc(sizeof(struct cgeMapChunk), 1);

	chunk->chunkPosX = x;
	chunk->chunkPosY = y;
	chunk->chunkPosZ = z;

	chunk->tiles = calloc(sizeof(unsigned short), 32 * 32);
	return chunk;
}

