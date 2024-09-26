#ifndef _TRIANGLE_H_

#define _TRIANGLE_H_

#include <Eigen/Eigen>

class Triangle {
private:
	Eigen::Vector4f vertex[3];
	Eigen::Vector2f texcoord[3];
	Eigen::Vector3f normal[3];

public:
	Triangle() {};
	void setVertex(Eigen::Vector4f v, int k);
	void setNormal(Eigen::Vector4f v, int k);
	void setTexcoord(Eigen::Vector2f v, int k);

public:
	Eigen::Vector4f vert(int k) const;
	Eigen::Vector2f tex(int k) const;
	Eigen::Vector3f norm(int k) const;
	std::array<Eigen::Vector4f, 3ULL> toVec4() const;
};

#endif