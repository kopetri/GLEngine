#include "postprocess.h"
#include "shader.h"
#include "shape.h"
#include <imgui.h>
#include <iostream>

Postprocess::Postprocess(GLuint width, GLuint height) :
      fbo(0)
    , postprocessBuffer(0)
    , width(width)
    , height(height)
    , firstpassPPShader(std::make_shared<Shader>())
    , quadRender(std::make_shared<Shape>())
    , cameraAperture(16.0f)
    , cameraShutterSpeed(0.5f)
    , cameraISO(1000.0f)
    , saoMode(false)
    , fxaaMode(false)
    , motionBlurMode(false)
    , tonemappingMode(1)
    , motionBlurMaxSamples(32)
{
}

Postprocess::~Postprocess()
{
}

void Postprocess::setup(GLuint fbo)
{
    firstpassPPShader->setShader("postprocess/postprocess.vert", "postprocess/firstpass.frag");

    // Post-processing Buffer
    this->fbo = fbo;
    //glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glGenTextures(1, &postprocessBuffer);
    glBindTexture(GL_TEXTURE_2D, postprocessBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postprocessBuffer, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Postprocess Framebuffer not complete !" << std::endl;

    firstpassPPShader->useShader();
    glUniform1i(glGetUniformLocation(firstpassPPShader->Program, "sao"), 1);
    glUniform1i(glGetUniformLocation(firstpassPPShader->Program, "gEffects"), 2);
}

void Postprocess::draw(GBuffer &gBuffer, SSAO &ssao, Lighting &lighting)
{
    glClear(GL_COLOR_BUFFER_BIT);

    firstpassPPShader->useShader();
    glUniform1i(glGetUniformLocation(firstpassPPShader->Program, "gBufferView"), lighting.gBufferView);
    glUniform2f(glGetUniformLocation(firstpassPPShader->Program, "screenTextureSize"), 1.0f / width, 1.0f / height);
    glUniform1f(glGetUniformLocation(firstpassPPShader->Program, "cameraAperture"), cameraAperture);
    glUniform1f(glGetUniformLocation(firstpassPPShader->Program, "cameraShutterSpeed"), cameraShutterSpeed);
    glUniform1f(glGetUniformLocation(firstpassPPShader->Program, "cameraISO"), cameraISO);
    glUniform1i(glGetUniformLocation(firstpassPPShader->Program, "saoMode"), saoMode);
    glUniform1i(glGetUniformLocation(firstpassPPShader->Program, "fxaaMode"), fxaaMode);
    glUniform1i(glGetUniformLocation(firstpassPPShader->Program, "motionBlurMode"), motionBlurMode);
    glUniform1f(glGetUniformLocation(firstpassPPShader->Program, "motionBlurScale"), int(ImGui::GetIO().Framerate) / 60.0f);
    glUniform1i(glGetUniformLocation(firstpassPPShader->Program, "motionBlurMaxSamples"), motionBlurMaxSamples);
    glUniform1i(glGetUniformLocation(firstpassPPShader->Program, "tonemappingMode"), tonemappingMode);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, postprocessBuffer);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, ssao.saoBlurBuffer);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, gBuffer.gEffects);

    quadRender->drawShape();
}


void Postprocess::setRender(const Shape &render)
{
    quadRender = std::make_shared<Shape>(render);
}