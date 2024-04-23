#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "camera.h"
#include "basic_camera.h"
#include "fan.h"

#include <iostream>

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// modelling transform
float rotateAngle_X = 0.0;
float rotateAngle_Y = 0.0;
float rotateAngle_Z = 0.0;
float rotateAxis_X = 0.0;
float rotateAxis_Y = 0.0;
float rotateAxis_Z = 1.0;
float translate_X = 0.0;
float translate_Y = 0.0;
float translate_Z = 0.0;
float scale_X = 1.0;
float scale_Y = 1.0;
float scale_Z = 1.0;
bool fan_turn = false;
bool rotate_around = false;

// camera
Camera camera(glm::vec3(-1.0f, 2.5f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float eyeX = 0.0, eyeY = 1.0, eyeZ = 3.0;
float lookAtX = 0.0, lookAtY = 0.0, lookAtZ = 0.0;
glm::vec3 V = glm::vec3(0.0f, 1.0f, 0.0f);
BasicCamera basic_camera(eyeX, eyeY, eyeZ, lookAtX, lookAtY, lookAtZ, V);

// timing
float deltaTime = 0.0f;    // time between current frame and last frame
float lastFrame = 0.0f;

glm::mat4 transforamtion(float tx, float ty, float tz, float rx, float ry, float rz, float sx, float sy, float sz) {
    glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model;
    translateMatrix = glm::translate(identityMatrix, glm::vec3(tx, ty, tz));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rx), glm::vec3(1.0f, 0.0f, 0.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(ry), glm::vec3(0.0f, 1.0f, 0.0f));
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rz), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(sx, sy, sz));
    model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
    return model;
}

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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "CSE 4208: Computer Graphics Laboratory", NULL, NULL);
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

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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
    Shader ourShader("vertexShader.vs", "fragmentShader.fs");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    
    //axis line 

    float cube_vertices2[] = {
        -0.25f, -0.25f, -0.25f, 0.0f, 0.0f, 0.0f,
        0.25f, -0.25f, -0.25f, 0.0f, 0.0f, 0.0f,
        0.25f, 0.25f, -0.25f, 0.0f, 0.0f, 0.0f,
        -0.25f, 0.25f, -0.25f, 0.0f, 0.0f, 0.0f,
        -0.25f, -0.25f, 0.25f, 0.3f, 0.8f, 0.5f,
        0.25f, -0.25f, 0.25f, 0.5f, 0.4f, 0.3f,
        0.25f, 0.25f, 0.25f, 0.2f, 0.7f, 0.3f,
        -0.25f, 0.25f, 0.25f, 0.6f, 0.2f, 0.8f,
    };

    float cube_vertices[] = {
        0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f,

        0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.0f, 0.5f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,

        0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.0f, 0.5f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,

        0.0f, 0.0f, 0.5f, 1.0f, 1.0f, 0.0f,
        0.0f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f,
        0.0f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,

        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.5f, 0.5f, 0.0f, 1.0f, 1.0f,

        0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        0.5f, 0.0f, 0.5f, 1.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 0.5f, 1.0f, 0.0f, 1.0f
    };
    unsigned int cube_indices[] = {
        0, 3, 2,
        2, 1, 0,

        4, 5, 7,
        7, 6, 4,

        8, 9, 10,
        10, 11, 8,

        12, 13, 14,
        14, 15, 12,

        16, 17, 18,
        18, 19, 16,

        20, 21, 22,
        22, 23, 20
    };
    float floor[] = {
        0.0f, 0.0f, 0.0f, 0.69f, 0.69f, 0.69f,
        0.5f, 0.0f, 0.0f, 0.69f, 0.69f, 0.69f,
        0.5f, 0.5f, 0.0f, 0.69f, 0.69f, 0.69f,
        0.0f, 0.5f, 0.0f, 0.69f, 0.69f, 0.69f,

        0.5f, 0.0f, 0.0f, 0.69f, 0.69f, 0.69f,
        0.5f, 0.5f, 0.0f, 0.69f, 0.69f, 0.69f,
        0.5f, 0.0f, 0.5f, 0.69f, 0.69f, 0.69f,
        0.5f, 0.5f, 0.5f, 0.69f, 0.69f, 0.69f,

        0.0f, 0.0f, 0.5f, 0.69f, 0.69f, 0.69f,
        0.5f, 0.0f, 0.5f, 0.69f, 0.69f, 0.69f,
        0.5f, 0.5f, 0.5f, 0.69f, 0.69f, 0.69f,
        0.0f, 0.5f, 0.5f, 0.69f, 0.69f, 0.69f,

        0.0f, 0.0f, 0.5f, 0.69f, 0.69f, 0.69f,
        0.0f, 0.5f, 0.5f, 0.69f, 0.69f, 0.69f,
        0.0f, 0.5f, 0.0f, 0.69f, 0.69f, 0.69f,
        0.0f, 0.0f, 0.0f, 0.69f, 0.69f, 0.69f,

        0.5f, 0.5f, 0.5f, 0.69f, 0.69f, 0.69f,
        0.5f, 0.5f, 0.0f, 0.69f, 0.69f, 0.69f,
        0.0f, 0.5f, 0.0f, 0.69f, 0.69f, 0.69f,
        0.0f, 0.5f, 0.5f, 0.69f, 0.69f, 0.69f,

        0.0f, 0.0f, 0.0f, 0.69f, 0.69f, 0.69f,
        0.5f, 0.0f, 0.0f, 0.69f, 0.69f, 0.69f,
        0.5f, 0.0f, 0.5f, 0.69f, 0.69f, 0.69f,
        0.0f, 0.0f, 0.5f, 0.69f, 0.69f, 0.69f
    };
    //0.92f, 0.91f, 0.83f,
    float wall1[] = {
        0.0f, 0.0f, 0.0f, 0.92f, 0.91f, 0.83f,
        0.5f, 0.0f, 0.0f, 0.92f, 0.91f, 0.83f,
        0.5f, 0.5f, 0.0f, 0.92f, 0.91f, 0.83f,
        0.0f, 0.5f, 0.0f, 0.92f, 0.91f, 0.83f,

        0.5f, 0.0f, 0.0f, 0.92f, 0.91f, 0.83f,
        0.5f, 0.5f, 0.0f, 0.92f, 0.91f, 0.83f,
        0.5f, 0.0f, 0.5f, 0.92f, 0.91f, 0.83f,
        0.5f, 0.5f, 0.5f, 0.92f, 0.91f, 0.83f,

        0.0f, 0.0f, 0.5f, 0.92f, 0.91f, 0.83f,
        0.5f, 0.0f, 0.5f, 0.92f, 0.91f, 0.83f,
        0.5f, 0.5f, 0.5f, 0.92f, 0.91f, 0.83f,
        0.0f, 0.5f, 0.5f, 0.92f, 0.91f, 0.83f,

        0.0f, 0.0f, 0.5f, 0.92f, 0.91f, 0.83f,
        0.0f, 0.5f, 0.5f, 0.92f, 0.91f, 0.83f,
        0.0f, 0.5f, 0.0f, 0.92f, 0.91f, 0.83f,
        0.0f, 0.0f, 0.0f, 0.92f, 0.91f, 0.83f,

        0.5f, 0.5f, 0.5f, 0.92f, 0.91f, 0.83f,
        0.5f, 0.5f, 0.0f, 0.92f, 0.91f, 0.83f,
        0.0f, 0.5f, 0.0f, 0.92f, 0.91f, 0.83f,
        0.0f, 0.5f, 0.5f, 0.92f, 0.91f, 0.83f,

        0.0f, 0.0f, 0.0f, 0.92f, 0.91f, 0.83f,
        0.5f, 0.0f, 0.0f, 0.92f, 0.91f, 0.83f,
        0.5f, 0.0f, 0.5f, 0.92f, 0.91f, 0.83f,
        0.0f, 0.0f, 0.5f, 0.92f, 0.91f, 0.83f
    };

    //0.99f, 0.84f, 0.70f,
    float wall2[] = {
        0.0f, 0.0f, 0.0f, 0.99f, 0.84f, 0.70f,
        0.5f, 0.0f, 0.0f, 0.99f, 0.84f, 0.70f,
        0.5f, 0.5f, 0.0f, 0.99f, 0.84f, 0.70f,
        0.0f, 0.5f, 0.0f, 0.99f, 0.84f, 0.70f,

        0.5f, 0.0f, 0.0f, 0.99f, 0.84f, 0.70f,
        0.5f, 0.5f, 0.0f, 0.99f, 0.84f, 0.70f,
        0.5f, 0.0f, 0.5f, 0.99f, 0.84f, 0.70f,
        0.5f, 0.5f, 0.5f, 0.99f, 0.84f, 0.70f,

        0.0f, 0.0f, 0.5f, 0.99f, 0.84f, 0.70f,
        0.5f, 0.0f, 0.5f, 0.99f, 0.84f, 0.70f,
        0.5f, 0.5f, 0.5f, 0.99f, 0.84f, 0.70f,
        0.0f, 0.5f, 0.5f, 0.99f, 0.84f, 0.70f,

        0.0f, 0.0f, 0.5f, 0.99f, 0.84f, 0.70f,
        0.0f, 0.5f, 0.5f, 0.99f, 0.84f, 0.70f,
        0.0f, 0.5f, 0.0f, 0.99f, 0.84f, 0.70f,
        0.0f, 0.0f, 0.0f, 0.99f, 0.84f, 0.70f,

        0.5f, 0.5f, 0.5f, 0.99f, 0.84f, 0.70f,
        0.5f, 0.5f, 0.0f, 0.99f, 0.84f, 0.70f,
        0.0f, 0.5f, 0.0f, 0.99f, 0.84f, 0.70f,
        0.0f, 0.5f, 0.5f, 0.99f, 0.84f, 0.70f,

        0.0f, 0.0f, 0.0f, 0.99f, 0.84f, 0.70f,
        0.5f, 0.0f, 0.0f, 0.99f, 0.84f, 0.70f,
        0.5f, 0.0f, 0.5f, 0.99f, 0.84f, 0.70f,
        0.0f, 0.0f, 0.5f, 0.99f, 0.84f, 0.70f
    };

    float box[] = {
        0.0f, 0.0f, 0.0f, 0.647f, 0.165f, 0.165f,
        0.5f, 0.0f, 0.0f, 0.647f, 0.165f, 0.165f,
        0.5f, 0.5f, 0.0f, 0.647f, 0.165f, 0.165f,
        0.0f, 0.5f, 0.0f, 0.647f, 0.165f, 0.165f,

        0.5f, 0.0f, 0.0f, 0.647f, 0.165f, 0.165f,
        0.5f, 0.5f, 0.0f, 0.647f, 0.165f, 0.165f,
        0.5f, 0.0f, 0.5f, 0.647f, 0.165f, 0.165f,
        0.5f, 0.5f, 0.5f, 0.647f, 0.165f, 0.165f,

        0.0f, 0.0f, 0.5f, 0.647f, 0.165f, 0.165f,
        0.5f, 0.0f, 0.5f, 0.647f, 0.165f, 0.165f,
        0.5f, 0.5f, 0.5f, 0.647f, 0.165f, 0.165f,
        0.0f, 0.5f, 0.5f, 0.647f, 0.165f, 0.165f,

        0.0f, 0.0f, 0.5f, 0.647f, 0.165f, 0.165f,
        0.0f, 0.5f, 0.5f, 0.647f, 0.165f, 0.165f,
        0.0f, 0.5f, 0.0f, 0.647f, 0.165f, 0.165f,
        0.0f, 0.0f, 0.0f, 0.647f, 0.165f, 0.165f,

        0.5f, 0.5f, 0.5f, 0.647f, 0.165f, 0.165f,
        0.5f, 0.5f, 0.0f, 0.647f, 0.165f, 0.165f,
        0.0f, 0.5f, 0.0f, 0.647f, 0.165f, 0.165f,
        0.0f, 0.5f, 0.5f, 0.647f, 0.165f, 0.165f,

        0.0f, 0.0f, 0.0f, 0.647f, 0.165f, 0.165f,
        0.5f, 0.0f, 0.0f, 0.647f, 0.165f, 0.165f,
        0.5f, 0.0f, 0.5f, 0.647f, 0.165f, 0.165f,
        0.0f, 0.0f, 0.5f, 0.647f, 0.165f, 0.165f
    };

    float box2[] = {
        0.0f, 0.0f, 0.0f, 0.1f, 0.714f, 0.757f,
        0.5f, 0.0f, 0.0f, 0.1f, 0.714f, 0.757f,
        0.5f, 0.5f, 0.0f, 0.1f, 0.714f, 0.757f,
        0.0f, 0.5f, 0.0f, 0.1f, 0.714f, 0.757f,

        0.5f, 0.0f, 0.0f, 0.1f, 0.714f, 0.757f,
        0.5f, 0.5f, 0.0f, 0.1f, 0.714f, 0.757f,
        0.5f, 0.0f, 0.5f, 0.1f, 0.714f, 0.757f,
        0.5f, 0.5f, 0.5f, 0.1f, 0.714f, 0.757f,

        0.0f, 0.0f, 0.5f, 0.1f, 0.714f, 0.757f,
        0.5f, 0.0f, 0.5f, 0.1f, 0.714f, 0.757f,
        0.5f, 0.5f, 0.5f, 0.1f, 0.714f, 0.757f,
        0.0f, 0.5f, 0.5f, 0.1f, 0.714f, 0.757f,

        0.0f, 0.0f, 0.5f, 0.1f, 0.714f, 0.757f,
        0.0f, 0.5f, 0.5f, 0.1f, 0.714f, 0.757f,
        0.0f, 0.5f, 0.0f, 0.1f, 0.714f, 0.757f,
        0.0f, 0.0f, 0.0f, 0.1f, 0.714f, 0.757f,

        0.5f, 0.5f, 0.5f, 0.1f, 0.714f, 0.757f,
        0.5f, 0.5f, 0.0f, 0.1f, 0.714f, 0.757f,
        0.0f, 0.5f, 0.0f, 0.1f, 0.714f, 0.757f,
        0.0f, 0.5f, 0.5f, 0.1f, 0.714f, 0.757f,

        0.0f, 0.0f, 0.0f, 0.1f, 0.714f, 0.757f,
        0.5f, 0.0f, 0.0f, 0.1f, 0.714f, 0.757f,
        0.5f, 0.0f, 0.5f, 0.1f, 0.714f, 0.757f,
        0.0f, 0.0f, 0.5f, 0.1f, 0.714f, 0.757f
    };

    float ac[] = {
        0.0f, 0.0f, 0.0f,0.2f, 0.2f, 0.2f,
        0.25f, 0.0f, 0.0f, 0.2f, 0.2f, 0.2f,
        0.25f, 0.25f, 0.0f, 0.2f, 0.2f, 0.2f,
        0.0f, 0.5f, 0.0f, 0.2f, 0.2f, 0.2f,

        0.25f, 0.0f, 0.0f, 0.8f, 0.8f, 0.8f,
        0.25f, 0.25f, 0.0f, 0.8f, 0.8f, 0.8f,
        0.25f, 0.0f, 0.5f, 0.8f, 0.8f, 0.8f,
        0.25f, 0.25f, 0.5f, 0.8f, 0.8f, 0.8f,

        0.0f, 0.0f, 0.5f, 0.2f, 0.2f, 0.2f,
        0.25f, 0.0f, 0.5f, 0.2f, 0.2f, 0.2f,
        0.25f, 0.25f, 0.5f, 0.2f, 0.2f, 0.2f,
        0.0f, 0.5f, 0.5f, 0.2f, 0.2f, 0.2f,

        0.0f, 0.0f, 0.5f, 0.2f, 0.2f, 0.2f,
        0.0f, 0.5f, 0.5f, 0.2f, 0.2f, 0.2f,
        0.0f, 0.5f, 0.0f, 0.2f, 0.2f, 0.2f,
        0.0f, 0.0f, 0.0f, 0.2f, 0.2f, 0.2f,

        0.25f, 0.25f, 0.5f, 0.2f, 0.2f, 0.2f,
        0.25f, 0.25f, 0.0f, 0.2f, 0.2f, 0.2f,
        0.0f, 0.5f, 0.0f, 0.2f, 0.2f, 0.2f,
        0.0f, 0.5f, 0.5f, 0.2f, 0.2f, 0.2f,

        0.0f, 0.0f, 0.0f, 0.2f, 0.2f, 0.2f,
        0.25f, 0.0f, 0.0f, 0.2f, 0.2f, 0.2f,
        0.25f, 0.0f, 0.5f, 0.2f, 0.2f, 0.2f,
        0.0f, 0.0f, 0.5f, 0.2f, 0.2f, 0.2f
    };

    float ceiling[] = {
        0.0f, 0.0f, 0.0f, 0.95f, 0.95f, 0.95f,
        0.5f, 0.0f, 0.0f, 0.95f, 0.95f, 0.95f,
        0.5f, 0.5f, 0.0f, 0.95f, 0.95f, 0.95f,
        0.0f, 0.5f, 0.0f, 0.95f, 0.95f, 0.95f,

        0.5f, 0.0f, 0.0f, 0.95f, 0.95f, 0.95f,
        0.5f, 0.5f, 0.0f, 0.95f, 0.95f, 0.95f,
        0.5f, 0.0f, 0.5f, 0.95f, 0.95f, 0.95f,
        0.5f, 0.5f, 0.5f, 0.95f, 0.95f, 0.95f,

        0.0f, 0.0f, 0.5f, 0.95f, 0.95f, 0.95f,
        0.5f, 0.0f, 0.5f, 0.95f, 0.95f, 0.95f,
        0.5f, 0.5f, 0.5f, 0.95f, 0.95f, 0.95f,
        0.0f, 0.5f, 0.5f, 0.95f, 0.95f, 0.95f,

        0.0f, 0.0f, 0.5f, 0.95f, 0.95f, 0.95f,
        0.0f, 0.5f, 0.5f, 0.95f, 0.95f, 0.95f,
        0.0f, 0.5f, 0.0f, 0.95f, 0.95f, 0.95f,
        0.0f, 0.0f, 0.0f, 0.95f, 0.95f, 0.95f,

        0.5f, 0.5f, 0.5f, 0.95f, 0.95f, 0.95f,
        0.5f, 0.5f, 0.0f, 0.95f, 0.95f, 0.95f,
        0.0f, 0.5f, 0.0f, 0.95f, 0.95f, 0.95f,
        0.0f, 0.5f, 0.5f, 0.95f, 0.95f, 0.95f,

        0.0f, 0.0f, 0.0f, 0.95f, 0.95f, 0.95f,
        0.5f, 0.0f, 0.0f, 0.95f, 0.95f, 0.95f,
        0.5f, 0.0f, 0.5f, 0.95f, 0.95f, 0.95f,
        0.0f, 0.0f, 0.5f, 0.95f, 0.95f, 0.95f
    };

    float fan_holder[] = {
        0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f,
        0.0f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f,

        0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f,
        0.5f, 0.0f, 0.5f, 1.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f,

        0.0f, 0.0f, 0.5f, 1.0f, 1.0f, 1.0f,
        0.5f, 0.0f, 0.5f, 1.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f,
        0.0f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f,

        0.0f, 0.0f, 0.5f, 1.0f, 1.0f, 1.0f,
        0.0f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f,
        0.0f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,

        0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f,
        0.0f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f,
        0.0f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f,

        0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        0.5f, 0.0f, 0.5f, 1.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 0.5f, 1.0f, 1.0f, 1.0f
    };
    float fan_pivot[] = {
        0.0f, 0.0f, 0.0f, .44f, .22f, .05f,
        0.5f, 0.0f, 0.0f, .44f, .22f, .05f,
        0.5f, 0.5f, 0.0f, .44f, .22f, .05f,
        0.0f, 0.5f, 0.0f, .44f, .22f, .05f,

        0.5f, 0.0f, 0.0f, .44f, .22f, .05f,
        0.5f, 0.5f, 0.0f, .44f, .22f, .05f,
        0.5f, 0.0f, 0.5f, .44f, .22f, .05f,
        0.5f, 0.5f, 0.5f, .44f, .22f, .05f,

        0.0f, 0.0f, 0.5f, .44f, .22f, .05f,
        0.5f, 0.0f, 0.5f, .44f, .22f, .05f,
        0.5f, 0.5f, 0.5f, .44f, .22f, .05f,
        0.0f, 0.5f, 0.5f, .44f, .22f, .05f,

        0.0f, 0.0f, 0.5f, .44f, .22f, .05f,
        0.0f, 0.5f, 0.5f, .44f, .22f, .05f,
        0.0f, 0.5f, 0.0f, .44f, .22f, .05f,
        0.0f, 0.0f, 0.0f, .44f, .22f, .05f,

        0.5f, 0.5f, 0.5f, .44f, .22f, .05f,
        0.5f, 0.5f, 0.0f, .44f, .22f, .05f,
        0.0f, 0.5f, 0.0f, .44f, .22f, .05f,
        0.0f, 0.5f, 0.5f, .44f, .22f, .05f,

        0.0f, 0.0f, 0.0f, .44f, .22f, .05f,
        0.5f, 0.0f, 0.0f, .44f, .22f, .05f,
        0.5f, 0.0f, 0.5f, .44f, .22f, .05f,
        0.0f, 0.0f, 0.5f, .44f, .22f, .05f
    };
    float fan_blade[] = {
        0.0f, 0.0f, 0.0f, .0f, .0f, .42f,
        0.5f, 0.0f, 0.0f, .0f, .0f, .42f,
        0.5f, 0.5f, 0.0f, .0f, .0f, .42f,
        0.0f, 0.5f, 0.0f, .0f, .0f, .42f,

        0.5f, 0.0f, 0.0f, .0f, .0f, .42f,
        0.5f, 0.5f, 0.0f, .0f, .0f, .42f,
        0.5f, 0.0f, 0.5f, .0f, .0f, .42f,
        0.5f, 0.5f, 0.5f, .0f, .0f, .42f,

        0.0f, 0.0f, 0.5f, .0f, .0f, .42f,
        0.5f, 0.0f, 0.5f, .0f, .0f, .42f,
        0.5f, 0.5f, 0.5f, .0f, .0f, .42f,
        0.0f, 0.5f, 0.5f, .0f, .0f, .42f,

        0.0f, 0.0f, 0.5f, .0f, .0f, .42f,
        0.0f, 0.5f, 0.5f, .0f, .0f, .42f,
        0.0f, 0.5f, 0.0f, .0f, .0f, .42f,
        0.0f, 0.0f, 0.0f, .0f, .0f, .42f,

        0.5f, 0.5f, 0.5f, .0f, .0f, .42f,
        0.5f, 0.5f, 0.0f, .0f, .0f, .42f,
        0.0f, 0.5f, 0.0f, .0f, .0f, .42f,
        0.0f, 0.5f, 0.5f, .0f, .0f, .42f,

        0.0f, 0.0f, 0.0f, .0f, .0f, .42f,
        0.5f, 0.0f, 0.0f, .0f, .0f, .42f,
        0.5f, 0.0f, 0.5f, .0f, .0f, .42f,
        0.0f, 0.0f, 0.5f, .0f, .0f, .42f
    };

    float glass[] = {
        0.0f, 0.0f, 0.0f, 0.53f, 0.8f, 0.98f,
        0.5f, 0.0f, 0.0f, 0.53f, 0.8f, 0.98f,
        0.5f, 0.5f, 0.0f, 0.53f, 0.8f, 0.98f,
        0.0f, 0.5f, 0.0f, 0.53f, 0.8f, 0.98f,

        0.5f, 0.0f, 0.0f, 0.53f, 0.8f, 0.98f,
        0.5f, 0.5f, 0.0f, 0.53f, 0.8f, 0.98f,
        0.5f, 0.0f, 0.5f, 0.53f, 0.8f, 0.98f,
        0.5f, 0.5f, 0.5f, 0.53f, 0.8f, 0.98f,

        0.0f, 0.0f, 0.5f, 0.53f, 0.8f, 0.98f,
        0.5f, 0.0f, 0.5f, 0.53f, 0.8f, 0.98f,
        0.5f, 0.5f, 0.5f, 0.53f, 0.8f, 0.98f,
        0.0f, 0.5f, 0.5f, 0.53f, 0.8f, 0.98f,

        0.0f, 0.0f, 0.5f, 0.53f, 0.8f, 0.98f,
        0.0f, 0.5f, 0.5f, 0.53f, 0.8f, 0.98f,
        0.0f, 0.5f, 0.0f, 0.53f, 0.8f, 0.98f,
        0.0f, 0.0f, 0.0f, 0.53f, 0.8f, 0.98f,

        0.5f, 0.5f, 0.5f, 0.53f, 0.8f, 0.98f,
        0.5f, 0.5f, 0.0f, 0.53f, 0.8f, 0.98f,
        0.0f, 0.5f, 0.0f, 0.53f, 0.8f, 0.98f,
        0.0f, 0.5f, 0.5f, 0.53f, 0.8f, 0.98f,

        0.0f, 0.0f, 0.0f, 0.53f, 0.8f, 0.98f,
        0.5f, 0.0f, 0.0f, 0.53f, 0.8f, 0.98f,
        0.5f, 0.0f, 0.5f, 0.53f, 0.8f, 0.98f,
        0.0f, 0.0f, 0.5f, 0.53f, 0.8f, 0.98f
    };

    float cabinate[] = {
        0.0f, 0.0f, 0.0f, 0.29f, 0.0f, 0.29f,
        0.5f, 0.0f, 0.0f, 0.29f, 0.0f, 0.29f,
        0.5f, 0.5f, 0.0f, 0.29f, 0.0f, 0.29f,
        0.0f, 0.5f, 0.0f, 0.29f, 0.0f, 0.29f,

        0.5f, 0.0f, 0.0f, 0.29f, 0.0f, 0.29f,
        0.5f, 0.5f, 0.0f, 0.29f, 0.0f, 0.29f,
        0.5f, 0.0f, 0.5f, 0.29f, 0.0f, 0.29f,
        0.5f, 0.5f, 0.5f, 0.29f, 0.0f, 0.29f,

        0.0f, 0.0f, 0.5f, 0.29f, 0.0f, 0.29f,
        0.5f, 0.0f, 0.5f, 0.29f, 0.0f, 0.29f,
        0.5f, 0.5f, 0.5f, 0.29f, 0.0f, 0.29f,
        0.0f, 0.5f, 0.5f, 0.29f, 0.0f, 0.29f,

        0.0f, 0.0f, 0.5f, 0.29f, 0.0f, 0.29f,
        0.0f, 0.5f, 0.5f, 0.29f, 0.0f, 0.29f,
        0.0f, 0.5f, 0.0f, 0.29f, 0.0f, 0.29f,
        0.0f, 0.0f, 0.0f, 0.29f, 0.0f, 0.29f,

        0.5f, 0.5f, 0.5f, 0.29f, 0.0f, 0.29f,
        0.5f, 0.5f, 0.0f, 0.29f, 0.0f, 0.29f,
        0.0f, 0.5f, 0.0f, 0.29f, 0.0f, 0.29f,
        0.0f, 0.5f, 0.5f, 0.29f, 0.0f, 0.29f,

        0.0f, 0.0f, 0.0f, 0.29f, 0.0f, 0.29f,
        0.5f, 0.0f, 0.0f, 0.29f, 0.0f, 0.29f,
        0.5f, 0.0f, 0.5f, 0.29f, 0.0f, 0.29f,
        0.0f, 0.0f, 0.5f, 0.29f, 0.0f, 0.29f
    };
    float circle_vertices5[] = {
        0.0,.70,0.0, 0.0,0.0,0.0,
       0.25,.70,0.0, 0.0,1.0,1.0,
       .177,.70,0.177, 0.0,1.0,1.0,
       0.0,.70,0.25, 0.0,1.0,1.0,
       -.177,.70,0.177, 0.0,1.0,1.0,
       -0.25,.70,0.0, 0.0,1.0,1.0,
       -.177,.70,-0.177, 0.0,1.0,1.0,
       0.0,.70,-0.25, 0.0,1.0,1.0,
       .177,.70,-0.177, 0.0,1.0,1.0,


       0.0,-.30,0.0, 0.0,0.0,0.0,
       1.0 / 2,-.30,0.0, 0.0,1.0,1.0,
       .707 / 2,-.30,.707 / 2, 0.0,1.0,1.0,
       0.0,-.30,1.0 / 2, 0.0,1.0,1.0,
       -.707 / 2,-.30,.707 / 2, 0.0,1.0,1.0,
       -1.0 / 2,-.30,0.0, 0.0,1.0,1.0,
       -.707 / 2,-.30,-.707 / 2, 0.0,1.0,1.0,
       0.0,-.30,-1.0 / 2, 0.0,1.0,1.0,
       .707 / 2,-.30,-.707 / 2, 0.0,1.0,1.0,

    };
    unsigned int circle_indices[] = {
       0,1,2,
       0,2,3,
       0,3,4,
       0,4,5,
       0,5,6,
       0,6,7,
       0,7,8,
       0,8,1,

       9,10,11,
       9,11,12,
       9,12,13,
       9,13,14,
       9,14,15,
       9,15,16,
       9,16,17,
       9,17,10,

       1,10,2,
       10,11,2,
       11,2,12,
       2,12,3,
       3,12,13,
       3,13,4,
       4,13,14,
       4,14,5,
       5,14,15,
       5,15,6,
       6,15,16,
       6,16,7,
       7,16,17,
       7,17,8,
       8,17,10,
       8,10,1,


    };

    unsigned int circle_VBO, circle_VAO, circle_EBO;
    glGenVertexArrays(1, &circle_VAO);
    glGenBuffers(1, &circle_VBO);
    glGenBuffers(1, &circle_EBO);

    glBindVertexArray(circle_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, circle_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(circle_vertices5), circle_vertices5, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, circle_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(circle_indices), circle_indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);



    unsigned int VBOCA, VAOCA, EBOCA;
    glGenVertexArrays(1, &VAOCA);
    glGenBuffers(1, &VBOCA);
    glGenBuffers(1, &EBOCA);
    glBindVertexArray(VAOCA);
    glBindBuffer(GL_ARRAY_BUFFER, VBOCA);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cabinate), cabinate, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOCA);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);

    unsigned int VBOT, VAOT, EBOT;
    glGenVertexArrays(1, &VAOT);
    glGenBuffers(1, &VBOT);
    glGenBuffers(1, &EBOT);
    glBindVertexArray(VAOT);
    glBindBuffer(GL_ARRAY_BUFFER, VBOT);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ceiling), ceiling, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOT);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);


    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ac), ac, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);


    unsigned int VBOG, VAOG, EBOG;
    glGenVertexArrays(1, &VAOG);
    glGenBuffers(1, &VBOG);
    glGenBuffers(1, &EBOG);
    glBindVertexArray(VAOG);
    glBindBuffer(GL_ARRAY_BUFFER, VBOG);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floor), floor, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOG);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);


    unsigned int VBOW, VAOW, EBOW;
    glGenVertexArrays(1, &VAOW);
    glGenBuffers(1, &VBOW);
    glGenBuffers(1, &EBOW);
    glBindVertexArray(VAOW);
    glBindBuffer(GL_ARRAY_BUFFER, VBOW);
    glBufferData(GL_ARRAY_BUFFER, sizeof(wall1), wall1, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);

    unsigned int VBOW1, VAOW1, EBOW1;
    glGenVertexArrays(1, &VAOW1);
    glGenBuffers(1, &VBOW1);
    glGenBuffers(1, &EBOW1);
    glBindVertexArray(VAOW1);
    glBindBuffer(GL_ARRAY_BUFFER, VBOW1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(wall2), wall2, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOW1);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);


    unsigned int VBOC, VAOC, EBOC;
    glGenVertexArrays(1, &VAOC);
    glGenBuffers(1, &VBOC);
    glGenBuffers(1, &EBOC);
    glBindVertexArray(VAOC);
    glBindBuffer(GL_ARRAY_BUFFER, VBOC);
    glBufferData(GL_ARRAY_BUFFER, sizeof(box), box, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOC);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);


    unsigned int VBOC2, VAOC2, EBOC2;
    glGenVertexArrays(1, &VAOC2);
    glGenBuffers(1, &VBOC2);
    glGenBuffers(1, &EBOC2);
    glBindVertexArray(VAOC2);
    glBindBuffer(GL_ARRAY_BUFFER, VBOC2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(box2), box2, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOC2);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);

    //Fan
    unsigned int VBOF1, VAOF1, EBOF1;
    glGenVertexArrays(1, &VAOF1);
    glGenBuffers(1, &VBOF1);
    glGenBuffers(1, &EBOF1);
    glBindVertexArray(VAOF1);
    glBindBuffer(GL_ARRAY_BUFFER, VBOF1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(fan_holder), fan_holder, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOF1);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);

    unsigned int VBOF2, VAOF2, EBOF2;
    glGenVertexArrays(1, &VAOF2);
    glGenBuffers(1, &VBOF2);
    glGenBuffers(1, &EBOF2);
    glBindVertexArray(VAOF2);
    glBindBuffer(GL_ARRAY_BUFFER, VBOF2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(fan_pivot), fan_pivot, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOF2);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);

    unsigned int VBOF3, VAOF3, EBOF3;
    glGenVertexArrays(1, &VAOF3);
    glGenBuffers(1, &VBOF3);
    glGenBuffers(1, &EBOF3);
    glBindVertexArray(VAOF3);
    glBindBuffer(GL_ARRAY_BUFFER, VBOF3);
    glBufferData(GL_ARRAY_BUFFER, sizeof(fan_blade), fan_blade, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOF3);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);
    int i = 0;

    unsigned int VBOS, VAOS, EBOS;
    glGenVertexArrays(1, &VAOS);
    glGenBuffers(1, &VBOS);
    glGenBuffers(1, &EBOS);
    glBindVertexArray(VAOS);
    glBindBuffer(GL_ARRAY_BUFFER, VBOS);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glass), glass, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOS);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);




    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // activate shader
        ourShader.use();
        glm::mat4 model,rotate,scale;
        // pass projection matrix to shader (note that in this case it could change every frame)
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        //glm::mat4 projection = glm::ortho(-2.0f, +2.0f, -1.5f, +1.5f, 0.1f, 100.0f);
        ourShader.setMat4("projection", projection);

        // camera/view transformation
        glm::mat4 view = camera.GetViewMatrix();
        //glm::mat4 view = basic_camera.createViewMatrix();
        ourShader.setMat4("view", view);

        


        

        

        
        //Floor

        model = transforamtion(0, 0, 0, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 20, 0.1, 20);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOG);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //Ceiling

        model = transforamtion(0, 5, 0, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 20, 0.1, 20);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOT);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //Wall1

        model = transforamtion(0, 0, 0, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 20, 10, 0.1);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOW);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        model = transforamtion(0, 0, 10, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 20, 10, 0.1);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOW);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //Wall2

        model = transforamtion(10, 0, 0, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 0.1, 10, 20);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOW1);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //Bed

        model = transforamtion(10, 0, 3, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -7, 1.5, 6);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOC);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        model = transforamtion(10, 0, 3, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -1, 3.5, 6);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOC);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        model = transforamtion(10, 0, 2.95, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -1, 3.5, .1);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOF2);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        model = transforamtion(10, 0, 6, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -1, 3.5, .1);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOF2);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        model = transforamtion(10, 1.75, 2.95, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -1, .1, 6.2);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOF2);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        model = transforamtion(6, 0, 3.75, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -3, 0.2, 3);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOC2);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        model = transforamtion(9.5, 0.75, 3, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -6, 0.5, 6);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOW);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        model = transforamtion(9.5, 0.95, 3.25, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -2, .2, 2);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOC2);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        model = transforamtion(9.5, 0.95, 4.75, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -2, .2, 2);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOC2);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //Table
        model = transforamtion(10, 0.95, 7, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -4, .5, 5);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOF2);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        model = transforamtion(8.25, 0, 7, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -.5, 2, .5);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOC);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        model = transforamtion(10, 0, 7, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -.5, 2, .5);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOC);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        model = transforamtion(8.25, 0, 9.25, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -.5, 2, .5);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOC);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        model = transforamtion(10, 0, 9.25, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -.5, 2, .5);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOC);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);



        //Chair
        model = transforamtion(8.75, 0.5, 7.75, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -2, .5, 2);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOF2);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        model = transforamtion(8, 0, 7.75, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -.25, 1, .25);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOC);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        model = transforamtion(8, 0, 8.6, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -.25, 1, .25);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOC);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        model = transforamtion(8.75, 0, 8.6, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -.25, 1, .25);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOC);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        model = transforamtion(8.75, 0, 7.78, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -.25, 1, .25);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOC);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        model = transforamtion(7.82, 0.75, 7.82, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -.15, 1.65, .15);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOC);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        model = transforamtion(7.82, 0.75, 8.6, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -.15, 1.65, .15);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOC);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        model = transforamtion(7.80, 1.75, 7.75, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -.15, -1.5, 2);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOF2);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //AC
        model = transforamtion(10, 3, 4, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -5, 2, 6);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //Cabinate
        model = transforamtion(10, 0, 0, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -6, 4, 2);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOCA);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        model = transforamtion(10, 2, 0, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -6.115, .15, 2.115);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOC2);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        model = transforamtion(10, 1.5, 0, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -6.115, .15, 2.115);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOC2);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        model = transforamtion(10, 1, 0, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -6.115, .15, 2.115);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOC2);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        model = transforamtion(10, .5, 0, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -6.115, .15, 2.115);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOC2);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        model = transforamtion(10, 0, 0, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -6.115, .15, 2.115);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOC2);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


        //Mirror
        model = transforamtion(10, 0.5, 1.45, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -.15, 5, 2.5);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOC);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        model = transforamtion(9.98, 0.62, 1.58, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -.17, 4.5, 2);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOF1);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //Mirror
        model = transforamtion(3, 1.5, 10, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 7, 5, -.15);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOC);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        model = transforamtion(3.15, 1.65, 10, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 2, 4.5, -.151);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOS);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        model = transforamtion(4.25, 1.65, 10, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 2, 4.5, -.151);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOS);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        model = transforamtion(5.35, 1.65, 10, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 2, 4.5, -.151);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOS);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


        //Lamp
        model = transforamtion(6, 2, 0.5, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 1,1,1);
        ourShader.setMat4("model", model);
        glBindVertexArray(circle_VAO);
        glDrawElements(GL_TRIANGLES, 96, GL_UNSIGNED_INT, 0);

        model = transforamtion(6, 0, 0.5, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, .15, 5, .15);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOF2);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        model = transforamtion(5.95, 0, 0.35, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, .6, .6, .6);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOF2);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        



        
        

        //Fan
        model = transforamtion(4.95, 3.45, 4.85, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, .6, .6, .6);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOF1);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        model = transforamtion(4.95, 3.5, 4.95, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, .15, 3, .15);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOF2);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        Fan fan;
        ourShader = fan.local_rotation(ourShader, VAOF3, i);

        if (fan_turn)
            i += 5;
        /*if (rotate_around)
            camera.ProcessKeyboard(Y_LEFT, deltaTime);*/








     
        

            

        


        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        camera.ProcessKeyboard(UP, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        camera.ProcessKeyboard(DOWN, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
        camera.ProcessKeyboard(P_UP, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        camera.ProcessKeyboard(P_DOWN, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
        camera.ProcessKeyboard(Y_LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
        camera.ProcessKeyboard(Y_RIGHT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
        camera.ProcessKeyboard(R_LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        camera.ProcessKeyboard(R_RIGHT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
        if (!fan_turn) {
            fan_turn = true;
        }
        else {
            fan_turn = false;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
        if (!rotate_around) {
            rotate_around = true;
        }
        else {
            rotate_around = false;
        }
    
    }

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
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
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
