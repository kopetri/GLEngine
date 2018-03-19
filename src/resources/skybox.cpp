#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "stb_image.h"
#include "skybox.h"


Skybox::Skybox() :
    envMapProjection(glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f))
{
    envMapView =
    {
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };
}


Skybox::~Skybox()
{

}

void Skybox::setup()
{
    // init renderer
    envCubeRender.setShape("cube", glm::vec3(0.0f));
    quadRender.setShape("quad", glm::vec3(0.0f));
    // init shader
    latlongToCubeShader.setShader("resources/shaders/latlongToCube.vert", "resources/shaders/latlongToCube.frag");
    irradianceIBLShader.setShader("resources/shaders/lighting/irradianceIBL.vert", "resources/shaders/lighting/irradianceIBL.frag");
    prefilterIBLShader.setShader("resources/shaders/lighting/prefilterIBL.vert", "resources/shaders/lighting/prefilterIBL.frag");
    integrateIBLShader.setShader("resources/shaders/lighting/integrateIBL.vert", "resources/shaders/lighting/integrateIBL.frag");
    // init texture
    envMapHDR.setTextureHDR("resources/textures/hdr/appart.hdr", "appartHDR", true);
    envMapCube.setTextureCube(512, GL_RGB, GL_RGB16F, GL_FLOAT, GL_LINEAR_MIPMAP_LINEAR);
    envMapIrradiance.setTextureCube(32, GL_RGB, GL_RGB16F, GL_FLOAT, GL_LINEAR);
    envMapPrefilter.setTextureCube(128, GL_RGB, GL_RGB16F, GL_FLOAT, GL_LINEAR_MIPMAP_LINEAR);
    envMapPrefilter.computeTexMipmap();
    envMapLUT.setTextureHDR(512, 512, GL_RG, GL_RG16F, GL_FLOAT, GL_LINEAR);

    latlongToCubeShader.useShader();
    glUniform1i(glGetUniformLocation(latlongToCubeShader.Program, "envMap"), 0);

    irradianceIBLShader.useShader();
    glUniform1i(glGetUniformLocation(irradianceIBLShader.Program, "envMap"), 0);

    prefilterIBLShader.useShader();
    glUniform1i(glGetUniformLocation(prefilterIBLShader.Program, "envMap"), 0);
}


void Skybox::setSkyboxTexture(const char* texPath)
{
    this->texSkybox.setTextureHDR(texPath, "cubemapHDR", true);
}


void Skybox::renderToShader(Shader& shaderSkybox, glm::mat4& projection, glm::mat4& view)
{
    shaderSkybox.useShader();
    glActiveTexture(GL_TEXTURE0);
    this->texSkybox.useTexture();

    glUniform1i(glGetUniformLocation(shaderSkybox.Program, "envMap"), 0);
    glUniformMatrix4fv(glGetUniformLocation(shaderSkybox.Program, "inverseView"), 1, GL_FALSE, glm::value_ptr(glm::transpose(view)));
    glUniformMatrix4fv(glGetUniformLocation(shaderSkybox.Program, "inverseProj"), 1, GL_FALSE, glm::value_ptr(glm::inverse(projection)));
    glUniform1f(glGetUniformLocation(shaderSkybox.Program, "cameraAperture"), this->cameraAperture);
    glUniform1f(glGetUniformLocation(shaderSkybox.Program, "cameraShutterSpeed"), this->cameraShutterSpeed);
    glUniform1f(glGetUniformLocation(shaderSkybox.Program, "cameraISO"), this->cameraISO);
}


void Skybox::setExposure(GLfloat aperture, GLfloat shutterSpeed, GLfloat iso)
{
    this->cameraAperture = aperture;
    this->cameraShutterSpeed = shutterSpeed;
    this->cameraISO = iso;
}

void Skybox::setTextureHDR(const char * texPath, std::string texName, bool texFlip)
{
    envMapHDR.setTextureHDR(texPath, texName, texFlip);
}


