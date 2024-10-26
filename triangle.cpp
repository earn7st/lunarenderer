#include "triangle.h"
#include <Eigen/Eigen>

void Triangle::setVertex(Eigen::Vector4f v, int k) {
    this->vertex[k] = v;
}

void Triangle::setTexcoord(Eigen::Vector2f v, int k) {
    this->texcoord[k] = v;
}

void Triangle::setNormal(Eigen::Vector4f v, int k) {
    this->normal[k] = Eigen::Vector3f(v[0], v[1], v[2]);
}

Eigen::Vector4f Triangle::vert(int k) const {
    return this->vertex[k];
}

Eigen::Vector2f Triangle::tex(int k) const {
    return this->texcoord[k];
}

Eigen::Vector3f Triangle::norm(int k) const {
    return this->normal[k];
}

std::array<Eigen::Vector4f, 3ULL> Triangle::toVec4() const {
    std::array<Eigen::Vector4f, 3ULL> v;
    for (int i = 0; i < 3; i++) {
        v[i] = Eigen::Vector4f(vertex[i].x(), vertex[i].y(), vertex[i].z(), 1.0f);
    }
    return v;
}