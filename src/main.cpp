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
#include <imgui.h>
#include <imgui_impl_glfw_gl3.h>

#include <string>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <iostream>


static void error_callback(int error, const char* description);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void cameraMove();
void imGuiSetup();

GLuint loadCubemap(vector<const char*> faces);


const GLuint WIDTH = 1024;
const GLuint HEIGHT = 576;

bool keys[1024];

Camera camera(glm::vec3(0.0f, 0.0f, 4.0f));

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
GLfloat deltaLightTime = 0.0f;
GLfloat deltaModelTime = 0.0f;
GLfloat deltaShapeTime = 0.0f;
GLfloat deltaCubemapTime = 0.0f;
GLfloat roughness = 0.5f;
GLfloat metallicity = 0.0f;
GLfloat fresnelReflectance = 0.658f;
GLfloat lastX = WIDTH / 2;
GLfloat lastY = HEIGHT / 2;

bool cameraMode;
bool firstMouse = true;
bool show_another_window = false;
bool show_test_window = false;
bool guiIsOpen = true;

ImVec4 backgroundColor = ImColor(15, 15, 15);
ImVec4 albedoDiffuseColor = ImColor(255, 255, 255);

glm::vec3 lightColor1 = glm::vec3(1.0f, 0.0f, 0.0f);
glm::vec3 lightColor2 = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 lightColor3 = glm::vec3(0.0f, 0.0f, 1.0f);



int main(int argc, char* argv[])
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_SAMPLES, 16);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "GLEngine", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSwapInterval(1);

    gladLoadGL();

    glViewport(0, 0, WIDTH, HEIGHT);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    ImGui_ImplGlfwGL3_Init(window, true);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // Model(s)
//    Model sphereModel("resources/models/sphere/sphere.obj");
    Model shaderballModel("resources/models/shaderball/shaderball.obj");
//    Model suitModel("resources/models/nanosuit/nanosuit.obj");


    // Shader(s)
    Shader lampShader("resources/shaders/lamp.vert", "resources/shaders/lamp.frag");
    Shader lambertShader("resources/shaders/lambert.vert", "resources/shaders/lambert.frag");
    Shader cookTorranceShader("resources/shaders/cooktorrance.vert", "resources/shaders/cooktorrance.frag");
    Shader IBLShader("resources/shaders/IBL.vert", "resources/shaders/IBL.frag");
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
    LightObject light1("point", lamp1.getShapePosition(), glm::vec4(lightColor1, 1.0f));
    LightObject light2("point", lamp2.getShapePosition(), glm::vec4(lightColor2, 1.0f));
    LightObject light3("point", lamp3.getShapePosition(), glm::vec4(lightColor3, 1.0f));


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


    // Queries setting for profiling
    GLuint64 startLightTime, startModelTime, startShapeTime, startCubemapTime;
    GLuint64 stopLightTime, stopModelTime, stopShapeTime, stopCubemapTime;

    unsigned int queryIDLight[2];
    unsigned int queryIDModel[2];
    unsigned int queryIDShape[2];
    unsigned int queryIDCubemap[2];

    glGenQueries(2, queryIDLight);
    glGenQueries(2, queryIDModel);
    glGenQueries(2, queryIDShape);
    glGenQueries(2, queryIDCubemap);


    while(!glfwWindowShouldClose(window))
    {
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents();
        cameraMove();


        // ImGui setting
        imGuiSetup();


        glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, backgroundColor.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);


        // Shader setting
        cookTorranceShader.Use();
