/*
 * Vox2Png
 * Converts a MagicaVoxel Vox file into layered sprites and writes them to a Png file
 *
 * Written by Stijn Brouwer
 *
 * This is free and unencumbered software released into the public domain.
 *
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 *
 * In jurisdictions that recognize copyright laws, the author or authors
 * of this software dedicate any and all copyright interest in the
 * software to the public domain. We make this dedication for the benefit
 * of the public at large and to the detriment of our heirs and
 * successors. We intend this dedication to be an overt act of
 * relinquishment in perpetuity of all present and future rights to this
 * software under copyright law.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * For more information, please refer to <http://unlicense.org/>
 */

#include "stdlib.h"
#include "stdio.h"
#include "stddef.h"
#include "stdint.h"
#include "string.h"
#include "math.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

const uint32_t FileMagic   = 542658390;   /* V O X [space] */
const uint32_t FileVersion = 150;         /* MagicaVoxel 0.98 */

const uint32_t PackId      = 1262698832;  /* P A C K */
const uint32_t SizeId      = 1163544915;  /* S I Z E */
const uint32_t VoxelId     = 1230657880;  /* X Y Z I */
const uint32_t PaletteId   = 1094862674;  /* R G B A */

/* TODO: Replace this with a table of ints like in the spec:
   https://github.com/ephtracy/voxel-model/blob/master/MagicaVoxel-file-format-vox.txt
   The spec's color palette doesn't match with the actual color palette */

