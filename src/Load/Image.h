#pragma once

class Image {
public:
    enum Format {RGB, RGBA, ERR};

public:
    Format format;
    int width;
    int height;
    unsigned char* data;

public:
    Image(int width, int height, unsigned char* data, Format format);
    ~Image();

    static Image LoadImage(const char* filename);
};
