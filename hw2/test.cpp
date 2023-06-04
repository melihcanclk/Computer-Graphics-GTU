#include "glad.h"
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "glm/glm/gtc/type_ptr.hpp"

#include "shaders/shader_m.h"

#include "camera.h"

#include <iostream>
#include <queue>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(const char *path);

// settings
const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 720;

// camera
Camera camera(glm::vec3(0.0f, 1.0f, 7.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f; // time between current frame and last frame
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos1(1.2f, 1.0f, 2.0f);

glm::vec3 lightPos2(8.2f, 1.0f, 2.0f);

bool isModelArrayFull = false;

std::queue<glm::mat4> modelQueue;

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

    // glfw window creation fullscreen
    // --------------------
    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode *mode = glfwGetVideoMode(monitor);
    GLFWwindow *window = glfwCreateWindow(mode->width, mode->height, "LearnOpenGL", monitor, NULL);
    // GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window); // make the context of the specified window current on the calling thread.
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

    Shader light_cube_shader("vs/light_cube.vs", "fs/light_cube.fs");

    Shader lightning_shader("vs/light.vs", "fs/light.fs");

    Shader basic_lightning_shader("vs/basic_lightning.vs", "fs/basic_lightning.fs");

    Shader phong_lightning_shader("vs/phong_lightning.vs", "fs/phong_lightning.fs");

    Shader material_shader("vs/material.vs", "fs/material.fs");

    Shader lightning_map_shader("vs/lightning_map.vs", "fs/lightning_map.fs");

    Shader light_directional_shader("vs/light_directional.vs", "fs/light_directional.fs");

    Shader light_point_shader("vs/light_point.vs", "fs/light_point.fs");

    Shader light_spot_shader("vs/light_spot.vs", "fs/light_spot.fs");

    Shader multiple_lights_shader("vs/multiple_lights.vs", "fs/multiple_lights.fs");

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

    float cube_vertices[] = {
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,

        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f};

    float cube_only_vertices[] = {
        -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, 0.5f, -0.5f,
        0.5f, 0.5f, -0.5f,
        -0.5f, 0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, -0.5f, 0.5f,
        0.5f, -0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f, 0.5f,
        -0.5f, -0.5f, 0.5f,

        -0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, 0.5f,
        -0.5f, 0.5f, 0.5f,

        0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,

        -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, 0.5f,
        0.5f, -0.5f, 0.5f,
        -0.5f, -0.5f, 0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, 0.5f, -0.5f,
        0.5f, 0.5f, -0.5f,
        0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f, -0.5f};

    float cube_with_normals[] = {
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

    float cube_with_normal_and_textures[] = {
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
        glm::vec3(0.0f, 2.0f, 0.0f),
        glm::vec3(2.0f, 5.0f, -15.0f),
        glm::vec3(-1.5f, 2.2f, -2.5f),
        glm::vec3(-3.8f, 2.0f, -12.3f),
        glm::vec3(2.4f, 2.4f, -3.5f),
        glm::vec3(-1.7f, 3.0f, -7.5f),
        glm::vec3(1.5f, 2.0f, -7.5f),
        glm::vec3(1.5f, 2.2f, -1.5f),
        glm::vec3(-1.3f, 1.0f, -1.5f)};

    // positions of the point lights
    glm::vec3 pointLightPositions[] = {
        glm::vec3(0.7f, 0.2f, 2.0f),
        glm::vec3(2.3f, 3.3f, -4.0f),
        glm::vec3(-4.0f, 2.0f, -12.0f),
        glm::vec3(0.0f, 0.0f, -3.0f)};

    // ------------------ triangle ------------------
    unsigned int triangle_VBO,
        triangle_VAO, triangle_EBO;
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

    // ------------------ cube ------------------
    unsigned int cube_VBO,
        cube_VAO;
    glGenVertexArrays(1, &cube_VAO);
    glGenBuffers(1, &cube_VBO); // generate buffer id

    glBindVertexArray(cube_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, cube_VBO); // bind buffer to GL_ARRAY_BUFFER
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0); // tell opengl how to interpret vertex data
    glEnableVertexAttribArray(0);                                                  // enable vertex attribute

    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float))); // tell opengl how to interpret vertex data
    glEnableVertexAttribArray(1);                                                                    // enable vertex attribute

    glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind buffer

    glBindVertexArray(0); // unbind vertex array

    // -------------- cube without texture ------------
    unsigned int cube_only_VBO,
        cube_only_VAO;
    glGenVertexArrays(1, &cube_only_VAO);
    glGenBuffers(1, &cube_only_VBO); // generate buffer id

    glBindVertexArray(cube_only_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, cube_only_VBO); // bind buffer to GL_ARRAY_BUFFER
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_only_vertices), cube_only_vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0); // tell opengl how to interpret vertex data
    glEnableVertexAttribArray(0);                                                  // enable vertex attribute

    glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind buffer

    glBindVertexArray(0); // unbind vertex array

    // ------------------ light cube ------------------
    unsigned int light_cube_VBO,
        light_cube_VAO;
    glGenVertexArrays(1, &light_cube_VAO);
    glGenBuffers(1, &light_cube_VBO); // generate buffer id

    glBindVertexArray(light_cube_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, light_cube_VBO); // bind buffer to GL_ARRAY_BUFFER
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_only_vertices), cube_only_vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0); // tell opengl how to interpret vertex data
    glEnableVertexAttribArray(0);                                                  // enable vertex attribute

    // ------------------ cube with normals ------------------
    unsigned int cube_with_normals_VBO,
        cube_with_normals_VAO;
    glGenVertexArrays(1, &cube_with_normals_VAO);
    glGenBuffers(1, &cube_with_normals_VBO); // generate buffer id

    glBindVertexArray(cube_with_normals_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, cube_with_normals_VBO); // bind buffer to GL_ARRAY_BUFFER
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_with_normals), cube_with_normals, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0); // tell opengl how to interpret vertex data
    glEnableVertexAttribArray(0);                                                  // enable vertex attribute

    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float))); // tell opengl how to interpret vertex data
    glEnableVertexAttribArray(1);                                                                    // enable vertex attribute

    // ------------------ cube with normals and textures ------------------
    unsigned int cube_with_normal_and_textures_VBO,
        cube_with_normal_and_textures_VAO;
    glGenVertexArrays(1, &cube_with_normal_and_textures_VAO);
    glGenBuffers(1, &cube_with_normal_and_textures_VBO); // generate buffer id

    glBindVertexArray(cube_with_normal_and_textures_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, cube_with_normal_and_textures_VBO); // bind buffer to GL_ARRAY_BUFFER
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_with_normal_and_textures), cube_with_normal_and_textures, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0); // tell opengl how to interpret vertex data
    glEnableVertexAttribArray(0);                                                  // enable vertex attribute

    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float))); // tell opengl how to interpret vertex data
    glEnableVertexAttribArray(1);                                                                    // enable vertex attribute

    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float))); // tell opengl how to interpret vertex data
    glEnableVertexAttribArray(2);                                                                    // enable vertex attribute

    // ------------------ texture ------------------
    // texture1
    unsigned int container = loadTexture("assets/container.jpg");

    // texture2
    unsigned int awesomeface = loadTexture("assets/awesomeface.png");

    // texture 3
    unsigned int container_2 = loadTexture("assets/container2.png");

    // texture 4
    unsigned int container_specular = loadTexture("assets/container2_specular.png");

    // texture 5
    unsigned int grass = loadTexture("assets/grass.jpg");

    // init texture units
    camera_shader.use();
    glUniform1i(glGetUniformLocation(camera_shader.ID, "texture1"), 0);
    // or set it via the texture class
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
        // std::cout << 1 / deltaTime << std::endl;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw a cube with only vertices
        {
            camera_shader.use();
            camera_shader.setMat4("view", camera.GetViewMatrix());
            camera_shader.setMat4("projection", glm::perspective(
                                                    glm::radians(camera.Zoom),
                                                    static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT),
                                                    0.1f, 100.0f));

            // bind textures on corresponding texture units
            glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture
            glBindTexture(GL_TEXTURE_2D, grass);

            // render container
            glBindVertexArray(cube_VAO);
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
            model = glm::scale(model, glm::vec3(50.0f, 50.0f, 50.0f));
            model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0f));
            camera_shader.setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);

            glBindVertexArray(0);
        }

        // triangle
        {
            triangle_shader.use();
            glBindVertexArray(triangle_VAO);
            glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
        }

        // cubes
        {
            camera_shader.use();
            camera_shader.setMat4("view", camera.GetViewMatrix());
            camera_shader.setMat4("projection", glm::perspective(
                                                    glm::radians(camera.Zoom),
                                                    static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT),
                                                    0.1f, 100.0f));

            // bind textures on corresponding texture units
            glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture
            glBindTexture(GL_TEXTURE_2D, container);

            glActiveTexture(GL_TEXTURE1); // activate the texture unit first before binding texture
            glBindTexture(GL_TEXTURE_2D, awesomeface);

            // render container
            glBindVertexArray(cube_VAO);
            // get number of elements in cubePositions
            unsigned int numElements = 9;

            for (unsigned int i = 0; i < numElements; i++)
            {
                // calculate the model matrix for each object and pass it to shader before drawing
                glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
                model = glm::translate(model, cubePositions[i]);
                float angle = 20.0f * static_cast<float>(i);
                model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f)); // rotate the cube

                camera_shader.setMat4("model", model);

                if (!isModelArrayFull)
                {
                    modelQueue.push(model);
                }

                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
        }

        // lightning shader
        {
            lightning_shader.use();
            lightning_shader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
            lightning_shader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);

            // view/projection transformations
            glm::mat4 projection = glm::perspective(
                glm::radians(camera.Zoom),
                static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT),
                0.1f, 100.0f);
            glm::mat4 view = camera.GetViewMatrix();
            lightning_shader.setMat4("projection", projection);
            lightning_shader.setMat4("view", view);

            // world transformation
            glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
            model = glm::translate(model, glm::vec3(1.0f, 1.0f, 4.0f));
            float angle = 20.0f * static_cast<float>(glfwGetTime());
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f)); // rotate the cube
            lightning_shader.setMat4("model", model);

            if (!isModelArrayFull)
            {
                modelQueue.push(model);
            }

            // render the cube
            glBindVertexArray(cube_only_VAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // basic lightning shader
        {
            basic_lightning_shader.use();
            basic_lightning_shader.setVec3("objectColor", 1.0f, 0.5f, 0.3f);
            basic_lightning_shader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
            basic_lightning_shader.setVec3("lightPos", lightPos1);
            basic_lightning_shader.setVec3("viewPos", camera.Position);

            // view/projection transformations
            glm::mat4 projection = glm::perspective(
                glm::radians(camera.Zoom),
                static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT),
                0.1f, 100.0f);
            glm::mat4 view = camera.GetViewMatrix();
            basic_lightning_shader.setMat4("projection", projection);
            basic_lightning_shader.setMat4("view", view);

            // world transformation
            glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
            model = glm::translate(model, glm::vec3(0.0f, 1.0f, 2.0f));
            float angle = 20.0f * static_cast<float>(glfwGetTime());
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f)); // rotate the cube
            basic_lightning_shader.setMat4("model", model);

            if (!isModelArrayFull)
            {
                modelQueue.push(model);
            }

            // render the cube
            glBindVertexArray(cube_with_normals_VAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // phong lightning shader
        {
            phong_lightning_shader.use();
            phong_lightning_shader.setVec3("objectColor", 1.0f, 0.5f, 0.3f);
            phong_lightning_shader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
            phong_lightning_shader.setVec3("lightPos", lightPos1);
            phong_lightning_shader.setVec3("viewPos", camera.Position);

            // view/projection transformations
            glm::mat4 projection = glm::perspective(
                glm::radians(camera.Zoom),
                static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT),
                0.1f, 100.0f);
            glm::mat4 view = camera.GetViewMatrix();
            phong_lightning_shader.setMat4("projection", projection);
            phong_lightning_shader.setMat4("view", view);

            // world transformation
            glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
            model = glm::translate(model, glm::vec3(2.5f, 1.0f, 2.0f));
            float angle = 20.0f * static_cast<float>(glfwGetTime());
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f)); // rotate the cube
            phong_lightning_shader.setMat4("model", model);

            if (!isModelArrayFull)
            {
                modelQueue.push(model);
            }

            // render the cube
            glBindVertexArray(cube_with_normals_VAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // material shader
        {
            material_shader.use();
            material_shader.setVec3("light.position", lightPos1);
            material_shader.setVec3("viewPos", camera.Position);

            // light properties
            glm::vec3 lightColor;
            lightColor.x = sin(static_cast<float>(glfwGetTime() * 2.0f));
            lightColor.y = sin(static_cast<float>(glfwGetTime() * 0.7f));
            lightColor.z = sin(static_cast<float>(glfwGetTime() * 1.3f));
            glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);   // decrease the influence
            glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f); // low influence
            material_shader.setVec3("light.ambient", ambientColor);
            material_shader.setVec3("light.diffuse", diffuseColor);
            material_shader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

            // material properties
            material_shader.setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
            material_shader.setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
            material_shader.setVec3("material.specular", 0.5f, 0.5f, 0.5f); // specular lighting doesn't have full effect on this object's material
            material_shader.setFloat("material.shininess", 32.0f);

            // view/projection transformations
            glm::mat4 projection = glm::perspective(
                glm::radians(camera.Zoom),
                static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT),
                0.1f, 100.0f);

            glm::mat4 view = camera.GetViewMatrix();
            material_shader.setMat4("projection", projection);
            material_shader.setMat4("view", view);

            // world transformation
            glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
            model = glm::translate(model, glm::vec3(0.0f, 3.0f, 2.0f));
            float angle = 20.0f * static_cast<float>(glfwGetTime());
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f)); // rotate the cube
            material_shader.setMat4("model", model);

            if (!isModelArrayFull)
            {
                modelQueue.push(model);
            }

            // render the cube
            glBindVertexArray(cube_with_normals_VAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // lightning maps
        {
            lightning_map_shader.use();
            lightning_map_shader.setVec3("light.position", lightPos1);
            lightning_map_shader.setVec3("viewPos", camera.Position);

            // light properties
            lightning_map_shader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
            lightning_map_shader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
            lightning_map_shader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

            // material properties
            lightning_map_shader.setFloat("material.shininess", 32.0f);

            // view/projection transformations
            glm::mat4 projection = glm::perspective(
                glm::radians(camera.Zoom),
                static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT),
                0.1f, 100.0f);
            glm::mat4 view = camera.GetViewMatrix();
            lightning_map_shader.setMat4("projection", projection);
            lightning_map_shader.setMat4("view", view);

            // world transformation
            glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
            model = glm::translate(model, glm::vec3(2.0f, 3.0f, 2.0f));
            float angle = 20.0f * static_cast<float>(glfwGetTime());
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f)); // rotate the cube
            lightning_map_shader.setMat4("model", model);

            if (!isModelArrayFull)
            {
                modelQueue.push(model);
            }

            // bind textures on corresponding texture units
            glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture
            glBindTexture(GL_TEXTURE_2D, container_2);

            // render the cube
            glBindVertexArray(cube_with_normal_and_textures_VAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // directional light
        {
            light_directional_shader.use();
            light_directional_shader.setVec3("light.direction", -1.2f, -1.0f, -0.3f);
            light_directional_shader.setVec3("viewPos", camera.Position);

            // light properties
            light_directional_shader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
            light_directional_shader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
            light_directional_shader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

            // material properties
            light_directional_shader.setFloat("material.shininess", 32.0f);

            // view/projection transformations
            glm::mat4 projection = glm::perspective(
                glm::radians(camera.Zoom),
                static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT),
                0.1f, 100.0f);

            glm::mat4 view = camera.GetViewMatrix();
            light_directional_shader.setMat4("projection", projection);
            light_directional_shader.setMat4("view", view);

            // world transformation
            glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
            model = glm::translate(model, glm::vec3(7.0f, 3.0f, 2.0f));
            float angle = 20.0f * static_cast<float>(glfwGetTime());
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f)); // rotate the cube
            light_directional_shader.setMat4("model", model);

            if (!isModelArrayFull)
            {
                modelQueue.push(model);
            }

            // bind textures on corresponding texture units
            glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture
            glBindTexture(GL_TEXTURE_2D, container_2);

            // render the cube
            glBindVertexArray(cube_with_normal_and_textures_VAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // point light
        {
            light_point_shader.use();
            light_point_shader.setVec3("light.position", lightPos2);
            light_point_shader.setVec3("viewPos", camera.Position);

            // light properties
            light_point_shader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
            light_point_shader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
            light_point_shader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
            light_point_shader.setFloat("light.constant", 1.0f);
            light_point_shader.setFloat("light.linear", 0.09f);
            light_point_shader.setFloat("light.quadratic", 0.032f);

            // material properties
            light_point_shader.setFloat("material.shininess", 32.0f);

            // view/projection transformations
            glm::mat4 projection = glm::perspective(
                glm::radians(camera.Zoom),
                static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT),
                0.1f, 100.0f);

            glm::mat4 view = camera.GetViewMatrix();
            light_point_shader.setMat4("projection", projection);
            light_point_shader.setMat4("view", view);

            // world transformation
            glm::mat4 model = glm::mat4(1.0f);
            glm::vec3 pos(8.9f, 2.0f, 2.0f);
            model = glm::translate(model, pos);
            float angle = 20.0f * static_cast<float>(glfwGetTime());
            model = glm::rotate(model, glm::radians(angle), pos); // rotate the cube
            light_point_shader.setMat4("model", model);

            if (!isModelArrayFull)
            {
                modelQueue.push(model);
            }

            // bind textures on corresponding texture units
            glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture
            glBindTexture(GL_TEXTURE_2D, container_2);

            glActiveTexture(GL_TEXTURE1);                     // activate the texture unit first before binding texture
            glBindTexture(GL_TEXTURE_2D, container_specular); // render the cube
            glBindVertexArray(cube_with_normal_and_textures_VAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // spot light with lightPos2
        {
            light_spot_shader.use();
            glm::vec3 pos(8.9f, -1.0f, 2.0f);
            light_spot_shader.setVec3("light.position", lightPos2);
            light_spot_shader.setVec3("light.direction", glm::normalize(pos - lightPos2));
            light_spot_shader.setFloat("light.cutOff", glm::cos(glm::radians(12.5f)));
            light_spot_shader.setFloat("light.outerCutOff", glm::cos(glm::radians(17.5f)));
            light_spot_shader.setVec3("viewPos", camera.Position);

            // light properties
            light_spot_shader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
            light_spot_shader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
            light_spot_shader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
            light_spot_shader.setFloat("light.constant", 1.0f);
            light_spot_shader.setFloat("light.linear", 0.09f);
            light_spot_shader.setFloat("light.quadratic", 0.032f);

            // material properties
            light_spot_shader.setFloat("material.shininess", 32.0f);

            // view/projection transformations
            glm::mat4 projection = glm::perspective(
                glm::radians(camera.Zoom),
                static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT),
                0.1f, 100.0f);

            glm::mat4 view = camera.GetViewMatrix();
            light_spot_shader.setMat4("projection", projection);
            light_spot_shader.setMat4("view", view);

            // world transformation
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, pos);
            float angle = 20.0f * static_cast<float>(glfwGetTime());
            model = glm::rotate(model, glm::radians(angle), pos); // rotate the cube
            light_spot_shader.setMat4("model", model);

            if (!isModelArrayFull)
            {
                modelQueue.push(model);
            }

            // bind textures on corresponding texture units
            glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture
            glBindTexture(GL_TEXTURE_2D, container_2);

            // render the cube
            glBindVertexArray(cube_with_normal_and_textures_VAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // multiple lights
        {
            multiple_lights_shader.use();
            multiple_lights_shader.setVec3("viewPos", camera.Position);
            multiple_lights_shader.setFloat("material.shininess", 32.0f);

            // directional light
            multiple_lights_shader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
            multiple_lights_shader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
            multiple_lights_shader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
            multiple_lights_shader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

            // point light 1
            multiple_lights_shader.setVec3("pointLights[0].position", pointLightPositions[0]);
            multiple_lights_shader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
            multiple_lights_shader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
            multiple_lights_shader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
            multiple_lights_shader.setFloat("pointLights[0].constant", 1.0f);
            multiple_lights_shader.setFloat("pointLights[0].linear", 0.09);
            multiple_lights_shader.setFloat("pointLights[0].quadratic", 0.032);

            // point light 2
            multiple_lights_shader.setVec3("pointLights[1].position", pointLightPositions[1]);
            multiple_lights_shader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
            multiple_lights_shader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
            multiple_lights_shader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
            multiple_lights_shader.setFloat("pointLights[1].constant", 1.0f);
            multiple_lights_shader.setFloat("pointLights[1].linear", 0.09);
            multiple_lights_shader.setFloat("pointLights[1].quadratic", 0.032);

            // point light 3
            multiple_lights_shader.setVec3("pointLights[2].position", pointLightPositions[2]);
            multiple_lights_shader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
            multiple_lights_shader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
            multiple_lights_shader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
            multiple_lights_shader.setFloat("pointLights[2].constant", 1.0f);
            multiple_lights_shader.setFloat("pointLights[2].linear", 0.09);
            multiple_lights_shader.setFloat("pointLights[2].quadratic", 0.032f);
            // point light 4
            multiple_lights_shader.setVec3("pointLights[3].position", pointLightPositions[3]);
            multiple_lights_shader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
            multiple_lights_shader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
            multiple_lights_shader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
            multiple_lights_shader.setFloat("pointLights[3].constant", 1.0f);
            multiple_lights_shader.setFloat("pointLights[3].linear", 0.09f);
            multiple_lights_shader.setFloat("pointLights[3].quadratic", 0.032f);
            // spotLight
            multiple_lights_shader.setVec3("spotLight.position", camera.Position);
            multiple_lights_shader.setVec3("spotLight.direction", camera.Front);
            multiple_lights_shader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
            multiple_lights_shader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
            multiple_lights_shader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
            multiple_lights_shader.setFloat("spotLight.constant", 1.0f);
            multiple_lights_shader.setFloat("spotLight.linear", 0.09f);
            multiple_lights_shader.setFloat("spotLight.quadratic", 0.032f);
            multiple_lights_shader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
            multiple_lights_shader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

            // view/projection transformations
            glm::mat4 projection = glm::perspective(
                glm::radians(camera.Zoom),
                static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT),
                0.1f, 100.0f);

            glm::mat4 view = camera.GetViewMatrix();
            multiple_lights_shader.setMat4("projection", projection);
            multiple_lights_shader.setMat4("view", view);

            // world transformation
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(5.0f, 3.0f, 2.0f));
            float angle = 20.0f * static_cast<float>(glfwGetTime());
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f)); // rotate the cube
            multiple_lights_shader.setMat4("model", model);

            // bind textures on corresponding texture units
            glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture
            glBindTexture(GL_TEXTURE_2D, container_2);

            glm::vec3 pos(12.9f, -1.0f, 2.0f);

            unsigned int num_of_objects = 5;
            for (unsigned int i = 0; i < num_of_objects; i++)
            {
                // calculate the model matrix for each object and pass it to shader before drawing
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, pos + cubePositions[i]);
                float angle = 20.0f * static_cast<float>(glfwGetTime()) * (i + 1);
                model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f)); // rotate the cube
                multiple_lights_shader.setMat4("model", model);

                if (!isModelArrayFull)
                {
                    modelQueue.push(model);
                }

                // render the cube
                glBindVertexArray(cube_with_normal_and_textures_VAO);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }

            // also draw the lamp object(s)
            light_cube_shader.use();
            light_cube_shader.setMat4("projection", projection);
            light_cube_shader.setMat4("view", view);

            // we now draw as many light bulbs as we have point lights.
            glBindVertexArray(light_cube_VAO);
            for (unsigned int i = 0; i < 4; i++)
            {
                model = glm::mat4(1.0f);
                model = glm::translate(model, pos + pointLightPositions[i]);
                model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
                light_cube_shader.setMat4("model", model);

                if (!isModelArrayFull)
                {
                    modelQueue.push(model);
                }

                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
        }

        // light cube
        light_cube_shader.use();
        glm::mat4 projection = glm::perspective(
            glm::radians(camera.Zoom),
            static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT),
            0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        light_cube_shader.setMat4("projection", projection);
        light_cube_shader.setMat4("view", view);

        // world transformation
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos1);
        model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
        light_cube_shader.setMat4("model", model);

        if (!isModelArrayFull)
        {
            modelQueue.push(model);
        }

        // render the cube
        glBindVertexArray(light_cube_VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // render another light cube in lightPos2
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos2);
        model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
        light_cube_shader.setMat4("model", model);

        if (!isModelArrayFull)
        {
            modelQueue.push(model);
        }

        // render the cube
        glBindVertexArray(light_cube_VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        isModelArrayFull = true;

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &cube_only_VAO);
    glDeleteBuffers(1, &cube_only_VBO);
    glDeleteVertexArrays(1, &cube_with_normals_VAO);
    glDeleteBuffers(1, &cube_with_normals_VBO);
    glDeleteVertexArrays(1, &cube_with_normal_and_textures_VAO);
    glDeleteBuffers(1, &cube_with_normal_and_textures_VBO);
    glDeleteVertexArrays(1, &light_cube_VAO);
    glDeleteBuffers(1, &light_cube_VBO);

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
        camera.ProcessKeyboard(FORWARD, deltaTime, modelQueue);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime, modelQueue);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime, modelQueue);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime, modelQueue);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime, modelQueue);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime, modelQueue);
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
// reset mouse position to center of screen
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn)
{
    if (firstMouse)
    {
        lastX = xposIn;
        lastY = yposIn;
        firstMouse = false;
    }

    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

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