#include "ssao.h"
#include "shader.h"
#include "shape.h"
#include <iostream>

SSAO::SSAO(GLuint width, GLuint height) :
      width(width)
    , height(height)
    , saoShader(std::make_shared<Shader>())
    , saoBlurShader(std::make_shared<Shader>())
    , quadRenderer(std::make_shared<Shape>())
    , saoRadius(0.3f)
    , saoBias(0.001f)
    , saoScale(0.7f)
    , saoContrast(0.8f)
    , saoSamples(12)
    , saoTurns(7)
    , saoBlurSize(4)
{
}

SSAO::~SSAO()
{
}

void SSAO::setup()
{
    // init renderer
    quadRenderer->setShape("quad", glm::vec3(0.0f));

    // init shader
    saoShader->setShader("postprocess/sao.vert", "postprocess/sao.frag");
    saoBlurShader->setShader("postprocess/sao.vert", "postprocess/saoBlur.frag");

    saoShader->useShader();
    glUniform1i(glGetUniformLocation(saoShader->Program, "gPosition"), 0);
    glUniform1i(glGetUniformLocation(saoShader->Program, "gNormal"), 1);

    // SAO Buffer
    glGenFramebuffers(1, &saoFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, saoFBO);
    glGenTextures(1, &saoBuffer);
    glBindTexture(GL_TEXTURE_2D, saoBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, saoBlurBuffer, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "SAO Blur Framebuffer not complete !" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SSAO::draw(GBuffer &gBuffer)
{
    glBindFramebuffer(GL_FRAMEBUFFER, saoFBO);
    glClear(GL_COLOR_BUFFER_BIT);

    // SAO noisy texture
    saoShader->useShader();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gBuffer.gPosition);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gBuffer.gNormal);

    glUniform1i(glGetUniformLocation(saoShader->Program, "saoSamples"), saoSamples);
    glUniform1f(glGetUniformLocation(saoShader->Program, "saoRadius"), saoRadius);
    glUniform1i(glGetUniformLocation(saoShader->Program, "saoTurns"), saoTurns);
    glUniform1f(glGetUniformLocation(saoShader->Program, "saoBias"), saoBias);
    glUniform1f(glGetUniformLocation(saoShader->Program, "saoScale"), saoScale);
    glUniform1f(glGetUniformLocation(saoShader->Program, "saoContrast"), saoContrast);
    glUniform1i(glGetUniformLocation(saoShader->Program, "viewportWidth"), width);
    glUniform1i(glGetUniformLocation(saoShader->Program, "viewportHeight"), height);

    quadRenderer->drawShape();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // SAO blur pass
    glBindFramebuffer(GL_FRAMEBUFFER, saoBlurFBO);
    glClear(GL_COLOR_BUFFER_BIT);

    saoBlurShader->useShader();

    glUniform1i(glGetUniformLocation(saoBlurShader->Program, "saoBlurSize"), saoBlurSize);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, saoBuffer);

    quadRenderer->drawShape();
    

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SSAO::setRender(const Shape &render)
{
    quadRenderer = std::make_shared<Shape>(render);
}