#include "model.h"
#include "rasterizer.h"
#include "shader.hpp"
#include "tgaimage.h"
#include "triangle.h"
#include "texture.hpp"
#include "light.h"
#include <Eigen/Eigen>
#include <cmath>
#include <iostream>
#include <vector>
#include <algorithm>

const int width = 800;
const int height = 800;
const float MY_PI = 3.1415;
const int FOV = 45;
const float eyeRatio = 1;
const float zNear = 1;
const float zFar = 200;

const float angle = 0;
struct light li = {
    .color = Eigen::Vector3f(255.0, 255.0, 255.0),
    .position = Eigen::Vector3f(20, 20, 20),
    .intensity = Eigen::Vector3f(1000, 1000, 1000)};
const Eigen::Vector3f cameraPos{0, 0, 10};     // camera position
const Eigen::Vector3f cameraLookat{0, 0, -1};  // camera direction
const Eigen::Vector3f cameraUp{0, 1, 0};       // camera up vector

Eigen::Matrix4f getModelMat(float angle) {
    Eigen::Matrix4f rotation;
    angle = angle * MY_PI / 180.f;
    rotation << cos(angle), 0, sin(angle), 0, 0, 1, 0, 0, -sin(angle), 0, cos(angle), 0, 0, 0, 0, 1;

    Eigen::Matrix4f scale;
    scale << 2, 0, 0, 0, 0, 2, 0, 0, 0, 0, 2, 0, 0, 0, 0, 1;

    Eigen::Matrix4f translate;
    translate << 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1;

    return translate * rotation * scale;
}

Eigen::Matrix4f getViewMat(
    const Eigen::Vector3f cameraPos,
    const Eigen::Vector3f cameraLookat,
    const Eigen::Vector3f cameraUp) {
    Eigen::Matrix4f translate, rotate;

    Eigen::Vector3f crossGT = (cameraLookat.cross(cameraUp)).normalized();

    translate << 1, 0, 0, -cameraPos[0], 0, 1, 0, -cameraPos[1], 0, 0, 1, -cameraPos[2], 0, 0, 0, 1;

    rotate << crossGT[0], crossGT[1], crossGT[2], 0, cameraUp[0], cameraUp[1], cameraUp[2], 0, -cameraLookat[0],
        -cameraLookat[1], -cameraLookat[2], 0, 0, 0, 0, 1;

    return rotate * translate;
}

Eigen::Matrix4f getProjMat(float eye_fov, float aspect_ratio, float zNear, float zFar) {
    Eigen::Matrix4f projection = Eigen::Matrix4f::Identity();

    Eigen::Matrix4f Perst2Ortho;
    Eigen::Matrix4f Ortho, OrthoScale, OrthoMove;

    Perst2Ortho << zNear, 0, 0, 0, 0, zNear, 0, 0, 0, 0, zNear + zFar, -(zNear * zFar), 0, 0, 1, 0;

    float l, r, b, t, n, f;
    t = tan(eye_fov / 2 / 180.0 * MY_PI) * -zNear;
    b = -t;
    r = aspect_ratio * t;
    l = -r;
    n = zNear;
    f = zFar;

    OrthoScale << 2 / (r - l), 0, 0, 0, 0, 2 / (t - b), 0, 0, 0, 0, 2 / (n - f), 0, 0, 0, 0, 1;

    OrthoMove << 1, 0, 0, -(r + l) / 2, 0, 1, 0, -(t + b) / 2, 0, 0, 1, -(n + f) / 2, 0, 0, 0, 1;

    Ortho = OrthoScale * OrthoMove;

    projection = Ortho * Perst2Ortho * projection;

    return projection;
}

int main(int argc, char **argv) {
    /*
    if (argc < 2) {
        printf("Usage: .\\lunarenderer <model1_path> <model2_path> ... ");
        return 0;
    }
    */

    const char *obj_path = "../models/african_head/african_head.obj";
    const char *texture_path = "../models/african_head/african_head_diffuse.tga";

    rasterizer r(width, height);
    r.clear();

    std::function<Eigen::Vector3f(fragment_shader_payload)> active_shader = blinn_phong_fragment_shader;

    r.setModelMat(getModelMat(angle));
    r.setViewMat(getViewMat(cameraPos, cameraLookat, cameraUp));
    r.setProjMat(getProjMat(FOV, eyeRatio, zNear, zFar));

    r.setFragmentShader(active_shader);

    r.setTexture(Texture(texture_path));

    std::vector<Triangle *> triangleList;

    Model m(obj_path);
    for (int j = 0; j < m.nfaces(); j++) {
        Triangle *t = new Triangle();
        for (int k = 0; k < 3; k++) {
            t->setVertex(
                Eigen::Vector4f(m.vert(m.face(j)[k])[0], m.vert(m.face(j)[k])[1], m.vert(m.face(j)[k])[2], 1.0f), k);
            t->setTexcoord(m.texcoord(m.tex_indexs(j)[k]), k);
            t->setNormal(
                Eigen::Vector4f(
                    m.normal(m.norm_indexs(j)[k])[0],
                    m.normal(m.norm_indexs(j)[k])[1],
                    m.normal(m.norm_indexs(j)[k])[2],
                    0.0f),
                k);
        }

        triangleList.push_back(t);
    }

    r.render(triangleList);

    FILE *fp = fopen("binary.ppm", "wb");

    fprintf(fp, "P6\n%d %d\n255\n", width, height);

    for (int x = height - 1; x >= 0; x--) {
        for (int y = 0; y < width; y++) {
            int ind = x * width + y;
            unsigned char color[3];
            color[0] = (unsigned char)(r.getFrameBuf(ind).x());
            color[1] = (unsigned char)(r.getFrameBuf(ind).y());
            color[2] = (unsigned char)(r.getFrameBuf(ind).z());

            fwrite(color, 1, 3, fp);
        }
    }
    fclose(fp);

    return 0;
}