//        IBLShader.Use();
//        lambertShader.Use();


        // Camera setting
        glm::mat4 projection = glm::perspective(camera.cameraFOV, (float)WIDTH/(float)HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glUniformMatrix4fv(glGetUniformLocation(cookTorranceShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(cookTorranceShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniform3f(glGetUniformLocation(cookTorranceShader.Program, "viewPos"), camera.cameraPosition.x, camera.cameraPosition.y, camera.cameraPosition.z);
        glUniform3f(glGetUniformLocation(cookTorranceShader.Program, "albedoDiffuseColor"), albedoDiffuseColor.x, albedoDiffuseColor.y, albedoDiffuseColor.z);
        glUniform1f(glGetUniformLocation(cookTorranceShader.Program, "roughness"), roughness);
        glUniform1f(glGetUniformLocation(cookTorranceShader.Program, "metallicity"), metallicity);
        glUniform3f(glGetUniformLocation(cookTorranceShader.Program, "F0"), fresnelReflectance, fresnelReflectance, fresnelReflectance);


        // Light source(s) rendering
        glQueryCounter(queryIDLight[0], GL_TIMESTAMP);
        light1.setLightColor(glm::vec4(lightColor1, 1.0f));
        light2.setLightColor(glm::vec4(lightColor2, 1.0f));
        light3.setLightColor(glm::vec4(lightColor3, 1.0f));
        light1.renderToShader(cookTorranceShader);
        light2.renderToShader(cookTorranceShader);
        light3.renderToShader(cookTorranceShader);
        glQueryCounter(queryIDLight[1], GL_TIMESTAMP);


        // Model(s) rendering
        glQueryCounter(queryIDModel[0], GL_TIMESTAMP);
        glm::mat4 model;
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        GLfloat angle = glfwGetTime()/5.0f * 5.0f;
        model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
        glUniformMatrix4fv(glGetUniformLocation(cookTorranceShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));

//        sphereModel.Draw(cookTorranceShader);
        shaderballModel.Draw(cookTorranceShader);
//        suitModel.Draw(cookTorranceShader);
        glQueryCounter(queryIDModel[1], GL_TIMESTAMP);


        // Shape(s) rendering
        glQueryCounter(queryIDShape[0], GL_TIMESTAMP);
        lamp1.drawShape(lampShader, view, projection, camera);
        lamp2.drawShape(lampShader, view, projection, camera);
        lamp3.drawShape(lampShader, view, projection, camera);
        glQueryCounter(queryIDShape[1], GL_TIMESTAMP);


        // Cubemap rendering
        glQueryCounter(queryIDCubemap[0], GL_TIMESTAMP);
        cubemapEnv.renderToShader(cubemapShader, cookTorranceShader, projection, camera);
        glQueryCounter(queryIDCubemap[1], GL_TIMESTAMP);


        // ImGui rendering
        ImGui::Render();
        glfwSwapBuffers(window);


        // GPU profiling
        GLint stopLightTimerAvailable = 0;
        GLint stopModelTimerAvailable = 0;
        GLint stopShapeTimerAvailable = 0;
        GLint stopCubemapTimerAvailable = 0;

        while (!stopLightTimerAvailable && !stopModelTimerAvailable && !stopShapeTimerAvailable && !stopCubemapTimerAvailable)
        {
            glGetQueryObjectiv(queryIDLight[1], GL_QUERY_RESULT_AVAILABLE, &stopLightTimerAvailable);
            glGetQueryObjectiv(queryIDModel[1], GL_QUERY_RESULT_AVAILABLE, &stopModelTimerAvailable);
            glGetQueryObjectiv(queryIDShape[1], GL_QUERY_RESULT_AVAILABLE, &stopShapeTimerAvailable);
            glGetQueryObjectiv(queryIDShape[1], GL_QUERY_RESULT_AVAILABLE, &stopCubemapTimerAvailable);
        }

        glGetQueryObjectui64v(queryIDLight[0], GL_QUERY_RESULT, &startLightTime);
        glGetQueryObjectui64v(queryIDLight[1], GL_QUERY_RESULT, &stopLightTime);
        glGetQueryObjectui64v(queryIDModel[0], GL_QUERY_RESULT, &startModelTime);
        glGetQueryObjectui64v(queryIDModel[1], GL_QUERY_RESULT, &stopModelTime);
        glGetQueryObjectui64v(queryIDShape[0], GL_QUERY_RESULT, &startShapeTime);
        glGetQueryObjectui64v(queryIDShape[1], GL_QUERY_RESULT, &stopShapeTime);
        glGetQueryObjectui64v(queryIDCubemap[0], GL_QUERY_RESULT, &startCubemapTime);
        glGetQueryObjectui64v(queryIDCubemap[1], GL_QUERY_RESULT, &stopCubemapTime);

        deltaLightTime = (stopLightTime - startLightTime) / 1000000.0;
        deltaModelTime = (stopModelTime - startModelTime) / 1000000.0;
        deltaShapeTime = (stopShapeTime - startShapeTime) / 1000000.0;
        deltaCubemapTime = (stopCubemapTime - startCubemapTime) / 1000000.0;
    }

    // Cleaning
    ImGui_ImplGlfwGL3_Shutdown();
    glfwTerminate();

    return 0;
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


void imGuiSetup()
{
    ImGui_ImplGlfwGL3_NewFrame();

    ImGui::Begin("GLEngine", &guiIsOpen, ImVec2(0, 0), 0.5f, ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
    ImGui::SetWindowPos(ImVec2(10, 10));
    ImGui::SetWindowSize(ImVec2(400, HEIGHT - 20));

    if (ImGui::CollapsingHeader("Rendering options", 0, true, true))
    {
        if (ImGui::TreeNode("GLClear Color"))
        {
            ImGui::ColorEdit3("Color", (float*)&backgroundColor);

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Material options"))
        {
            ImGui::ColorEdit3("Albedo", (float*)&albedoDiffuseColor);
            ImGui::SliderFloat("Roughness", &roughness, 0.0f, 1.0f);
            ImGui::SliderFloat("Metallicity", &metallicity, 0.0f, 1.0f);
            ImGui::SliderFloat("F0", &fresnelReflectance, 0.0f, 1.0f);

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Lighting options"))
        {
            ImGui::ColorEdit3("Light Color 1", (float*)&lightColor1);
            ImGui::ColorEdit3("Light Color 2", (float*)&lightColor2);
            ImGui::ColorEdit3("Light Color 3", (float*)&lightColor3);

            ImGui::TreePop();
        }
    }

    if (ImGui::CollapsingHeader("Profiling", 0, true, true))
    {
        ImGui::Text("Light rendering %.4f ms", deltaLightTime);
        ImGui::Text("Model rendering %.4f ms", deltaModelTime);
        ImGui::Text("Shape rendering %.4f ms", deltaShapeTime);
        ImGui::Text("Cubemap rendering %.4f ms", deltaCubemapTime);
    }

    if (ImGui::CollapsingHeader("Application Info", 0, true, true))
    {
        char* glInfos = (char*)glGetString(GL_VERSION);
        char* hardwareInfos = (char*)glGetString(GL_RENDERER);

        ImGui::Text("OpenGL Version :");
        ImGui::Text(glInfos);
        ImGui::Text("Hardware Informations :");
        ImGui::Text(hardwareInfos);
        ImGui::Text("\nFramerate %.2f FPS / Frametime %.4f ms", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
    }

    if (ImGui::CollapsingHeader("About", 0, true, true))
    {
        ImGui::Text("GLEngine by Joshua Senouf\n\nEmail: joshua.senouf@gmail.com\nTwitter: @JoshuaSenouf");
    }

    ImGui::End();
}



//---------------
// GLFW Callbacks
//---------------

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error %d: %s\n", error, description);
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (key == GLFW_KEY_F11 && action == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    if (key == GLFW_KEY_F12 && action == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
            keys[key] = true;
        else if (action == GLFW_RELEASE)
            keys[key] = false;
    }
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

    if(cameraMode)
        camera.mouseCall(xoffset, yoffset);
}


void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
        cameraMode = true;
    else
        cameraMode = false;
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if(cameraMode)
        camera.scrollCall(yoffset);
}
