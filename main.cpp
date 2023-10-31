#include <iostream>
#include <vector>
#include <Magick++.h>
#include "graphics.hpp"
#include "geometry.hpp"
#include "curve.hpp"
#include "geom_object.hpp"
#include <glm/glm.hpp>
#include <math.h>

void bresenham_wa(glm::ivec2 start, glm::ivec2 end, Magick::Image & img, Magick::Color const & col) {
    int dy = std::abs(end.y - start.y);
    int dx = std::abs(end.x - start.x);

    // for coding octant
    bool change_dir = (dx > dy)? true: false;
    int diry = (end.y > start.y)? 1:-1;
    int dirx = (end.x > start.x)? 1:-1;

    if(!change_dir)
        std::swap(dx,dy);

    int err = -dx + 2*dy;
    glm::ivec2 curr_point = start;

    for(int i = 1; i <= dx; i++)
    {
        img.pixelColor(curr_point.x,curr_point.y,col);
        while(err >= 0) {
            change_dir ?  curr_point.y+=diry : curr_point.x+=dirx;
            err -= 2*dx;
        }
        
        change_dir ? curr_point.x+=dirx:curr_point.y+=diry;
        err+= 2*dy;
    }
    img.pixelColor(curr_point.x,curr_point.y,col);
}

void draw_polygon(polygon const& p, Magick::Image & img, Magick::Color const& col) {
    int n = p.points.size();
    for(int i = 0; i < n; i++)
    {
        bresenham_wa(p.points[i], p.points[(i+1)%n], img, col);
    }
}


void filled_polygon_even_odd(polygon const& p, Magick::Image & img, Magick::Color const& col) {
    int x_min = 1000000;
    int y_min = 1000000;
    int y_max = -1;
    int x_max = -1;
    int n = p.points.size();
    for(int i = 0; i < p.points.size(); i++) {
        if(p.points[i].x < x_min){
            x_min = p.points[i].x;
        }
        if(p.points[i].x > x_max){
            x_max = p.points[i].x;
        }
        if(p.points[i].y < y_min){
            y_min = p.points[i].y;
        }
        if(p.points[i].y > y_max){
            y_max = p.points[i].y;
        }
    }
    for(int y=y_min; y <= y_max; y++) {
        for(int x = x_min; x <= x_max; x++) {
            int N = 0;
            for(int k = 0; k < n; k++) {
                auto res = is_intersection(line{glm::ivec2{x, y}, glm::ivec2{x_max, y}},
                                            line{p.points[k], p.points[(k + 1) % n]});
                if(res.class_ == classification_intersection::INTERSECT_BY_SEGM &&
                    (glm::length(res.point_intersection - glm::vec2{x, y}) > PRESISION)) {
                    N++;
                }
            }
            if(N % 2 == 1){
                img.pixelColor(x, y, col);
            }
        }
    }
}

void filled_polygon_nzw(polygon const& p, Magick::Image &img, Magick::Color const& col) {
    int x_min = 1000000;
    int y_min = 1000000;
    int y_max = -1;
    int x_max = -1;
    int n = p.points.size();
    for(int i = 0; i < p.points.size(); i++) {
        if(p.points[i].x < x_min){
            x_min = p.points[i].x;
        }
        if(p.points[i].x > x_max){
            x_max = p.points[i].x;
        }
        if(p.points[i].y < y_min){
            y_min = p.points[i].y;
        }
        if(p.points[i].y > y_max){
            y_max = p.points[i].y;
        }
    }
    for(int x = x_min; x <= x_max; x++) {
        for(int y = y_min; y <= y_max; y++) {
            int wn = 0;
            for(int k = 0; k < n; k++) {
                auto res = is_intersection(line{glm::ivec2{x, y}, glm::ivec2{x_max, y}},
                                            line{p.points[k], p.points[(k + 1) % n]});
                if(res.class_ == classification_intersection::INTERSECT_BY_SEGM &&
                    (glm::length(res.point_intersection - glm::vec2{x, y}) > PRESISION)) {
                    switch (point_classify(line{p.points[k], p.points[(k + 1) % n]}, glm::vec2{x, y}))
                    {
                    case orientation_point::LEFT:
                        wn--;
                        break;
                    case orientation_point::RIGHT:
                        wn++;
                        break;
                    default:
                        break;
                    }
                }
            }
            if(wn != 0) {
                img.pixelColor(x, y, col);
            }
        }
    }
}

