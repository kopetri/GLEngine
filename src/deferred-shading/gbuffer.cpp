#include "gbuffer.h"
#include "shader.h"
#include "texture.h"
#include "model.h"
#include "boundingbox.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

GBuffer::GBuffer(const GLuint width, const GLuint height) :
    gBuffer(0)
    , gPosition(0)
    , gAlbedo(0)
    , gEffects()
    , gNormal(0)
    , zBuffer(0)
    , modelRotationSpeed(0)
    , materialRoughness(0.01f)
    , materialMetallicity(0.02f)
    , useRoughnessTexture(false)
    , useAlbedoTexture(false)
    , useMetalnessTexture(false)
    , useNormalTexture(false)
    , negativeNormals(false)
    , enableBoundingBox(true)
    , modelRotationAxis(glm::vec3(0.0f, 1.0f, 0.0f))
    , modelPosition(glm::vec3(0.0f))
    , modelScale(glm::vec3(0.1f))
    , albedoColor(glm::vec3(1.0f))
    , width(width)
    , height(height)
    , projViewModel(glm::mat4(1.0))
    , prevProjViewModel(glm::mat4(1.0))
{
}

GBuffer::~GBuffer()
{
}

void GBuffer::setup()
{
    // init shader
    shader = std::make_shared<Shader>();
    shader->setShader("gBuffer.vert", "gBuffer.frag");

    // init textures
    objectAlbedo = std::make_shared<Texture>();
    objectAlbedo->setTexture("pbr/rustediron/rustediron_albedo.png", "ironAlbedo", true);
    objectNormal = std::make_shared<Texture>();
    objectNormal->setTexture("pbr/rustediron/rustediron_normal.png", "ironNormal", true);
    objectRoughness = std::make_shared<Texture>();
    objectRoughness->setTexture("pbr/rustediron/rustediron_roughness.png", "ironRoughness", true);
    objectMetalness = std::make_shared<Texture>();
    objectMetalness->setTexture("pbr/rustediron/rustediron_metalness.png", "ironMetalness", true);
    objectAO = std::make_shared<Texture>();
    objectAO->setTexture("pbr/rustediron/rustediron_ao.png", "ironAO", true);

    // init model
    objectModel = std::make_shared<Model>();
    objectModel->loadModel("shaderball/shaderball.obj");

    // init Bounding Box
    boundingBox = std::make_shared<BoundingBox>();

    // init buffers
    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

    // Define the COLOR_ATTACHMENTS for the G-Buffer
    std::vector<GLuint> attachments = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4 };

    // Position
    glGenTextures(1, &gPosition);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachments[0], GL_TEXTURE_2D, gPosition, 0);

    // Albedo + Roughness
    glGenTextures(1, &gAlbedo);
    glBindTexture(GL_TEXTURE_2D, gAlbedo);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachments[1], GL_TEXTURE_2D, gAlbedo, 0);

    // Normals + Metalness
    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachments[2], GL_TEXTURE_2D, gNormal, 0);

    // Effects (AO + Velocity)
    glGenTextures(1, &gEffects);
    glBindTexture(GL_TEXTURE_2D, gEffects);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachments[3], GL_TEXTURE_2D, gEffects, 0);

    // Bounding Box
    glGenTextures(1, &gBoundingBox);
    glBindTexture(GL_TEXTURE_2D, gBoundingBox);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachments[4], GL_TEXTURE_2D, gBoundingBox, 0);

    glDrawBuffers(attachments.size(), attachments.data());

    // Z-Buffer
    glGenRenderbuffers(1, &zBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, zBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, zBuffer);

    // Check if the framebuffer is complete before continuing
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete !" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GBuffer::draw(Camera camera)
{
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Camera setting
    glm::mat4 projection = camera.GetProjectionMatrix();
    glm::mat4 view = camera.GetViewMatrix();

    // Model(s) rendering
    shader->useShader();

    glUniformMatrix4fv(glGetUniformLocation(shader->Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(shader->Program, "view"), 1, GL_FALSE, glm::value_ptr(view));

    const GLfloat rotationAngle = glfwGetTime() / 5.0f * modelRotationSpeed;
    auto model = glm::mat4(1.0);
    
    model = glm::rotate(model, rotationAngle, modelRotationAxis);
    model = glm::translate(model, -objectModel->getCenter()*modelScale);
    model = glm::scale(model, modelScale);

    projViewModel = projection * view * model;

    glUniformMatrix4fv(glGetUniformLocation(shader->Program, "projViewModel"), 1, GL_FALSE, glm::value_ptr(projViewModel));
    glUniformMatrix4fv(glGetUniformLocation(shader->Program, "prevProjViewModel"), 1, GL_FALSE, glm::value_ptr(prevProjViewModel));
    glUniformMatrix4fv(glGetUniformLocation(shader->Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniform3f(glGetUniformLocation(shader->Program, "albedoColor"), albedoColor.r, albedoColor.g, albedoColor.b);
    glUniform1f(glGetUniformLocation(shader->Program, "materialRoughness"), materialRoughness);
    glUniform1f(glGetUniformLocation(shader->Program, "materialMetallicity"), materialMetallicity);
    glUniform1i(glGetUniformLocation(shader->Program, "useRoughnessTexture"), useRoughnessTexture);
    glUniform1i(glGetUniformLocation(shader->Program, "useAlbedoTexture"), useAlbedoTexture);
    glUniform1i(glGetUniformLocation(shader->Program, "useMetalnessTexture"), useMetalnessTexture);
    glUniform1i(glGetUniformLocation(shader->Program, "useNormalTexture"), useNormalTexture);
    glUniform1i(glGetUniformLocation(shader->Program, "negativeNormals"), negativeNormals);
    glUniform1i(glGetUniformLocation(shader->Program, "enableBoundingBox"), enableBoundingBox);


    // Material
    // pbrMat.renderToShader();

    glActiveTexture(GL_TEXTURE0);
    objectAlbedo->useTexture();
    glUniform1i(glGetUniformLocation(shader->Program, "texAlbedo"), 0);
    glActiveTexture(GL_TEXTURE1);
    objectNormal->useTexture();
    glUniform1i(glGetUniformLocation(shader->Program, "texNormal"), 1);
    glActiveTexture(GL_TEXTURE2);
    objectRoughness->useTexture();
    glUniform1i(glGetUniformLocation(shader->Program, "texRoughness"), 2);
    glActiveTexture(GL_TEXTURE3);
    objectMetalness->useTexture();
    glUniform1i(glGetUniformLocation(shader->Program, "texMetalness"), 3);
    glActiveTexture(GL_TEXTURE4);
    objectAO->useTexture();
    glUniform1i(glGetUniformLocation(shader->Program, "texAO"), 4);

    objectModel->Draw();
    if (enableBoundingBox) {
        boundingBox->setDimensions(objectModel->getCenter(), objectModel->getSize());
        boundingBox->Draw(projViewModel);
    }

    prevProjViewModel = projViewModel;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GBuffer::loadModel(const std::string path, const glm::vec3 scale)
{
    objectModel = std::make_shared<Model>();
    objectModel->loadModel(path);
    modelScale = scale;
}

void GBuffer::setTexture(TextureUsage texUse, const char* path, std::string name, bool flip)
{
    switch (texUse)
    {
    case Normal:
        objectNormal->setTexture(path, name, flip);
        break;
    case Albedo:
        objectAlbedo->setTexture(path, name, flip);
        break;
    case Roughness:
        objectRoughness->setTexture(path, name, flip);
        break;
    case Metalness:
        objectMetalness->setTexture(path, name, flip);
        break;
    case AmbientOcclusion:
        objectAO->setTexture(path, name, flip);
        break;
    default:
        std::cout << "unnown TextureUsage given..." << std::endl;
        break;
    }
}
