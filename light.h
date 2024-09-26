#ifndef _LIGHT_H_
#define _LIGHT_H_

#include <Eigen/Eigen>

struct light {
    Eigen::Vector3f color;
    Eigen::Vector3f position;
    Eigen::Vector3f intensity;
};

#endif