#pragma once

#include <vector>
#include <string>


class Image {
public:
    enum Format {RGB, RGBA};

public:
    Format format;
    int width;
    int height;
    std::vector<char> data;

public:
    Image(int width, int height, const std::vector<char>& data, Format format);
    static Image LoadImage(const std::string& path);
};
