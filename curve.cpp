#include "curve.hpp"

static const glm::mat4x4 BEZIER_MAT{glm::vec4{-1,  3, -3, 1},
                                    glm::vec4{ 3, -6,  3, 0},
                                    glm::vec4{-3,  3,  0, 0},
                                    glm::vec4{ 1,  0,  0, 0}};

glm::vec2 bezier_curve_3(std::array<glm::vec2, 4> const& P, float t)
{
    glm::vec4 temp_t_vec{t*t*t, t*t, t, 1};
    glm::vec4 p_x;
    glm::vec4 p_y;
    for(int i = 0; i < 4; i++)
    {
        p_x[i] = P[i].x;
        p_y[i] = P[i].y;
    }

    return glm::vec2{glm::dot(temp_t_vec, BEZIER_MAT*p_x), glm::dot(temp_t_vec, BEZIER_MAT*p_y)};

}