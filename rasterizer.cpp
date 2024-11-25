#include "lunarender.h"

extern TGAImage framebuffer;

rasterizer::rasterizer(int width, int height) : width(width), height(height) {
    frame_buf.resize(width * height);
    depth_buf.resize(width * height);
}

void rasterizer::clear() {
    std::fill(frame_buf.begin(), frame_buf.end(), Eigen::Vector3f{0, 0, 0});
    std::fill(depth_buf.begin(), depth_buf.end(),
              std::numeric_limits<float>::infinity());
}

void rasterizer::setModelMat(Eigen::Matrix4f mat) { this->modelMat = mat; }
void rasterizer::setViewMat(Eigen::Matrix4f mat) { this->viewMat = mat; }
void rasterizer::setProjMat(Eigen::Matrix4f mat) { this->projMat = mat; }

void rasterizer::setFragmentShader(
    std::function<Eigen::Vector3f(fragment_shader_payload)> frag_shader) {
    fragment_shader = frag_shader;
}

void rasterizer::setTexture(Texture tex) { texture = tex; }

int rasterizer::get_index(int x, int y) { return y * width + x; }

Eigen::Vector3f rasterizer::getFrameBuf(int i) { return frame_buf[i]; }

bool insideTriangle(float x, float y, std::array<Eigen::Vector4f, 3ULL> &_v) {
    Eigen::Vector3f v[3];
    for (int i = 0; i < 3; i++)
        v[i] = {_v[i].x(), _v[i].y(), 1.0};
    Eigen::Vector3f f0, f1, f2;
    f0 = v[1].cross(v[0]);
    f1 = v[2].cross(v[1]);
    f2 = v[0].cross(v[2]);
    Eigen::Vector3f p(x, y, 1.);
    if ((p.dot(f0) * f0.dot(v[2]) > 0) && (p.dot(f1) * f1.dot(v[0]) > 0) &&
        (p.dot(f2) * f2.dot(v[1]) > 0))
        return true;
    return false;
}

static std::tuple<float, float, float>
computeBarycentric2D(float x, float y, std::array<Eigen::Vector4f, 3ULL> &v) {
    float c1 =
        (x * (v[1].y() - v[2].y()) + (v[2].x() - v[1].x()) * y +
         v[1].x() * v[2].y() - v[2].x() * v[1].y()) /
        (v[0].x() * (v[1].y() - v[2].y()) + (v[2].x() - v[1].x()) * v[0].y() +
         v[1].x() * v[2].y() - v[2].x() * v[1].y());
    float c2 =
        (x * (v[2].y() - v[0].y()) + (v[0].x() - v[2].x()) * y +
         v[2].x() * v[0].y() - v[0].x() * v[2].y()) /
        (v[1].x() * (v[2].y() - v[0].y()) + (v[0].x() - v[2].x()) * v[1].y() +
         v[2].x() * v[0].y() - v[0].x() * v[2].y());
    float c3 =
        (x * (v[0].y() - v[1].y()) + (v[1].x() - v[0].x()) * y +
         v[0].x() * v[1].y() - v[1].x() * v[0].y()) /
        (v[2].x() * (v[0].y() - v[1].y()) + (v[1].x() - v[0].x()) * v[2].y() +
         v[0].x() * v[1].y() - v[1].x() * v[0].y());
    return {c1, c2, c3};
}

static Eigen::Vector3f interpolate(float alpha, float beta, float gamma,
                                   const Eigen::Vector3f &vert1,
                                   const Eigen::Vector3f &vert2,
                                   const Eigen::Vector3f &vert3, float weight) {
    return (alpha * vert1 + beta * vert2 + gamma * vert3) / weight;
}

static Eigen::Vector2f interpolate(float alpha, float beta, float gamma,
                                   const Eigen::Vector2f &vert1,
                                   const Eigen::Vector2f &vert2,
                                   const Eigen::Vector2f &vert3, float weight) {
    auto u = (alpha * vert1[0] + beta * vert2[0] + gamma * vert3[0]);
    auto v = (alpha * vert1[1] + beta * vert2[1] + gamma * vert3[1]);

    u /= weight;
    v /= weight;

    return Eigen::Vector2f(u, v);
}