void vertex_transform(geom_object & obj){
    glm::mat4 transform = /*get_id_mat();*/get_rotate_x_mat(M_PI / 6.);
    std::for_each(obj.vertex_buffer.begin(), obj.vertex_buffer.end(), [&transform](glm::vec4 & value){ value = transform * value;});
    std::for_each(obj.norm_buffer.begin(), obj.norm_buffer.end(), [&transform](glm::vec4 & value){ value = transform * value;});
}

void vertex_transform_animation(geom_object & obj){
    const double delta_a = 0.1;
    glm::mat4 transform = get_rotate_xyz_mat(glm::vec3{1,1,1}, delta_a);//get_rotate_y_mat(delta_a);
    std::for_each(obj.vertex_buffer.begin(), obj.vertex_buffer.end(), [&transform](glm::vec4 & value){ value = transform * value;});
    std::for_each(obj.norm_buffer.begin(), obj.norm_buffer.end(), [&transform](glm::vec4 & value){ value = transform * value;});
}

geom_object2 ortho_projection(geom_object const& obj)
{
    geom_object2 res;
    for(auto const& elem:obj.vertex_buffer)
    {
        res.vertex_buffer.emplace_back(glm::vec2{elem.x, elem.y});
    }
    return res;
}

geom_object2 perspective_projection(geom_object const& obj)
{
    geom_object2 res;
    glm::mat4 perspec = get_perspective_proj(3.);
    for(auto const& elem:obj.vertex_buffer)
    {
        glm::vec4 temp = perspec * elem;
        temp /= temp.w;
        res.vertex_buffer.emplace_back(glm::vec2{temp.x, temp.y});
    }

    for(auto const& face:obj.faces_buffer)
    {
        std::array<glm::vec4, 3> temp_vec;
        for(int i = 0; i < 3; i++)
        {
            temp_vec[i] = perspec * obj.vertex_buffer[face[i]];
            temp_vec[i] /= temp_vec[i].w;
        }
        glm::vec4 first_vec = temp_vec[1] - temp_vec[0];
        glm::vec4 second_vec = temp_vec[2] - temp_vec[1];
        glm::vec4 norm_i = cross4(first_vec, second_vec);
        norm_i /= glm::length(norm_i);
        res.temp_norm_buffer.push_back(norm_i);
    }
    return res;
}

void delete_edge(geom_object2 & res, geom_object const& obj)
{
    if (res.temp_norm_buffer.size() == 0){
        for(int i = 0; i < obj.faces_buffer.size(); i++)
        {
            if (glm::dot(glm::vec4{0,0,1,0}, obj.norm_buffer[i]) > 0)
            {
                res.faces_buffer.push_back(obj.faces_buffer[i]);
            }
        }
    }else {
        for(int i = 0; i < obj.faces_buffer.size(); i++)
        {
            if (glm::dot(glm::vec4{0,0,1,0}, res.temp_norm_buffer[i]) > 0)
            {
                res.faces_buffer.push_back(obj.faces_buffer[i]);
            }
        }
    }
}

void viewport(geom_object2 & obj, uint32_t width, uint32_t height)
{
    glm::mat2 scaling{glm::vec2{width / 2, 0}, glm::vec2{0, height / 2}};
    for(auto & elem:obj.vertex_buffer)
    {
        elem = scaling * (elem + glm::vec2{1,1});
        elem.x = int(elem.x);
        elem.y = int(elem.y);
    }
}

void rasterizetion(geom_object2 const& obj, Magick::Image & img, Magick::Color const& col)
{
    for(auto const& face:obj.faces_buffer)
    {
        for(int i = 0; i < 3; i++)
        {
            bresenham_wa(obj.vertex_buffer[face[i]], obj.vertex_buffer[face[(i+1)%3]], img, col);
        }
        // std::cout << "plot" << std::endl;
    }
}


