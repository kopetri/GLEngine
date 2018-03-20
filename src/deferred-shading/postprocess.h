#pragma once

#include "gbuffer.h"
#include "ssao.h"
#include "lighting.h"
#include <memory>
#include <glad\glad.h>

class Shader;
class Shape;

class Postprocess
{
public:
    Postprocess(GLuint width, GLuint height);
    ~Postprocess();

    GLfloat cameraAperture;
    GLfloat cameraShutterSpeed;
    GLfloat cameraISO;

    GLint tonemappingMode;
    GLint motionBlurMaxSamples;

    bool saoMode;
    bool fxaaMode;
    bool motionBlurMode;


    void setup(GLuint fbo);
    void draw(GBuffer &gBuffer, SSAO &ssao, Lighting &lighting);
    void setRender(const Shape &render);
private:
    GLuint fbo;
    GLuint postprocessBuffer;

    GLuint width;
    GLuint height;

    std::shared_ptr<Shape> quadRender;
    std::shared_ptr<Shader> firstpassPPShader;
};