/* MagicaVoxel's default color palette */
const unsigned char defaultPalette[] = {
    "\378\377\377\377\377\377\314\377\377\377\231\377\377\377f\377\377\377\063"
    "\378\377\377\000\377\377\314\377\377\377\314\314\377\377\314\231\377\377\314"
    "f\378\377\314\063\377\377\314\000\377\377\231\377\377\377\231\314\377\377\231"
    "\232\377\377\231f\377\377\231\063\377\377\231\000\377\377f\377\377\377f\314"
    "\378\377f\231\377\377ff\377\377f\063\377\377f\000\377\377\063\377\377\377\063"
    "\315\377\377\063\231\377\377\063f\377\377\063\063\377\377\063\000\377\377\000\377"
    "\378\377\000\314\377\377\000\231\377\377\000f\377\377\000\063\377\377\000\000\377\314"
    "\378\377\377\314\377\314\377\314\377\231\377\314\377f\377\314\377\063\377"
    "\315\377\000\377\314\314\377\377\314\314\314\377\314\314\231\377\314\314f\377"
    "\315\314\063\377\314\314\000\377\314\231\377\377\314\231\314\377\314\231\231"
    "\378\314\231f\377\314\231\063\377\314\231\000\377\314f\377\377\314f\314\377"
    "\315f\231\377\314ff\377\314f\063\377\314f\000\377\314\063\377\377\314\063\314"
    "\378\314\063\231\377\314\063f\377\314\063\063\377\314\063\000\377\314\000\377\377"
    "\315\000\314\377\314\000\231\377\314\000f\377\314\000\063\377\314\000\000\377\231\377"
    "\378\377\231\377\314\377\231\377\231\377\231\377f\377\231\377\063\377\231"
    "\378\000\377\231\314\377\377\231\314\314\377\231\314\231\377\231\314f\377\231"
    "\315\063\377\231\314\000\377\231\231\377\377\231\231\314\377\231\231\231\377"
    "\232\231f\377\231\231\063\377\231\231\000\377\231f\377\377\231f\314\377\231"
    "f\232\377\231ff\377\231f\063\377\231f\000\377\231\063\377\377\231\063\314\377"
    "\232\063\231\377\231\063f\377\231\063\063\377\231\063\000\377\231\000\377\377\231"
    "\001\314\377\231\000\231\377\231\000f\377\231\000\063\377\231\000\000\377f\377\377\377"
    "f\378\314\377f\377\231\377f\377f\377f\377\063\377f\377\000\377f\314\377\377"
    "f\315\314\377f\314\231\377f\314f\377f\314\063\377f\314\000\377f\231\377\377"
    "f\232\314\377f\231\231\377f\231f\377f\231\063\377f\231\000\377ff\377\377ff\314"
    "\378ff\231\377fff\377ff\063\377ff\000\377f\063\377\377f\063\314\377f\063\231\377"
    "f\064f\377f\063\063\377f\063\000\377f\000\377\377f\000\314\377f\000\231\377f\000f\377f"
    "\001\063\377f\000\000\377\063\377\377\377\063\377\314\377\063\377\231\377\063\377f\377"
    "\064\377\063\377\063\377\000\377\063\314\377\377\063\314\314\377\063\314\231\377"
    "\064\314f\377\063\314\063\377\063\314\000\377\063\231\377\377\063\231\314\377\063"
    "\232\231\377\063\231f\377\063\231\063\377\063\231\000\377\063f\377\377\063f\314\377"
    "\064f\231\377\063ff\377\063f\063\377\063f\000\377\063\063\377\377\063\063\314\377\063"
    "\064\231\377\063\063f\377\063\063\063\377\063\063\000\377\063\000\377\377\063\000\314\377"
    "\064\000\231\377\063\000f\377\063\000\063\377\063\000\000\377\000\377\377\377\000\377\314\377"
    "\001\377\231\377\000\377f\377\000\377\063\377\000\377\000\377\000\314\377\377\000\314\314"
    "\378\000\314\231\377\000\314f\377\000\314\063\377\000\314\000\377\000\231\377\377\000\231"
    "\315\377\000\231\231\377\000\231f\377\000\231\063\377\000\231\000\377\000f\377\377\000f"
    "\315\377\000f\231\377\000ff\377\000f\063\377\000f\000\377\000\063\377\377\000\063\314\377"
    "\001\063\231\377\000\063f\377\000\063\063\377\000\063\000\377\000\000\377\377\000\000\314\377\000"
    "\001\231\377\000\000f\377\000\000\063\377\356\000\000\377\335\000\000\377\273\000\000\377\252\000"
    "\001\377\210\000\000\377w\000\000\377U\000\000\377D\000\000\377\"\000\000\377\021\000\000\377\000\356"
    "\001\377\000\335\000\377\000\273\000\377\000\252\000\377\000\210\000\377\000w\000\377\000U\000\377"
    "\001D\000\377\000\"\000\377\000\021\000\377\000\000\356\377\000\000\335\377\000\000\273\377\000\000\252"
    "\378\000\000\210\377\000\000w\377\000\000U\377\000\000D\377\000\000\"\377\000\000\021\377\356\356"
    "\357\377\335\335\335\377\273\273\273\377\252\252\252\377\210\210\210\377"
    "www\378UUU\377DDD\377\"\"\"\377\021\021\021\377\000\000\000\377"
};

/* Header of the .VOX file */
typedef struct {
    uint32_t magic, version;
} FileHeader;

/* Header of a chunk */
typedef struct {
    uint32_t id;
    uint32_t sizeContent;
    uint32_t sizeChildren;
} ChunkHeader;

/* A chunk that contains the number of models/keyframes */
typedef struct {
    ChunkHeader header;
    uint32_t numModels;
} PackChunk;

/* A chunk that contains the dimensions of a model */
typedef struct {
    ChunkHeader header;
    uint32_t x, y, z;
} SizeChunk;

/* A chunk that contains the voxels of a model */
typedef struct {
    ChunkHeader header;
    uint32_t numVoxels;
} VoxelChunk;

/* A chunk that contains the color palette */
typedef struct {
    ChunkHeader header;
    uint32_t colors[256];
} PaletteChunk;

/* A voxel in VoxelChunk */
typedef struct {
    uint8_t x, y, z;
    uint8_t colorIndex;
} Voxel;

/* Returns the a pointer to the first voxel in a VoxelChunk */
const Voxel *getVoxels(const VoxelChunk *voxelChunk) {
    return (const Voxel *) ((&voxelChunk->numVoxels) + 1);
}

