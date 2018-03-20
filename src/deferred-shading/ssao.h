#pragma once

#include <memory>
#include <glad\glad.h>
#include "gbuffer.h"

class Shape;
class Shader;

class SSAO
{
public:
    SSAO(GLuint width, GLuint height);
    ~SSAO();

    GLuint saoFBO;
    GLuint saoBuffer;
    GLuint saoBlurFBO;
    GLuint saoBlurBuffer;

    GLfloat saoRadius;
    GLfloat saoBias;
    GLfloat saoScale;
    GLfloat saoContrast;

    GLint saoSamples;
    GLint saoTurns;
    GLint saoBlurSize;

    void setup();
    void draw(GBuffer &gBuffer);
    void setRender(const Shape &render);
private:
    std::shared_ptr<Shader> saoShader;
    std::shared_ptr<Shader> saoBlurShader;
    std::shared_ptr<Shape> quadRenderer;

    GLuint width;
    GLuint height;
};