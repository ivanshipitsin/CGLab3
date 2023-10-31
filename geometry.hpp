#ifndef GEOMETRY_HPP
#define GEOMETRY_HPP
#include <glm/glm.hpp>


glm::mat4 get_id_mat();
glm::mat4 get_rotate_x_mat(double);
glm::mat4 get_rotate_y_mat(double);
glm::mat4 get_rotate_z_mat(double);
glm::mat4 get_rotate_xyz_mat(glm::vec3, double);
glm::mat4 get_shift_mat(glm::vec3 const&);
glm::mat4 get_perspective_proj(double z_c);
glm::vec4 cross4(glm::vec4,glm::vec4);


void test_rotate_x();
void test_rotate_z();
void test_rotate_y();
void test_shift();
#endif