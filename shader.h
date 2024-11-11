#ifndef _SHADER_H_
#define _SHADER_H_

#include "texture.hpp"

struct fragment_shader_payload {
    fragment_shader_payload() {
        texture = nullptr;
    }

    fragment_shader_payload(
        const Eigen::Vector3f& col,
        const Eigen::Vector3f& nor,
        const Eigen::Vector2f& tc,
        const Eigen::Vector3f& vs_pos,
        Texture* tex) :
            color(col), normal(nor), texcoord(tc), viewspace_pos(vs_pos), texture(tex) {}

    Eigen::Vector3f viewspace_pos;
    Eigen::Vector3f color;
    Eigen::Vector3f normal;
    Eigen::Vector2f texcoord;
    Texture* texture;
};

Eigen::Vector3f phong_fragment_shader(const fragment_shader_payload& payload);
Eigen::Vector3f blinn_phong_fragment_shader(const fragment_shader_payload& payload);
Eigen::Vector3f texture_fragment_shader(const fragment_shader_payload& payload);

#endif