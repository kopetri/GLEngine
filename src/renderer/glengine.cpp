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
#include "ssao.h"
#include "lighting.h"
#include "postprocess.h"
#include "boundingbox.h"

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
void captureFrame(std::string output_path = "");
void updatePose();
void cameraMove();
void imGuiSetup();
void prepareModel();
void prepareSkybox();
float random(float min, float max);

//---------------------------------
// Variables & objects declarations
//---------------------------------

GLuint WIDTH = 1200;
GLuint HEIGHT = 800;

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

GLfloat theta = 0.0f;
GLfloat rho = 0.0f;
GLfloat radius = 3.5;

bool cameraMode;


bool screenMode = false;
bool firstMouse = true;
bool guiIsOpen = true;

bool isOrbitCamera = false;

bool keys[1024];
/*
std::vector<bool> invertNormal = {
    true,
    true,
    true
};
*/
std::vector<bool> invertNormal = {
    false,
    true,
    true,
    false,
    true,
    true,
    true,
    true,
    false,
    true,
    true,
    true,
    true,
    false,
    true,
    false,
    false,
};

/*
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
*/

int frame = -1;
int imageCount = 50;
int useEnvmapIndex = 0;
bool recording = false;
bool takeSnapShot = false;
bool enableSegmentation = false;
bool renderSegmentation = false;
int recorded_frame = 0;
bool use_unique_output_index = false;
bool iterate_over_skyboxes = false;

bool targetRendered = false;
bool labelRendered = false;

int currentSnapshot = 0;
std::string recording_path = "./";
std::string snapshot_path = TEXTURE_PATH;
std::vector<fs::path> model_pool_paths;
std::vector<fs::path> background_paths;
std::vector<fs::path>::iterator current_pool_model;
std::vector<fs::path>::iterator current_skybox;
std::vector<fs::path> skyboxes;






Camera camera(static_cast<GLfloat>(WIDTH), static_cast<GLfloat>(HEIGHT), 0.1f, 100.0f, glm::vec3(0.0f, 0.0f, 4.0f));

Skybox skybox;
GBuffer gBuffer(WIDTH, HEIGHT);
SSAO ssao(WIDTH, HEIGHT);
Lighting lighting(WIDTH, HEIGHT);
Postprocess postprocess(WIDTH, HEIGHT);



Shape quadRender;



