#ifndef __MODEL_H__
#define __MODEL_H__

#include <Eigen/Eigen>
#include <vector>

class Model {
private:
	std::vector<Eigen::Vector3f> verts_;
	std::vector<Eigen::Vector2f> texcoords_;
	std::vector<Eigen::Vector3f> normals_;
	std::vector<std::vector<int>> faces_;
	std::vector<std::vector<int>> texs_;
	std::vector<std::vector<int>> norms_;

public:
	Model(const char* filename);
	~Model();
	Model& operator=(const Model& a);
	int nverts();
	int nfaces();
	Eigen::Vector3f vert(int i);
	Eigen::Vector2f texcoord(int i);
	Eigen::Vector3f normal(int i);
	std::vector<int> face(int idx);
	std::vector<int> tex_indexs(int idx);
	std::vector<int> norm_indexs(int idx);
};

#endif //__MODEL_H__