#include <iostream>
#include <cstdlib>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Renderer.h"
#include "XmlReader.h"

std::map<const char*, float> plane_map;
std::list<std::map<const char*, float>> lines_map_list;

GLfloat rotation_x;
GLfloat rotation_y;

int screen_width = 1000;
int screen_height = 1000;
int screen_depth = 1000;

bool loadData();
void keyCallback(GLFWwindow *window, int key, int scan_code, int action, int mods);

bool modifires = false;

int main()
{
    if (!loadData())
        std::cout << "Error parse data" << std::endl;
    
    if (!glfwInit())
        return false;

    // Create a windowed mode window and its OpenGL context
    GLFWwindow* window = glfwCreateWindow(screen_width, screen_height, "CSoft test", NULL, NULL);

    glfwSetKeyCallback(window, keyCallback);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, 1);

    int window_width;
    int window_height;
    glfwGetFramebufferSize(window, &window_width, &window_height);

    GLfloat halfScreenWidth = static_cast<GLfloat>(screen_width/2);
    GLfloat halfScreenHeight = static_cast<GLfloat>(screen_height/2);
    GLfloat halfScreenDepth = static_cast<GLfloat>(screen_depth/2);

    if (!window)
    {
        glfwTerminate();
        return false;
    }

    // Make the window's context current
    glfwMakeContextCurrent( window );

    glViewport(0, 0, window_width, window_height); // specifies the part of the window to which OpenGL will draw (in pixels), convert from normalised to pixels
    glMatrixMode(GL_PROJECTION); // projection matrix defines the properties of the camera that views the objects in the world coordinate frame. Here you typically set the zoom factor, aspect ratio and the near and far clipping planes
    glLoadIdentity(); // replace the current matrix with the identity matrix and starts us a fresh because matrix transforms such as glOrpho and glRotate cumulate, basically puts us at (0, 0, 0)
    glOrtho(0, window_width, 0, window_height, 0, screen_depth); // essentially set coordinate system
    glMatrixMode(GL_MODELVIEW); // (default matrix mode) modelview matrix defines how your objects are transformed (meaning translation, rotation and scaling) in your world
    glLoadIdentity(); // same as above comment

    glEnable(GL_BLEND); 
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Renderer renderer;
    renderer.setAxis(screen_width, screen_height, screen_depth);

    // Loop until the user closes the window
    while ( !glfwWindowShouldClose( window ) )
    {
        glClear( GL_COLOR_BUFFER_BIT );

        // Render OpenGL here
        glPushMatrix();
        glTranslatef(halfScreenWidth, halfScreenHeight, -halfScreenDepth);
        glRotatef(rotation_x, 1, 0, 0);
        glRotatef(rotation_y, 0, 1, 0);
        glTranslatef(-halfScreenWidth, -halfScreenHeight, halfScreenDepth);

        if (modifires)
        {
            renderer.setPlaneData(plane_map);
            renderer.setLinesData(lines_map_list);
            modifires = false;
        }
        renderer.render();

        glPopMatrix();

        // Swap front and back buffers
        glfwSwapBuffers(window);

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
    const GLfloat rotationSpeed = 5;
    
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
