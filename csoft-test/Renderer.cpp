#include "Renderer.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

void Renderer::setPlaneData(std::map<const char*, float>& plane)
{
    this->plane = &plane;
}

void Renderer::setLinesData(std::list<std::map<const char*, float>>& lines)
{
    separation(lines);

    translate(outside_lines);
    translate(inside_lines);
}

void Renderer::translate(std::vector<float>& lines)
{
    for (uint8_t i = 0; i < lines.size() / 3; ++i)
    {
        lines[3 * i] += screen_width / 2;
        lines[3 * i + 1] += screen_height / 2;
        lines[3 * i + 2] += -screen_depth / 2;
    }
}

void Renderer::setAxis(int x_axis_size, int y_axis_size, int z_axis_size)
{
    screen_width = static_cast<float>(x_axis_size);
    screen_height = static_cast<float>(y_axis_size);
    screen_depth = static_cast<float>(z_axis_size);
}

void Renderer::render()
{
    drawPlane();
    drawAxis();
    drawLines();
}

void Renderer::separation(std::list<std::map<const char*, float>>& lines_map_list)
{
    inside_lines.clear();
    outside_lines.clear();

    const int size = lines_map_list.size();
    std::vector<GLfloat> line;

    const char* normal_name[] = { "n_x", "n_y", "n_z" };
    const char* coordinate_name[] = { "x", "y", "z" };

    for(auto &map: lines_map_list)
    {
        line.clear();

        for (int i = 0; i < 3; ++i)
            if ((map[normal_name[i]] == 0 && (*plane)[normal_name[i]] != 0) &&
                ((*plane)[coordinate_name[i]] == map[coordinate_name[i]]))
            {
                for (uint8_t j = 0; j < 3; ++j)
                    inside_lines.push_back(map[coordinate_name[j]]);

                for (int j = 0; j < 3; ++j)
                {
                    if (map[normal_name[j]] > 0)
                        map[coordinate_name[j]] += map["length"];
                    if (map[normal_name[j]] < 0)
                        map[coordinate_name[j]] -= map["length"];

                    inside_lines.push_back(map[coordinate_name[j]]);
                }
                map["length"] = 0;
                i = 3; // exit from the cycle
            }
    }

    for(auto &map: lines_map_list)
    {
        float length = map["length"];
        if (length == 0)
            continue;

        for (int i = 0; i < 3; ++i)
            outside_lines.push_back(map[coordinate_name[i]]);

        for (int i = 0; i < 3; ++i)
        {
            if (map[normal_name[i]] > 0)
                map[coordinate_name[i]] += length;
            if (map[normal_name[i]] < 0)
                map[coordinate_name[i]] -= length;

            outside_lines.push_back(map[coordinate_name[i]]);
        }
    }

}

void Renderer::drawAxis()
{
    GLfloat axis[] = {
        // oX
        0.0f, screen_height/2, -screen_depth/2, 
        screen_width, screen_height/2, -screen_depth/2,

        // oY
        screen_width/2, 0.0f, -screen_depth/2,
        screen_width/2, screen_height, -screen_depth/2,

        // oZ
        screen_width/2, screen_height/2, 0.0f,
        screen_width/2, screen_height/2, -screen_depth,
    };

    glColor4f(1.0, 0.0, 0.0, 1.0);
    glLineWidth(2);

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, axis);
    glDrawArrays(GL_LINES, 0, 6);
    glDisableClientState(GL_VERTEX_ARRAY);
}

void Renderer::drawLines()
{
    // draw inside lines
    glColor3f(0.0, 0.0, 1.0);
    glLineWidth(2);

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, inside_lines.data());
    glDrawArrays(GL_LINES, 0, inside_lines.size()/3);
    glDisableClientState(GL_VERTEX_ARRAY);

    // draw outside lines
    glColor4f(0.0, 1.0, 1.0, 1.0);
    glLineWidth(2);

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, outside_lines.data());
    glDrawArrays(GL_LINES, 0, outside_lines.size()/3);
    glDisableClientState(GL_VERTEX_ARRAY);
}

void Renderer::drawPlane()
{
    GLfloat plane_matrix[] = {
            screen_width, screen_height, -screen_depth,
            screen_width, screen_height, -screen_depth,
            screen_width, screen_height, -screen_depth,
            screen_width, screen_height, -screen_depth
        };

    int n_x = static_cast<int>((*plane)["n_x"]);
    int n_y = static_cast<int>((*plane)["n_y"]);
    int n_z = static_cast<int>((*plane)["n_z"]);

    // YZ
    if (n_x != 0)
    {
        float x = (*plane)["x"];
        plane_matrix[0] = screen_width/2 + x;
        plane_matrix[3] = screen_width/2 + x;
        plane_matrix[6] = screen_width/2 + x;
        plane_matrix[9] = screen_width/2 + x;

        plane_matrix[4] = 0;
        plane_matrix[7] = 0;
        plane_matrix[8] = 0;
        plane_matrix[11] = 0;
    }
    // ZX
    else if (n_y != 0)
    {
        float y = (*plane)["y"];
        
        plane_matrix[1] = screen_height/2 + y;
        plane_matrix[4] = screen_height/2 + y;
        plane_matrix[7] = screen_height/2 + y;
        plane_matrix[10] = screen_height/2 + y;

        plane_matrix[3] = 0;
        plane_matrix[6] = 0;
        plane_matrix[8] = 0;
        plane_matrix[11] = 0;
    }
    // XY
    if (n_z != 0)
    {
        float z = (*plane)["z"];
        plane_matrix[2] = -screen_depth/2 + z;
        plane_matrix[5] = -screen_depth/2 + z;
        plane_matrix[8] = -screen_depth/2 + z;
        plane_matrix[11] = -screen_depth/2 + z;

        plane_matrix[4] = 0;
        plane_matrix[6] = 0;
        plane_matrix[7] = 0;
        plane_matrix[9] = 0;
    }

    glPolygonMode(GL_FRONT_AND_BACK, GL_QUADS);
    glColor4f(0.0, 1.0, 0.0, 0.5);
    glLineWidth(10);

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, plane_matrix);
    glDrawArrays(GL_QUADS, 0, 4);
    glDisableClientState(GL_VERTEX_ARRAY);
}
