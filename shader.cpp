#include <Eigen/Eigen>
#include "rasterizer.h"
#include "light.h"
#include "shader.hpp"

extern struct light li;

Eigen::Vector3f blinn_phong_fragment_shader(const fragment_shader_payload &payload) {
    Eigen::Vector3f eye_pos = {0, 0, 0};
    Eigen::Vector3f ka = Eigen::Vector3f(0.005, 0.005, 0.005);
    Eigen::Vector3f amb_light_intensity{10, 10, 10};
    Eigen::Vector3f kd = payload.color;
    Eigen::Vector3f ks = Eigen::Vector3f(0.7937, 0.7937, 0.7937);

    Eigen::Vector3f color = payload.color;
    Eigen::Vector2f tc = payload.texcoord;
    Eigen::Vector3f point = payload.viewspace_pos;
    Eigen::Vector3f normal = payload.normal;

    Eigen::Vector3f v = eye_pos - point;
    Eigen::Vector3f l = li.position - point;
    Eigen::Vector3f h = (v + l).normalized();
    float r_2 = std::pow(l.norm(), 2);
    float p = 150;

    Eigen::Vector3f ambient = (ka.array() * amb_light_intensity.array()).matrix();
    Eigen::Vector3f diffuse =
        (kd.array() * (li.intensity / r_2).array() * std::max(0.0f, normal.normalized().dot(l.normalized()))).matrix();
    Eigen::Vector3f specular = (ks.array() * (li.intensity / r_2).array() *
                                std::pow(std::max(0.0f, normal.normalized().dot(h.normalized())), p))
                                   .matrix();

    Eigen::Vector3f result_color;
    result_color += ambient + diffuse + specular;

    return result_color;
}