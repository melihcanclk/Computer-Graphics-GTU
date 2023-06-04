#include "glad.h"
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "glm/glm/gtc/type_ptr.hpp"

#include "shaders/shader_m.h"

#include "camera.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(const char *path);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f; // time between current frame and last frame
float lastFrame = 0.0f;

// this is not used in directional light and spot light but
// it is used in point light and camera so i put it here
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // capture cursor

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    // build and compile our shader zprogram
    // ------------------------------------
    Shader triangle_shader("vs/shader.vs", "fs/shader.fs");
    Shader camera_shader("vs/camera.vs", "fs/camera.fs");
    Shader light_cube_shader = Shader("vs/light_cube.vs", "fs/light_cube.fs");
    Shader material_cube_shader = Shader("vs/material.vs", "fs/material.fs");
    // Shader lightning_map_shader = Shader("vs/lightning_map.vs", "fs/lightning_map.fs");
    Shader phong_shader = Shader("vs/phong_shading.vs", "fs/phong_shading.fs");

    // directional light
    // Shader lightning_map_shader = Shader("vs/directional_light.vs", "fs/directional_light.fs");

    // point light
    // Shader lightning_map_shader = Shader("vs/point_light.vs", "fs/point_light.fs");

    // spot light
    // Shader lightning_map_shader = Shader("vs/spot_light.vs", "fs/spot_light.fs");

    // multiple lights
    Shader lightning_map_shader = Shader("vs/multiple_lights.vs", "fs/multiple_lights.fs");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------

    float triangle_vertices[] = {
        // positions of triangle
        0.8f, -0.7f, 0.0f, // top
        0.9f, -0.9f, 0.0f, // bottom right
        0.7f, -0.9f, 0.0f, // bottom left
    };
    unsigned int triangle_indices[] = {
        // note that we start from 0!
        0, 1, 2, // first triangle
    };

    float vertices[] = {
        // positions(3), texture coords(2), normals(3)
        // front
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0, 0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0, 0.0f, -1.0f,
        // back
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0, 0.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0, 0.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0, 0.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0, 0.0f, 1.0f,
        // left
        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f, -1.0, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 1.0f, 1.0f, -1.0, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, -1.0, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, -1.0, 0.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, -1.0, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f, -1.0, 0.0f, 0.0f,
        // right
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0, 0.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0, 0.0f, 0.0f,
        // bottom
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0, -1.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0, -1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0, -1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0, -1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0, -1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0, -1.0f, 0.0f,
        // top
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0, 1.0f, 0.0f};

    float light_cube_coordinates[] = {
        // positions(3), normals(3)
        // front
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,

        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,

        -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,

        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,

        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f

    };

    float material_cube[] = {
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,

        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,

        -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,

        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,

        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f};

    float lightning_map_obj_vertices[] = {
        // positions(3)          // normals(3)           // texture coords(2)
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

        -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,

        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f};
    // world space positions of our cubes
    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(2.0f, 5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f, 3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f, 2.0f, -2.5f),
        glm::vec3(1.5f, 0.2f, -1.5f),
        glm::vec3(-1.3f, 1.0f, -1.5f)};

    // ------------------ triangle ------------------
    unsigned int triangle_VBO, triangle_VAO, triangle_EBO;
    glGenVertexArrays(1, &triangle_VAO);
    glGenBuffers(1, &triangle_VBO); // generate buffer id
    glGenBuffers(1, &triangle_EBO); // generate buffer id

    glBindVertexArray(triangle_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, triangle_VBO); // bind buffer to GL_ARRAY_BUFFER
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_vertices), triangle_vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangle_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(triangle_indices), triangle_indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0); // tell opengl how to interpret vertex data
    glEnableVertexAttribArray(0);                                                  // enable vertex attribute

    glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind buffer

    glBindVertexArray(0); // unbind vertex array

    // ------------------ rectangle ------------------
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO); // generate buffer id

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);                                        // bind buffer to GL_ARRAY_BUFFER
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // copy vertices to buffer

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0); // tell opengl how to interpret vertex data
    glEnableVertexAttribArray(0);                                                  // enable vertex attribute

    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float))); // tell opengl how to interpret vertex data
    glEnableVertexAttribArray(1);                                                                    // enable vertex attribute

    // normal attribute
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(5 * sizeof(float))); // tell opengl how to interpret vertex data
    glEnableVertexAttribArray(2);                                                                    // enable vertex attribute

    // ------------------ light source ------------------
    // light cube
    unsigned int light_cube_VBO, light_cube_VAO;
    glGenVertexArrays(1, &light_cube_VAO);
    glGenBuffers(1, &light_cube_VBO); // generate buffer id

    glBindVertexArray(light_cube_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, light_cube_VBO);                                                         // bind buffer to GL_ARRAY_BUFFER
    glBufferData(GL_ARRAY_BUFFER, sizeof(light_cube_coordinates), light_cube_coordinates, GL_STATIC_DRAW); // copy vertices to buffer

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0); // tell opengl how to interpret vertex data
    glEnableVertexAttribArray(0);                                                  // enable vertex attribute

    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float))); // tell opengl how to interpret vertex data
    glEnableVertexAttribArray(1);                                                                    // enable vertex attribute

    // ------------------ material cube ------------------
    // material cube
    unsigned int material_cube_VBO, material_cube_VAO;
    glGenVertexArrays(1, &material_cube_VAO);
    glGenBuffers(1, &material_cube_VBO); // generate buffer id

    glBindVertexArray(material_cube_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, material_cube_VBO);                                    // bind buffer to GL_ARRAY_BUFFER
    glBufferData(GL_ARRAY_BUFFER, sizeof(material_cube), material_cube, GL_STATIC_DRAW); // copy vertices to buffer

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0); // tell opengl how to interpret vertex data
    glEnableVertexAttribArray(0);                                                  // enable vertex attribute

    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float))); // tell opengl how to interpret vertex data
    glEnableVertexAttribArray(1);                                                                    // enable vertex attribute

    // ------------------ lightning map obj ------------------
    // lightning map obj
    unsigned int lightning_map_obj_VBO, lightning_map_obj_VAO;
    glGenVertexArrays(1, &lightning_map_obj_VAO);
    glGenBuffers(1, &lightning_map_obj_VBO); // generate buffer id

    glBindVertexArray(lightning_map_obj_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, lightning_map_obj_VBO); // bind buffer to GL_ARRAY_BUFFER
    glBufferData(GL_ARRAY_BUFFER, sizeof(lightning_map_obj_vertices), lightning_map_obj_vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0); // tell opengl how to interpret vertex data
    glEnableVertexAttribArray(0);                                                  // enable vertex attribute

    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float))); // tell opengl how to interpret vertex data
    glEnableVertexAttribArray(1);                                                                    // enable vertex attribute

    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float))); // tell opengl how to interpret vertex data
    glEnableVertexAttribArray(2);                                                                    // enable vertex attribute

    unsigned int diffuseMap = loadTexture("assets/container2.png");
    unsigned int specularMap = loadTexture("assets/container2_specular.png");

    // shader configuration
    lightning_map_shader.use();
    lightning_map_shader.setInt("material.diffuse", 0);
    lightning_map_shader.setInt("material.specular", 1);

    // ------------------ phong cube ------------------
    // material cube
    unsigned int phong_cube_VBO, phong_cube_VAO;
    glGenVertexArrays(1, &phong_cube_VAO);
    glGenBuffers(1, &phong_cube_VBO); // generate buffer id

    glBindVertexArray(phong_cube_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, phong_cube_VBO);                                       // bind buffer to GL_ARRAY_BUFFER
    glBufferData(GL_ARRAY_BUFFER, sizeof(material_cube), material_cube, GL_STATIC_DRAW); // copy vertices to buffer

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0); // tell opengl how to interpret vertex data
    glEnableVertexAttribArray(0);                                                  // enable vertex attribute

    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float))); // tell opengl how to interpret vertex data
    glEnableVertexAttribArray(1);                                                                    // enable vertex attribute

    // load and create a texture
    // -------------------------
    unsigned int texture1, texture2;
    // texture 1
    // ---------
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    unsigned char *data = stbi_load("assets/wall.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    // texture 2
    // ---------
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    data = stbi_load("assets/awesomeface.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        // note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------
    camera_shader.use();
    camera_shader.setInt("texture1", 0);
    camera_shader.setInt("texture2", 1);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // print fps
        std::cout << 1 / deltaTime << std::endl;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // bind textures on corresponding texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        // activate shader
        camera_shader.use();

        camera_shader.setVec3("lightPos", lightPos);
        camera_shader.setVec3("viewPos", camera.Position);

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT), 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        camera_shader.setMat4("projection", projection);
        camera_shader.setMat4("view", view);

        glm::mat4 model = glm::mat4(1.0f);
        camera_shader.setMat4("model", model);

        // render boxes
        glBindVertexArray(VAO);
        for (unsigned int i = 0; i < 10; i++)
        {
            // calculate the model matrix for each object and pass it to shader before drawing
            model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
            model = glm::translate(model, cubePositions[i]);
            float angle = static_cast<float>(glfwGetTime()) * 30.0f;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(0.5f, 1.0f, 0.0f)); // rotate around (0.5f, 1.0f, 0.0f)
            // scale cubes differently
            model = glm::scale(model, glm::vec3(0.1f * (i + 1)));
            camera_shader.setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // triangle
        triangle_shader.use();
        triangle_shader.setVec3("ourColor", 1.0f, 0.5f, 0.2f);
        glBindVertexArray(triangle_VAO);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

        // render material cube
        material_cube_shader.use();
        material_cube_shader.setVec3("light.position", lightPos);
        material_cube_shader.setVec3("viewPos", camera.Position);

        // light properties
        glm::vec3 lightColor;
        lightColor.x = sin(static_cast<float>(glfwGetTime() * 2.0f));
        lightColor.y = sin(static_cast<float>(glfwGetTime() * 0.7f));
        lightColor.z = sin(static_cast<float>(glfwGetTime() * 1.3f));
        glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);   // decrease the influence
        glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f); // low influence
        material_cube_shader.setVec3("light.ambient", ambientColor);
        material_cube_shader.setVec3("light.diffuse", diffuseColor);
        material_cube_shader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

        // material properties
        material_cube_shader.setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
        material_cube_shader.setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
        material_cube_shader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
        material_cube_shader.setFloat("material.shininess", 32.0f);

        // view/projection transformations
        projection = glm::perspective(glm::radians(camera.Zoom), static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT), 0.1f, 100.0f);
        view = camera.GetViewMatrix();
        material_cube_shader.setMat4("projection", projection);
        material_cube_shader.setMat4("view", view);

        // world transformation with moved by 1.0f on x-axis
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0));
        material_cube_shader.setMat4("model", model);

        // render the cube
        glBindVertexArray(material_cube_VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // render phong cube, it has lightColor and objectColor
        phong_shader.use();
        phong_shader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        phong_shader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);

        // view/projection transformations
        projection = glm::perspective(glm::radians(camera.Zoom), static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT), 0.1f, 100.0f);
        view = camera.GetViewMatrix();
        phong_shader.setMat4("projection", projection);
        phong_shader.setMat4("view", view);

        // world transformation
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-2.0f, 0.0f, 0.0));
        phong_shader.setMat4("model", model);

        // render the cube
        glBindVertexArray(phong_cube_VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // // render lightning map obj 1
        // lightning_map_shader.use();
        // lightning_map_shader.setVec3("light.position", lightPos);
        // lightning_map_shader.setVec3("viewPos", camera.Position);

        // // light properties
        // lightning_map_shader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
        // lightning_map_shader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
        // lightning_map_shader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

        // // material properties
        // lightning_map_shader.setFloat("material.shininess", 32.0f);

        // // view/projection transformations
        // projection = glm::perspective(glm::radians(camera.Zoom), static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT), 0.1f, 100.0f);
        // view = camera.GetViewMatrix();
        // lightning_map_shader.setMat4("projection", projection);
        // lightning_map_shader.setMat4("view", view);

        // // world transformation
        // model = glm::mat4(1.0f);
        // model = glm::translate(model, glm::vec3(4.0f, 0.0f, 0.0));
        // lightning_map_shader.setMat4("model", model);

        // // bind diffuse map
        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, diffuseMap);

        // // bind specular map
        // glActiveTexture(GL_TEXTURE1);
        // glBindTexture(GL_TEXTURE_2D, specularMap);

        // // render the cube
        // glBindVertexArray(lightning_map_obj_VAO);
        // glDrawArrays(GL_TRIANGLES, 0, 36);

        // render lightning map obj with directional light
        // lightning_map_shader.use();
        // lightning_map_shader.setVec3("light.direction", 0.2f, 1.0f, -0.3f);
        // lightning_map_shader.setVec3("viewPos", camera.Position);

        // // light properties
        // lightning_map_shader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
        // lightning_map_shader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
        // lightning_map_shader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

        // // material properties
        // lightning_map_shader.setFloat("material.shininess", 32.0f);

        // // view/projection transformations
        // projection = glm::perspective(glm::radians(camera.Zoom), static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT), 0.1f, 100.0f);
        // view = camera.GetViewMatrix();
        // lightning_map_shader.setMat4("projection", projection);
        // lightning_map_shader.setMat4("view", view);

        // // world transformation
        // model = glm::mat4(1.0f);
        // model = glm::translate(model, glm::vec3(4.0f, 0.0f, 0.0));
        // lightning_map_shader.setMat4("model", model);

        // // bind diffuse map
        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, diffuseMap);

        // // bind specular map
        // glActiveTexture(GL_TEXTURE1);
        // glBindTexture(GL_TEXTURE_2D, specularMap);

        // // render the cube
        // glBindVertexArray(lightning_map_obj_VAO);
        // glDrawArrays(GL_TRIANGLES, 0, 36);

        // // render lightning obj with point light
        // lightning_map_shader.use();
        // lightning_map_shader.setVec3("light.position", lightPos);
        // lightning_map_shader.setVec3("viewPos", camera.Position);

        // // light properties
        // lightning_map_shader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
        // lightning_map_shader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
        // lightning_map_shader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
        // lightning_map_shader.setFloat("light.constant", 1.0f);
        // lightning_map_shader.setFloat("light.linear", 0.09f);
        // lightning_map_shader.setFloat("light.quadratic", 0.032f);

        // // material properties
        // lightning_map_shader.setFloat("material.shininess", 32.0f);

        // // view/projection transformations
        // projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        // view = camera.GetViewMatrix();
        // lightning_map_shader.setMat4("projection", projection);
        // lightning_map_shader.setMat4("view", view);

        // // world transformation
        // model = glm::mat4(1.0f);
        // lightning_map_shader.setMat4("model", model);

        // // bind diffuse map
        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, diffuseMap);
        // // bind specular map
        // glActiveTexture(GL_TEXTURE1);
        // glBindTexture(GL_TEXTURE_2D, specularMap);

        // // render the cube
        // glBindVertexArray(lightning_map_obj_VAO);
        // glDrawArrays(GL_TRIANGLES, 0, 36);

        // // render lightning obj with spot light
        // lightning_map_shader.use();
        // lightning_map_shader.setVec3("light.position", camera.Position);
        // lightning_map_shader.setVec3("light.direction", camera.Front);
        // lightning_map_shader.setFloat("light.cutOff", glm::cos(glm::radians(12.5f)));
        // lightning_map_shader.setFloat("light.outerCutOff", glm::cos(glm::radians(17.5f)));
        // lightning_map_shader.setVec3("viewPos", camera.Position);

        // // light properties
        // lightning_map_shader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
        // lightning_map_shader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
        // lightning_map_shader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
        // lightning_map_shader.setFloat("light.constant", 1.0f);
        // lightning_map_shader.setFloat("light.linear", 0.09f);
        // lightning_map_shader.setFloat("light.quadratic", 0.032f);

        // // material properties
        // lightning_map_shader.setFloat("material.shininess", 32.0f);

        // // view/projection transformations
        // projection = glm::perspective(glm::radians(camera.Zoom), static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT), 0.1f, 100.0f);
        // view = camera.GetViewMatrix();
        // lightning_map_shader.setMat4("projection", projection);
        // lightning_map_shader.setMat4("view", view);

        // // world transformation
        // model = glm::mat4(1.0f);
        // lightning_map_shader.setMat4("model", model);

        // // bind diffuse map
        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, diffuseMap);
        // // bind specular map
        // glActiveTexture(GL_TEXTURE1);
        // glBindTexture(GL_TEXTURE_2D, specularMap);

        // // render the cube
        // glBindVertexArray(lightning_map_obj_VAO);
        // glDrawArrays(GL_TRIANGLES, 0, 36);

        // render light cube
        // light_cube_shader.use();
        // light_cube_shader.setMat4("projection", projection);
        // light_cube_shader.setMat4("view", view);
        // model = glm::mat4(1.0f);
        // model = glm::translate(model, lightPos);
        // light_cube_shader.setMat4("model", model);

        // glBindVertexArray(light_cube_VAO);
        // glDrawArrays(GL_TRIANGLES, 0, 36);

        glm::vec3 pointLightPositions[] = {
            glm::vec3(0.7f, 0.2f, 2.0f),
            glm::vec3(2.3f, -3.3f, -4.0f),
            glm::vec3(-4.0f, 2.0f, -12.0f),
            glm::vec3(0.0f, 0.0f, -3.0f)};

        // render lightning map obj with multiple point lights
        lightning_map_shader.use();
        lightning_map_shader.setVec3("viewPos", camera.Position);
        lightning_map_shader.setFloat("material.shininess", 32.0f);

        // directional light
        lightning_map_shader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        lightning_map_shader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
        lightning_map_shader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        lightning_map_shader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

        // point light 1
        lightning_map_shader.setVec3("pointLights[0].position", pointLightPositions[0]);
        lightning_map_shader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
        lightning_map_shader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
        lightning_map_shader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
        lightning_map_shader.setFloat("pointLights[0].constant", 1.0f);
        lightning_map_shader.setFloat("pointLights[0].linear", 0.09);
        lightning_map_shader.setFloat("pointLights[0].quadratic", 0.032);

        // point light 2
        lightning_map_shader.setVec3("pointLights[1].position", pointLightPositions[1]);
        lightning_map_shader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
        lightning_map_shader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
        lightning_map_shader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
        lightning_map_shader.setFloat("pointLights[1].constant", 1.0f);
        lightning_map_shader.setFloat("pointLights[1].linear", 0.09f);
        lightning_map_shader.setFloat("pointLights[1].quadratic", 0.032f);
        // point light 3
        lightning_map_shader.setVec3("pointLights[2].position", pointLightPositions[2]);
        lightning_map_shader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
        lightning_map_shader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
        lightning_map_shader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
        lightning_map_shader.setFloat("pointLights[2].constant", 1.0f);
        lightning_map_shader.setFloat("pointLights[2].linear", 0.09f);
        lightning_map_shader.setFloat("pointLights[2].quadratic", 0.032f);
        // point light 4
        lightning_map_shader.setVec3("pointLights[3].position", pointLightPositions[3]);
        lightning_map_shader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
        lightning_map_shader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
        lightning_map_shader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
        lightning_map_shader.setFloat("pointLights[3].constant", 1.0f);
        lightning_map_shader.setFloat("pointLights[3].linear", 0.09f);
        lightning_map_shader.setFloat("pointLights[3].quadratic", 0.032f);

        lightning_map_shader.setVec3("spotLight.position", camera.Position);
        lightning_map_shader.setVec3("spotLight.direction", camera.Front);
        lightning_map_shader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        lightning_map_shader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        lightning_map_shader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        lightning_map_shader.setFloat("spotLight.constant", 1.0f);
        lightning_map_shader.setFloat("spotLight.linear", 0.09f);
        lightning_map_shader.setFloat("spotLight.quadratic", 0.032f);
        lightning_map_shader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        lightning_map_shader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

        // view/projection transformations
        projection = glm::perspective(glm::radians(camera.Zoom), static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT), 0.1f, 100.0f);
        view = camera.GetViewMatrix();
        lightning_map_shader.setMat4("projection", projection);
        lightning_map_shader.setMat4("view", view);

        // world transformation
        model = glm::mat4(1.0f);
        lightning_map_shader.setMat4("model", model);

        // bind diffuse map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);

        // bind specular map
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);

        // render the cube
        glBindVertexArray(lightning_map_obj_VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // render multiple light cubes
        light_cube_shader.use();
        light_cube_shader.setMat4("projection", projection);
        light_cube_shader.setMat4("view", view);

        glBindVertexArray(light_cube_VAO);
        for (unsigned int i = 0; i < 4; i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
            light_cube_shader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // render phong cube
        phong_shader.use();
        phong_shader.setVec3("light.position", lightPos);
        phong_shader.setVec3("viewPos", camera.Position);

        // light properties
        phong_shader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
        phong_shader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
        phong_shader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

        // material properties
        phong_shader.setFloat("material.shininess", 32.0f);

        // view/projection transformations
        projection = glm::perspective(glm::radians(camera.Zoom), static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT), 0.1f, 100.0f);
        view = camera.GetViewMatrix();
        phong_shader.setMat4("projection", projection);
        phong_shader.setMat4("view", view);

        // world transformation
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-2.0f, 0.0f, 0.0));
        phong_shader.setMat4("model", model);

        // bind diffuse map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);

        // bind specular map
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);

        // render the cube
        glBindVertexArray(material_cube_VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const *path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}