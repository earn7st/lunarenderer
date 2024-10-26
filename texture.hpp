#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include "tgaimage.h"
#include <Eigen/Eigen>
#include <iostream>

extern const int width, height;

class Texture {
public:
    Texture(const std::string path) {
        std::cerr << img.read_tga_file(path.c_str()) ? "ok" : "fail";
    }
    Eigen::Vector3f getColor(float u, float v) {
        TGAColor color;
        float img_u = u * width;
        float img_v = (1 - v) * height;
        color = img.get(u * img.width(), v * img.height());
        return Eigen::Vector3f(color.bgra[2], color.bgra[1], color.bgra[0]);
    }

private:
    TGAImage img{};
};

#endif