/* Returns the color from the palette at the specified index */
uint32_t getColor(const uint32_t *palette, uint8_t index) {
    return palette[((int) index - 1 + 0xFF) % 0xFF];
}

/* A struct containing pointers to all the data we need from the .vox file */
typedef struct {
    /* The number of models the file contains */
    uint32_t numModels;
    /* An array of pointers to the size chunks */
    const SizeChunk **sizeChunks;
    /* An array of pointers to the voxel chunks */
    const VoxelChunk **voxelChunks;
    /* A pointer to the used palette */
    const uint32_t *palette;
} ParsedVox;

/* Parses a .vox file and puts the relevant data in a ParsedVox struct */
ParsedVox parseVox(size_t len, const char *buf) {
    /* Check the file header before we begin parsing */
    FileHeader *fHeader = (FileHeader *) buf;
    if (len < 8 || fHeader->magic != FileMagic) {
        fputs("Error: Vox file is corrupted\n", stderr);
        exit(-1);
    }
    if (fHeader->version > FileVersion) {
        fputs("Warning: Vox file is for a newer MagicaVoxel version than vox2png supports\n", stderr);
    }

    uint32_t numModels = 1;
    const SizeChunk **sizeChunks = NULL;
    int currentSize = 0;
    const VoxelChunk **voxelChunks = NULL;
    int currentVoxel = 0;
    const uint32_t *palette = (uint32_t *) &defaultPalette[0];

    /* Iterate the .vox file chunk by chunk */
    const ChunkHeader *chunk = (ChunkHeader *) (fHeader + 1);
    for (;;) {
        uint32_t id = chunk->id;

        /* Handle the chunk types we care about */
        if (id == PackId) {
            /* Store the amount of models */
            const PackChunk *pack = (PackChunk *) chunk;
            numModels = pack->numModels;
            printf("Found %i models\n", numModels);
        }
        else if (id == SizeId) {
            /* Create an array of SizeChunks if we don't have one already */
            if (sizeChunks == NULL) {
                sizeChunks = malloc(sizeof(SizeChunk *) * numModels);
            }
            /* Store the SizeChunk in the array */
            const SizeChunk *size = (SizeChunk *) chunk;
            sizeChunks[currentSize] = size;
            currentSize++;
        }
        else if (id == VoxelId) {
            /* Create an array of VoxelChunks if we don't have one already */
            if (voxelChunks == NULL) {
                voxelChunks = malloc(sizeof(VoxelChunk *) * numModels);
            }
            /* Store the VoxelChunk in the array */
            const VoxelChunk *voxels = (VoxelChunk *) chunk;
            voxelChunks[currentVoxel] = voxels;
            currentVoxel++;
        }
        else if (id == PaletteId) {
            /* Store the palette */
            const PaletteChunk *pal = (PaletteChunk *) chunk;
            palette = &pal->colors[0];
            puts("Found a palette");
        }

        /* Step to the next chunk */
        chunk = (ChunkHeader *) ((char *) chunk + sizeof(ChunkHeader) + chunk->sizeContent);
        /* Break if we're at the end of the file */
        if ((char *) chunk >= buf + len) break;
    }

    return (ParsedVox) {
        numModels, sizeChunks, voxelChunks, palette
    };
}

/* Frees the arrays allocated by parseVox */
void freeParsedVox(ParsedVox parsedVox) {
    free(parsedVox.sizeChunks);
    free(parsedVox.voxelChunks);
}

/* Allocates a buffer and reads the contents of the file into it */
void readFile(const char *path, size_t *len, char **buf) {
    FILE *handle = fopen(path, "rb");
    if (!handle) {
        fputs("Error: Could not read file, are you sure it exists?\n", stderr);
        exit(-1);
    }
    /* Get the length of the file */
    fseek(handle, 0, SEEK_END);
    *len = ftell(handle);
    fseek(handle, 0, SEEK_SET);
    /* Allocate a big enough buffer to fit the file */
    *buf = malloc(*len + 1);
    /* Read the file into the buffer and close it */
    fread(*buf, *len, 1, handle);
    fclose(handle);
}

