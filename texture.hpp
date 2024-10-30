#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include "tgaimage.h"
#include <iostream>

extern const int width, height;

class Texture {
public:
    Texture(const std::string path) {
        std::cerr << (img.read_tga_file(path.c_str()) ? "ok\n" : "fail\n");
    }
    TGAColor getColor(float u, float v) {
        return img.get(u * img.width(), v * img.height());
    }

private:
    TGAImage img{};
};

#endif