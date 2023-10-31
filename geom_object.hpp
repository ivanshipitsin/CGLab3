#ifndef GEOM_OBJECT_HPP
#define GEOM_OBJECT_HPP
#include <vector>
#include <array>
#include <glm/glm.hpp>

struct geom_object{
    std::vector<glm::vec4> vertex_buffer;
    std::vector<std::array<uint32_t, 3>> faces_buffer;
    std::vector<glm::vec4> norm_buffer;
};

struct geom_object2{
    std::vector<glm::vec2> vertex_buffer;
    std::vector<std::array<uint32_t, 3>> faces_buffer;
    std::vector<glm::vec4> temp_norm_buffer;
};

inline glm::vec3 vec4_to_vec3(glm::vec4 const& a)
{
    return glm::vec3{a.x,a.y,a.z};
}

inline glm::vec4 vec3_to_vec4(glm::vec3 const& a)
{
    return glm::vec4{a.x,a.y,a.z, 0};
}


#endif 