void printUsage(void) {
    puts("Usage: vox2png INPUT.vox OUTPUT.png [PACKING-MODE]");
    puts("    Where INPUT.vox is the input file and OUTPUT.png is the output file name");
    puts("      * You should leave the .png away in OUTPUT when you're using either multifile or gamemaker");
    puts("    PACKING-MODE can be one of:");
    puts("      * animated puts depth on the X axis and keyframes on the Y axis");
    puts("        This is the only mode that supports animation keyframes");
    puts("      * horizontal puts all cells next to each other on the X axis of the sprite sheet.");
    puts("      * vertical does the same thing but on the Y axis.");
    puts("      * square goes left->right and top->bottom, like Minecraft's terrain.png.");
    puts("      * multifile makes a different file for each cell, don't put .png after the output file in this mode.");
    puts("      * gamemaker is the same as horizontal, but it adds _stripN after the filename,");
    puts("        which makes it easier to import in GameMaker. Don't put .png after the output file in this mode.");
    puts("    The default PACKING-MODE is animated");
}

/* Describes how the sprites should be packed into the sprite sheet */
typedef enum {
    /* Animation keyframes under eachother on the Y axis and
       depth next to eachother on the X axis */
    PM_ANIMATED = 0,
    /* Next to each other on the X axis */
    PM_HORIZONTAL,
    /* Next to each other on the Y axis */
    PM_VERTICAL,
    /* Left to right and top to bottom */
    PM_SQUARE,
    /* One file per sprite */
    PM_MULTIFILE,
    /* Like horizontal, but with _stripXX appended to the filename,
       where XX is the amount of sprites */
    PM_GAMEMAKER,

    PM_SIZE,
} PackingMode;

/* The strings used to get the PackingMode from the CLI arguments */
const char *packingModeStrings[PM_SIZE] = {
    "animated",
    "horizontal",
    "vertical",
    "square",
    "multifile",
    "gamemaker",
};

/* The parsed command line arguments */
typedef struct {
    const char *inFile;
    const char *outFile;
    PackingMode mode;
} CLArgs;

/* Parses the command line arguments */
CLArgs parseArgs(int argc, char **argv) {
    if (argc < 3 || argc > 4) {
        fputs("Error: Wrong number of arguments\n", stderr);
        printUsage();
        exit(-1);
    }
    if (argc >= 2 &&
            (strcmp(argv[1], "--help") == 0 ||
             strcmp(argv[1], "-h") == 0)) {
        printUsage();
        exit(0);
    }

    CLArgs args;
    args.inFile = argv[1];
    args.outFile = argv[2];
    args.mode = PM_ANIMATED;
    if (argc == 4) {
        for (int i = 0; i < PM_SIZE; ++i) {
            if (strcmp(argv[3], packingModeStrings[i]) == 0) {
                args.mode = i;
                return args;
            }
        }
        fputs("Error: Unknown packing mode\n", stderr);
        exit(-1);
    }
    return args;
}

/* An RGBA image */
typedef struct {
    uint32_t width, height;
    uint32_t *rgba;
} Image;

/* Frees the image data */
void freeImage(Image img) {
    free(img.rgba);
}

/* Makes a PM_ANIMATED sheet */
Image makeAnimatedSheet(ParsedVox vox) {
    /* Determine the size of the resulting sheet */
    uint32_t width = 0, height = 0;
    for (uint32_t i = 0; i < vox.numModels; ++i) {
        const SizeChunk *size = vox.sizeChunks[i];
        uint32_t thisWidth = size->x * size->z;
        if (thisWidth > width) width = thisWidth;
        height += size->y;
    }
    /* Allocate the image data */
    uint32_t *rgba = calloc(width * height, sizeof(uint32_t));

    /* Iterate over the keyframes */
    const SizeChunk *currentSize = NULL;
    int currentY = 0;
    for (uint32_t i = 0; i < vox.numModels; ++i) {
        currentSize = vox.sizeChunks[i];
        uint32_t voxelCount = vox.voxelChunks[i]->numVoxels;
        const Voxel *voxels = getVoxels(vox.voxelChunks[i]);

        /* Iterate over the voxels */
        for (uint32_t j = 0; j < voxelCount; ++j) {
            Voxel currentVoxel = voxels[j];

            int xOffset = currentVoxel.z * currentSize->x;
            int x = currentVoxel.x + xOffset;
            int y = currentY + currentVoxel.y;
            int index = x + y * width;

            rgba[index] = getColor(vox.palette, currentVoxel.colorIndex);
        }

        currentY += currentSize->y;
    }

    return (Image) {
        width, height,
        rgba
    };
}