void Skybox::iblSetup(int WIDTH, int HEIGHT)
{
    // Latlong to Cubemap conversion
    glGenFramebuffers(1, &envToCubeFBO);
    glGenRenderbuffers(1, &envToCubeRBO);
    glBindFramebuffer(GL_FRAMEBUFFER, envToCubeFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, envToCubeRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, envMapCube.getTexWidth(), envMapCube.getTexHeight());
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, envToCubeRBO);

    latlongToCubeShader.useShader();

    glUniformMatrix4fv(glGetUniformLocation(latlongToCubeShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(envMapProjection));
    glActiveTexture(GL_TEXTURE0);
    envMapHDR.useTexture();

    glViewport(0, 0, envMapCube.getTexWidth(), envMapCube.getTexHeight());
    glBindFramebuffer(GL_FRAMEBUFFER, envToCubeFBO);

    for (unsigned int i = 0; i < 6; ++i)
    {
        glUniformMatrix4fv(glGetUniformLocation(latlongToCubeShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(envMapView[i]));
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envMapCube.getTexID(), 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        envCubeRender.drawShape();
    }

    envMapCube.computeTexMipmap();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Diffuse irradiance capture
    glGenFramebuffers(1, &irradianceFBO);
    glGenRenderbuffers(1, &irradianceRBO);
    glBindFramebuffer(GL_FRAMEBUFFER, irradianceFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, irradianceRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, envMapIrradiance.getTexWidth(), envMapIrradiance.getTexHeight());

    irradianceIBLShader.useShader();

    glUniformMatrix4fv(glGetUniformLocation(irradianceIBLShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(envMapProjection));
    glActiveTexture(GL_TEXTURE0);
    envMapCube.useTexture();

    glViewport(0, 0, envMapIrradiance.getTexWidth(), envMapIrradiance.getTexHeight());
    glBindFramebuffer(GL_FRAMEBUFFER, irradianceFBO);

    for (unsigned int i = 0; i < 6; ++i)
    {
        glUniformMatrix4fv(glGetUniformLocation(irradianceIBLShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(envMapView[i]));
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envMapIrradiance.getTexID(), 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        envCubeRender.drawShape();
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Prefilter cubemap
    prefilterIBLShader.useShader();

    glUniformMatrix4fv(glGetUniformLocation(prefilterIBLShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(envMapProjection));
    envMapCube.useTexture();

    glGenFramebuffers(1, &prefilterFBO);
    glGenRenderbuffers(1, &prefilterRBO);
    glBindFramebuffer(GL_FRAMEBUFFER, prefilterFBO);

    unsigned int maxMipLevels = 5;

    for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
    {
        unsigned int mipWidth = envMapPrefilter.getTexWidth() * std::pow(0.5, mip);
        unsigned int mipHeight = envMapPrefilter.getTexHeight() * std::pow(0.5, mip);

        glBindRenderbuffer(GL_RENDERBUFFER, prefilterRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);

        glViewport(0, 0, mipWidth, mipHeight);

        float roughness = (float)mip / (float)(maxMipLevels - 1);

        glUniform1f(glGetUniformLocation(prefilterIBLShader.Program, "roughness"), roughness);
        glUniform1f(glGetUniformLocation(prefilterIBLShader.Program, "cubeResolutionWidth"), envMapPrefilter.getTexWidth());
        glUniform1f(glGetUniformLocation(prefilterIBLShader.Program, "cubeResolutionHeight"), envMapPrefilter.getTexHeight());

        for (unsigned int i = 0; i < 6; ++i)
        {
            glUniformMatrix4fv(glGetUniformLocation(prefilterIBLShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(envMapView[i]));
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envMapPrefilter.getTexID(), mip);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            envCubeRender.drawShape();
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // BRDF LUT
    glGenFramebuffers(1, &brdfLUTFBO);
    glGenRenderbuffers(1, &brdfLUTRBO);
    glBindFramebuffer(GL_FRAMEBUFFER, brdfLUTFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, brdfLUTRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, envMapLUT.getTexWidth(), envMapLUT.getTexHeight());
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, envMapLUT.getTexID(), 0);

    glViewport(0, 0, envMapLUT.getTexWidth(), envMapLUT.getTexHeight());
    integrateIBLShader.useShader();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    quadRender.drawShape();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glViewport(0, 0, WIDTH, HEIGHT);
}

void Skybox::bindEnvMapHDRTexture()
{
    envMapHDR.useTexture();
}

void Skybox::bindEnvMapIrradianceTexture()
{
    envMapIrradiance.useTexture();
}

void Skybox::bindEnvMapPrefilterTexture()
{
    envMapPrefilter.useTexture();
}

void Skybox::bindEnvMapLUTTexture()
{
    envMapLUT.useTexture();
}