int main(int argc, char* argv[])
{
    if(argc < 2) {
        std::cout << "Using graph \%filename\%" << std::endl;
        return 0;
    }
    else {
        std::cout << "Create img:" << argv[1] << std::endl;
    }

    geom_object cuboid;
    double x_min = -0.5;
    double x_max = 0.5;
    double y_min = -0.5;
    double y_max = 0.5;

    cuboid.vertex_buffer = std::vector<glm::vec4>{
        glm::vec4{x_max,y_max, 0.5,1},
        glm::vec4{x_min,y_max, 0.5,1},
        glm::vec4{x_max,y_min, 0.5,1},
        glm::vec4{x_max,y_max,-0.5,1},
        glm::vec4{x_min,y_min, 0.5,1},
        glm::vec4{x_max,y_min,-0.5,1},
        glm::vec4{x_min,y_max,-0.5,1},
        glm::vec4{x_min,y_min,-0.5,1}
    };
    cuboid.faces_buffer = std::vector<std::array<uint32_t, 3>>{
        std::array<uint32_t, 3> {0,1,2},
        std::array<uint32_t, 3> {1,4,2},
        std::array<uint32_t, 3> {5,0,2},
        std::array<uint32_t, 3> {5,3,0},
        std::array<uint32_t, 3> {0,6,1},
        std::array<uint32_t, 3> {0,3,6},
        std::array<uint32_t, 3> {3,5,6},
        std::array<uint32_t, 3> {6,5,7},
        std::array<uint32_t, 3> {2,4,7},
        std::array<uint32_t, 3> {2,7,5},
        std::array<uint32_t, 3> {4,1,7},
        std::array<uint32_t, 3> {1,6,7},
    };

    cuboid.norm_buffer = std::vector<glm::vec4>(cuboid.faces_buffer.size());
    for (int i = 0; i < cuboid.faces_buffer.size(); i++)
    {
        glm::vec4 first_vec = cuboid.vertex_buffer[cuboid.faces_buffer[i][1]] - cuboid.vertex_buffer[cuboid.faces_buffer[i][0]];
        glm::vec4 second_vec = cuboid.vertex_buffer[cuboid.faces_buffer[i][2]] - cuboid.vertex_buffer[cuboid.faces_buffer[i][1]];
        glm::vec4 norm_i = cross4(first_vec, second_vec);
        norm_i /= glm::length(norm_i);
        cuboid.norm_buffer[i] = norm_i;
    }
    // const uint32_t count_frame = 60;

    // std::vector<Magick::Image> frames;

    // for(int i = 0; i < count_frame; i++)
    // {
    //     frames.emplace_back(Magick::Image("300x300", "white"));
    //     // frames[i].animationDelay(10);
    // }
    Magick::Image img("300x300", "white");

    // std::vector<std::vector<int32_t> > z_buffer(300, std::vector(300, -1));
    vertex_transform(cuboid);
    // auto res = ortho_projection(cuboid);
    auto res = perspective_projection(cuboid);
    delete_edge(res, cuboid);
    viewport(res, 300, 300);
    // rasterizetion(res, frames[0], Magick::Color(0, 0, 0));
    rasterizetion(res, img, Magick::Color(0, 0, 0));

    // for(int i = 1; i < count_frame; i++)
    // {
    //     vertex_transform_animation(cuboid);
    //     // auto res = ortho_projection(cuboid);
    //     auto res = perspective_projection(cuboid);
    //     delete_edge(res, cuboid);
    //     viewport(res, 300, 300);
    //     rasterizetion(res, frames[i], Magick::Color(0, 0, 0));
    // }


    // bresenham_wa(glm::ivec2{299,10}, glm::ivec2{0,0}, img, Magick::Color(0 , (2<<16) - 1, 0));

    // for(auto & elem:frames)
    // {
    //     elem.flip();
    //     elem.animationDelay(1);
    // }

    // Magick::writeImages(frames.begin(), frames.end(), argv[1]);

    img.flip();
    img.magick("png");
    img.write(argv[1]);
    return 0;
}