/* Makes the other sheets */
Image makeSheet(ParsedVox vox, PackingMode mode) {
    /* TODO: Clean this function up */

    int width, height;
    int xCells;
    const SizeChunk *size = vox.sizeChunks[0];
    int voxXDim = (int) size->x;
    int voxYDim = (int) size->y;
    int voxZDim = (int) size->z;
    if (mode == PM_HORIZONTAL || mode == PM_GAMEMAKER) {
        width = voxXDim * voxZDim;
        height = voxYDim;
        xCells = voxZDim;
    } else if (mode == PM_VERTICAL || mode == PM_MULTIFILE) {
        width = voxXDim;
        height = voxYDim * voxZDim;
        xCells = 1;
    } else if (mode == PM_SQUARE) {
        int squareCells = (int) ceil(sqrt(voxZDim));
        width = voxXDim * squareCells;
        height = voxYDim * squareCells;
        xCells = squareCells;
    }

    const VoxelChunk *voxChunk = vox.voxelChunks[0];
    const Voxel *voxels = getVoxels(voxChunk);
    uint32_t *data = calloc(width * height, sizeof(uint32_t));
    for (uint32_t i = 0; i < voxChunk->numVoxels; ++i) {
        Voxel currentVoxel = voxels[i];

        int x = currentVoxel.x + currentVoxel.z % xCells * voxXDim;
        int y = currentVoxel.y + currentVoxel.z / xCells * voxYDim;
        int index = x + y * width;

        data[index] = getColor(vox.palette, currentVoxel.colorIndex);
    }

    return (Image) {
        width, height,
        data
    };
}

void writeImage(Image img, const char *path) {
    if (!stbi_write_png(path, img.width, img.height, 4, img.rgba, img.width * 4)) {
        fputs("Error: Failed to write png file\n", stderr);
    }
}

int main(int argc, char **argv) {
    CLArgs args = parseArgs(argc, argv);

    char *voxBuffer;
    size_t voxLength;
    readFile(args.inFile, &voxLength, &voxBuffer);

    ParsedVox parsed = parseVox(voxLength, voxBuffer);

    Image img;

    if (args.mode == PM_ANIMATED) {
        img = makeAnimatedSheet(parsed);
        writeImage(img, args.outFile);
    }
    else {
        /* TODO: Clean this up */

        img = makeSheet(parsed, args.mode);
        char nameBuffer[128];
        const SizeChunk *size = parsed.sizeChunks[0];

        if (args.mode == PM_MULTIFILE) {
            uint32_t *baseImg = img.rgba;

            for (uint32_t i = 0; i < size->z; ++i) {
                snprintf(nameBuffer, sizeof(nameBuffer) - 1, "%s%03i.png", args.outFile, (int) i);
                img.rgba = baseImg + size->x * size->y * i;
                writeImage(img, nameBuffer);
            }
            img.rgba = baseImg;
        }
        else if (args.mode == PM_GAMEMAKER) {
            snprintf(nameBuffer, sizeof(nameBuffer) - 1, "%s_strip%02i.png", args.outFile, size->z);
            writeImage(img, nameBuffer);
        }
        else {
            writeImage(img, args.outFile);
        }
    }

    puts("Done");

    freeImage(img);
    freeParsedVox(parsed);
    free(voxBuffer);
    return 0;
}

