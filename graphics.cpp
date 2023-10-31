#include "graphics.hpp"

#include <iostream>

glm::vec3 cross(glm::vec3 a, glm::vec3 b)
{
    return {a.y*b.z - a.z*b.y, 
            a.z*b.x - a.x*b.z, 
            a.x*b.y - a.y*b.x};
}

static glm::vec3 vec2_to_vec3(glm::vec2 vec)
{
    return glm::vec3{vec.x, vec.y, 0};
}

static glm::vec3 point2_to_point3(glm::vec2 vec)
{
    return glm::vec3{vec.x, vec.y, 1};
}

status_intersection is_intersection(line const& line_1, line const& line_2) {
    status_intersection res;
    glm::vec3 line_dir_1 = vec2_to_vec3(line_1.end - line_1.start);
    glm::vec3 line_dir_2 = vec2_to_vec3(line_2.end - line_2.start);
    glm::vec3 temp_v = vec2_to_vec3(line_2.start - line_1.start);
    glm::vec3  res_1 = cross(line_dir_1, line_dir_2);
    glm::vec3  res_2 = cross(temp_v, line_dir_2);
    glm::vec3  res_3 = cross(temp_v, line_dir_1);
    if(glm::abs(res_1.z) > PRESISION) {
        float t = res_2.z / res_1.z;
        float s = res_3.z / res_1.z;
        if(t>= 0 && t <= 1 && s >= 0 && s <= 1){
            res.class_ = classification_intersection::INTERSECT_BY_SEGM;
        } else {
            res.class_ = classification_intersection::INTERSECT_OUT_SEGM;
        }
        res.point_intersection = line_1.start + t * glm::vec2{line_dir_1.x, line_dir_1.y};
    } else {
        if(glm::abs(res_2.z) > PRESISION) {
            res.class_ = classification_intersection::PARALLEL;
        }
        else {
            res.class_ = classification_intersection::SAME;
        }
    }
    return res;
}

orientation_point point_classify(line const& l, glm::vec2 const& p) {
    glm::vec3 dir_line = vec2_to_vec3(l.end - l.start);
    glm::vec3 dir_point = vec2_to_vec3(p - l.start);
    glm::vec3 class_vec = cross(dir_line, dir_point);
    if(class_vec.z > 0) {
        return orientation_point::LEFT;
    } else if(class_vec.z == 0) {
        return orientation_point::ON;
    } else {
        return orientation_point::RIGHT;
    }
}

/*---- About polygon ----*/

bool is_convex_polygon(polygon const& p) {
    glm::vec3 temp_cross = cross(vec2_to_vec3(p.points[1] - p.points[0]), 
                                      vec2_to_vec3(p.points[2] - p.points[1]));
    bool cw = temp_cross.z >= 0;
    int n = p.points.size();
    for(int i = 1; i < n; i++) {
        temp_cross = cross(vec2_to_vec3(p.points[(i+1)%n] - p.points[i]), 
                                vec2_to_vec3(p.points[(i+2)%n] - p.points[(i+1)%n]));
        if(temp_cross.z >= 0 != cw)
        {
            return false;
        }
    }
    return true;
}

bool is_cw_polygon(polygon const& p) {
    return cross(vec2_to_vec3(p.points[1] - p.points[0]), 
                                      vec2_to_vec3(p.points[2] - p.points[1])).z <= 0;
}

void change_cw_polygon(polygon & p) {
    std::reverse(p.points.begin(), p.points.end());
}

/*-------- Super-duper algorithm ------------*/

bool is_simple_polygon(polygon const& p) {
    int n = p.points.size();
    std::vector<int> points_sort(n);
    for(int i = 0; i < n; i++){
        points_sort[i] = i;
    }
    auto compar_funct = [&p](std::vector<int>::value_type left, 
                             std::vector<int>::value_type right) {
                                if(glm::abs(p.points[left].x - p.points[right].x) > PRESISION)
                                    return p.points[left].x < p.points[right].x;
                                else
                                    return left < right;
                            };

    std::sort(points_sort.begin(), points_sort.end(), compar_funct);

    for(int k = 0; k < n - 1; k++) {
        int j = points_sort[k + 1];
        int i = points_sort[k];
        for(int t = 0; t < 4; t++)
        {
            int dirj = (t % 2 == 0)? 1:-1;
            int diri = (t / 2 == 0)? 1:-1;
            auto res = is_intersection(line{p.points[j], p.points[(j + dirj) % n]}, 
                                       line{p.points[i], p.points[(i + diri) % n]});
            if(res.class_ == classification_intersection::INTERSECT_BY_SEGM 
            && glm::length(res.point_intersection - p.points[(i + diri) % n]) > PRESISION
            && glm::length(res.point_intersection - p.points[(j + dirj) % n]) > PRESISION) {
                return false;
            }
        }
        
    }

    return true;
}

line cyruc_beck_clipping(line const& segment, polygon const& p)
{
    line res;
    size_t n = p.points.size();
    std::vector<int> class_intersection;
    std::vector<glm::vec2> intersection;
    for(int i = 0; i < n; i++)
    {
        auto intersect_res = is_intersection(segment, line{p.points[i], p.points[(i+1)%n]});
        if (intersect_res.class_ == classification_intersection::INTERSECT_BY_SEGM)
        {
            intersection.push_back(intersect_res.point_intersection);
            if(glm::dot(p.points[(i + 1)%n] - p.points[i], segment.end - segment.start) > 0) {
                class_intersection.push_back(1); // torward interior
            }else{
                class_intersection.push_back(-1); // away from interior
            }
        }
    }
    if (intersection.size() == 0)
    {
        orientation_point orint = point_classify(line{p.points[0], p.points[1]}, segment.start);
        for(int i = 1; i < n; i++)
        {
            auto res_or = point_classify(line{p.points[i], p.points[(i+1)%n]}, segment.start);
            if (res_or != orint)
            {
                return line{glm::vec2{-1,-1}, glm::vec2{-1,-1}};
            }
            
        }
        return segment;
    }
    if (intersection.size() == 1)
    {
        if (class_intersection[0] == 1)
        {
            res.start = intersection[0];
            res.end = segment.end;
        }
        else
        {
            res.start = segment.start;
            res.end = intersection[0];
        }
        return res;
    }
    if(class_intersection[0] == 1)
    {
        res.start = intersection[0];
        res.end = intersection[1];
    }else{
        res.start = intersection[1];
        res.end = intersection[0];
    }
    return res;
}