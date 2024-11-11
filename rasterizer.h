#ifndef _RASTERIZER_H_

#define _RASTERIZER_H_
#include "model.h"
#include "shader.h"
#include "triangle.h"
#include <eigen/include/eigen3/Eigen/Eigen>

class rasterizer {
private:
    int width, height;

    Eigen::Matrix4f modelMat;
    Eigen::Matrix4f viewMat;
    Eigen::Matrix4f projMat;
    Eigen::Matrix4f mvpMat;

    std::function<Eigen::Vector3f(fragment_shader_payload)> fragment_shader;

    std::optional<Texture> texture;    

    std::vector<float> depth_buf;
    std::vector<Eigen::Vector3f> frame_buf;

public:
    rasterizer(int width, int height);
    void clear();

    void setModelMat(Eigen::Matrix4f mat);
    void setViewMat(Eigen::Matrix4f mat);
    void setProjMat(Eigen::Matrix4f mat);
    void setFragmentShader(std::function<Eigen::Vector3f(fragment_shader_payload)> frag_shader);
    void setTexture(Texture tex);

    int get_index(int x, int y);
    Eigen::Vector3f getFrameBuf(int i);

    void render(std::vector<Triangle *> triangleList);
    void drawTriangle(const Triangle &t, std::array<Eigen::Vector3f, 3> &viewSpaceVertexs);
};

#endif