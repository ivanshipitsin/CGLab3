#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP
#include <vector>
#include <algorithm>
#include <set>
#include <glm/glm.hpp>

const float PRESISION = 1e-5;

typedef struct {
    glm::vec2 start;
    glm::vec2 end;
} line;

enum class classification_intersection {
    PARALLEL,
    INTERSECT_OUT_SEGM,
    INTERSECT_BY_SEGM,
    SAME
};

enum class orientation_point {
    LEFT,
    RIGHT,
    ON
};

typedef struct {
    classification_intersection class_;
    glm::vec2 point_intersection;
} status_intersection;

status_intersection is_intersection(line const& line_1, line const& line_2);
glm::vec3 cross(glm::vec3 a, glm::vec3 b);

/*--------------- Polygon defenition---------------*/
struct polygon {
    std::vector<glm::vec2> points;
};

orientation_point point_classify(line const& l, glm::vec2 const& p);

bool is_simple_polygon(polygon const& p);
bool is_convex_polygon(polygon const& p);
bool is_cw_polygon(polygon const& p);
void change_cw_polygon(polygon & p);

line cyruc_beck_clipping(line const& segment, polygon const& interior);

#endif