void rasterizer::render(std::vector<Triangle *> triangleList) {
    Eigen::Matrix4f mvpMat = projMat * viewMat * modelMat;
    Eigen::Matrix4f Object2ViewNormal =
        (viewMat * modelMat).inverse().transpose();
    for (int i = 0; i < triangleList.size(); i++) {
        std::clog << "\rRender Process: " << std::setprecision(4)
                  << (float(i + 1) / triangleList.size()) * 100 << "%   "
                  << std::flush;
        Triangle *t = triangleList[i];
        Triangle newTri = *t;
        Eigen::Vector4f v[] = {mvpMat * t->vert(0), mvpMat * t->vert(1),
                               mvpMat * t->vert(2)};

        std::array<Eigen::Vector4f, 3> viewSpaceVertexs4 = {
            (viewMat * modelMat) * t->vert(0),
            (viewMat * modelMat) * t->vert(1),
            (viewMat * modelMat) * t->vert(2)};

        std::array<Eigen::Vector3f, 3> viewSpaceVertexs3;

        std::transform(viewSpaceVertexs4.begin(), viewSpaceVertexs4.end(),
                       viewSpaceVertexs3.begin(),
                       [](auto &v) { return v.template head<3>(); });

        for (auto &vec : v) {
            vec.x() /= vec.w();
            vec.y() /= vec.w();
            vec.z() /= vec.w();
            vec.w() = 1.0f;
        }

        for (auto &vec : v) {
            vec.x() = (vec.x() + 1) * width * 0.5;
            vec.y() = (vec.y() + 1) * height * 0.5;
        }

        Eigen::Vector4f n[] = {
            Object2ViewNormal * Eigen::Vector4f(t->norm(0).x(), t->norm(0).y(),
                                                t->norm(0).z(), 0.0f),
            Object2ViewNormal * Eigen::Vector4f(t->norm(1).x(), t->norm(1).y(),
                                                t->norm(1).z(), 0.0f),
            Object2ViewNormal * Eigen::Vector4f(t->norm(2).x(), t->norm(2).y(),
                                                t->norm(2).z(), 0.0f)};

        for (int j = 0; j < 3; j++) {
            newTri.setVertex(v[j], j);
            newTri.setNormal(n[j], j);
        }
        this->drawTriangle(newTri, viewSpaceVertexs3);
    }
    std::clog << "\rDone.               \n";
    return;
}

void rasterizer::drawTriangle(
    const Triangle &t, std::array<Eigen::Vector3f, 3> &viewSpaceVertexs) {

    std::array<Eigen::Vector4f, 3> v = t.toVec4();

    float minPix[2] = {float(width + 1), float(height + 1)}, maxPix[2] = {0, 0};
    for (int i = 0; i < 3; i++) {
        minPix[0] = std::max(0.f, std::min(minPix[0], v[i].x()));
        minPix[1] = std::max(0.f, std::min(minPix[1], v[i].y()));
        maxPix[0] = std::min(float(width - 1), std::max(maxPix[0], v[i].x()));
        maxPix[1] = std::min(float(height - 1), std::max(maxPix[1], v[i].y()));
    }
    for (int y = int(minPix[1]); y <= int(maxPix[1]); y++) {
        for (int x = int(minPix[0]); x <= int(maxPix[0]); x++) {
            float cx = x + 0.5;
            float cy = y + 0.5;
            if (insideTriangle(cx, cy, v)) {
                auto [alpha, beta, gamma] = computeBarycentric2D(cx, cy, v);
                float Z = 1.0 / (alpha / v[0].w() + beta / v[1].w() +
                                 gamma / v[2].w());
                float zp = alpha * v[0].z() / v[0].w() +
                           beta * v[1].z() / v[1].w() +
                           gamma * v[2].z() / v[2].w();
                zp *= Z;
                int ind = get_index(x, y);
                if (depth_buf[ind] > zp) {
                    depth_buf[ind] = zp;
                    Eigen::Vector3f interpolated_color =
                        Eigen::Vector3f{200, 200, 200};

                    Eigen::Vector2f interpolated_texcoords = interpolate(
                        alpha, beta, gamma, t.tex(0), t.tex(1), t.tex(2), 1);

                    Eigen::Vector3f interpolated_normals = interpolate(
                        alpha, beta, gamma, t.norm(0), t.norm(1), t.norm(2), 1);

                    Eigen::Vector3f interpolated_vs_pos = interpolate(
                        alpha, beta, gamma, viewSpaceVertexs[0],
                        viewSpaceVertexs[1], viewSpaceVertexs[2], 1);

                    fragment_shader_payload payload(
                        interpolated_color, interpolated_normals.normalized(),
                        interpolated_texcoords, interpolated_vs_pos,
                        texture ? &*texture : nullptr);

                    Eigen::Vector3f pixel_color = fragment_shader(payload);
                    frame_buf[ind] = pixel_color;
                }
            }
        }
    }

    return;
}