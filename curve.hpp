#ifndef CURVE_HPP
#define CURVE_HPP
#include <vector>
#include <array>
#include <iostream>
#include <algorithm>
#include <glm/glm.hpp>

glm::vec2 bezier_curve_3(std::array<glm::vec2, 4> const& P, float t);


#endif