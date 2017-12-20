#pragma once

#include <list>
#include <map>
#include <vector>

class Renderer
{
public:
    void render();
    void setAxis(int x_axis_size, int y_axis_size, int z_axis_size);
    void setPlaneData(std::map<const char*, float>& plane);
    void setLinesData(std::list<std::map<const char*, float>>& lines);

private:
    void drawAxis();
    void drawLines();
    void drawPlane();

    void translate(std::vector<float>& lines);
    void separation(std::list<std::map<const char*, float>>& lines);

    float screen_width;
    float screen_height;
    float screen_depth;
    std::map<const char*, float> *plane;
    std::vector<float> outside_lines;
    std::vector<float> inside_lines;
};
