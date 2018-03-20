// GLEngine by Joshua Senouf - 2016
// Credits to Joey de Vries (LearnOpenGL) and Kevin Fung (Glitter)


#include "shader.h"
#include "camera.h"
#include "model.h"
#include "shape.h"
#include "texture.h"
#include "light.h"
#include "skybox.h"
#include "material.h"
#include "gbuffer.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <imgui.h>
#include <imgui_impl_glfw_gl3.h>

#include <string>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <tuple>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <random>
#include <filesystem>
#include <random>

namespace fs = std::experimental::filesystem;


//---------------
// GLFW Callbacks
//---------------

static void error_callback(int error, const char* description);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

//---------------------
// Functions prototypes
//---------------------

void cameraMove();
void imGuiSetup();
void gBufferSetup();
void saoSetup();
void postprocessSetup();
float random(float min, float max);

//---------------------------------
// Variables & objects declarations
//---------------------------------

GLuint WIDTH = 512;
GLuint HEIGHT = 512;

GLuint screenQuadVAO, screenQuadVBO;
GLuint saoFBO, saoBlurFBO, saoBuffer, saoBlurBuffer;
GLuint postprocessFBO, postprocessBuffer;

GLint gBufferView = 1;
GLint tonemappingMode = 1;
GLint lightDebugMode = 3;
GLint attenuationMode = 2;
GLint saoSamples = 12;
GLint saoTurns = 7;
GLint saoBlurSize = 4;
GLint motionBlurMaxSamples = 32;

GLfloat lastX = WIDTH / 2;
GLfloat lastY = HEIGHT / 2;
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
GLfloat deltaGeometryTime = 0.0f;
GLfloat deltaLightingTime = 0.0f;
GLfloat deltaSAOTime = 0.0f;
GLfloat deltaPostprocessTime = 0.0f;
GLfloat deltaForwardTime = 0.0f;
GLfloat deltaGUITime = 0.0f;


GLfloat ambientIntensity = 0.005f;
GLfloat saoRadius = 0.3f;
GLfloat saoBias = 0.001f;
GLfloat saoScale = 0.7f;
GLfloat saoContrast = 0.8f;
GLfloat lightPointRadius1 = 3.0f;
GLfloat lightPointRadius2 = 3.0f;
GLfloat lightPointRadius3 = 3.0f;
GLfloat cameraAperture = 16.0f;
GLfloat cameraShutterSpeed = 0.5f;
GLfloat cameraISO = 1000.0f;

GLfloat theta = 0.0f;
GLfloat rho = 0.0f;
GLfloat radius = 1.0f;

bool cameraMode;
bool pointMode = false;
bool directionalMode = false;
bool iblMode = true;
bool saoMode = false;
bool fxaaMode = false;
bool motionBlurMode = false;
bool screenMode = false;
bool firstMouse = true;
bool guiIsOpen = true;

bool isOrbitCamera = false;

bool keys[1024];

std::vector<bool> invertNormal = {
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    true,
    true,
    false,
    true,
    true,
    false,
    true,
    true,
    false,
    true,
    true,
    false,
    true,
    true,
    true,
    true,
    true,
    true,
    false,
    false,
    false,
    true,
    true,
    true,
    true,
    true,
    true,
    true,
    true,
    true,
    true,
    true,
    true,
    true,
    true,
    false,
    true,
    true,
    true,
    false,
    true,
    false,
    false,
    false
};

int fullRotationTicks = 72;
int halfRotationTicks = 12;
int index_fullRotationTicks = fullRotationTicks + 1;
int index_halfRotationTicks = halfRotationTicks + 1;
int frame = 0;

int useEnvmapIndex = 0;
bool record = false;
bool recording = false;
std::string recording_path = "./";
std::vector<fs::path> model_pool_paths;
std::vector<fs::path>::iterator current_pool_model;


glm::vec3 materialF0 = glm::vec3(0.04f);  // UE4 dielectric
glm::vec3 lightPointPosition1 = glm::vec3(1.5f, 0.75f, 1.0f);
glm::vec3 lightPointPosition2 = glm::vec3(-1.5f, 1.0f, 1.0f);
glm::vec3 lightPointPosition3 = glm::vec3(0.0f, 0.75f, -1.2f);
glm::vec3 lightPointColor1 = glm::vec3(1.0f);
glm::vec3 lightPointColor2 = glm::vec3(1.0f);
glm::vec3 lightPointColor3 = glm::vec3(1.0f);
glm::vec3 lightDirectionalDirection1 = glm::vec3(-0.2f, -1.0f, -0.3f);
glm::vec3 lightDirectionalColor1 = glm::vec3(1.0f);



Camera camera(static_cast<GLfloat>(WIDTH), static_cast<GLfloat>(HEIGHT), 0.1f, 100.0f, glm::vec3(0.0f, 0.0f, 4.0f));

Skybox skybox;
GBuffer gBuffer(WIDTH, HEIGHT);

Shader simpleShader;
Shader lightingBRDFShader;



Shader firstpassPPShader;
Shader saoShader;
Shader saoBlurShader;






Material pbrMat;



Light lightPoint1;
Light lightPoint2;
Light lightPoint3;
Light lightDirectional1;

Shape quadRender;
Shape envCubeRender;

