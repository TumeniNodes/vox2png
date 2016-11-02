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

/* MagicaVoxel's default color palette */
unsigned int defaultPalette[256] = {
	0x00000000, 0xffffffff, 0xffccffff, 0xff99ffff, 0xff66ffff, 0xff33ffff, 0xff00ffff, 0xffffccff, 0xffccccff, 0xff99ccff, 0xff66ccff, 0xff33ccff, 0xff00ccff, 0xffff99ff, 0xffcc99ff, 0xff9999ff,
	0xff6699ff, 0xff3399ff, 0xff0099ff, 0xffff66ff, 0xffcc66ff, 0xff9966ff, 0xff6666ff, 0xff3366ff, 0xff0066ff, 0xffff33ff, 0xffcc33ff, 0xff9933ff, 0xff6633ff, 0xff3333ff, 0xff0033ff, 0xffff00ff,
	0xffcc00ff, 0xff9900ff, 0xff6600ff, 0xff3300ff, 0xff0000ff, 0xffffffcc, 0xffccffcc, 0xff99ffcc, 0xff66ffcc, 0xff33ffcc, 0xff00ffcc, 0xffffcccc, 0xffcccccc, 0xff99cccc, 0xff66cccc, 0xff33cccc,
	0xff00cccc, 0xffff99cc, 0xffcc99cc, 0xff9999cc, 0xff6699cc, 0xff3399cc, 0xff0099cc, 0xffff66cc, 0xffcc66cc, 0xff9966cc, 0xff6666cc, 0xff3366cc, 0xff0066cc, 0xffff33cc, 0xffcc33cc, 0xff9933cc,
	0xff6633cc, 0xff3333cc, 0xff0033cc, 0xffff00cc, 0xffcc00cc, 0xff9900cc, 0xff6600cc, 0xff3300cc, 0xff0000cc, 0xffffff99, 0xffccff99, 0xff99ff99, 0xff66ff99, 0xff33ff99, 0xff00ff99, 0xffffcc99,
	0xffcccc99, 0xff99cc99, 0xff66cc99, 0xff33cc99, 0xff00cc99, 0xffff9999, 0xffcc9999, 0xff999999, 0xff669999, 0xff339999, 0xff009999, 0xffff6699, 0xffcc6699, 0xff996699, 0xff666699, 0xff336699,
	0xff006699, 0xffff3399, 0xffcc3399, 0xff993399, 0xff663399, 0xff333399, 0xff003399, 0xffff0099, 0xffcc0099, 0xff990099, 0xff660099, 0xff330099, 0xff000099, 0xffffff66, 0xffccff66, 0xff99ff66,
	0xff66ff66, 0xff33ff66, 0xff00ff66, 0xffffcc66, 0xffcccc66, 0xff99cc66, 0xff66cc66, 0xff33cc66, 0xff00cc66, 0xffff9966, 0xffcc9966, 0xff999966, 0xff669966, 0xff339966, 0xff009966, 0xffff6666,
	0xffcc6666, 0xff996666, 0xff666666, 0xff336666, 0xff006666, 0xffff3366, 0xffcc3366, 0xff993366, 0xff663366, 0xff333366, 0xff003366, 0xffff0066, 0xffcc0066, 0xff990066, 0xff660066, 0xff330066,
	0xff000066, 0xffffff33, 0xffccff33, 0xff99ff33, 0xff66ff33, 0xff33ff33, 0xff00ff33, 0xffffcc33, 0xffcccc33, 0xff99cc33, 0xff66cc33, 0xff33cc33, 0xff00cc33, 0xffff9933, 0xffcc9933, 0xff999933,
	0xff669933, 0xff339933, 0xff009933, 0xffff6633, 0xffcc6633, 0xff996633, 0xff666633, 0xff336633, 0xff006633, 0xffff3333, 0xffcc3333, 0xff993333, 0xff663333, 0xff333333, 0xff003333, 0xffff0033,
	0xffcc0033, 0xff990033, 0xff660033, 0xff330033, 0xff000033, 0xffffff00, 0xffccff00, 0xff99ff00, 0xff66ff00, 0xff33ff00, 0xff00ff00, 0xffffcc00, 0xffcccc00, 0xff99cc00, 0xff66cc00, 0xff33cc00,
	0xff00cc00, 0xffff9900, 0xffcc9900, 0xff999900, 0xff669900, 0xff339900, 0xff009900, 0xffff6600, 0xffcc6600, 0xff996600, 0xff666600, 0xff336600, 0xff006600, 0xffff3300, 0xffcc3300, 0xff993300,
	0xff663300, 0xff333300, 0xff003300, 0xffff0000, 0xffcc0000, 0xff990000, 0xff660000, 0xff330000, 0xff0000ee, 0xff0000dd, 0xff0000bb, 0xff0000aa, 0xff000088, 0xff000077, 0xff000055, 0xff000044,
	0xff000022, 0xff000011, 0xff00ee00, 0xff00dd00, 0xff00bb00, 0xff00aa00, 0xff008800, 0xff007700, 0xff005500, 0xff004400, 0xff002200, 0xff001100, 0xffee0000, 0xffdd0000, 0xffbb0000, 0xffaa0000,
	0xff880000, 0xff770000, 0xff550000, 0xff440000, 0xff220000, 0xff110000, 0xffeeeeee, 0xffdddddd, 0xffbbbbbb, 0xffaaaaaa, 0xff888888, 0xff777777, 0xff555555, 0xff444444, 0xff222222, 0xff111111
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
    puts("");
    puts("=== IMPORTANT ===");
    puts("If you're having trouble with the colors being off, change a color in the vox files color palette.'");
    puts("This forces MagicaVoxel to save the palette data ensuring that you'll get the right colors.");
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

