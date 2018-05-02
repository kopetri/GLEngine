#pragma once

#include "camera.h"
#include "gbuffer.h"
#include "ssao.h"
#include "skybox.h"
#include <glm\glm.hpp>
#include <memory>

class Shader;
class Light;
class Shape;
class Texture;

class Lighting
{
public:
    Lighting(GLuint width, GLuint height);
    ~Lighting();

    glm::vec3 lightPointPosition1;
    glm::vec3 lightPointPosition2;
    glm::vec3 lightPointPosition3;
    glm::vec3 lightPointColor1;
    glm::vec3 lightPointColor2;
    glm::vec3 lightPointColor3;
    glm::vec3 lightDirectionalDirection1;
    glm::vec3 lightDirectionalColor1;
    glm::vec3 backgroundColor;
    glm::vec3 foregroundColor;

    GLfloat lightPointRadius1;
    GLfloat lightPointRadius2;
    GLfloat lightPointRadius3;

    glm::vec3 materialF0;  // UE4 dielectric
    GLfloat ambientIntensity;
    GLint gBufferView;
    GLint attenuationMode;
    bool pointMode;
    bool directionalMode;
    bool iblMode;
    bool enableEnvMap;
    bool enableBackground;

    std::shared_ptr<Texture> backgroundTexture;

    void setup();
    void draw(Camera &camera, GBuffer &gBuffer, SSAO &ssao, Skybox &skybox, bool segmentation);
    void forwardPass(Camera &camera, GBuffer &gBuffer);
    void setRender(const Shape &render);
    GLuint framebuffer() { return fbo; }
private:
    std::shared_ptr<Shader> simpleShader;
    std::shared_ptr<Shader> lightingBRDFShader;
    std::shared_ptr<Shape> quadRender;

    // lights
    std::shared_ptr<Light> lightPoint1;
    std::shared_ptr<Light> lightPoint2;
    std::shared_ptr<Light> lightPoint3;
    std::shared_ptr<Light> lightDirectional1;

    GLuint width;
    GLuint height;

    GLuint fbo;
};