int main(int argc, char* argv[])
{
    std::srand(std::time(nullptr));
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWmonitor* glfwMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* glfwMode = glfwGetVideoMode(glfwMonitor);

    glfwWindowHint(GLFW_RED_BITS, glfwMode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, glfwMode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, glfwMode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, glfwMode->refreshRate);

    //WIDTH = glfwMode->width;
    //HEIGHT = glfwMode->height;

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "GLEngine", NULL, NULL);
    //    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "GLEngine", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSwapInterval(1);

    gladLoadGL();

    glViewport(0, 0, WIDTH, HEIGHT);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    ImGui_ImplGlfwGL3_Init(window, true);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);

    //----------
    // Shader(s)
    //----------
    
    

    simpleShader.setShader("lighting/simple.vert", "lighting/simple.frag");
    lightingBRDFShader.setShader("lighting/lightingBRDF.vert", "lighting/lightingBRDF.frag");
    
    
    

    firstpassPPShader.setShader("postprocess/postprocess.vert", "postprocess/firstpass.frag");
    saoShader.setShader("postprocess/sao.vert", "postprocess/sao.frag");
    saoBlurShader.setShader("postprocess/sao.vert", "postprocess/saoBlur.frag");


    //-----------
    // Textures(s)
    //-----------
    



    //-----------
    // Material(s)
    //-----------
    //    pbrMat.setShader(gBufferShader);
    //    pbrMat.addTexture("texAlbedo", ironAlbedo);
    //    pbrMat.addTexture("texNormal", ironNormal);
    //    pbrMat.addTexture("texRoughness", ironRoughness);
    //    pbrMat.addTexture("texMetalness", ironMetalness);
    //    pbrMat.addTexture("texAO", ironAO);
    //    pbrMat.addTexture("texAlbedo", TextureObject ("pbr/rustediron/rustediron_albedo.png", "ironAlbedo", true));
    //    pbrMat.addTexture("texNormal", TextureObject ("pbr/rustediron/rustediron_normal.png", "ironNormal", true));
    //    pbrMat.addTexture("texRoughness", TextureObject ("pbr/rustediron/rustediron_roughness.png", "ironRoughness", true));
    //    pbrMat.addTexture("texMetalness", TextureObject ("pbr/rustediron/rustediron_metalness.png", "ironMetalness", true));
    //    pbrMat.addTexture("texAO", TextureObject ("pbr/rustediron/rustediron_ao.png", "ironAO", true));


    //---------
    // Model(s) 
    //---------
    for (auto & p : fs::directory_iterator(std::string(MODEL_PATH).append("pool/")))
    {
        if(!is_directory(p))
        {
            auto f = p.path().generic_string().substr(std::string(MODEL_PATH).size());
            std::cout << "found model " << f << " in pool." << std::endl;
            model_pool_paths.push_back(f);
        }
    }

    //---------------
    // G-Buffer setup
    //---------------
    gBuffer.setup();
    gBuffer.loadModel(model_pool_paths[0].generic_string(), glm::vec3(2.0));
    //---------------
    // Shape(s)
    //---------------
    envCubeRender.setShape("cube", glm::vec3(0.0f));
    quadRender.setShape("quad", glm::vec3(0.0f));


    //----------------
    // Light source(s)
    //----------------
    lightPoint1.setLight(lightPointPosition1, glm::vec4(lightPointColor1, 1.0f), lightPointRadius1, true);
    lightPoint2.setLight(lightPointPosition2, glm::vec4(lightPointColor2, 1.0f), lightPointRadius2, true);
    lightPoint3.setLight(lightPointPosition3, glm::vec4(lightPointColor3, 1.0f), lightPointRadius3, true);

    lightDirectional1.setLight(lightDirectionalDirection1, glm::vec4(lightDirectionalColor1, 1.0f));


    //-------
    // Skybox
    //-------
    skybox.setup();

    //---------------------------------------------------------
    // Set the samplers for the lighting/post-processing passes
    //---------------------------------------------------------
    lightingBRDFShader.useShader();
    glUniform1i(glGetUniformLocation(lightingBRDFShader.Program, "gPosition"), 0);
    glUniform1i(glGetUniformLocation(lightingBRDFShader.Program, "gAlbedo"), 1);
    glUniform1i(glGetUniformLocation(lightingBRDFShader.Program, "gNormal"), 2);
    glUniform1i(glGetUniformLocation(lightingBRDFShader.Program, "gEffects"), 3);
    glUniform1i(glGetUniformLocation(lightingBRDFShader.Program, "sao"), 4);
    glUniform1i(glGetUniformLocation(lightingBRDFShader.Program, "envMap"), 5);
    glUniform1i(glGetUniformLocation(lightingBRDFShader.Program, "envMapIrradiance"), 6);
    glUniform1i(glGetUniformLocation(lightingBRDFShader.Program, "envMapPrefilter"), 7);
    glUniform1i(glGetUniformLocation(lightingBRDFShader.Program, "envMapLUT"), 8);

    saoShader.useShader();
    glUniform1i(glGetUniformLocation(saoShader.Program, "gPosition"), 0);
    glUniform1i(glGetUniformLocation(saoShader.Program, "gNormal"), 1);

    firstpassPPShader.useShader();
    glUniform1i(glGetUniformLocation(firstpassPPShader.Program, "sao"), 1);
    glUniform1i(glGetUniformLocation(firstpassPPShader.Program, "gEffects"), 2);


    //------------
    // SAO setup
    //------------
    saoSetup();


    //---------------------
    // Postprocessing setup
    //---------------------
    postprocessSetup();


    //----------
    // IBL setup
    //----------
    skybox.iblSetup(WIDTH, HEIGHT);


    //------------------------------
    // Queries setting for profiling
    //------------------------------
    GLuint64 startGeometryTime, startLightingTime, startSAOTime, startPostprocessTime, startForwardTime, startGUITime;
    GLuint64 stopGeometryTime, stopLightingTime, stopSAOTime, stopPostprocessTime, stopForwardTime, stopGUITime;

    unsigned int queryIDGeometry[2];
    unsigned int queryIDLighting[2];
    unsigned int queryIDSAO[2];
    unsigned int queryIDPostprocess[2];
    unsigned int queryIDForward[2];
    unsigned int queryIDGUI[2];

    glGenQueries(2, queryIDGeometry);
    glGenQueries(2, queryIDLighting);
    glGenQueries(2, queryIDSAO);
    glGenQueries(2, queryIDPostprocess);
    glGenQueries(2, queryIDForward);
    glGenQueries(2, queryIDGUI);


    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);


    while (!glfwWindowShouldClose(window))
    {
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents();
        cameraMove();

        //--------------
        // ImGui setting
        //--------------
        imGuiSetup();


        //------------------------
        // Geometry Pass rendering
        //------------------------
        glQueryCounter(queryIDGeometry[0], GL_TIMESTAMP);
        gBuffer.draw(camera);
        glQueryCounter(queryIDGeometry[1], GL_TIMESTAMP);

        //---------------
        // sao rendering
        //---------------
        glQueryCounter(queryIDSAO[0], GL_TIMESTAMP);
        glBindFramebuffer(GL_FRAMEBUFFER, saoFBO);
        glClear(GL_COLOR_BUFFER_BIT);

        if (saoMode)
        {
            // SAO noisy texture
            saoShader.useShader();

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, gBuffer.gPosition);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, gBuffer.gNormal);

            glUniform1i(glGetUniformLocation(saoShader.Program, "saoSamples"), saoSamples);
            glUniform1f(glGetUniformLocation(saoShader.Program, "saoRadius"), saoRadius);
            glUniform1i(glGetUniformLocation(saoShader.Program, "saoTurns"), saoTurns);
            glUniform1f(glGetUniformLocation(saoShader.Program, "saoBias"), saoBias);
            glUniform1f(glGetUniformLocation(saoShader.Program, "saoScale"), saoScale);
            glUniform1f(glGetUniformLocation(saoShader.Program, "saoContrast"), saoContrast);
            glUniform1i(glGetUniformLocation(saoShader.Program, "viewportWidth"), WIDTH);
            glUniform1i(glGetUniformLocation(saoShader.Program, "viewportHeight"), HEIGHT);

            quadRender.drawShape();

            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            // SAO blur pass
            glBindFramebuffer(GL_FRAMEBUFFER, saoBlurFBO);
            glClear(GL_COLOR_BUFFER_BIT);

            saoBlurShader.useShader();

            glUniform1i(glGetUniformLocation(saoBlurShader.Program, "saoBlurSize"), saoBlurSize);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, saoBuffer);

            quadRender.drawShape();
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glQueryCounter(queryIDSAO[1], GL_TIMESTAMP);


        //------------------------
        // Lighting Pass rendering
        //------------------------
        glQueryCounter(queryIDLighting[0], GL_TIMESTAMP);
        glBindFramebuffer(GL_FRAMEBUFFER, postprocessFBO);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        lightingBRDFShader.useShader();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gBuffer.gPosition);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gBuffer.gAlbedo);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, gBuffer.gNormal);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, gBuffer.gEffects);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, saoBlurBuffer);
        glActiveTexture(GL_TEXTURE5);
        skybox.bindEnvMapHDRTexture();
        glActiveTexture(GL_TEXTURE6);
        skybox.bindEnvMapIrradianceTexture();
        glActiveTexture(GL_TEXTURE7);
        skybox.bindEnvMapPrefilterTexture();
        glActiveTexture(GL_TEXTURE8);
        skybox.bindEnvMapLUTTexture();

        lightPoint1.setLightPosition(lightPointPosition1);
        lightPoint2.setLightPosition(lightPointPosition2);
        lightPoint3.setLightPosition(lightPointPosition3);
        lightPoint1.setLightColor(glm::vec4(lightPointColor1, 1.0f));
        lightPoint2.setLightColor(glm::vec4(lightPointColor2, 1.0f));
        lightPoint3.setLightColor(glm::vec4(lightPointColor3, 1.0f));
        lightPoint1.setLightRadius(lightPointRadius1);
        lightPoint2.setLightRadius(lightPointRadius2);
        lightPoint3.setLightRadius(lightPointRadius3);

        for (int i = 0; i < Light::lightPointList.size(); i++)
        {
            Light::lightPointList[i].renderToShader(lightingBRDFShader, camera);
        }

        lightDirectional1.setLightDirection(lightDirectionalDirection1);
        lightDirectional1.setLightColor(glm::vec4(lightDirectionalColor1, 1.0f));

        for (int i = 0; i < Light::lightDirectionalList.size(); i++)
        {
            Light::lightDirectionalList[i].renderToShader(lightingBRDFShader, camera);
        }

        glUniformMatrix4fv(glGetUniformLocation(lightingBRDFShader.Program, "inverseView"), 1, GL_FALSE, glm::value_ptr(glm::transpose(camera.GetViewMatrix())));
        glUniformMatrix4fv(glGetUniformLocation(lightingBRDFShader.Program, "inverseProj"), 1, GL_FALSE, glm::value_ptr(glm::inverse(camera.GetProjectionMatrix())));
        glUniformMatrix4fv(glGetUniformLocation(lightingBRDFShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(camera.GetViewMatrix()));
        glUniform1f(glGetUniformLocation(lightingBRDFShader.Program, "materialRoughness"), gBuffer.materialRoughness);
        glUniform1f(glGetUniformLocation(lightingBRDFShader.Program, "materialMetallicity"), gBuffer.materialMetallicity);
        glUniform3f(glGetUniformLocation(lightingBRDFShader.Program, "materialF0"), materialF0.r, materialF0.g, materialF0.b);
        glUniform1f(glGetUniformLocation(lightingBRDFShader.Program, "ambientIntensity"), ambientIntensity);
        glUniform1i(glGetUniformLocation(lightingBRDFShader.Program, "gBufferView"), gBufferView);
        glUniform1i(glGetUniformLocation(lightingBRDFShader.Program, "pointMode"), pointMode);
        glUniform1i(glGetUniformLocation(lightingBRDFShader.Program, "directionalMode"), directionalMode);
        glUniform1i(glGetUniformLocation(lightingBRDFShader.Program, "iblMode"), iblMode);
        glUniform1i(glGetUniformLocation(lightingBRDFShader.Program, "attenuationMode"), attenuationMode);

        quadRender.drawShape();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glQueryCounter(queryIDLighting[1], GL_TIMESTAMP);


        //-------------------------------
        // Post-processing Pass rendering
        //-------------------------------
        glQueryCounter(queryIDPostprocess[0], GL_TIMESTAMP);
        glClear(GL_COLOR_BUFFER_BIT);

        firstpassPPShader.useShader();
        glUniform1i(glGetUniformLocation(firstpassPPShader.Program, "gBufferView"), gBufferView);
        glUniform2f(glGetUniformLocation(firstpassPPShader.Program, "screenTextureSize"), 1.0f / WIDTH, 1.0f / HEIGHT);
        glUniform1f(glGetUniformLocation(firstpassPPShader.Program, "cameraAperture"), cameraAperture);
        glUniform1f(glGetUniformLocation(firstpassPPShader.Program, "cameraShutterSpeed"), cameraShutterSpeed);
        glUniform1f(glGetUniformLocation(firstpassPPShader.Program, "cameraISO"), cameraISO);
        glUniform1i(glGetUniformLocation(firstpassPPShader.Program, "saoMode"), saoMode);
        glUniform1i(glGetUniformLocation(firstpassPPShader.Program, "fxaaMode"), fxaaMode);
        glUniform1i(glGetUniformLocation(firstpassPPShader.Program, "motionBlurMode"), motionBlurMode);
        glUniform1f(glGetUniformLocation(firstpassPPShader.Program, "motionBlurScale"), int(ImGui::GetIO().Framerate) / 60.0f);
        glUniform1i(glGetUniformLocation(firstpassPPShader.Program, "motionBlurMaxSamples"), motionBlurMaxSamples);
        glUniform1i(glGetUniformLocation(firstpassPPShader.Program, "tonemappingMode"), tonemappingMode);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, postprocessBuffer);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, saoBlurBuffer);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, gBuffer.gEffects);

        quadRender.drawShape();

        glQueryCounter(queryIDPostprocess[1], GL_TIMESTAMP);


        //-----------------------
        // Forward Pass rendering
        //-----------------------
        glQueryCounter(queryIDForward[0], GL_TIMESTAMP);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer.gBuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);


        // Copy the depth informations from the Geometry Pass into the default framebuffer
        glBlitFramebuffer(0, 0, WIDTH, HEIGHT, 0, 0, WIDTH, HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        

        // Shape(s) rendering
        if (pointMode)
        {
            simpleShader.useShader();
            glUniformMatrix4fv(glGetUniformLocation(simpleShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(camera.GetProjectionMatrix()));
            glUniformMatrix4fv(glGetUniformLocation(simpleShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(camera.GetViewMatrix()));

            for (int i = 0; i < Light::lightPointList.size(); i++)
            {
                glUniform4f(glGetUniformLocation(simpleShader.Program, "lightColor"), Light::lightPointList[i].getLightColor().r, Light::lightPointList[i].getLightColor().g, Light::lightPointList[i].getLightColor().b, Light::lightPointList[i].getLightColor().a);

                if (Light::lightPointList[i].isMesh())
                    Light::lightPointList[i].lightMesh.drawShape(simpleShader, camera.GetViewMatrix(), camera.GetProjectionMatrix(), camera);
            }
        }
        glQueryCounter(queryIDForward[1], GL_TIMESTAMP);

        //----------------
        // Capture
        //----------------
        if(recording)
        {
            const auto channels = 4;
            unsigned char* ptr = new unsigned char[WIDTH * HEIGHT * channels];
            glPixelStorei(GL_PACK_ALIGNMENT, 1);
            glReadPixels(0, 0, WIDTH, HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, ptr);
            // glReadPixels reads the given rectangle from bottom-left to top-right, so we must
            // reverse it
            for (int y = 0; y < HEIGHT / 2; y++)
            {
                const int swapY = HEIGHT - y - 1;
                for (int x = 0; x < WIDTH; x++)
                {
                    const int offset = channels * (x + y * WIDTH);
                    const int swapOffset = channels * (x + swapY * WIDTH);

                    // Swap R, G and B of the 2 pixels
                    std::swap(ptr[offset + 0], ptr[swapOffset + 0]);
                    std::swap(ptr[offset + 1], ptr[swapOffset + 1]);
                    std::swap(ptr[offset + 2], ptr[swapOffset + 2]);
                }
            }
            std::cout << "writing frame" << frame << " to " << recording_path << std::endl;
            stbi_write_png(std::string(recording_path+"frame" + std::to_string(frame++) + ".png").c_str(), WIDTH, HEIGHT, 4, ptr, WIDTH * channels);
            delete[] ptr;
        }


        //----------------
        // ImGui rendering
        //----------------
        glQueryCounter(queryIDGUI[0], GL_TIMESTAMP);
        ImGui::Render();
        glQueryCounter(queryIDGUI[1], GL_TIMESTAMP);


        //--------------
        // GPU profiling
        //--------------
        GLint stopGeometryTimerAvailable = 0;
        GLint stopLightingTimerAvailable = 0;
        GLint stopSAOTimerAvailable = 0;
        GLint stopPostprocessTimerAvailable = 0;
        GLint stopForwardTimerAvailable = 0;
        GLint stopGUITimerAvailable = 0;

        while (!stopGeometryTimerAvailable && !stopLightingTimerAvailable && !stopSAOTimerAvailable && !stopPostprocessTimerAvailable && !stopForwardTimerAvailable && !stopGUITimerAvailable)
        {
            glGetQueryObjectiv(queryIDGeometry[1], GL_QUERY_RESULT_AVAILABLE, &stopGeometryTimerAvailable);
            glGetQueryObjectiv(queryIDLighting[1], GL_QUERY_RESULT_AVAILABLE, &stopLightingTimerAvailable);
            glGetQueryObjectiv(queryIDSAO[1], GL_QUERY_RESULT_AVAILABLE, &stopSAOTimerAvailable);
            glGetQueryObjectiv(queryIDPostprocess[1], GL_QUERY_RESULT_AVAILABLE, &stopPostprocessTimerAvailable);
            glGetQueryObjectiv(queryIDForward[1], GL_QUERY_RESULT_AVAILABLE, &stopForwardTimerAvailable);
            glGetQueryObjectiv(queryIDGUI[1], GL_QUERY_RESULT_AVAILABLE, &stopGUITimerAvailable);
        }

        glGetQueryObjectui64v(queryIDGeometry[0], GL_QUERY_RESULT, &startGeometryTime);
        glGetQueryObjectui64v(queryIDGeometry[1], GL_QUERY_RESULT, &stopGeometryTime);
        glGetQueryObjectui64v(queryIDLighting[0], GL_QUERY_RESULT, &startLightingTime);
        glGetQueryObjectui64v(queryIDLighting[1], GL_QUERY_RESULT, &stopLightingTime);
        glGetQueryObjectui64v(queryIDSAO[0], GL_QUERY_RESULT, &startSAOTime);
        glGetQueryObjectui64v(queryIDSAO[1], GL_QUERY_RESULT, &stopSAOTime);
        glGetQueryObjectui64v(queryIDPostprocess[0], GL_QUERY_RESULT, &startPostprocessTime);
        glGetQueryObjectui64v(queryIDPostprocess[1], GL_QUERY_RESULT, &stopPostprocessTime);
        glGetQueryObjectui64v(queryIDForward[0], GL_QUERY_RESULT, &startForwardTime);
        glGetQueryObjectui64v(queryIDForward[1], GL_QUERY_RESULT, &stopForwardTime);
        glGetQueryObjectui64v(queryIDGUI[0], GL_QUERY_RESULT, &startGUITime);
        glGetQueryObjectui64v(queryIDGUI[1], GL_QUERY_RESULT, &stopGUITime);

        deltaGeometryTime = (stopGeometryTime - startGeometryTime) / 1000000.0;
        deltaLightingTime = (stopLightingTime - startLightingTime) / 1000000.0;
        deltaSAOTime = (stopSAOTime - startSAOTime) / 1000000.0;
        deltaPostprocessTime = (stopPostprocessTime - startPostprocessTime) / 1000000.0;
        deltaForwardTime = (stopForwardTime - startForwardTime) / 1000000.0;
        deltaGUITime = (stopGUITime - startGUITime) / 1000000.0;

        glfwSwapBuffers(window);
    }

    //---------
    // Cleaning
    //---------
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

    ImGui::Begin("GLEngine", &guiIsOpen, ImVec2(0, 0), 0.5f, ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoSavedSettings);
    ImGui::SetWindowSize(ImVec2(350, HEIGHT));

    if (ImGui::CollapsingHeader("Rendering", 0, true, true))
    {
        if (ImGui::TreeNode("Material"))
        {
            ImGui::Checkbox("Negative Normals", &gBuffer.negativeNormals);
            ImGui::Checkbox("Roughness Texture", &gBuffer.useRoughnessTexture);
            ImGui::Checkbox("Albedo Texture", &gBuffer.useAlbedoTexture);
            ImGui::Checkbox("Normal Texture", &gBuffer.useNormalTexture);
            ImGui::Checkbox("Metalness Texture", &gBuffer.useMetalnessTexture);
            if(!gBuffer.useAlbedoTexture)
                ImGui::ColorEdit3("Albedo", (float*)&gBuffer.albedoColor);
            if(!gBuffer.useRoughnessTexture)
                ImGui::SliderFloat("Roughness", &gBuffer.materialRoughness, 0.0f, 1.0f);
            if (!gBuffer.useMetalnessTexture)
                ImGui::SliderFloat("Metalness", &gBuffer.materialMetallicity, 0.0f, 1.0f);
            ImGui::SliderFloat3("F0", (float*)&materialF0, 0.0f, 1.0f);
            ImGui::SliderFloat("Ambient Intensity", &ambientIntensity, 0.0f, 1.0f);

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Lighting"))
        {
            if (ImGui::TreeNode("Mode"))
            {
                ImGui::Checkbox("Point", &pointMode);
                ImGui::Checkbox("Directional", &directionalMode);
                ImGui::Checkbox("Image-Based Lighting", &iblMode);

                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Point"))
            {
                if (ImGui::TreeNode("Position"))
                {
                    ImGui::SliderFloat3("Point 1", (float*)&lightPointPosition1, -5.0f, 5.0f);
                    ImGui::SliderFloat3("Point 2", (float*)&lightPointPosition2, -5.0f, 5.0f);
                    ImGui::SliderFloat3("Point 3", (float*)&lightPointPosition3, -5.0f, 5.0f);

                    ImGui::TreePop();
                }

                if (ImGui::TreeNode("Color"))
                {
                    ImGui::ColorEdit3("Point 1", (float*)&lightPointColor1);
                    ImGui::ColorEdit3("Point 2", (float*)&lightPointColor2);
                    ImGui::ColorEdit3("Point 3", (float*)&lightPointColor3);

                    ImGui::TreePop();
                }

                if (ImGui::TreeNode("Radius"))
                {
                    ImGui::SliderFloat("Point 1", &lightPointRadius1, 0.0f, 10.0f);
                    ImGui::SliderFloat("Point 2", &lightPointRadius2, 0.0f, 10.0f);
                    ImGui::SliderFloat("Point 3", &lightPointRadius3, 0.0f, 10.0f);

                    ImGui::TreePop();
                }

                if (ImGui::TreeNode("Attenuation"))
                {
                    ImGui::RadioButton("Quadratic", &attenuationMode, 1);
                    ImGui::RadioButton("UE4", &attenuationMode, 2);

                    ImGui::TreePop();
                }

                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Directional"))
            {
                if (ImGui::TreeNode("Direction"))
                {
                    ImGui::SliderFloat3("Direction 1", (float*)&lightDirectionalDirection1, -5.0f, 5.0f);

                    ImGui::TreePop();
                }

                if (ImGui::TreeNode("Color"))
                {
                    ImGui::ColorEdit3("Direct. 1", (float*)&lightDirectionalColor1);

                    ImGui::TreePop();
                }

                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Environment map"))
            {
                if (ImGui::Button("Appartment"))
                {
                    skybox.setTextureHDR("hdr/appart.hdr", "appartHDR", true);
                    skybox.iblSetup(WIDTH, HEIGHT);
                }

                if (ImGui::Button("Pisa"))
                {
                    skybox.setTextureHDR("hdr/pisa.hdr", "pisaHDR", true);
                    skybox.iblSetup(WIDTH, HEIGHT);
                }

                if (ImGui::Button("Canyon"))
                {
                    skybox.setTextureHDR("hdr/canyon.hdr", "canyonHDR", true);
                    skybox.iblSetup(WIDTH, HEIGHT);
                }

                if (ImGui::Button("Loft"))
                {
                    skybox.setTextureHDR("hdr/loft.hdr", "loftHDR", true);
                    skybox.iblSetup(WIDTH, HEIGHT);
                }

                if (ImGui::Button("Path"))
                {
                    skybox.setTextureHDR("hdr/path.hdr", "pathHDR", true);
                    skybox.iblSetup(WIDTH, HEIGHT);
                }

                if (ImGui::Button("Circus"))
                {
                    skybox.setTextureHDR("hdr/circus.hdr", "circusHDR", true);
                    skybox.iblSetup(WIDTH, HEIGHT);
                }

                ImGui::TreePop();
            }

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Post processing"))
        {
            if (ImGui::TreeNode("SAO"))
            {
                ImGui::Checkbox("Enable", &saoMode);

                ImGui::SliderInt("Samples", &saoSamples, 0, 64);
                ImGui::SliderFloat("Radius", &saoRadius, 0.0f, 3.0f);
                ImGui::SliderInt("Turns", &saoTurns, 0, 16);
                ImGui::SliderFloat("Bias", &saoBias, 0.0f, 0.1f);
                ImGui::SliderFloat("Scale", &saoScale, 0.0f, 3.0f);
                ImGui::SliderFloat("Contrast", &saoContrast, 0.0f, 3.0f);
                ImGui::SliderInt("Blur Size", &saoBlurSize, 0, 8);

                ImGui::TreePop();
            }

            if (ImGui::TreeNode("FXAA"))
            {
                ImGui::Checkbox("Enable", &fxaaMode);

                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Motion Blur"))
            {
                ImGui::Checkbox("Enable", &motionBlurMode);
                ImGui::SliderInt("Max Samples", &motionBlurMaxSamples, 1, 128);

                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Tonemapping"))
            {
                ImGui::RadioButton("Reinhard", &tonemappingMode, 1);
                ImGui::RadioButton("Filmic", &tonemappingMode, 2);
                ImGui::RadioButton("Uncharted", &tonemappingMode, 3);

                ImGui::TreePop();
            }

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Camera"))
        {
            ImGui::SliderFloat("Aperture", &cameraAperture, 1.0f, 32.0f);
            ImGui::SliderFloat("Shutter Speed", &cameraShutterSpeed, 0.001f, 1.0f);
            ImGui::SliderFloat("ISO", &cameraISO, 100.0f, 3200.0f);
            ImGui::Checkbox("Orbitting Camera", &isOrbitCamera);
            if(isOrbitCamera)
            {
                ImGui::Checkbox("Record Mode", &record);
                if (record)
                {
                    if (ImGui::Button("Start Record"))
                    {
                        // set first model
                        current_pool_model = model_pool_paths.begin();
                        gBuffer.loadModel(current_pool_model->generic_string(), glm::vec3(2.0f));

                        auto d = std::find(model_pool_paths.begin(), model_pool_paths.end(), static_cast<fs::path>(*current_pool_model)) - model_pool_paths.begin();
                        if (d < invertNormal.size()) {
                            gBuffer.negativeNormals = invertNormal[d];
                        }

                        //set proper output dir
                        auto output = current_pool_model->generic_string();
                        const auto outputDir = fs::path(output.substr(0, output.size() - 4));
                        if(!fs::is_directory(outputDir))
                        {
                            fs::create_directory(outputDir);
                        }
                        recording_path = outputDir.generic_string() + "/";

                        // set background
                        useEnvmapIndex = 0;
                        skybox.setTextureHDR("hdr/appart.hdr", "appartHDR", true);
                        skybox.iblSetup(WIDTH, HEIGHT);
                        
                        // reset indices
                        index_fullRotationTicks = 0;
                        index_halfRotationTicks = 0;

                        // reset frame index
                        frame = 0;

                        // set recording
                        recording = true;
                    }

                    if(recording)
                    {
                        if(ImGui::Button("Stop Recording"))
                        {
                            recording = false;
                            index_fullRotationTicks = fullRotationTicks + 1;
                            index_halfRotationTicks = halfRotationTicks + 1;
                            current_pool_model = model_pool_paths.end();
                        }
                    }

                    if (index_fullRotationTicks <= fullRotationTicks && index_halfRotationTicks <= halfRotationTicks)
                    {
                        theta = -glm::pi<float>() + 2.0f * glm::pi<float>() / static_cast<float>(fullRotationTicks) * index_fullRotationTicks;
                        rho = -glm::pi<float>()*.5f + glm::pi<float>() / static_cast<float>(halfRotationTicks) * index_halfRotationTicks;
                        index_fullRotationTicks++;
                        if (index_fullRotationTicks >= fullRotationTicks)
                        {
                            index_fullRotationTicks = 0;
                            index_halfRotationTicks++;
                        }
                    } else
                    {
                        if (useEnvmapIndex < 6 && recording)
                        {
                            ++useEnvmapIndex;
                            switch (useEnvmapIndex)
                            {
                            case 0:
                                skybox.setTextureHDR("hdr/appart.hdr", "appartHDR", true);
                                break;
                            case 1:
                                skybox.setTextureHDR("hdr/canyon.hdr", "canyonHDR", true);
                                break;
                            case 2:
                                skybox.setTextureHDR("hdr/pisa.hdr", "pisaHDR", true);
                                break;
                            case 3:
                                skybox.setTextureHDR("hdr/loft.hdr", "loftHDR", true);
                                break;
                            case 4:
                                skybox.setTextureHDR("hdr/path.hdr", "pathHDR", true);
                                break;
                            case 5:
                                skybox.setTextureHDR("hdr/circus.hdr", "circusHDR", true);
                                break;
                            default:
                                skybox.setTextureHDR("hdr/appart.hdr", "appartHDR", true);
                                break;
                            }

                            skybox.iblSetup(WIDTH, HEIGHT);

                            // reset indices
                            index_fullRotationTicks = 0;
                            index_halfRotationTicks = 0;
                        } else
                        if(current_pool_model != model_pool_paths.end() && recording)
                        {
                            // iterate next model
                            ++current_pool_model;
                            gBuffer.loadModel(current_pool_model->generic_string(), glm::vec3(2.0f));

                            auto d = static_cast<int>(std::find(model_pool_paths.begin(), model_pool_paths.end(), static_cast<fs::path>(*current_pool_model)) - model_pool_paths.begin());
                            if (d < invertNormal.size()) {
                                gBuffer.negativeNormals = invertNormal[d];
                            }

                            //set proper output dir
                            auto output = current_pool_model->generic_string();
                            const auto outputDir = fs::path(output.substr(0, output.size() - 4));
                            if (!fs::is_directory(outputDir))
                            {
                                fs::create_directory(outputDir);
                            }

                            recording_path = outputDir.generic_string() + "/";

                            useEnvmapIndex = 0;

                            // reset indices
                            index_fullRotationTicks = 0;
                            index_halfRotationTicks = 0;

                            // reset frame index
                            frame = 0;
                        } else
                        {
                            recording = false;
                        }
                    }
                }
                else
                {
                    ImGui::SliderFloat("Theta", &theta, -glm::pi<float>(), glm::pi<float>());
                    ImGui::SliderFloat("Rho", &rho, -glm::pi<float>()*.5f, glm::pi<float>()*.5f);
                    ImGui::SliderFloat("Radius", &radius, 1.0, 10.f);
                }
                const auto d = glm::cos(rho) * radius;
                const auto x = d * glm::cos(theta);
                const auto y = glm::sqrt(radius * radius - d * d) * (rho<0 ? 1.0f : -1.0f);
                const auto z = d * glm::sin(theta);
                if (glm::abs(rho) >= glm::pi<float>()*.5f)
                {
                    camera.lookAt(glm::vec3(x, y, z), glm::vec3(0.f), glm::vec3(0.f, -1.f, 0.f));
                }else
                {
                    camera.lookAt(glm::vec3(x, y, z), glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f));
                }
            }
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Object"))
        {
            ImGui::SliderFloat3("Position", (float*)&gBuffer.modelPosition, -5.0f, 5.0f);
            ImGui::SliderFloat("Rotation Speed", &gBuffer.modelRotationSpeed, 0.0f, 50.0f);
            ImGui::SliderFloat3("Rotation Axis", (float*)&gBuffer.modelRotationAxis, 0.0f, 1.0f);

            if (ImGui::TreeNode("Model"))
            {
                if (ImGui::Button("Sphere"))
                {
                    gBuffer.loadModel("sphere/sphere.obj", glm::vec3(0.6f));
                }

                if (ImGui::Button("Teapot"))
                {
                    gBuffer.loadModel("teapot/teapot.obj", glm::vec3(0.6f));
                }

                if (ImGui::Button("Shader ball"))
                {
                    gBuffer.loadModel("shaderball/shaderball.obj", glm::vec3(0.1f));
                }

                if (ImGui::TreeNode("Pool"))
                {
                    for(auto p : model_pool_paths)
                    {
                        if (ImGui::Button(std::string(p.filename().generic_string()).c_str()))
                        {
                            gBuffer.loadModel(p.generic_string(), glm::vec3(2.0f));
                            auto d = static_cast<int>(std::find(model_pool_paths.begin(), model_pool_paths.end(), p) - model_pool_paths.begin());
                            if (d < invertNormal.size()) {
                                gBuffer.negativeNormals = invertNormal[d];
                            }
                        }
                    }
                }

                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Material"))
            {
                if (ImGui::Button("Rusted iron"))
                {
                    gBuffer.setTexture(GBuffer::Albedo, "pbr/rustediron/rustediron_albedo.png", "ironAlbedo", true);
                    gBuffer.setTexture(GBuffer::Normal, "pbr/rustediron/rustediron_normal.png", "ironNormal", true);
                    gBuffer.setTexture(GBuffer::Roughness, "pbr/rustediron/rustediron_roughness.png", "ironRoughness", true);
                    gBuffer.setTexture(GBuffer::Metalness, "pbr/rustediron/rustediron_metalness.png", "ironMetalness", true);
                    gBuffer.setTexture(GBuffer::AmbientOcclusion, "pbr/rustediron/rustediron_ao.png", "ironAO", true);

                    materialF0 = glm::vec3(0.04f);
                }

                if (ImGui::Button("Gold"))
                {
                    gBuffer.setTexture(GBuffer::Albedo, "pbr/gold/gold_albedo.png", "goldAlbedo", true);
                    gBuffer.setTexture(GBuffer::Normal, "pbr/gold/gold_normal.png", "goldNormal", true);
                    gBuffer.setTexture(GBuffer::Roughness, "pbr/gold/gold_roughness.png", "goldRoughness", true);
                    gBuffer.setTexture(GBuffer::Metalness, "pbr/gold/gold_metalness.png", "goldMetalness", true);
                    gBuffer.setTexture(GBuffer::AmbientOcclusion, "pbr/gold/gold_ao.png", "goldAO", true);

                    materialF0 = glm::vec3(1.0f, 0.72f, 0.29f);
                }

                if (ImGui::Button("Ceramic"))
                {
                    gBuffer.setTexture(GBuffer::Albedo, "pbr/ceramic/ceramic_albedo.png", "goldAlbedo", true);
                    gBuffer.setTexture(GBuffer::Normal, "pbr/ceramic/ceramic_normal.png", "goldNormal", true);
                    gBuffer.setTexture(GBuffer::Roughness, "pbr/ceramic/ceramic_roughness.png", "goldRoughness", true);
                    gBuffer.setTexture(GBuffer::Metalness, "pbr/ceramic/ceramic_metalness.png", "goldMetalness", true);
                    gBuffer.setTexture(GBuffer::AmbientOcclusion, "pbr/ceramic/ceramic_ao.png", "goldAO", true);

                    materialF0 = glm::vec3(1.0f, 1.0f, 1.0f);
                }

                if (ImGui::Button("Woodfloor"))
                {
                    gBuffer.setTexture(GBuffer::Albedo, "pbr/woodfloor/woodfloor_albedo.png", "woodfloorAlbedo", true);
                    gBuffer.setTexture(GBuffer::Normal, "pbr/woodfloor/woodfloor_normal.png", "woodfloorNormal", true);
                    gBuffer.setTexture(GBuffer::Roughness, "pbr/woodfloor/woodfloor_roughness.png", "woodfloorRoughness", true);
                    gBuffer.setTexture(GBuffer::Metalness, "pbr/woodfloor/woodfloor_metalness.png", "woodfloorMetalness", true);
                    gBuffer.setTexture(GBuffer::AmbientOcclusion, "pbr/woodfloor/woodfloor_ao.png", "woodfloorAO", true);

                    materialF0 = glm::vec3(0.04f);
                }

                ImGui::TreePop();
            }

            ImGui::TreePop();
        }
    }

    if (ImGui::CollapsingHeader("Profiling", 0, true, true))
    {
        ImGui::Text("Geometry Pass :    %.4f ms", deltaGeometryTime);
        ImGui::Text("Lighting Pass :    %.4f ms", deltaLightingTime);
        ImGui::Text("SAO Pass :         %.4f ms", deltaSAOTime);
        ImGui::Text("Postprocess Pass : %.4f ms", deltaPostprocessTime);
        ImGui::Text("Forward Pass :     %.4f ms", deltaForwardTime);
        ImGui::Text("GUI Pass :         %.4f ms", deltaGUITime);
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


void saoSetup()
{
    // SAO Buffer
    glGenFramebuffers(1, &saoFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, saoFBO);
    glGenTextures(1, &saoBuffer);
    glBindTexture(GL_TEXTURE_2D, saoBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, saoBuffer, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "SAO Framebuffer not complete !" << std::endl;

    // SAO Blur Buffer
    glGenFramebuffers(1, &saoBlurFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, saoBlurFBO);
    glGenTextures(1, &saoBlurBuffer);
    glBindTexture(GL_TEXTURE_2D, saoBlurBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, saoBlurBuffer, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "SAO Blur Framebuffer not complete !" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void postprocessSetup()
{
    // Post-processing Buffer
    glGenFramebuffers(1, &postprocessFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, postprocessFBO);

    glGenTextures(1, &postprocessBuffer);
    glBindTexture(GL_TEXTURE_2D, postprocessBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postprocessBuffer, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Postprocess Framebuffer not complete !" << std::endl;
}


static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error %d: %s\n", error, description);
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (key == GLFW_KEY_F11 && action == GLFW_PRESS)
    {
        screenMode = !screenMode;


    }

    if (keys[GLFW_KEY_1])
        gBufferView = 1;

    if (keys[GLFW_KEY_2])
        gBufferView = 2;

    if (keys[GLFW_KEY_3])
        gBufferView = 3;

    if (keys[GLFW_KEY_4])
        gBufferView = 4;

    if (keys[GLFW_KEY_5])
        gBufferView = 5;

    if (keys[GLFW_KEY_6])
        gBufferView = 6;

    if (keys[GLFW_KEY_7])
        gBufferView = 7;

    if (keys[GLFW_KEY_8])
        gBufferView = 8;

    if (keys[GLFW_KEY_9])
        gBufferView = 9;

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

    if (cameraMode)
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
    if (cameraMode)
        camera.scrollCall(yoffset);
}

float random(float min, float max)
{
    return ((static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX)) * (max - min)) + min;
}