#include "model.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

Model::Model(const char* filename)
    : verts_()
    , faces_() {
	std::ifstream in;
	in.open(filename, std::ifstream::in);
	if (in.fail())
		return;
	std::string line;
	while (!in.eof()) {
		std::getline(in, line);
		std::istringstream iss(line.c_str());
		char trash;
		if (!line.compare(0, 2, "v ")) {
			iss >> trash;
			Eigen::Vector3f v;
			for (int i = 0; i < 3; i++)
				iss >> v[i];
			verts_.push_back(v);
		} else if (!line.compare(0, 3, "vt ")) {
			float u, v;
			iss >> trash >> trash;
			iss >> u >> v;
			texcoords_.push_back(Eigen::Vector2f(u, 1 - v));
		} else if (!line.compare(0, 3, "vn ")) {
			Eigen::Vector3f v;
			iss >> trash >> trash;
			for (int i = 0; i < 3; i++)
				iss >> v[i];
			normals_.push_back(v);
		} else if (!line.compare(0, 2, "f ")) {
			std::vector<int> f;
			std::vector<int> tex;
			std::vector<int> norm;
			int idx, texidx, normidx;
			iss >> trash;
			while (iss >> idx >> trash >> texidx >> trash >> normidx) {
				idx--; // in wavefront obj all indices start at 1, not zero
				texidx--;
				normidx--;
				f.push_back(idx);
				tex.push_back(texidx);
				norm.push_back(normidx);
			}
			faces_.push_back(f);
			texs_.push_back(tex);
			norms_.push_back(norm);
		}
	}
	std::cerr << "# v# " << verts_.size() << " f# " << faces_.size()
	          << std::endl;
}

Model::~Model() {}

int Model::nverts() { return (int)verts_.size(); }

int Model::nfaces() { return (int)faces_.size(); }

std::vector<int> Model::face(int idx) { return faces_[idx]; }

std::vector<int> Model::tex_indexs(int idx) { return texs_[idx]; }

std::vector<int> Model::norm_indexs(int idx) { return norms_[idx]; }

Eigen::Vector2f Model::texcoord(int i) { return texcoords_[i]; }

Eigen::Vector3f Model::vert(int i) { return verts_[i]; }

Eigen::Vector3f Model::normal(int i) { return normals_[i]; }
