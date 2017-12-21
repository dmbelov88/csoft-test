#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "CustomWindow.h"
#include "Renderer.h"
#include "XmlReader.h"

std::map<const char*, float> plane_map;
std::list<std::map<const char*, float>> lines_map_list;

float rotation_x = 0;
float rotation_y = 0;

bool modifires = false;

bool loadData();
void keyCallback(GLFWwindow *window, int key, int scan_code, int action, int mods);

int main()
{
    if (!loadData())
        std::cout << "Error parse data" << std::endl;
    
    if (!glfwInit())
        return false;

    // Create a windowed mode window and its OpenGL context
    CustomWindow free_window(1000, 1000, 1000);
    CustomWindow orto_window(500, 500, 500);

    free_window.keyCallback = keyCallback;
    orto_window.keyCallback = keyCallback;

    free_window.setup();
    orto_window.setup();

    if (!free_window.glfw_window && !orto_window.glfw_window)
    {
        glfwTerminate();
        return false;
    }

    glEnable(GL_BLEND); 
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Renderer free_renderer;
    free_renderer.setAxis(free_window.screen_width, free_window.screen_height, free_window.screen_depth);

    Renderer orto_renderer;
    orto_renderer.setAxis(orto_window.screen_width, orto_window.screen_height, orto_window.screen_depth);

    // Loop until the user closes the window
    while ( !glfwWindowShouldClose(free_window.glfw_window) && 
        !glfwWindowShouldClose(orto_window.glfw_window) )
    {
        free_window.putRotation(rotation_x, rotation_y);
        free_window.switchWindow();

        if (modifires)
        {
            free_renderer.setPlaneData(plane_map);
            free_renderer.setLinesData(lines_map_list);

            orto_renderer.setPlaneData(plane_map);
            orto_renderer.setLinesData(lines_map_list);

            modifires = false;
        }

        free_renderer.render();

        glPopMatrix();

        // Swap front and back buffers
        glfwSwapBuffers(free_window.glfw_window);

        if (plane_map["n_x"] == 1)
            orto_window.putRotation(90, 0);
        if (plane_map["n_y"] == 1)
            orto_window.putRotation(0, 90);
        if (plane_map["n_z"] == 1)
            orto_window.putRotation(90, 90);

        orto_window.switchWindow();

        orto_renderer.render();

        glPopMatrix();

        // Swap front and back buffers
        glfwSwapBuffers(orto_window.glfw_window);

        // Poll for and process events
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}

bool loadData()
{
    plane_map.clear();
    lines_map_list.clear();

    XmlReader reader;
    if (reader.init("test.xml"))
    {
        plane_map = reader.getPlane();
        lines_map_list = reader.getLines();
        modifires = true;
        return true;
    }
    return false;
}

void keyCallback( GLFWwindow *window, int key, int scan_code, int action, int mods )
{
    const float rotationSpeed = 5;
    
    // actions are GLFW_PRESS, GLFW_RELEASE or GLFW_REPEAT
    if ( action == GLFW_PRESS || action == GLFW_REPEAT )
    {
        switch ( key )
        {
            case GLFW_KEY_UP:
                rotation_x -= rotationSpeed;
                break;
            case GLFW_KEY_DOWN:
                rotation_x += rotationSpeed;
                break;
            case GLFW_KEY_RIGHT:
                rotation_y += rotationSpeed;
                break;
            case GLFW_KEY_LEFT:
                rotation_y -= rotationSpeed;
                break;
            case GLFW_KEY_SPACE:
                loadData();
                break;
            default:
                break;
        }
    }
}
