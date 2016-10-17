// GLEngine by Joshua Senouf - 2016
// Credits to Joey de Vries (LearnOpenGL) and Kevin Fung (Glitter)


#include "shader.h"
#include "camera.h"
#include "model.h"
#include "basicshape.h"
#include "textureobject.h"
#include "lightobject.h"
#include "cubemap.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <iostream>


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void cameraMove();
GLuint loadCubemap(vector<const char*> faces);


const GLuint WIDTH = 1600;
const GLuint HEIGHT = 900;

bool keys[1024];

Camera camera(glm::vec3(0.0f, 0.0f, 4.0f));

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

GLfloat lastX = WIDTH / 2;
GLfloat lastY = HEIGHT / 2;
bool firstMouse = true;


int main(int argc, char* argv[])
{
    cout << argv[0] << endl;

    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_SAMPLES, 16);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "GLEngine", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    gladLoadGL();

    glViewport(0, 0, WIDTH, HEIGHT);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);


    // Model(s)
//    Model sphereModel("resources/models/sphere/sphere.obj");
    Model shaderballModel("resources/models/shaderball/shaderball.obj");
//    Model suitModel("resources/models/nanosuit/nanosuit.obj");


    // Shader(s)
    Shader lampShader("resources/shaders/lamp.vert", "resources/shaders/lamp.frag");
    Shader lambertShader("resources/shaders/lambert.vert", "resources/shaders/lambert.frag");
    Shader cookTorranceShader("resources/shaders/cooktorrance.vert", "resources/shaders/cooktorrance.frag");
    Shader cubemapShader("resources/shaders/cubemap.vert", "resources/shaders/cubemap.frag");
//    Shader phongShader("resources/shaders/phong.vert", "resources/shaders/phong.frag");


    // Basic shape(s)
    BasicShape lamp1("cube", glm::vec3(1.5f, 0.75f, 1.0f));
    lamp1.setShapeScale(glm::vec3(0.15f, 0.15f, 0.15f));
    BasicShape lamp2("cube", glm::vec3(-1.5f, 1.0f, 1.0f));
    lamp2.setShapeScale(glm::vec3(0.15f, 0.15f, 0.15f));
    BasicShape lamp3("cube", glm::vec3(0.0f, 0.75f, -1.2f));
    lamp3.setShapeScale(glm::vec3(0.15f, 0.15f, 0.15f));
//    BasicShape testFloor("plane", glm::vec3(0.0f, 0.0f, 0.0f));
//    testFloor.setShapeScale(glm::vec3(5.0f, 5.0f, 5.0f));
//    testFloor.setDiffuseTexture("resources/textures/container2_diffuse.png");
//    testFloor.setSpecularTexture("resources/textures/container2_specular.png");


    // Light source(s)
    LightObject light1("point", lamp1.getShapePosition(), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    LightObject light2("point", lamp2.getShapePosition(), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
    LightObject light3("point", lamp3.getShapePosition(), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));


    //Cubemap
    vector<const char*> cubeFaces;

    cubeFaces.push_back("resources/textures/cubemaps/lake/right.jpg");
    cubeFaces.push_back("resources/textures/cubemaps/lake/left.jpg");
    cubeFaces.push_back("resources/textures/cubemaps/lake/top.jpg");
    cubeFaces.push_back("resources/textures/cubemaps/lake/bottom.jpg");
    cubeFaces.push_back("resources/textures/cubemaps/lake/back.jpg");
    cubeFaces.push_back("resources/textures/cubemaps/lake/front.jpg");

//    cubeFaces.push_back("resources/textures/cubemaps/yokoN/right.jpg");
//    cubeFaces.push_back("resources/textures/cubemaps/yokoN/left.jpg");
//    cubeFaces.push_back("resources/textures/cubemaps/yokoN/top.jpg");
//    cubeFaces.push_back("resources/textures/cubemaps/yokoN/bottom.jpg");
//    cubeFaces.push_back("resources/textures/cubemaps/yokoN/back.jpg");
//    cubeFaces.push_back("resources/textures/cubemaps/yokoN/front.jpg");

//    cubeFaces.push_back("resources/textures/cubemaps/yokoD/right.jpg");
//    cubeFaces.push_back("resources/textures/cubemaps/yokoD/left.jpg");
//    cubeFaces.push_back("resources/textures/cubemaps/yokoD/top.jpg");
//    cubeFaces.push_back("resources/textures/cubemaps/yokoD/bottom.jpg");
//    cubeFaces.push_back("resources/textures/cubemaps/yokoD/back.jpg");
//    cubeFaces.push_back("resources/textures/cubemaps/yokoD/front.jpg");

    CubeMap cubemapEnv(cubeFaces);



    while(!glfwWindowShouldClose(window))
    {
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents();
        cameraMove();

        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);


        // Shader setting
        cookTorranceShader.Use();
//        lambertShader.Use();


        // Camera setting
        glm::mat4 projection = glm::perspective(camera.cameraFOV, (float)WIDTH/(float)HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glUniformMatrix4fv(glGetUniformLocation(cookTorranceShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(cookTorranceShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniform3f(glGetUniformLocation(cookTorranceShader.Program, "viewPos"), camera.cameraPosition.x, camera.cameraPosition.y, camera.cameraPosition.z);
        glUniform1f(glGetUniformLocation(cookTorranceShader.Program, "roughness"), 0.5f);
        glUniform1f(glGetUniformLocation(cookTorranceShader.Program, "metallicity"), 0.0f);


        // Light source(s) rendering
        light1.renderToShader(cookTorranceShader);
        light2.renderToShader(cookTorranceShader);
        light3.renderToShader(cookTorranceShader);


        // Model(s) rendering
        glm::mat4 model;
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        GLfloat angle = glfwGetTime()/5.0f * 5.0f;
        model = glm::rotate(model, 0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
        glUniformMatrix4fv(glGetUniformLocation(cookTorranceShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));

//        sphereModel.Draw(cookTorranceShader);
        shaderballModel.Draw(cookTorranceShader);
//        suitModel.Draw(cookTorranceShader);


        // Shape(s) rendering
        lamp1.drawShape(lampShader, view, projection, camera);
        lamp2.drawShape(lampShader, view, projection, camera);
        lamp3.drawShape(lampShader, view, projection, camera);


        // Cubemap rendering
        cubemapEnv.renderToShader(cubemapShader, cookTorranceShader, projection, camera);


        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
            keys[key] = true;
        else if (action == GLFW_RELEASE)
            keys[key] = false;
    }
}


void cameraMove()
{
    if (keys[GLFW_KEY_W])
        camera.keyboardCall(FORWARD, deltaTime);
    if (keys[GLFW_KEY_S])
        camera.keyboardCall(BACKWARD, deltaTime);
    if (keys[GLFW_KEY_A])
        camera.keyboardCall(LEFT, deltaTime);
    if (keys[GLFW_KEY_D])
        camera.keyboardCall(RIGHT, deltaTime);
}


void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    camera.mouseCall(xoffset, yoffset);
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.scrollCall(yoffset);
}
