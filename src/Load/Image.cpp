#include "Image.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <iostream>
#include <memory>

Image::Image(int width, int height, const std::vector<char>& data, Format format)
    : width(width), height(height), data(data), format(format) { }

Image Image::LoadImage(const std::string& path) {
    int x, y, channels;
    // Load image with filename
    std::unique_ptr<stbi_uc> data = std::unique_ptr<stbi_uc>(stbi_load(("res/textures/" + path).c_str(), &x, &y, &channels, 0));

    Format format;
    switch (channels) {
        case 3:
            format = RGB;
            break;

        case 4:
            format = RGBA;
            break;

        default:
            std::cout << "Incorrect number of channels (" + std::to_string(channels) + ") in file: " + path << std::endl;
            std::exit(EXIT_FAILURE);
    }

    int size = x * y * channels;
    return Image(x, y, std::vector<char>{data.get(), data.get() + size}, format);
}
