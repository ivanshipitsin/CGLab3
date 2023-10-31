#include "geometry.hpp"
#include <iostream>


glm::mat4 get_id_mat()
{
    return glm::mat4{glm::vec4{1,0,0,0}, 
                     glm::vec4{0,1,0,0}, 
                     glm::vec4{0,0,1,0}, 
                     glm::vec4{0,0,0,1}};
}

glm::mat4 get_rotate_x_mat( double alpha)
{
    return glm::mat4{glm::vec4{1, 0, 0, 0},
                         glm::vec4{0, glm::cos(alpha), glm::sin(alpha), 0},
                         glm::vec4{0, -glm::sin(alpha), glm::cos(alpha), 0},
                         glm::vec4{0, 0, 0, 1}};
}
glm::mat4 get_rotate_z_mat( double alpha)
{
    return glm::mat4{glm::vec4{glm::cos(alpha), glm::sin(alpha), 0, 0},
                         glm::vec4{-glm::sin(alpha), glm::cos(alpha), 0, 0},
                         glm::vec4{0, 0, 1, 0},
                         glm::vec4{0, 0, 0, 1}};
}
glm::mat4 get_rotate_y_mat( double alpha)
{
    return glm::mat4{glm::vec4{glm::cos(alpha), 0, -glm::sin(alpha), 0},
                         glm::vec4{0, 1, 0, 0},
                         glm::vec4{glm::sin(alpha), 0, glm::cos(alpha), 0},
                         glm::vec4{0, 0, 0, 1}};
}

glm::mat4 get_rotate_xyz_mat(glm::vec3 c, double gamma)
{
    double d = glm::sqrt(c.x*c.x + c.z*c.z);
    glm::mat4 alpha_rotate{ glm::vec4{c.x / d, 0, -c.z / d, 0},
                            glm::vec4{      0, 1,        0, 0},
                            glm::vec4{c.z / d, 0,  c.x / d, 0},
                            glm::vec4{      0, 0,        0, 1}};
    glm::mat4 alpha_rotate_inv{ glm::vec4{c.x / d, 0, c.z / d, 0},
                            glm::vec4{      0, 1,        0, 0},
                            glm::vec4{-c.z / d, 0,  c.x / d, 0},
                            glm::vec4{      0, 0,        0, 1}};
    double d_d = glm::sqrt(c.y*c.y + d*d);
    
    glm::mat4 beta_rotate{glm::vec4{c.y / d_d, d / d_d, 0, 0},
                         glm::vec4{-d / d_d, c.y / d_d, 0, 0},
                         glm::vec4{0, 0, 1, 0},
                         glm::vec4{0, 0, 0, 1}};
    
    glm::mat4 beta_rotate_inv{glm::vec4{c.y / d_d, -d / d_d, 0, 0},
                         glm::vec4{d / d_d, c.y / d_d, 0, 0},
                         glm::vec4{0, 0, 1, 0},
                         glm::vec4{0, 0, 0, 1}};

    glm::mat4 temp_mat{glm::vec4{glm::cos(gamma), 0, -glm::sin(gamma), 0},
                         glm::vec4{0, 1, 0, 0},
                         glm::vec4{glm::sin(gamma), 0, glm::cos(gamma), 0},
                         glm::vec4{0, 0, 0, 1}};

    return alpha_rotate_inv * beta_rotate_inv * temp_mat * beta_rotate * alpha_rotate;
                            

}

glm::mat4 get_shift_mat(glm::vec3 const& shift_vec){
    return glm::mat4{glm::vec4{1,0,0,0},
                     glm::vec4{0,1,0,0},
                     glm::vec4{0,0,1,0},
                     glm::vec4{shift_vec.x, shift_vec.y, shift_vec.z, 1}};
}

glm::mat4 get_perspective_proj(double z_c)
{
    return glm::mat4{glm::vec4{1,0,0,0},
                     glm::vec4{0,1,0,0},
                     glm::vec4{0,0,1,-1/z_c},
                     glm::vec4{0, 0, 0, 1}};
}

glm::vec4 cross4(glm::vec4 a, glm::vec4 b)
{
    return {a.y*b.z - a.z*b.y, 
            a.z*b.x - a.x*b.z, 
            a.x*b.y - a.y*b.x,
            0};
}

void test_rotate_x()
{
    std::cout << "TEST ROTATE_X" << std::endl;
    glm::vec4 test_point{0, 1, 0, 1};
    glm::vec4 res = get_rotate_x_mat(M_PI / 2.) * test_point;
    if (glm::abs(res.z - 1) > 1e-10)
    {
        std::cerr << "Error in rotate_x" << std::endl;
    }

    std::cout << "Start point:" << std::endl;
    for (int i = 0; i < 4; i++)
    {
        std::cout << test_point[i] << " ";
    }
    std::cout << std::endl;

    std::cout << "End point:" << std::endl;
    for (int i = 0; i < 4; i++)
    {
        std::cout << res[i] << " ";
    }
    std::cout << std::endl;
}

void test_rotate_z()
{
    std::cout << "TEST ROTATE_Z" << std::endl;
    glm::vec4 test_point{0, 1, 0, 1};
    glm::vec4 res = get_rotate_z_mat(M_PI/2.)*test_point;
    if (glm::abs(res.x + 1) > 1e-10)
    {
        std::cerr << "Error in rotate_z" << std::endl;
    }

    std::cout << "Start point:" << std::endl;
    for (int i = 0; i < 4; i++)
    {
        std::cout << test_point[i] << " ";
    }
    std::cout << std::endl;

    std::cout << "End point:" << std::endl;
    for (int i = 0; i < 4; i++)
    {
        std::cout << res[i] << " ";
    }
    std::cout << std::endl;
}

void test_rotate_y()
{
    std::cout << "TEST ROTATE_Y" << std::endl;
    glm::vec4 test_point{0, 0, 1, 1};
    glm::vec4 res = get_rotate_y_mat(M_PI/2.)*test_point;
    if (glm::abs(res.x - 1) > 1e-10)
    {
        std::cerr << "Error in rotate_y" << std::endl;
    }

    std::cout << "Start point:" << std::endl;
    for (int i = 0; i < 4; i++)
    {
        std::cout << test_point[i] << " ";
    }
    std::cout << std::endl;

    std::cout << "End point:" << std::endl;
    for (int i = 0; i < 4; i++)
    {
        std::cout << res[i] << " ";
    }
    std::cout << std::endl;
}

void test_shift(){
    std::cout << "TEST SHIFT" << std::endl;
    glm::vec4 test_point{0, 0, 1, 1};
    glm::vec4 res = get_shift_mat(glm::vec3{1,1,1}) * test_point;
    if (glm::abs(res.x - 1) > 1e-10)
    {
        std::cerr << "Error in rotate_y" << std::endl;
    }

    std::cout << "Start point:" << std::endl;
    for (int i = 0; i < 4; i++)
    {
        std::cout << test_point[i] << " ";
    }
    std::cout << std::endl;

    std::cout << "End point:" << std::endl;
    for (int i = 0; i < 4; i++)
    {
        std::cout << res[i] << " ";
    }
    std::cout << std::endl;
}