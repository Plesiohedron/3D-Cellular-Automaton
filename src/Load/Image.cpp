#include "Image.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <stdio.h>

#ifdef _WIN32
#define PATH_MAX _MAX_PATH
#endif

#ifdef __unix__
#include <limits.h>
#endif 

Image::Image(int width, int height, unsigned char* data, Format format)
    : width(width), height(height), data(data), format(format) { }

Image::~Image() {
    delete[] data;
}

Image Image::LoadImage(const char* filename) {
    char full_path[PATH_MAX];  
    snprintf(full_path, sizeof(full_path), "%s%s", "./res/textures/", filename);

    int x, y, channels;
    // Load raw image data with filename
    stbi_uc* data = stbi_load(full_path, &x, &y, &channels, 0);

    Format format;
    switch (channels) {
        case 3:
            format = RGB;
            break;

        case 4:
            format = RGBA;
            break;

        default:
            fprintf(stderr, "%s\n", full_path);
            fprintf(stderr, "Wrong image format.\n");
            return Image(0, 0, data, ERR);
    }

    int size = x * y * channels;
    return Image(x, y, data, format);
}