Material pbrMat;


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
    for (auto & p : fs::directory_iterator(std::string(MODEL_PATH).append("pool2/")))
    {
        if(!is_directory(p))
        {
            auto f = p.path().generic_string().substr(std::string(MODEL_PATH).size());
            std::cout << "found model " << f << " in pool." << std::endl;
            model_pool_paths.push_back(f);
        }
    }

    //---------
    // Background patches 
    //---------
    for (auto & p : fs::directory_iterator(std::string(TEXTURE_PATH).append("patches-512/")))
    {
        if (!is_directory(p))
        {
            auto f = p.path().generic_string().substr(std::string(TEXTURE_PATH).size());
            background_paths.push_back(f);
        }
    }


    quadRender.setShape("quad", glm::vec3(0.0f));


    //-------
    // Skybox
    //-------
    for (auto & p : fs::directory_iterator(std::string(TEXTURE_PATH).append("jpg/")))
    {
        if (!is_directory(p))
        {
            auto f = p.path().generic_string().substr(std::string(TEXTURE_PATH).size());
            std::cout << "found skybox " << f << " in pool." << std::endl;
            skyboxes.push_back(f);
        }
    }
    current_skybox = skyboxes.begin();
    skybox.setup();

    //---------------------------------------------------------
    // Set the samplers for the lighting/post-processing passes
    //---------------------------------------------------------
    lighting.setup();
    lighting.setRender(quadRender);

    

    //---------------
    // G-Buffer setup
    //---------------
    gBuffer.setup();

    //------------
    // SAO setup
    //------------
    ssao.setup();
    ssao.setRender(quadRender);


    //---------------------
    // Postprocessing setup
    //---------------------
    postprocess.setup(lighting.framebuffer());
    postprocess.setRender(quadRender);


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

    gBuffer.loadModel(model_pool_paths[0].generic_string(), glm::vec3(2.0));

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

        if (recording) {
            // check for next model to be loaded
            if (frame < 0 || frame >= imageCount)
            {
                frame = 0;
                renderSegmentation = false;
                prepareModel();
            }

            // update pose
            if (!enableSegmentation || targetRendered && labelRendered) {
                updatePose();
                targetRendered = false;
                labelRendered = false;
                renderSegmentation = false;
            }

            // check for segmentation pass
            if (enableSegmentation)
                renderSegmentation = !renderSegmentation;
            else
                renderSegmentation = enableSegmentation;
        }
        else {
            renderSegmentation = enableSegmentation;
        }

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
        if (postprocess.saoMode) ssao.draw(gBuffer);
        glQueryCounter(queryIDSAO[1], GL_TIMESTAMP);


        //------------------------
        // Lighting Pass rendering
        //------------------------
        glQueryCounter(queryIDLighting[0], GL_TIMESTAMP);
        lighting.draw(camera, gBuffer, ssao, skybox, renderSegmentation);
        glQueryCounter(queryIDLighting[1], GL_TIMESTAMP);


        //-------------------------------
        // Post-processing Pass rendering
        //-------------------------------
        glQueryCounter(queryIDPostprocess[0], GL_TIMESTAMP);
        postprocess.draw(gBuffer, ssao, lighting, renderSegmentation);
        glQueryCounter(queryIDPostprocess[1], GL_TIMESTAMP);


        //-----------------------
        // Forward Pass rendering
        //-----------------------
        glQueryCounter(queryIDForward[0], GL_TIMESTAMP);
        lighting.forwardPass(camera, gBuffer);
        glQueryCounter(queryIDForward[1], GL_TIMESTAMP);

        //----------------
        // Capture
        //----------------
        if(recording)
        {
            captureFrame();
        }
        if (takeSnapShot) {
            takeSnapShot = false;
            captureFrame(std::string(snapshot_path).append("snapshot-").append(std::to_string(currentSnapshot++)).append(".png"));
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
        if (ImGui::Button("Take Snapchot")) {
            takeSnapShot = true;
        }

        ImGui::Checkbox("Enable Segmentation", &enableSegmentation);
        if (enableSegmentation) {
            ImGui::ColorEdit3("Class: Background", (float*)&lighting.backgroundColor);
            ImGui::ColorEdit3("Class: Foreground", (float*)&lighting.foregroundColor);
        }

        if (ImGui::TreeNode("Material"))
        {
            ImGui::Checkbox("Negative Normals", &gBuffer.negativeNormals);
            ImGui::Checkbox("Roughness Texture", &gBuffer.useRoughnessTexture);
            ImGui::Checkbox("Albedo Texture", &gBuffer.useAlbedoTexture);
            ImGui::Checkbox("Normal Texture", &gBuffer.useNormalTexture);
            ImGui::Checkbox("Metalness Texture", &gBuffer.useMetalnessTexture);
            if (!gBuffer.useAlbedoTexture)
                ImGui::ColorEdit3("Albedo", (float*)&gBuffer.albedoColor);
            if (!gBuffer.useRoughnessTexture)
                ImGui::SliderFloat("Roughness", &gBuffer.materialRoughness, 0.0f, 1.0f);
            if (!gBuffer.useMetalnessTexture)
                ImGui::SliderFloat("Metalness", &gBuffer.materialMetallicity, 0.0f, 1.0f);
            ImGui::SliderFloat3("F0", (float*)&lighting.materialF0, 0.0f, 1.0f);
            ImGui::SliderFloat("Ambient Intensity", &lighting.ambientIntensity, 0.0f, 1.0f);

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Lighting"))
        {
            if (ImGui::TreeNode("Mode"))
            {
                ImGui::Checkbox("Point", &lighting.pointMode);
                ImGui::Checkbox("Directional", &lighting.directionalMode);
                ImGui::Checkbox("Image-Based Lighting", &lighting.iblMode);

                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Point"))
            {
                if (ImGui::TreeNode("Position"))
                {
                    ImGui::SliderFloat3("Point 1", (float*)&lighting.lightPointPosition1, -5.0f, 5.0f);
                    ImGui::SliderFloat3("Point 2", (float*)&lighting.lightPointPosition2, -5.0f, 5.0f);
                    ImGui::SliderFloat3("Point 3", (float*)&lighting.lightPointPosition3, -5.0f, 5.0f);

                    ImGui::TreePop();
                }

                if (ImGui::TreeNode("Color"))
                {
                    ImGui::ColorEdit3("Point 1", (float*)&lighting.lightPointColor1);
                    ImGui::ColorEdit3("Point 2", (float*)&lighting.lightPointColor2);
                    ImGui::ColorEdit3("Point 3", (float*)&lighting.lightPointColor3);

                    ImGui::TreePop();
                }

                if (ImGui::TreeNode("Radius"))
                {
                    ImGui::SliderFloat("Point 1", &lighting.lightPointRadius1, 0.0f, 10.0f);
                    ImGui::SliderFloat("Point 2", &lighting.lightPointRadius2, 0.0f, 10.0f);
                    ImGui::SliderFloat("Point 3", &lighting.lightPointRadius3, 0.0f, 10.0f);

                    ImGui::TreePop();
                }

                if (ImGui::TreeNode("Attenuation"))
                {
                    ImGui::RadioButton("Quadratic", &lighting.attenuationMode, 1);
                    ImGui::RadioButton("UE4", &lighting.attenuationMode, 2);

                    ImGui::TreePop();
                }

                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Directional"))
            {
                if (ImGui::TreeNode("Direction"))
                {
                    ImGui::SliderFloat3("Direction 1", (float*)&lighting.lightDirectionalDirection1, -5.0f, 5.0f);

                    ImGui::TreePop();
                }

                if (ImGui::TreeNode("Color"))
                {
                    ImGui::ColorEdit3("Direct. 1", (float*)&lighting.lightDirectionalColor1);

                    ImGui::TreePop();
                }

                ImGui::TreePop();
            }

            ImGui::Checkbox("Enable Environment map", &lighting.enableEnvMap);
            if (lighting.enableEnvMap)
            {
                ImGui::Checkbox("Iterate over skyboxes", &iterate_over_skyboxes);
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

                    for (auto sky : skyboxes) {
                        auto s = sky.generic_string();
                        if (ImGui::Button(std::string(s).c_str()))
                        {
                            skybox.setTexture(s.c_str(), s, true);
                            skybox.iblSetup(WIDTH, HEIGHT);
                        }
                    }

                    ImGui::TreePop();
                }
            }
            else {
                ImGui::Checkbox("Enable Background Texture", &lighting.enableBackground);
                if (lighting.enableBackground) {
                    int backgroundID = ceil(random(0, background_paths.size()-1));
                    lighting.backgroundTexture->setTexture(std::string("patches-512/").append(std::to_string(backgroundID)).append(".jpg").c_str(), "patch", true);
                }
                else {
                    ImGui::ColorEdit3("Background color", (float*)&lighting.backgroundColor);
                }
            }

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Post processing"))
        {
            if (ImGui::TreeNode("SAO"))
            {
                ImGui::Checkbox("Enable", &postprocess.saoMode);

                ImGui::SliderInt("Samples", &ssao.saoSamples, 0, 64);
                ImGui::SliderFloat("Radius", &ssao.saoRadius, 0.0f, 3.0f);
                ImGui::SliderInt("Turns", &ssao.saoTurns, 0, 16);
                ImGui::SliderFloat("Bias", &ssao.saoBias, 0.0f, 0.1f);
                ImGui::SliderFloat("Scale", &ssao.saoScale, 0.0f, 3.0f);
                ImGui::SliderFloat("Contrast", &ssao.saoContrast, 0.0f, 3.0f);
                ImGui::SliderInt("Blur Size", &ssao.saoBlurSize, 0, 8);

                ImGui::TreePop();
            }

            if (ImGui::TreeNode("FXAA"))
            {
                ImGui::Checkbox("Enable", &postprocess.fxaaMode);

                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Motion Blur"))
            {
                ImGui::Checkbox("Enable", &postprocess.motionBlurMode);
                ImGui::SliderInt("Max Samples", &postprocess.motionBlurMaxSamples, 1, 128);

                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Tonemapping"))
            {
                ImGui::RadioButton("Reinhard", &postprocess.tonemappingMode, 1);
                ImGui::RadioButton("Filmic", &postprocess.tonemappingMode, 2);
                ImGui::RadioButton("Uncharted", &postprocess.tonemappingMode, 3);

                ImGui::TreePop();
            }

            ImGui::TreePop();
        }
        

        if (ImGui::TreeNode("Camera"))
        {
            ImGui::SliderFloat("Aperture", &postprocess.cameraAperture, 1.0f, 32.0f);
            ImGui::SliderFloat("Shutter Speed", &postprocess.cameraShutterSpeed, 0.001f, 1.0f);
            ImGui::SliderFloat("ISO", &postprocess.cameraISO, 100.0f, 3200.0f);
            ImGui::Checkbox("Orbitting Camera", &isOrbitCamera);
            if(isOrbitCamera)
            {
                ImGui::Checkbox("Unique Output Index", &use_unique_output_index);
                if (recording) {
                    if (ImGui::Button("Stop Recording")) {
                        recording = false;
                    }
                }
                else {
                    if (ImGui::Button("Start Recording")) {
                        recording = true;
                        current_pool_model = model_pool_paths.begin();
                        frame = -1;
                    }
                }
                
                ImGui::SliderFloat("Theta", &theta, -glm::pi<float>(), glm::pi<float>());
                ImGui::SliderFloat("Rho", &rho, -glm::pi<float>()*.5f, glm::pi<float>()*.5f);
                ImGui::SliderFloat("Radius", &radius, 1.0, 10.f);
                
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

            if (!renderSegmentation) {
                if (ImGui::TreeNode("Material"))
                {
                    if (ImGui::Button("Rusted iron"))
                    {
                        gBuffer.setTexture(GBuffer::Albedo, "pbr/rustediron/rustediron_albedo.png", "ironAlbedo", true);
                        gBuffer.setTexture(GBuffer::Normal, "pbr/rustediron/rustediron_normal.png", "ironNormal", true);
                        gBuffer.setTexture(GBuffer::Roughness, "pbr/rustediron/rustediron_roughness.png", "ironRoughness", true);
                        gBuffer.setTexture(GBuffer::Metalness, "pbr/rustediron/rustediron_metalness.png", "ironMetalness", true);
                        gBuffer.setTexture(GBuffer::AmbientOcclusion, "pbr/rustediron/rustediron_ao.png", "ironAO", true);

                        lighting.materialF0 = glm::vec3(0.04f);
                    }

                    if (ImGui::Button("Gold"))
                    {
                        gBuffer.setTexture(GBuffer::Albedo, "pbr/gold/gold_albedo.png", "goldAlbedo", true);
                        gBuffer.setTexture(GBuffer::Normal, "pbr/gold/gold_normal.png", "goldNormal", true);
                        gBuffer.setTexture(GBuffer::Roughness, "pbr/gold/gold_roughness.png", "goldRoughness", true);
                        gBuffer.setTexture(GBuffer::Metalness, "pbr/gold/gold_metalness.png", "goldMetalness", true);
                        gBuffer.setTexture(GBuffer::AmbientOcclusion, "pbr/gold/gold_ao.png", "goldAO", true);

                        lighting.materialF0 = glm::vec3(1.0f, 0.72f, 0.29f);
                    }

                    if (ImGui::Button("Ceramic"))
                    {
                        gBuffer.setTexture(GBuffer::Albedo, "pbr/ceramic/ceramic_albedo.png", "goldAlbedo", true);
                        gBuffer.setTexture(GBuffer::Normal, "pbr/ceramic/ceramic_normal.png", "goldNormal", true);
                        gBuffer.setTexture(GBuffer::Roughness, "pbr/ceramic/ceramic_roughness.png", "goldRoughness", true);
                        gBuffer.setTexture(GBuffer::Metalness, "pbr/ceramic/ceramic_metalness.png", "goldMetalness", true);
                        gBuffer.setTexture(GBuffer::AmbientOcclusion, "pbr/ceramic/ceramic_ao.png", "goldAO", true);

                        lighting.materialF0 = glm::vec3(1.0f, 1.0f, 1.0f);
                    }

                    if (ImGui::Button("Woodfloor"))
                    {
                        gBuffer.setTexture(GBuffer::Albedo, "pbr/woodfloor/woodfloor_albedo.png", "woodfloorAlbedo", true);
                        gBuffer.setTexture(GBuffer::Normal, "pbr/woodfloor/woodfloor_normal.png", "woodfloorNormal", true);
                        gBuffer.setTexture(GBuffer::Roughness, "pbr/woodfloor/woodfloor_roughness.png", "woodfloorRoughness", true);
                        gBuffer.setTexture(GBuffer::Metalness, "pbr/woodfloor/woodfloor_metalness.png", "woodfloorMetalness", true);
                        gBuffer.setTexture(GBuffer::AmbientOcclusion, "pbr/woodfloor/woodfloor_ao.png", "woodfloorAO", true);

                        lighting.materialF0 = glm::vec3(0.04f);
                    }

                    ImGui::TreePop();
                }
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
        lighting.gBufferView = 1;

    if (keys[GLFW_KEY_2])
        lighting.gBufferView = 2;

    if (keys[GLFW_KEY_3])
        lighting.gBufferView = 3;

    if (keys[GLFW_KEY_4])
        lighting.gBufferView = 4;

    if (keys[GLFW_KEY_5])
        lighting.gBufferView = 5;

    if (keys[GLFW_KEY_6])
        lighting.gBufferView = 6;

    if (keys[GLFW_KEY_7])
        lighting.gBufferView = 7;

    if (keys[GLFW_KEY_8])
        lighting.gBufferView = 8;

    if (keys[GLFW_KEY_9])
        lighting.gBufferView = 9;

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

void updatePose()
{
    theta = random(-glm::pi<float>(), glm::pi<float>());
    rho = random(-glm::pi<float>(), 0);
    lighting.lightDirectionalDirection1.x = -5; //random(-5.f, 5.f);
    lighting.lightDirectionalDirection1.y = -5; //random(-5.f, 5.f);
    lighting.lightDirectionalDirection1.z = 5; //random(-5.f, 5.f);
    radius = random(1.7f, 3.5f);
}

void captureFrame(std::string output_path)
{
    
    const auto channels = 4;
    //unsigned char* ptr = new unsigned char[WIDTH * HEIGHT * channels];
    std::vector<unsigned char> data(WIDTH * HEIGHT * channels);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0, 0, WIDTH, HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);
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
            std::swap(data[offset + 0], data[swapOffset + 0]);
            std::swap(data[offset + 1], data[swapOffset + 1]);
            std::swap(data[offset + 2], data[swapOffset + 2]);
        }
    }
    if (output_path.size() <= 0) {
       
        if (renderSegmentation) {
            if (frame - 1 >= 0) {
                auto path = std::string(recording_path + "segment" + std::to_string(frame - 1) + ".jpg").c_str();
                std::cout << path << std::endl;
                stbi_write_jpg(path, WIDTH, HEIGHT, 4, data.data(), WIDTH * channels);
                
            }
            labelRendered = true;
        }
        else {
            if (frame < imageCount - 1) {
                auto path = "";
                if(use_unique_output_index)
                    path = std::string(recording_path + "frame" + std::to_string(recorded_frame) + ".jpg").c_str();
                else
                    path = std::string(recording_path + "frame" + std::to_string(frame) + ".jpg").c_str();
                std::cout << path << std::endl;
                stbi_write_jpg(path, WIDTH, HEIGHT, 4, data.data(), WIDTH * channels);
                recorded_frame++;
            }
            targetRendered = true;
        }
    }
    else {
        std::cout << "writing snapshot " << currentSnapshot-1 << " to " << output_path << std::endl;
        stbi_write_jpg(output_path.c_str(), WIDTH, HEIGHT, 4, data.data(), WIDTH * channels);
    }
    if (!enableSegmentation || labelRendered && targetRendered) {
        frame++;
    }
}

void prepareModel()
{
    if (!recording) return;

    if (current_pool_model >= model_pool_paths.end()) {
        if (iterate_over_skyboxes)
            prepareSkybox();
        else
            recording = false;
        return;
    }       

    std::cout << current_pool_model->generic_string() << std::endl;
    gBuffer.loadModel(current_pool_model->generic_string(), glm::vec3(2.0f));

    auto d = static_cast<int>(std::find(model_pool_paths.begin(), model_pool_paths.end(), static_cast<fs::path>(*current_pool_model)) - model_pool_paths.begin());
    if (d < invertNormal.size()) {
        gBuffer.negativeNormals = invertNormal[d];
    }

    //set proper output dir
    auto output = current_pool_model->generic_string();
    const auto outputDir = fs::path(std::string(MODEL_PATH) + output.substr(0, output.size() - 4));
    if (!fs::is_directory(outputDir))
    {
        fs::create_directory(outputDir);
    }

    recording_path = outputDir.generic_string() + "/";
    ++current_pool_model;
}


void prepareSkybox()
{
    if (!recording || !iterate_over_skyboxes) return;
    current_skybox++;
    if (current_skybox >= skyboxes.end()) {
        recording = false;
        return;
    }
    std::cout << current_skybox->generic_string() << std::endl;
    skybox.setTexture(current_skybox->generic_string().c_str(), current_skybox->generic_string().c_str(), true);
    skybox.iblSetup(WIDTH, HEIGHT);
    current_pool_model = model_pool_paths.begin();
}
